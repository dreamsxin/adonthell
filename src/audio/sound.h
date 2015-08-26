/*
   Copyright (C) 1999/2000/2001/2002   Alexandre Courbot <alexandrecourbot@linuxgames.com>
   Copyright (C) 2010                  Josh Glover       <jmglov@jmglov.net>
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
 * @file   audio/sound.h
 * @author Tyler Nielsen <tyler.nielsen@gmail.com>
 * @author Josh Glover   <jmglov@jmglov.net>
 *
 * @brief  The sound class for programs using audio.
 */

#ifndef AUDIO_SOUND_H_
#define AUDIO_SOUND_H_

#include <string>
#include <vector>

#include <adonthell/base/flat.h>
#include <adonthell/base/logging.h>

namespace audio {

    /**
     * Playback sound effects or background music.
     *
     */
    class sound
    {
    public:
        /**
         * Constuctors
         */
        sound () : m_filename(""), m_sample(NULL), m_channel(-1), m_forcedhalt(false)
        { }

        sound (const std::string &filename);

        /**
         * Get the filename associated with the sound
         *
         * @return \e filename
         */
        const std::string &getfilename(void) const {return m_filename;}

        /**
         * Play this sound
         *
         * @param loops number of times to repeat.  -1 means repeat forever
         * @return \e true if sound was successfully played
         */
        bool play (int loops = 0);

        /**
         * Fade the sound in
         *
         * @param sec time in seconds to spread the fade in over
         * @param loops number of times to repeat.  -1 means repeat forever
         * @return \e true if sound was successfully played
         */
        bool fadein(double sec, int loops = 0);

        /**
         * Set the position for the channel
         *
         * @param angle angle to position the sound 0=in front, -90/270=to left, 90=to right, 180=behind
         * @param distance relative distance to sound 0=close 1=far
         * @return \e true if position was successfully set
         */
        bool setposition(int angle, double distance);

        /**
         * Fade the channel out
         *
         * @param sec time in seconds to spread the fade in over
         */
        void fadeout(double sec);

        /**
         * Halt the sound
         */
        void halt(void);

        /**
         * Destructor: free all memory
         */
        virtual ~sound ();

#ifndef SWIG
        /**
         * Called by the library whenever a channel stops playing
         *
         * @param channel channel that stopped
         */
        static void handle_channel_stop(int channel);
#endif

        /**
         * Saves the basic %event %data (such as the type or script data)
         * to a stream.
         *
         * @param file stream where to save the %event.
         */
        void put_state (base::flat& file) const;

        /**
         * Loads the basic %event %date from a stream.
         *
         * @param file stream to load the %event from.
         * @return \e true if the %event could be loaded, \e false otherwise
         */
        bool get_state (base::flat& file);

    protected:
        /// the name of the file we loaded
        std::string m_filename; 
        /// the sound data
        void * m_sample;
        /// the channel the sound is playing on
        int m_channel; 
        /// Was this sound stopped by the user
        bool m_forcedhalt;

        //Functions from the audio backend
        static void *(*m_open)(const char *);
        static void (*m_close)(void *);
        static int (*m_play)(void *, int);
        static int (*m_fadein)(void *, double, int);
        static void (*m_halt)(int);
        static void (*m_fadeout)(int, double);
        static bool (*m_setpos)(int, int, double);

        /// This lets us know which channel is associated with which class.
        static std::vector<sound *> m_channels;

        bool handle_channel_create(void);

        friend bool init(const std::string &);

        /// Opens the sound file and sets up the object
        void open_file(void);

        /// Logs the internal state of the object
        void log_state(void) const;
    };
}

#endif
