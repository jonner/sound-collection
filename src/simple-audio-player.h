/*
 * simple-audio-player.h
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

#ifndef _SIMPLE_AUDIO_PLAYER_H
#define _SIMPLE_AUDIO_PLAYER_H

#include <gtkmm.h>
#include <gst/gst.h>
#include <tr1/memory>

namespace SC {
class SimpleAudioPlayer : public Gtk::Button {
public:
    SimpleAudioPlayer(const Glib::RefPtr<Gio::File>& file);
    void set_file(const Glib::RefPtr<Gio::File>& file);

private:
    static void on_pipeline_message_proxy(GstBus* bus, GstMessage* message, gpointer user_data);
    void on_pipeline_message(GstBus* bus, GstMessage* message);
    void pipeline_state_changed();
    virtual void on_clicked();

    struct Priv;
    std::tr1::shared_ptr<Priv> m_priv;
};
}

#endif /* _SIMPLE_AUDIO_PLAYER_H */

