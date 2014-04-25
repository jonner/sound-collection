/*
 * RecordingListBox.h
 * This file is part of <program name>
 *
 * Copyright (C) 2014 - Jonathon Jongsma
 *
 * <program name> is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * <program name> is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with <program name>. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _RECORDING_LIST_BOX_H
#define _RECORDING_LIST_BOX_H

#include <gom/gom.h>
#include <gtkmm.h>
#include <tr1/memory>

namespace SC {
class RecordingListBox : public Gtk::ListBox {
public:
    RecordingListBox(GomRepository* repository = 0);
    
    void set_repository(GomRepository* repository);

    void set_page_size(guint page_size);
    guint get_page_size() const;
    
    guint get_n_pages() const;

    void set_page(guint page);
    guint get_page() const;

    // takes ownership of filter
    void set_filter(GomFilter* filter);

private:
    void clear();
    void update();

    static void got_results_proxy(GObject* source,
                                  GAsyncResult* result,
                                  gpointer user_data);
    void got_results(GomRepository* repository, GAsyncResult* result);

    static void fetch_done_proxy(GObject* source,
                                 GAsyncResult* result,
                                 gpointer user_data);
    void fetch_done(GomResourceGroup* resources, GAsyncResult* result);

    struct Priv;
    std::tr1::shared_ptr<Priv> m_priv;
};
}

#endif /* _RECORDING_LIST_BOX_H */
