/*
   $Id: surface.cc,v 1.1 2003/07/18 15:16:09 gnurou Exp $

   Copyright (C) 1999/2000/2001/2002/2003   Alexandre Courbot <alexandrecourbot@linuxgames.com>
   Part of the Adonthell Project http://adonthell.linuxgames.com

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/


/**
 * @file gfx/surface.cc
 *
 * @author Alexandre Courbot <alexandrecourbot@linuxgames.com>
 * 
 * @brief  Defines the surface global interface.
 */ 

#include "surface.h"
#include "gfx/pnm.h"

using namespace gfx;

surface::surface()
{
    alpha_ = 255;
    is_masked_ = false;
}

surface::~surface()
{
}

bool surface::get_pnm (std::ifstream & file)
{
    void *rawdata;
    u_int16 l, h; 

    rawdata = pnm::get (file, l, h);

    if (!rawdata) return false;

    clear (); 
    /**
     * @bug We don't take care of endianness here! Colors
     * won't be rendered correctly on big-endian machines.
     */
    set_data(rawdata, l, h, 3, 0x0000ff, 0x00ff00, 0xff0000);

    free (rawdata);

    return true;
}

bool surface::load_pnm (const std::string & fname)
{
    std::ifstream file(fname.c_str());
    bool ret = true;

    if (!file.is_open())
        return false;
    ret = get_pnm (file);
    file.close();
    return ret;
}

bool surface::put_pnm (std::ofstream & file) const
{
    /**
     * @bug We don't take care of endianness here!
     * 
     */
    void * rawdata = get_data(3, 0x0000ff, 0x00ff00,
                              0xff0000);

    if (!rawdata) return false;

    pnm::put (file, (const char *)rawdata, length (), height ()); 

    free(rawdata);

    return true;
}

bool surface::save_pnm (const std::string & fname) const
{
    std::ofstream file(fname.c_str());
    bool ret = true;

    if (!file.is_open())
        return false;
    ret = put_pnm (file);
    file.close();
    return true;
}
