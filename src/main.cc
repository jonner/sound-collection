/*******************************************************************************
 *
 *  Copyright (c) 2014 Jonathon Jongsma
 *
 *  This file is part of SoundCollection
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, see <http://www.gnu.org/licenses/>
 *
 *******************************************************************************/

#include <gom/gom.h>
#include "sound-collection-app.h"

int main(int argc, char** argv)
{
    SoundCollectionApp app;
    return app.run(argc, argv);
}
