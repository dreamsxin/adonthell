/*
 Copyright (C) 2008 Rian Shelley
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
 * @file   gui/layout.cc
 * @author Rian Shelley
 * @brief  Implements the layout class.
 */

#include "layout.h"
#include "ui_event.h"
#include <adonthell/event/manager.h>
#include <adonthell/base/logging.h>

using gui::layout;

typedef std::vector<gui::layoutchild> vector_layoutchild;

// select next child
bool layout::moveright()
{
    return select_child(NEXT);
}

// select next child
bool layout::movedown()
{
    return select_child(NEXT);
}

// select previous child
bool layout::moveleft()
{
    return select_child(PREV);
}

// select previous child
bool layout::moveup()
{
    return select_child(PREV);
}

bool layout::select_child(gui::layout::select_direction direction)
{
    // no previous child that could possibly receive the focus
    if (Children.size() < 2) return false;

    u_int32 old = Selected;
    do
    {
        if (direction == NEXT)
        {
            Selected = (Selected + 1) % Children.size();
        }
        else
        {
            Selected = Selected ? Selected - 1 : Children.size() - 1;
        }

        // find a child willing to accept selection
        if (Children[Selected].Child->focus())
        {
            Children[old].Child->unfocus();

            gui::ui_event evt (this, "layout_switch");
            events::manager::raise_event (&evt);

            return true;
        }
    }
    while (Selected != old);

    return false;
}

void layout::draw(const s_int16 & x, const s_int16 & y, const gfx::drawing_area * da, gfx::surface * target) const
{
    if (!Visible) return;

    // draw background
    Look->draw (x, y, da, target, decoration::BACKGROUND);
    
    // client area of the layout
    gfx::drawing_area client_area (x, y, length(), height());
    client_area.shrink (Look->border ());
    client_area.assign_drawing_area (da);
    
    vector_layoutchild::const_iterator i;
    u_int32 c = 0;
    
    for (i = Children.begin(); i != Children.end(); ++i, c++)
    {
        // indicate selection of widget, if it wants us to. 
        (*i).Child->enable_focus (c == Selected && Focused);
        
        // draw widget at its position
        (*i).Child->draw((*i).Pos.x() + x, (*i).Pos.y() + y, &client_area, target);
    }

    // draw border
    Look->draw (x, y, da, target, decoration::BORDER);
}

// key pressed
bool layout::keydown(input::keyboard_event&k)
{
    if (!Visible) return false;
    
    if (Children.size())
    {
        // see if the selected child wants it.
        if (Children[Selected].Child->keydown(k)) return true;

        // see if the user wants to switch which widget is selected
        switch (k.key())
        {
            case input::keyboard_event::LEFT_KEY:
            {
                if (!moveleft()) return false;
                LOG(INFO) << logging::indent() << "Selected is now '" << Selected << "'";
                break;
            }
            case input::keyboard_event::RIGHT_KEY: // fallthrogh
            case input::keyboard_event::TAB_KEY:
            {
                if (!moveright()) return false;
                LOG(INFO) << logging::indent() << "Selected is now '" << Selected << "'";
                break;
            }
            case input::keyboard_event::DOWN_KEY:
            {
                if (!movedown()) return false;
                LOG(INFO) << logging::indent() << "Selected is now '" << Selected << "'";
                break;
            }
            case input::keyboard_event::UP_KEY:
            {
                if (!moveup()) return false;
                LOG(INFO) << logging::indent() << "Selected is now '" << Selected << "'";
                break;
            }
            default: return false;
        }
        return true;
    }
    return false;
}

// key released
bool layout::keyup(input::keyboard_event&k)
{
    if (!Visible) return false;
    
    if (Children.size())
    {
        // see if the selected child wants it. 
        if (Children[Selected].Child->keyup(k))
            return true;
    }
    return false;
}

// text typed
bool layout::input(input::keyboard_event&k)
{
    if (!Visible) return false;
    
    if (Children.size())
    {
        // see if the selected child wants it.
        if (Children[Selected].Child->input(k))
            return true;
    }
    return false;
}

bool layout::focus()
{
    if (Visible && Children.size())
    {
        // find a child willing to accept being selected
        u_int32 old = Selected;
        do
        {
            if (Children[Selected].Child->focus())
            {
                Focused = true;
                return true;
            }
            Selected = (Selected + 1) % Children.size();
        }
        while (Selected != old); //it will eventually wrap to where we started
    }
    Focused = false;
    return false;
}

void layout::add_child(gui::widget & c, const s_int16 & x, const s_int16 & y) 
{
    gfx::drawing_area a (x, y, c.length(), c.height());
    u_int16 nl = length(), nw = height();
    
    // if the width is too small, change it to fit
    if ((ResizeMode & GROW_X) == GROW_X && x + c.length() > length()) nl = x + c.length();
    if ((ResizeMode & GROW_Y) == GROW_Y && y + c.height() > height()) nw = y + c.height();
    
    // update widget size, if required
    if (nl != length() || nw != height()) set_size (nl, nw);

    // add new child
    Children.push_back(layoutchild(&c, a));
}

