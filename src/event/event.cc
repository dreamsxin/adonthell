/*
   $Id: event.cc,v 1.10 2009/04/25 13:17:49 ksterker Exp $

   Copyright (C) 2000/2001/2002/2003/2005 Kai Sterker <kaisterker@linuxgames.com>
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
 * @file	event/event.cc
 * @author 	Kai Sterker <kaisterker@linuxgames.com>
 * 
 * @brief  	Defines the base event class.
 */

#include <cstdio>
#include "event.h"
#include "types.h"
#include "date.h"
#include "time_event_manager.h"

#include <adonthell/base/savegame.h>

using events::event;
using events::event_type;

// time_event manager instance that is initialized when the event package is loaded
static events::time_event_manager *TimeEventManager = NULL;

void events::init(base::configuration & cfg)
{
    TimeEventManager = new events::time_event_manager;
    base::savegame::add (new base::serializer<events::date> ());
}

void events::cleanup()
{
    events::date::cleanup();

    delete TimeEventManager;
    TimeEventManager = NULL;
}

// constructor
event::event ()
{
    // repeat forever
    Repeat = -1;
    Type = 255;
}

u_int8 event::type ()
{
    if (Type != 255) return Type;
    return (Type = event_type::get_id (name ()));
}

// save the state of the script associated with the event
void event::put_state (base::flat & file) const
{
    file.put_string ("etp", name ());
    file.put_sint32 ("erp", Repeat);
}

// load the state of the script associated with the event 
bool event::get_state (base::flat & file) 
{
    // Note that 'Type' is already read by listener::get_state 
    // to determine what event subclass to instantiate
    Repeat = file.get_sint32 ("erp");
    
    return file.success ();
}
