/*
 Copyright (C) 2004/2005/2006/2010 Kai Sterker <kaisterker@linuxgames.com>
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
 * @file world/schedule.cc
 *
 * @author Kai Sterker
 * @brief Implements the character schedule class.
 */
 
#include "schedule.h"
#include <adonthell/event/date.h>
#include <adonthell/event/time_event.h>

using world::schedule;

// standart constructor
schedule::schedule ()
{
    Paused = 0;
    Running = false;
    QueuedSchedule = NULL;
    Owner = NULL;
}

// destructor
schedule::~schedule ()
{
    clear_schedule();
    delete QueuedSchedule;
}

// execute the schedule
void schedule::update ()
{
    // no schedule running --> assign a new one
    if (!Running) 
    {
        // if schedule queued, try to activate it 
        if (QueuedSchedule)
        {
            QueuedSchedule->activate (this);
            delete QueuedSchedule;
            QueuedSchedule = NULL;            
        }

        // no queued schedule or initialization failed
        if (!Running)
        {
            // clearing the schedule before the manager runs
            // allows the manager to access the most recent data
            clear_schedule ();
            
            // determine new schedule ...
            Manager.call_method ("run");
            
            // ... and start it
            Schedule.call_method ("start");
            
            // pause schedule, if required
            if (Paused > 0)
            {
                Schedule.call_method ("pause");
            }
        }
    }
}

// pause or resume schedule
void schedule::set_active (const bool & a)
{
    if (!a)
    {
        Paused++;
        if (Paused == 1)
        {
            Schedule.call_method ("pause");
        }
    }
    else if (Paused > 0)
    {
        Paused--;
        if (Paused == 0)
        {
            Schedule.call_method ("resume");
        }
    }
    else
    {
        LOG(WARNING) << "schedule " << Schedule.file_name() << "."
                     << Schedule.class_name() << " is active already!";
    }
}

// assign a (new) schedule
bool schedule::set_schedule (const string & file, PyObject *args)
{
    if (Running)
    {
        LOG(WARNING) << "stop current schedule first!";
        return false;
    }
    
    // pass schedule as first argument
    PyObject *new_args = add_schedule (args);

    // no need to clear anything, as py_object takes care of that
    if (Schedule.create_instance (SCHEDULE_DIR + file, file, new_args))
    {    
        // 'run' schedule
        Running = true;
    
        // cancel alarm
        Factory.clear ();
    
        Py_DECREF(new_args);
        return true;
    }

    Py_DECREF(new_args);
    return false;
}

// queue a schedule
void schedule::queue_schedule (const string & file, PyObject *args)
{
    if (QueuedSchedule) delete QueuedSchedule;

    QueuedSchedule = new schedule_data (file, args);
}

// assign a (new) manager script
bool schedule::set_manager (const string &file, PyObject *args)
{
    PyObject *new_args = add_schedule (args);
    return Manager.create_instance (SCHEDULE_DIR + file, file, new_args);
}

// get manager script, if initialized
const python::script *schedule::get_manager () const
{
    if (Manager.get_instance (false)) return &Manager;
    return NULL;
}

// set the alarm
void schedule::set_alarm (const string & time, const bool & absolute)
{
    string tm (time);
    
    // get rid of the current alarm
    Factory.clear ();
    
    // absolute hour of the day
    if (absolute)
    {
        char day[16];

        // if that hour has already passed today, assume the
        // same hour tomorrow
        if (events::date::parse_time (time) > (u_int32) events::date::hour () * 60)
            sprintf (day, "%id", events::date::day ());
        else
            sprintf (day, "%id", events::date::day () + 1);

        tm += day;
    }

    // create and register the new alarm
    events::time_event *ev = new events::time_event (tm, absolute);
    events::listener *li = Factory.add (ev, events::LISTENER_CXX);
    li->connect_callback (base::make_functor (*this, &schedule::on_alarm));
    li->set_id ("alarm");
}

// set alarm for a queued schedule
void schedule::queue_alarm (const string & time, const bool & absolute)
{
    if (!QueuedSchedule)
    {
        LOG(WARNING) << "queue_alarm: queue a schedule first!";
        return;
    }
    
    QueuedSchedule->set_alarm (time, absolute);
}

// add the schedule object to the python argument tuple
PyObject *schedule::add_schedule (PyObject* args) const
{
    // prepare callback arguments
    PyObject *new_args = python::pad_tuple (args, 1);
    if (new_args == NULL)
    {
        return args;
    }
    
    // first argument is the schedule itself
    PyTuple_SET_ITEM (new_args, 0, python::pass_instance (this));
    
    return new_args;
}

// save state to disk
void schedule::put_state (base::flat & file) const
{
    file.put_uint16 ("spa", Paused);
    file.put_bool ("scr", Running);
    
    // save manager script
    base::flat record;
    record.put_string ("script", Manager.class_name());
    python::put_tuple (Manager.get_args(), record, 1);
    file.put_flat ("mgr", record);
    
    // save schedule script, if any
    if (Schedule.get_instance() != NULL)
    {
        record.clear();
        record.put_string ("script", Schedule.class_name());
        python::put_tuple (Schedule.get_args(), record, 1);
        file.put_flat ("sdl", record);
    }

    // save queue, if any
    if (QueuedSchedule)
    {
        record.clear();
        QueuedSchedule->put_state (record);
        file.put_flat ("que", record);
    }
    
    // save alarm, if any
    Factory.put_state (file);
}

// load state from disk
bool schedule::get_state (base::flat & file)
{
    base::flat record;
    
    Paused = file.get_uint16 ("spa");
    Running = file.get_bool ("scr");

    // restore manager script
    
    record = file.get_flat ("mgr");
    std::string script = record.get_string ("script");
    PyObject *args = python::get_tuple (record, 1);
    PyTuple_SET_ITEM (args, 0, python::pass_instance (this));
    if (!Manager.create_instance (SCHEDULE_DIR + script, script, args))
    {
        LOG(ERROR) << "failed loading manager script '" << script << "'.";
        Py_DECREF(args);
        return false;
    }
    Py_DECREF(args);
    
    // restore schedule script, if any
    record = file.get_flat ("sdl", false);
    if (record.size() > 1)
    {
        script = record.get_string ("script");
        args = python::get_tuple (record, 1);
        PyTuple_SET_ITEM (args, 0, python::pass_instance (this));
        if (!Schedule.create_instance (SCHEDULE_DIR + script, script, args))
        {
            LOG(ERROR) << "failed loading schedule script '" << script << "'.";
            Py_DECREF(args);
            return false;
        }
        Py_DECREF(args);
        
        // restart schedule
        Schedule.call_method ("start");
        
        // pause schedule, if required
        if (Paused > 0)
        {
            Schedule.call_method ("pause");
        }        
    }
        
    // restore queue
    record = file.get_flat ("que", true);
    if (record.size() > 1)
    {
        QueuedSchedule = new schedule_data;
        if (!QueuedSchedule->get_state (record))
        {
            LOG(ERROR) << "failed loading queued schedule";
            return false;
        }
    }
    
    // restore alarm
    if (!Factory.get_state (file))
    {
        LOG(ERROR) << "failed loading alarm";
        return false;
    }
    
    // restore listener callback (which cannot be saved)
    events::listener *li = Factory.get_listener ("alarm");
    if (li != NULL)
    {
        li->connect_callback (base::make_functor (*this, &schedule::on_alarm));
    }
        
    return file.success ();
}
