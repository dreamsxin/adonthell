#include <iostream>
#include "gfx/gfx.h"
#include "input/input.h"
#include "main/adonthell.h"
#include "gui/gui.h"

class Exit {
public:
  
  bool letsexit ;
  
  Exit () : letsexit (false) {};
  
  bool handle_keys (input::keyboard_event *ev) {
    // On escape, the global variable letsexit will be set to 1
    if (ev->key() == input::keyboard_event::ESCAPE_KEY) {
      letsexit = true;
      std::cout << "Escape pressed, leaving..." << std::endl;
      return true;
    }
    return false;
  }
};


class GuiTest : public adonthell::app {

  int main () {
    
    Exit myExit;
    
    // Initialize the gfx and input systems
    init_modules (GFX | INPUT);
    
    // Set us a nice window
    gfx::screen::set_video_mode(640, 480);
    
    // Create our input_listener and connect the callback
    // to handle keyboard events
    input::listener il;
    il.connect_keyboard_function(base::make_functor_ret(myExit, &Exit::handle_keys));
    input::manager::add (&il);
    
    /*************************************************************************/
    /*************************************************************************/
    /*************************************************************************/
    /*************************************************************************/
    /*************************************************************************/
    /*************************************************************************/
    gui::container cont;
    cont.setSize (150, 150);
    cont.setLocation (20, 30);
    
    for (unsigned int i = 0; i < 150; ++i) {
      gui::base * a = new gui::base ();
      a->setSize (40, 20);
      a->setLocation (30, 10);
      cont.addChild (a);
    }
    
    gui::listlayout * layout = new gui::listlayout;
    cont.setLayout (layout);
    
    /*************************************************************************/
    /*************************************************************************/
    /*************************************************************************/
    /*************************************************************************/
    /*************************************************************************/
    /*************************************************************************/

    // Run this loop until letsexit is set to 1 by the
    // callback function. Every time a key event is raised,
    // the input manager will send it to the listeners it handles
    // (that is, only the one we've connected) which will call the
    // callback function that has been connected to handle keyboard events.
    while (!myExit.letsexit) {
      ::gfx::screen::update ();
      ::input::manager::update();
      cont.draw ();
    }
    
    // Do some cleanup, and we're ready to exit!
    cont.destroyAll ();

    return 0;
  }
} theApp;
