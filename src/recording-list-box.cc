/*
 * recording-list-box.cc
 * This file is part of SoundCollection
 *
 * Copyright (C) 2014 - Jonathon Jongsma
 *
 * SoundCollection is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * SoundCollection is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SoundCollection. If not, see <http://www.gnu.org/licenses/>.
 */

#include <limits>
#include <gom/gom.h>
#include "recording-list-box.h"
#include "recording-list-row.h"
#include "recording-resource.h"
#include "GRefPtr.h"

namespace SC {

struct RecordingListBox::Priv {
    Priv()
        : page_size(20)
        , page(0)
    {
    }
    WTF::GRefPtr<GomRepository> repository;
    WTF::GRefPtr<GomFilter> filter;
    WTF::GRefPtr<GomResourceGroup> results;

    guint page_size;
    guint page;
};

RecordingListBox::RecordingListBox(GomRepository* repository)
    : m_priv(new Priv())
{
    set_repository(repository);
}

void RecordingListBox::set_repository(GomRepository* repository)
{
    if (!repository)
        return;

    m_priv->repository = repository;
    update();
}

void RecordingListBox::set_page_size(guint page_size)
{
    // try to keep roughly the same results displayed in the list
    int first_item = m_priv->page * m_priv->page_size;
    m_priv->page_size = page_size;
    m_priv->page = first_item / page_size;

    update();
}

guint RecordingListBox::get_page_size() const
{
    return m_priv->page_size;
}

guint RecordingListBox::get_n_pages() const
{
    if (!m_priv->results)
        return 0;

    return std::ceil(static_cast<float>(gom_resource_group_get_count(m_priv->results.get()))
                     / m_priv->page_size);
}

void RecordingListBox::set_page(guint page)
{
    g_return_if_fail(page < 0);
    g_return_if_fail(page >= get_n_pages());
    m_priv->page = page;
    update();
}

guint RecordingListBox::get_page() const
{
    return m_priv->page;
}

void RecordingListBox::set_filter(GomFilter* filter)
{
    if (filter == m_priv->filter)
        return;
    m_priv->filter = WTF::adoptGRef(filter);
    // when setting a new filter, reset to the first page....
    m_priv->page = 0;
    update();
}

void RecordingListBox::clear()
{
    std::vector<Gtk::Widget*> children = get_children();
    for (std::vector<Gtk::Widget*>::iterator it = children.begin();
         it != children.end();
         ++it) {
        delete (*it);
    }
}

void RecordingListBox::update()
{
    g_debug("%s", G_STRFUNC);
    if (!m_priv->repository)
        return;
    clear();
    gom_repository_find_async(m_priv->repository.get(),
                              SC_TYPE_RECORDING_RESOURCE,
                              m_priv->filter.get(),
                              RecordingListBox::got_results_proxy,
                              this);
}

void RecordingListBox::got_results_proxy(GObject* source,
                                         GAsyncResult* result,
                                         gpointer user_data)
{
    GomRepository* repository = reinterpret_cast<GomRepository*>(source);
    RecordingListBox* self = reinterpret_cast<RecordingListBox*>(user_data);
    self->got_results(repository, result);
}

void RecordingListBox::got_results(GomRepository* repository, GAsyncResult* result)
{
    g_debug("%s", G_STRFUNC);
    GError* error = 0;
    m_priv->results = gom_repository_find_finish(repository, result, &error);
    if (error) {
        g_warning("Unable to find resources: %s", error->message);
        g_clear_error(&error);
        return;
    }

    g_debug("Found %i resources", gom_resource_group_get_count(m_priv->results.get()));
    int start = m_priv->page * m_priv->page_size;
    gom_resource_group_fetch_async(m_priv->results.get(),
                                   start,
                                   m_priv->page_size,
                                   RecordingListBox::fetch_done_proxy,
                                   this);
}

void RecordingListBox::fetch_done_proxy(GObject* source,
                                        GAsyncResult* result,
                                        gpointer user_data)
{
    RecordingListBox* self = reinterpret_cast<RecordingListBox*>(user_data);
    GomResourceGroup* resources = reinterpret_cast<GomResourceGroup*>(source);
    self->fetch_done(resources, result);
}

void RecordingListBox::fetch_done(GomResourceGroup* resources, GAsyncResult* result)
{
    g_debug("%s", G_STRFUNC);
    GError* error = 0;
    if (!gom_resource_group_fetch_finish(resources, result, &error)) {
        g_warning("Unable to fetch resources: %s", error->message);
        g_clear_error(&error);
        return;
    }

    guint start = m_priv->page * m_priv->page_size;
    guint end = std::min(start + m_priv->page_size,
                       gom_resource_group_get_count(m_priv->results.get()));
    for (int i = start; i < end; ++i) {
        GomResource* resource = gom_resource_group_get_index(resources, i);
        g_debug("appending recording row %i", i);
        RecordingListRow* row = Gtk::manage(new RecordingListRow(SC_RECORDING_RESOURCE(resource)));
        row->show();
        append(*row);
    }
}
}
