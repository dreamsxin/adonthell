/*
   $Id: audio_event.cc,v 1.3 2009/04/08 21:52:09 ksterker Exp $

   Copyright (C) 2005 Tyler Nielsen <tyler.nielsen@gmail.com>
   Part of the Adonthell Project http://adonthell.linuxgames.com

   Adonthell is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   Adonthell is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Adonthell; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/**
 * @file audio/audio_event.cc
 * @author Tyler Nielsen <tyler.nielsen@gmail.com>
 *
 * @brief Implements the audio_event class.
 */

#include "audio/audio_event.h"

using audio::audio_event;

// Save time event to file
void audio_event::put_state (base::flat & file) const
{
    //TODO Fill this in...
}

// load time event from file
bool audio_event::get_state (base::flat & file)
{
    //TODO Fill this in...
    return file.success ();
}
