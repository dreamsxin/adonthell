/*
   $Id: quest_event.cc,v 1.1 2005/08/14 16:52:55 ksterker Exp $

   Copyright (C) 2005 Kai Sterker <kaisterker@linuxgames.com>
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
 * @file 	rpg/quest_event.cc
 *
 * @author 	Kai Sterker
 * @brief 	Declares the quest_event class.
 */
 
#include "rpg/quest_event.h"

using rpg::quest_event;

// constructor
quest_event::quest_event (const std::string & pattern)
{
	u_int32 idx, pos = 0;
	
    // split pattern into its levels
    while ((idx = pattern.find (".", pos)) != pattern.npos)
    {
        Pattern.push_back (pattern.substr (pos, idx - pos));
        pos = idx + 1;
    }
    
    // add last part
    Pattern.push_back (pattern.substr (pos));
 }

// test two quest events for equality
bool quest_event::equals (const events::event * e)
{
	quest_event *qevt = (quest_event *) e;
	std::vector<std::string>::const_iterator i = qevt->begin();
	std::vector<std::string>::const_iterator j = Pattern.begin();
	
	for (; j != Pattern.end() && i != qevt->end(); i++, j++)
	{
		fprintf (stdout, "%s == %s\n", (*i).c_str(), (*j).c_str());
	
		// '>' matches rest of pattern
		if ((*i)[0] == '>' || (*j)[0] == '>')
		{
			return true;
		}
		
		// '*' matches one level
		if ((*i)[0] == '*' || (*j)[0] == '*')
		{
			continue;
		}
		
		// otherwise, levels need to be exactly the same
		if ((*i) != (*j))
		{
			return false;
		}
	}
	
	// both pattern matched, but we must still make sure that
	// they have the same size
	return Pattern.size () == qevt->size ();
}

// save quest event
void quest_event::put_state (base::flat& out) const
{
}

// load quest event
bool quest_event::get_state (base::flat& in)
{
	return true;
}

