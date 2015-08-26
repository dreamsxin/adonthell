/*
   $Id: manager_base.h,v 1.6 2009/03/21 14:29:06 ksterker Exp $

   Copyright (C) 2000/2001/2002/2003/2004 Kai Sterker <kaisterker@linuxgames.com>
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
 * @file   event/manager_base.h
 * @author Kai Sterker <kaisterker@linuxgames.com>
 * 
 * @brief  Declares the base class for event managers.
 * 
 */

#ifndef EVENT_MANAGER_BASE_H
#define EVENT_MANAGER_BASE_H

#include "listener.h"
#include "types.h"

namespace events
{
    /**
     * This is the base class for actual event managers. It
     * keeps track of listeners, recieves triggered events
     * and executes scripts associated with those events
     */ 
    class manager_base
    {
    public:
		/** 
		 * Register this %event %manager with the %event dispatching system.
		 * @param create_event function returning a new event of the type
		 *	this event manager takes care of.
		 */
		manager_base (new_event create_event)
		{
			event *evt = create_event ();
			Name = evt->name();
            delete evt;
            
			event_type::register_type (Name, this, create_event);
		}

        /**
         * Destructor
         */
        virtual ~manager_base () 
		{
		    event_type::remove_type (Name);
		}
    
        /** 
         * Registers a %listener.
         * 
         * @param li pointer to the %event to register.
         */
        virtual void add (listener* li) = 0;
    
        /** 
         * Unregister a %listener.
         * 
         * @param li pointer to the %event to unregister.
         */
        virtual void remove (listener* li) = 0;
    
        /** 
         * Execute scripts of listeners waiting for given %events. 
         * 
         * @param ev %event to raise.
         */
        virtual void raise_event (const event* ev) = 0;
		
	private:
		/** Type name of events handled by this manager */
		std::string Name;
    };
}
#endif // EVENT_MANAGER_BASE_H
