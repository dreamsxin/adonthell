/*
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
 * @file   rpg/character.h
 * @author Kai Sterker <kaisterker@linuxgames.com>
 *
 * @brief  Base class for creatures, NPCs and Player characters.
 */

#ifndef RPG_CHARACTER_H
#define RPG_CHARACTER_H

#include "pathfinding_costs.h"
#include "faction.h"

namespace rpg
{

#ifndef SWIG
    /** The Python package containing %charcter templates. */
#   define CHARACTER_PACKAGE "characters."
#endif

    /** types of characters */
    enum char_type
    {
        PLAYER   = 0,
        NPC      = 1,
        CREATURE = 2
    };

#define MALE    0
#define FEMALE  1

    /**
     * The character base class, containing all rpg-related attributes of a character.
     */
    class character : public python::script
    {
        public:
            /**
             * Create a new character and add it to the global %character storage.
             * @param name the characters name
             * @param id unique id under which the character will be available in global %character storage.
             * @param type the characters type (PLAYER, NPC or CREATURE)
             * @param specie name of the race to which the character belongs
             */
            character (const std::string & name, const std::string & id, const rpg::char_type & type,
                       const std::string & specie);

            /**
             * Convenience method to instantiate a character template. Its usage is
             * preferred over script::create_instance ().
             * @param templ name of the item template without .py extension
             * @param args argument tuple to pass to the template's constructor
             * @return \b true if instantiated successful, \b false otherwise.
             */
            bool create_instance (const std::string & templ, PyObject * args = NULL)
            {
                if (python::script::create_instance (CHARACTER_PACKAGE + templ, templ, args))
                {
                    // add reference to character
                    set_attribute ("this", python::pass_instance (this));
                    return true;
                }

                return false;
            }

            /**
             * @name Member access
             */
            //@{
            /**
             * Return name of this %character
             * @return characters name.
             */
            std::string name () const
            {
                return Name;
            }

            /**
             * Return id of this %character. Unlike the name,
             * a characters id must be unique.
             * @return characters id.
             */
            std::string id () const
            {
                return Id;
            }
            
            /**
             * Get the type of this %character.
             * @return the character's type.
             */
            char_type type () const
            {
                return Type;
            }
            
            /**
             * Set the color to use for speech produced by that
             * character.
             * @param color the color associated with the character.
             */
            void set_color (const u_int32 & color)
            {
                Color = color;
            }

            /**
             * Get the color associated to that character.
             * @return the color to use for character speech.
             */
            u_int32 color () const
            {
                return Color;
            }

            /**
             * Return dialogue script of this character
             * @return dialogue script assigned to this character.
             */
            std::string dialogue () const
            {
                return Dialogue;
            }

            /**
             * Assign a new dialogue script to this character
             * @param dialogue the new dialogue.
             */
            void set_dialogue (const std::string & dialogue)
            {
                Dialogue = dialogue;
            }
            //@}

            /**
             * @name Character's factions access
             */
            //@{
            /**
             * Add faction to this character's list
             * @param name name of the faction to be added
             * @return \b true on success, \b false otherwise
             */
             bool add_faction(const std::string & name);
            
            /**
             * Remove faction from this character's list
             * @param name name of the faction to be removed
             * @return \b true on success, \b false otherwise
             */
            bool remove_faction(const std::string & name);

            /**
             * Check if character belongs to given faction.
             * @param name the name of the faction to check.
             * @return true if the character belongs to the faction,
             * false otherwise.
             */
            bool belongs_to_faction(const std::string & name);

            /**
             * Calls each faction's estimate_speed and sums the values
             * This function is probably going to be called from calc_speed on the
             * python side
             * @param terrain the type of terrain this character is over
             * @return sum of all the values
             */
            s_int32 get_faction_estimate_speed(const std::string & terrain) const;
            //@}
           
            /**
             * @name Character speed
             */
            //@{
            /**
             * Set speed of the character.
             * @param speed this %character's speed.
             */
            void set_speed (float speed)
            {
                Speed = speed;
            }

            /**
             * Set base speed of the character.
             * @param base_speed this %character's base speed.
             */
            void set_base_speed (float base_speed)
            {
                Base_Speed = base_speed;
            }
            
            /**
             * Get the cost of walking over the terrain, taking into account the actual
             * pathfinding type, for pathfinding purposes.
             * @param terrain the terrain that's being walked over
             * @return the cost
             */
            s_int16 get_pathfinding_cost(const std::string & terrain) const
            {
                return PathfindingCosts.get_cost(terrain);
            }

