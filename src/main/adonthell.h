/*
   $Id: adonthell.h,v 1.3 2004/08/23 06:33:47 ksterker Exp $

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
 * @file   main/adonthell.h 
 * @author Kai Sterker <kaisterker@linuxgames.com>
 * 
 * @brief  The main application class for programs using the Adonthell framework.
 * 
 * 
 */

#ifndef ADONTHELL_APP_H
#define ADONTHELL_APP_H

#include "ltdl.h"
#include "base/configuration.h"

using std::string;

namespace adonthell {
    /**
     * Subclass adonthell::app to create the entry point for any application
     * using the Adonthell framework. The class you create must be named theApp
     * in order to be found by the linker. Override the main() method. It will
     * be called after all operating system dependend intialization has been
     * completed. You will find the command line args passed to your application
     * in the member variables argc and argv.
     */
    class app {
        public:
            /**
             * The different parts of the engine that can be initialized
             * seperately.
             */
            enum { GFX = 1, INPUT = 2, EVENT = 4, PYTHON = 8, RPG = 16, GUI = 32, AUDIO = 64 };
        
            /**
             * Destructor.
             */
            virtual ~app ();
            
            /**
             * This method is called after the engine has been initialized.
             * Overwrite it to add your game's main loop. Once this method
             * returns, the engine will quit.
             */
            virtual int main () = 0;
        
            /**
             * Call this to initialize selected subystems of the engine. These
             * will be shutdown automatically once the engine quits.
             * @param modules combination of GFX, INPUT, EVENT, PYTHON, RPG, GUI
             * @return true on success \b false otherwise.
             */
            bool init_modules (const u_int16 & modules);

/// allow main to call the following methods, but nobody else 
#ifndef SWIG
            /**
             * Called by the engine before main(). Used to do platform specific
             * initialization work. What needs to be done depends mainly on the
             * backend and OS used. For example, SDL on both MacOS X and Win32
             * needs to 'connect' to the windowing system before it can display
             * any windows or recieve input.
             *
             * This method does not initialize any subsystems of the Adonthell
             * engine itself. Do that in your main method using init_modules()
             * or the desired module's init function.
             *
             * @return \b true on success \b false otherwise.
             */
            bool init ();

            /**
             * Parse the given command line arguments for those required by
             * the engine. The following arguments are currently handled:
             * -b <backend> 	specifiy the backend to use (default 'sdl')
             * -c <config>		use given config file (default 'adonthell')
             * -g <directory> 	specify user game directory
             * -h				print help message and exit
             * -v 				print version number and exit
             *
             * @param argc argument count
             * @param argv argument vector
             */
            void parse_args (int & argc, char *argv[]);
            
            /**
             * Called automatically on shutdown of the Adonthell engine. Frees
             * any resources aquired by init(). Don't call it manually. 
             */
            void cleanup () const;
#endif // SWIG
            
        protected:
            /// Commandline argument count.
            int Argc;
            
            /// Commandline argument vector.
            char **Argv;

            /// Engine configuration
            base::configuration Cfg;
            
        private:
            /**
             * Print short usage message to cout.
             */
            void print_help () const;
        
            /// the backend we're using
            string Backend;
            
            /// the config file we're loading
            string Config;
            
            /// user supplied data directory
            string Userdatadir;
            
            /// game to launch
            string Game;
            
            /// modules currently loaded
            u_int16 Modules;
    };
}

#endif // ADONTHELL_APP_H
