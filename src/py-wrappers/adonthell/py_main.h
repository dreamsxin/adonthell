/*
   $Id: py_main.h,v 1.2 2004/08/23 06:33:47 ksterker Exp $

   Copyright (C) 2003/2004 Kai Sterker <kaisterker@linuxgames.com>
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
 * @file   py_wrappers/adonthell/py_main.h 
 * @author Kai Sterker <kaisterker@linuxgames.com>
 * 
 * @brief  The main application class for python scripts using the Adonthell framework.
 */

#ifndef PY_ADONTHELL_MAIN_H
#define PY_ADONTHELL_MAIN_H

#include "main/adonthell.h"
#include "python/callback.h"

/**
 * To use parts of the Adonthell framework from python scripts, some initialization
 * is required, depending on the backend/OS combination. To make that transparent,
 * the following implementation of the 'main class' is provided. It can be used in the
 * following way:
 *
 * \code
 * from adonthell import main
 *
 * class myApp:
 *    # -- ctor
 *    def __init__ (self, app):
 *        self.App = app ()
 *        
 *    # -- your application entry point
 *    def main (self):
 *        self.App.init_modules (self.App.GFX | ...)
 *        ...
 *
 * if __name__ == 'main':
 *      adonthellApp = main.AdonthellApp ()
 *      app = myApp (adonthellApp)
 *      adonthellApp.init (app.main)
 * \endcode
 *
 * This will ensure that your python scripts run on any platform supported by the
 * Adonthell framework.
 * 
 * \note code after \tt myApp.init will not be executed.
 */
class AdonthellApp : public adonthell::app 
{
public:
    /**
     * Create application.
     */
    AdonthellApp ();
    
    /**
     * Destroy application.
     */
    virtual ~AdonthellApp ();

    /**
     * The method is called after initialization is complete. Will in turn
     * call the python method passed to the init method. You should not
     * have to call that method yourself.
     *
     * @return value returned by the python method.
     */
    int main ();

    /**
     * This method needs to be called to init the Adonthell framework.
     * @param main_func Python method called after initialization.
     * @param parse_args whether to check arguments passed to script
     */
    void init (PyObject *main_func, const bool & parse_args = true);

private:
    /// python method called after initialization
    python::functor_0ret<int> *MainFunc;
};

#endif
