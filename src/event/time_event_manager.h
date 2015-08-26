/*
   $Id: time_event_manager.h,v 1.8 2007/07/22 21:50:37 ksterker Exp $

   Copyright (C) 2002/2003/2004 Kai Sterker <kaisterker@linuxgames.com>
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
 * @file 	event/time_event_manager.h
 *
 * @author 	Kai Sterker
 * @brief 	Declares the time_event_manager class.
 */

#ifndef EVENT_TIME_EVENT_MANAGER_H
#define EVENT_TIME_EVENT_MANAGER_H

#include "manager_base.h"
#include <list>

namespace events
{
    /**
     * This class keeps track of time events, i.e. events that are raised
     * at a certain point in (%game) time. All registered events are 
     * sorted by the time they need to be raised, so that only one
     * comparison decides upon whether an %event is to be raised.
     */
    class time_event_manager : public manager_base
    {
    public:
		/**
		 * Create a new time_event_manager and register with the event handling
		 * system.
		 */
		time_event_manager ();
	
        /**
         * Register a time %listener with the %event manager. It is inserted
         * into the list of registered listeners depending on its "alarm"
         * time. The %listener needs to be removed before it can be safely
         * deleted.
         *
         * @param li Pointer to the %listener to be registered.
         * @todo use binary search to find position for insert.
         */
        void add (listener *li);
        
        /**
         * Removes the given %listener from the %event manager. Once it is
         * no longer needed, it can be freed.
         * 
         * @param li Pointer to the %listener to be removed.
         */
        void remove (listener *li);
        
        /**
         * Raise one or more events in case the given time matches their
         * "alarm" time. When they need to be repeated, they are
         * re-inserted into the %event-list.
         *
         * @param evnt An %event structure with the current %game time in 
         *      minutes.
         */
        void raise_event (const event *evnt);

    private:
        /// storage for registered listeners.
        std::list<listener*> Listeners;
    };
}

#endif // EVENT_TIME_EVENT_MANAGER_H
