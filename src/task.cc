/*
 * task.cc
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

#include "task.h"

namespace SC {

static void delete_self(gpointer p)
{
    Task* t = reinterpret_cast<Task*>(p);
    delete t;
}

struct Task::Priv {
    GTask* gtask; // weak ref; the gtask will delete us
    Gio::SlotAsyncReady slot;

    Priv(Task* task, const Gio::SlotAsyncReady& cb)
        : gtask(g_task_new(0, 0, Priv::async_ready_proxy, task))
        , slot(cb)
    {
        g_task_set_task_data(gtask, task, delete_self);
    }

    static void async_ready_proxy(GObject* source,
                                  GAsyncResult* result,
                                  gpointer user_data)
    {
        Task* self = reinterpret_cast<Task*>(user_data);
        Glib::RefPtr<Gio::AsyncResult> cpptype = Glib::wrap(result);
        self->m_priv->slot(cpptype);
    }
};

GTask* Task::task()
{
    return m_priv->gtask;
}

Task::Task(const Gio::SlotAsyncReady& slot)
    : m_priv(new Priv(this, slot))
{
}
}
