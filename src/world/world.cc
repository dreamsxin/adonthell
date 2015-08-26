/*
 $Id: world.cc,v 1.1 2009/04/18 21:54:59 ksterker Exp $
 
 Copyright (C) 2009 Kai Sterker <kaisterker@linuxgames.com>
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
 * @file world/world.cc
 *
 * @author Kai Sterker
 * @brief Module initialization.
 */

#include "world.h"
#include "area_manager.h"
#include "move_event_manager.h"

#include <adonthell/base/savegame.h>

// move_event manager instance that is initialized when the world package is loaded
static world::move_event_manager *MoveEventManager;

// start world module
void world::init (base::configuration & cfg)
{
    MoveEventManager = new world::move_event_manager;

    base::savegame::add (new base::serializer<world::area_manager> ());
}

// shutdown world module
void world::cleanup ()
{
    area_manager::cleanup ();

    delete MoveEventManager;
    MoveEventManager = NULL;
}
