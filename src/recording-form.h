/*
 * recording-form.h
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

#ifndef _RECORDING_FORM_H
#define _RECORDING_FORM_H

#include <tr1/memory>
#include "recording.h"
#include "repository.h"
#include "resource-edit-form.h"

namespace SC {

class RecordingForm : public ResourceEditForm {
public:
    RecordingForm(const std::tr1::shared_ptr<Recording>& recording,
                  const std::tr1::shared_ptr<Repository>& repository);
    virtual GomResource* resource();

private:
    struct Priv;
    std::tr1::shared_ptr<Priv> m_priv;
};
}

#endif /* _RECORDING_FORM_H */

