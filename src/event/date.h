/*
   $Id: date.h,v 1.4 2004/04/29 08:07:49 ksterker Exp $

   Copyright (C) 2002 Kai Sterker <kaisterker@linuxgames.com>
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
 * @file event/date.h
 *
 * @author Kai Sterker
 * @brief Keeps track of time within the game.
 */

#ifndef EVENT_DATE_H
#define EVENT_DATE_H

#include "base/file.h"

#ifndef SWIG
/**
 * The number of hours that make one gameworld day.
 */
#define HOURS_PER_DAY 24

/**
 * The number of days that make one gameworld week.
 */
#define DAYS_PER_WEEK 7
#endif // SWIG

namespace event 
{
/**
 * Keeps track of the time the player spent within the game so far. This
 * time span is given in %game time minutes, not real time.
 * %date further includes functions to retrieve those minutes as day,
 * weekday, hour and minute values.
 */
class date
{
public:

    /**
     * Update the %game date. Whenever a minute of %gametime has 
     * passed, a time event will be raised.  
     */        
    static void update ();

    /**
     * Get the current %gametime.
     * @return %gametime in seconds since start of the game.
     */
    static u_int32 time ()
    { 
        return Time; 
    }

    /**
     * Get the current weekday.
     * @return weekday as a number between 0 and DAYS_PER_WEEK - 1
     */
    static u_int16 weekday ();
    /**
     * Returns the current day in the gameworld.
     * @return number of days spent in the gameworld, beginning with day 0.
     */
    static u_int16 day ();
    /**
     * Return the hour of the current day.
     * @return hour of the current day between 0 and HOURS_PER_DAY - 1
     */
    static u_int16 hour ();
    /**
     * Return the minute of the current hour.
     * @return minute of the current hour between 0 and 59.
     */
    static u_int16 minute ();

    /**
     * convert the time string to gametime minutes. The time string
     * has the format "<number>X", where X may be (w)eek, (d)ay,
     * (h)our, (m)inute or (t)enth minute. Several such pairs can be
     * concatenated.
     * Valid examples are "1w1d1h", "30m1h" but also "1h1h".
     *
     * @param time The time format string.
     * @return The time represented by the string in minutes.
     */
    static u_int32 parse_time (const std::string & time);

    /**
     * Load the state of the %gamedate class from disk
     * @param in stream to read the state from
     * @return \b true if the state was successfully retrieved,
     *      \b false otherwise.
     */
    static bool get_state (base::igzstream & in);
    /**
     * Save the state of the %gamedate class to disk
     * @param out stream to write the state to
     */
    static void put_state (base::ogzstream & out);
    
private:
#ifndef SWIG
    // number of game time seconds before a time event will be raised
    static u_int16 Resolution;

    // Time spent in the game in 1/10 gametime minutes
    static u_int32 Time;

    // number of game cycles since the last 1/10 gametime minute passed
    static double Ticks;
    
    // how many game cycles make one second of game time
    static float Scale;
#endif // SWIG
};

#endif // EVENT_DATE_H

}