            /**
             * Set the actual pathfinding type
             * @param type the new type
             */
            void set_pathfinding_type(const std::string & type)
            {
                PathfindingCosts.set_pathfinding_type(type);
            }

            /**
             * Get the actual pathfinding type
             * @return the actual pathfinding type
             */
            std::string get_pathfinding_type() const
            {
                return PathfindingCosts.get_pathfinding_type();
            }

            /**
             * Returns if the actual pathfinding type has forced impassable
             */
            bool has_forced_impassable() const
            {
                return PathfindingCosts.has_forced_impassable();
            }

            /**
             * Return speed of the character.
             * @return speed of this %character
             */
            float speed() const
            {
                return Speed;
            }

            /**
             * Return base speed of the character.
             * @return base speed this %character.
             */
            float base_speed() const
            {
                return Base_Speed;
            }

            /**
             * Updates this %character's speed based on the current terrain.
             * @param terrain a string with the type of terrain this character is over
             */
            void update_speed(const std::string & terrain);
            //@}

            /**
             * @name Global character access
             */
            //@{
            /**
             * Return a character from the list of characters by its unique id.
             * @param id unique id of character to return.
             * @return character with given id or NULL if no such character exists.
             */
            static character *get_character (const std::string & id);

            /**
             * Return the main character, usually controlled by the player.
             * @return the player character.
             */
            static character *get_player ()
            {
                return get_character (PlayerCharacterId);
            }
#ifndef SWIG
            /**
             * Get iterator pointing to first loaded %character.
             * @return iterator into the %character list.
             */
            static std::hash_map<std::string, rpg::character*>::const_iterator get_first()
            {
                return Characters.begin();
            }
            
            /**
             * Check whether the given iterator points to the last %character.
             * @param i iterator into the %character list.
             * @return true if i points to the end of the list.
             */
            static bool is_last (std::hash_map<std::string, rpg::character*>::const_iterator & i)
            {
                return i == Characters.end();
            }
#endif
            //@}

            /**
             * @name Loading/Saving
             */
            //@{
            /**
             * Load %character data of all characters. This will first load the %character
             * template to instantiate the underlying Python character class. Then it will
             * restore the actual %character data. If the character is already instantiated,
             * it will be replaced.
             *
             * @return \b true if loading successful, \b false otherwise.
             */
            static bool load ();

            /**
             * Save all existing %characters to given directory. This will save 
             * both the %character template plus the actual data to a file called
             * 'character.data'. The file will be replaced if it already exists.
             *
             * @param path name of the directory to save %character data to.
             * @return \b true if saving successful, \b false otherwise.
             */
            static bool save (const string & path);

            /**
             * Deletes all objects in the internal Character map
             */
            static void cleanup();

            /**
             * Loads the %character from a stream.
             * @param in stream to load the %character from.
             * @return \b true if loading successful, \b false otherwise.
             */
            bool get_state (base::flat & file);

            /**
             * Save the %character to a stream.
             * @param out stream where to save the %character.
             * @return \b true if loading successful, \b false otherwise.
             */
            bool put_state (base::flat & file) const;
            //@}

#ifndef SWIG
            GET_TYPE_NAME(rpg::character);
#endif // SWIG
            
        protected:
            /**
             * Create a character instance when loading from file.
             */
            character () {}
            /**
             * Delete character.
             */
            virtual ~character ();
            
        private:
            /// Type of the character
            char_type Type;
                
            /// Name of the character, not neccessarily unique
            std::string Name;

            /// Unique identifier for this character
            std::string Id;

            /// Color used for the character in dialogues
            u_int32 Color;

            /// Name of a sprite displayed during dialogues
            std::string Portrait;

            /// Dialogue script assigned to this character
            std::string Dialogue;

            /// Vector with the various factions to which this character belongs
            std::vector<rpg::faction *> Factions;
            
            /// Pathfinding Costs of this %character
            rpg::pathfinding_costs PathfindingCosts;

            /// Actual speed of this character
            float Speed;

            /// Default speed of this character
            float Base_Speed;

            /// list of all characters currently instantiated
            static std::hash_map<std::string, character*> Characters;

            /// id of the character currently controlled by the player
            static std::string PlayerCharacterId;
    };
}
#endif // RPG_CHARACTER_H
