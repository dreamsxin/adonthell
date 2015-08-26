/*
   Copyright (C) 2000/2001/2002/2003/2004/2006/2011 Kai Sterker <kaisterker@linuxgames.com>
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
 * @file   event/factory.h
 * @author Kai Sterker <kaisterker@linuxgames.com>
 * 
 * @brief  Declares the %event %factory class.
 * 
 */
 

#ifndef EVENT_FACTORY_H
#define EVENT_FACTORY_H

#include "listener.h"
#include <vector>

using std::string;

namespace events
{
    /**
     * Base class for objects that want to register events. It keeps track of
     * all the events an object has registered with the event_handler and can
     * automatically unregister them when the object is deallocated.
     *
     * It also provides the functionality to load and save the states of 
     * events it has created.
     *
     * Objects making use of events should use the %event %factory instead of
     * managing events themselves.
     */ 
    class factory
    {
    public:
        /**
         * Constructor - creates an empty, unpaused %event %factory
         */
        factory ();
        
        /**
         * Destructor - unregisters and deletes all events owned by this list.
         */ 
        virtual ~factory ();
    
        /**
         * Unregisters and deletes all events owned by this list.
         */ 
        void clear ();
    
        /**
         * @name High level factory operations
         */
        //@{
        /**
         * Connect a Python callback for the given event.
         *
         * @param ev pointer to the %event to add.
         * @param callback The method to call.
         */
        void register_event (event *ev, PyObject *callback);

        /**
         * Connect a C++ callback for the given event.
         *
         * @param ev pointer to the %event to add.
         * @param callback The method to call.
         */
        void register_event (event *ev, base::functor_1<const events::event*> * callback);
        //@}

        /**
         * @name Low level factory operations
         */
        //@{
        /** 
         * Creates a %listener for the given %event. The %listener will be
         * automatically registered with the %event %manager. The factory
         * also keeps track of the %listener to take care of its deletion.
         * 
         * @param ev_co pointer to the %event to add.
         * @param type type of listener to create. Default is a listener with
         *  a python callback attached.
         * @return pointer to the newly created %listener. Do not free it!
         */
        listener *add (event* ev_co, int type = LISTENER_PYTHON);
    
        /**
         * Removes a %listener from the %factory. This is usually called when a
         * %listener is destroyed.
         *
         * @param li pointer to the %listener to remove.
         */
        void remove (listener* li);
        
        /**
         * Try to retrieve the %listener with given id from the factory.
         *
         * @return a pointer to the %listener, or \b NULL if it's not in the list.
         */
        listener *get_listener (const string & id);
        //@}
        
        /**
         * @name Pausing / Resuming execution
         */
        //@{
        /**
         * Disable any listeners created by this %event %factory. This will
         * effectively stop all actions of the %object the %event %factory
         * belongs to, e.g. a NPC.
         */
        void pause ();
        
        /**
         * Re-enable the listeners created by this %event %factory, thus
         * 'awaking' the %object to life again. 
         */
        void resume ();
        //@}
        
        /**
         * @name Loading / Saving
         */
        //@{
        /** 
         * Save the %event %factory to a stream.
         * 
         * @param file stream where to save the %event %factory.
         */ 
        void put_state (base::flat& file) const;
        
        /** 
         * Loads the %event %factory from a stream and registers all loaded listeners.
         * @warning Before the %event %factory can load an %event, it needs a
         *     a callback function that returns a new instance of that %event.
         * 
         * @param file stream to load the %event %factory from.
         * 
         * @return \e true if the %event %factory was loaded successfully, \e false
         *     otherwise.
         * @sa manager::register_event ()
         */
        bool get_state (base::flat& file);
        //@}
    
#ifndef SWIG
    protected:
        /**
         * List of listeners created by this factory.
         */ 
        mutable std::vector<listener*> Listeners;
    
    private:
        /**
         * Whether listerners created by this %factory are paused or not.
         */
        u_int16 Paused;
#endif // SWIG
    };
}    
#endif // EVENT_FACTORY_H
