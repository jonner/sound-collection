/*
 * quality-widget.cc
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

#include "quality-widget.h"

namespace SC {

struct QualityWidget::Priv {
    int quality;
    typedef std::vector<std::tr1::shared_ptr<Gtk::Button> > ButtonVector;
    ButtonVector stars;
    sigc::signal<void> signal_changed;

    Priv(int q)
        : quality(q)
    {
        for (int i = 0; i < 5; ++i) {
            std::tr1::shared_ptr<Gtk::Button> button(new Gtk::Button());
            button->signal_clicked().connect(
                sigc::bind(sigc::mem_fun(this, &Priv::set_quality), i + 1));
            button->property_relief() = Gtk::RELIEF_NONE;
            stars.push_back(button);
        }

        update_stars();
    }

    void update_stars()
    {
        for (int i = stars.size(); i > 0; i--) {
            stars[i - 1]->set_image_from_icon_name((quality >= i) ? "starred" : "non-starred", Gtk::ICON_SIZE_MENU);
        }
    }

    void set_quality(int q)
    {
        quality = q;
        update_stars();
        signal_changed.emit();
    }
};

QualityWidget::QualityWidget(int quality)
    : Gtk::Box(Gtk::ORIENTATION_HORIZONTAL)
    , m_priv(new Priv(quality))
{
    for (Priv::ButtonVector::iterator it = m_priv->stars.begin();
         it != m_priv->stars.end();
         ++it) {
        (*it)->show();
        pack_start(**it, FALSE, FALSE, 5);
    }
}

void QualityWidget::set_quality(int quality)
{
    m_priv->set_quality(quality);
}

int QualityWidget::quality() const
{
    return m_priv->quality;
}

sigc::signal<void>& QualityWidget::signal_changed()
{
    return m_priv->signal_changed;
}
}