void layout::remove_child(gui::widget & c)
{
    vector<layoutchild>::iterator i;
    for (i = Children.begin(); i != Children.end(); i++)
    {
        if (&c == (*i).Child)
        {
            Children.erase(i);

            // we are guaranteed that i != Children.end() here, so
            // safe to cast the math
            if (Selected == (u_int32) (i - Children.begin()))
            {
                //just deleted the selected item.
                //TODO: what happens if nobody takes focus?
                Selected = 0;
                focus();
            }

            resize (ResizeMode);
            break;
        }
    }
}

// get location of child
const gfx::drawing_area& layout::get_location (const gui::widget & c) const
{
	static gfx::drawing_area no_such_pos(-1, -1, 0, 0);

    vector<layoutchild>::const_iterator i;
    for (i = Children.begin(); i != Children.end(); i++)
    {
        if (&c == i->Child)
        {
        	return i->Pos;
        }
    }

    return no_such_pos;
}

// set input listener
void layout::set_listener (input::listener *lstnr) 
{
    if (Listener) 
    {
        input::manager::remove (Listener);
        delete Listener;
    }
    
    Listener = lstnr;
    
    if (Listener) 
    {
        Listener->connect_keyboard_function (::base::make_functor_ret(*this, &layout::on_keyboard_event));
        Listener->connect_joystick_function (::base::make_functor_ret(*this, &layout::on_joystick_event));
    }
}

// get input listener
input::listener * layout::get_listener () const 
{
    return Listener;
}

// handle keyboard events
bool layout::on_keyboard_event (input::keyboard_event * evt) 
{
    switch (evt->type())
    {
        case input::keyboard_event::KEY_PUSHED:
        {
            return keydown (*evt);
        }
        case input::keyboard_event::KEY_RELEASED:
        {
            return keyup (*evt);
        }
        case input::keyboard_event::TEXT_INPUT:
        {
            return input (*evt);
        }
    }
    
    return false;
} 

// handle joystick events
bool layout::on_joystick_event (input::joystick_event *evt)
{
	// is button assigned to action?
	input::control_event::button_type action = input::control_event::joystick_mapping(evt->button());
	if (action != input::control_event::NO_BUTTON)
	{
		// map into default key for this action ...
		input::keyboard_event ke (evt->type() == input::joystick_event::BUTTON_PUSHED ?
				input::keyboard_event::KEY_PUSHED :
				input::keyboard_event::KEY_RELEASED,
				input::control_event::default_key(action), "");

		// ... and raise matching keyboard event
		return on_keyboard_event (&ke);
	}

	return false;
}

// resize layout
void layout::resize (const gui::layout::resize_mode & mode)
{
	if (mode == NONE) return;
    u_int16 nl = 0, nh = 0;

    // calculate optimum widget size
    vector<layoutchild>::iterator i;
    for (i = Children.begin(); i != Children.end(); i++)
    {
        i->Pos.resize(i->Child->length(), i->Child->height());

        // if the width is too small, change it to fit
        if (i->Pos.x() + i->Pos.length() > nl) nl = i->Pos.x() + i->Pos.length();
        if (i->Pos.y() + i->Pos.height() > nh) nh = i->Pos.y() + i->Pos.height();
    }

    if ((mode & GROW_X) == 0) nl = length();
    if ((mode & GROW_Y) == 0) nh = height();

    // update widget size, if required
    if (nl != length() || nh != height()) set_size (nl, nh);
}

// FIXME: convert to Adonthell's mouse input API
#if 0
bool layout::mousedown(SDL_MouseButtonEvent &m)
{
    if (!Visible)
        return false;
    if (m.button == SDL_BUTTON_LEFT || m.button == SDL_BUTTON_MIDDLE)
    {
        //search for something that takes the coordinates
        typedef vector<layoutchild> vector_layoutchild;
        vector_layoutchild::iterator i;
        int c=0;
        for (i = Children.begin(); i != Children.end(); i++,c++)
        {
            //Selected child did they click on?
            if (m.x > (*i).pos.x && 
                m.y > (*i).pos.y &&
                m.x < (*i).pos.x + (*i).pos.w &&
                m.y < (*i).pos.y + (*i).pos.h)
            {
                bool ret = false;
                Sint16 ox, oy;
                ox = m.x;
                oy = m.y;
                m.x -= (*i).pos.x;
                m.y -= (*i).pos.y;
                if ((*i).c->mousedown(m)) 
                {
                    if (Selected != c)
                        Children[Selected].c->unfocus();
                    if((ret = (*i).c->focus()))
                        Selected = c;
                }
                m.x = ox;
                m.y = oy;
                mousestate[0] = true;
                return ret;
            }
        }
    }
    return false;
}
bool layout::mouseup(SDL_MouseButtonEvent &m)
{
    if (!Visible)
        return false;
    //don't process it unless we saw it go down
    if (m.button == SDL_BUTTON_LEFT && mousestate[0])
    {
        mousestate[0] = false;
        //deliver it to Selected. 
        bool ret = false;
        Sint16 ox, oy;
        ox = m.x;
        oy = m.y;
        m.x -= Children[Selected].pos.x;
        m.y -= Children[Selected].pos.y;
        Children[Selected].c->mouseup(m);
        m.x = ox;
        m.y = oy;
        return true;
    }
    return false;
}
#endif
