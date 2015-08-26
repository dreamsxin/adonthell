/*
   $Id: linux.cc,v 1.4 2007/05/28 22:28:37 ksterker Exp $

   Copyright (C) 2003 Kai Sterker <kaisterker@linuxgames.com>
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
 * @file   main/sdl/linux.cc 
 * @author Kai Sterker <kaisterker@linuxgames.com>
 * 
 * @brief  SDL initialization for Linux target. Nothing to be done ...
 * 
 * 
 */

#include "../adonthell.h"
#include "SDL.h"

#ifdef USE_LIBTOOL
/* exported names for libltdl */
#define main_init _sdl_LTX_main_init
#endif

extern "C" {

/* Init */
int main_init (const adonthell::app *theApp)
{
    // no need to initialize anything, so just start the application ...
    int retval = ((adonthell::app *) theApp)->main ();

    // properly stop SDL
    SDL_Quit();

    return retval;
}

}
