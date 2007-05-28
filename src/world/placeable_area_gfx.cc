/*
 $Id: placeable_area_gfx.cc,v 1.2 2007/05/21 04:44:12 ksterker Exp $
 
 Copyright (C) 2002 Alexandre Courbot <alexandrecourbot@linuxgames.com>
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
 * @file   world/placeable_area_gfx.cc
 * @author Alexandre Courbot <alexandrecourbot@linuxgames.com>
 * 
 * @brief  Defines the placeable_area_gfx class.
 * 
 * 
 */


#include "world/placeable_area_gfx.h"

using world::placeable_area_gfx;

placeable_area_gfx::placeable_area_gfx() : animation ()
{
}

placeable_area_gfx::~placeable_area_gfx()
{
}

// set size
void placeable_area_gfx::set_area_size(u_int16 nx, u_int16 ny)
{
    drawable::set_length (nx * SQUARE_SIZE); 
    drawable::set_height (ny * SQUARE_SIZE); 
}