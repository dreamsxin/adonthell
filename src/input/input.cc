/*
   Copyright (C) 2003  Alexandre Courbot <alexandrecourbot@linuxgames.com>
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
 * @file   input.cc
 * @author Alexandre Courbot <alexandrecourbot@linuxgames.com>
 * 
 * @brief  Defines the inpuy module init and cleanup functions.
 * 
 * 
 */

#include "input.h"

#include "ltdl.h"
#include <iostream>

#include <adonthell/base/logging.h>
#include <adonthell/base/paths.h>

/**
 * The handler of our library file.
 * 
 */
static lt_dlhandle dlhandle = 0;

/**
 * Virtual backend init function.
 * 
 */
static bool (*inputinit)() = 0;

/**
 * Virtual backend cleanup function.
 * 
 */
static void (*inputcleanup)() = 0;

namespace input
{
    bool init(const std::string & backend_name)
    {
        if (lt_dlinit()) 
        { 
            LOG(ERROR) << logging::indent() << lt_dlerror();
            LOG(ERROR) << logging::indent() << "Error initializing liblt!\n"; 
            return false;
        }

        dlhandle = base::get_module(std::string("/input/_") + backend_name);
    
        if (!dlhandle) goto bigerror;

        inputinit = (bool(*)()) lt_dlsym(dlhandle, "input_init");
        if (!inputinit)
        {
            LOG(ERROR) << logging::indent() << lt_dlerror();
            goto bigerror;
        }
    
        inputcleanup = (void(*)()) lt_dlsym(dlhandle, "input_cleanup");
        if (!inputcleanup)
        {
            LOG(ERROR) << logging::indent() << lt_dlerror();
            goto bigerror;
        }

        input::manager::textinput_p = (void(*)(bool)) lt_dlsym(dlhandle, "input_text");
        if (!input::manager::textinput_p)
        {
            LOG(ERROR) << logging::indent() << lt_dlerror();
            goto bigerror;
        }
        
        input::manager::update_p = (void(*)()) lt_dlsym(dlhandle, "input_manager_update");
        if (!input::manager::update_p)
        {
            LOG(ERROR) << logging::indent() << lt_dlerror();
            goto bigerror;
        }

        goto success;
    
    bigerror:
        if (dlhandle) lt_dlclose(dlhandle);
        lt_dlexit();
        return false;

    success:
        
        return inputinit();
    }

    // setup from configuration
    void setup (base::configuration & cfg)
    {
        control_event::map_controls (cfg);
    }

    // shutdown input
    void cleanup()
    {
        if (inputcleanup) inputcleanup();
        inputcleanup = NULL;

        if (dlhandle) lt_dlclose(dlhandle);
        lt_dlexit();
    }
}
