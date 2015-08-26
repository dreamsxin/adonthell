/*
   Copyright (C) 2005/2006 Tyler Nielsen <tyler.nielsen@gmail.com>
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
 * @file   audio/audio.h
 * @author Tyler Nielsen <tyler.nielsen@gmail.com>
 *
 * @brief  Defines the audio module init and cleanup functions.
 *
 *
 */

#ifndef AUDIO_H_
#define AUDIO_H_

#include "sound.h"
#include <adonthell/base/logging.h>
#include <adonthell/base/configuration.h>

/**
 * This module gives an interface to handle sound output
 *
 */
namespace audio
{
    /**
     * Initialisation method. This function must be called
     * before any use of this class.
     *
     * @param backend_name The name of the backend to use.
     *
     * @return \e true if the initialization was successful,
     *         \e false otherwise.
     *
     */
    bool init(const std::string & backend_name);

    /**
     * Load audio settings from configuration file. This will
     * initialize settings like the volume, number of channels
     * and similar things.
     *
     * @param cfg the game configuration
     *
     */
    void setup (base::configuration & cfg);
        
    /**
     * Cleanup method. Call this function when you don't need
     * the audio module anymore.
     *
     */
    void cleanup();

#ifndef SWIG
    /**
     * Add pointer for the samples that have finished after the last call to update
     *
     * @param sample sound that has completed
     */
    void complete(sound *sample);
#endif

    /**
     * Update, create events for finished samples
     */
    void update(void);

}

#endif
