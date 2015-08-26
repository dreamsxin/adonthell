/*
 Copyright (C) 2008 Kai Sterker <kaisterker@linuxgames.com>
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
 * @file   world/entity.h
 * @author Kai Sterker <kaisterker@linuxgames.com>
 * 
 * @brief  Declares the entity class.
 */

#include "placeable.h"
#include "action.h"

#ifndef WORLD_ENTITY_H
#define WORLD_ENTITY_H

namespace world
{

/**
 * An anonymous entity on the map. Such entities can be
 * placed on the map, but there is no way to get at them
 * later. This is okay for most static scenery objects,
 * however.
 */
class entity
{
public:
    /**
     * Create an anonymous entity.
     * @param object representation of the entity.
     */
    entity (placeable *object) : Object (object), Action (NULL) { }

    /**
     * Delete entity and its associated object.
     */
    virtual ~entity ()
    {
        delete Object;
        delete Action;
    }
    
    /**
     * Return associated object.
     * @return associated object. 
     */
    placeable *get_object () const
    {
        return Object;
    }
    
    /** 
     * Attach an action to the entity.
     * @param hash a unique, internal id.
     * @return the newly attached action.
     */
    action *set_action (const std::string & hash)
    {
        if (Action == NULL)
        {
            Action = new world::action(hash);
        }

        return Action;
    }
    
    /**
     * Return associated action.
     * @return associated action.
     */
    action *get_action () const
    {
        return Action;
    }
    
    /**
     * Check whether this is a named entity.
     * @return true if this is the case, false otherwise.
     */
    bool has_name () const 
    { 
        return id () != NULL; 
    }
    
    /**
     * Return whether the %object representing this entity is
     * unique or not.
     * @return always true for anonymous entities.
     */
    virtual bool is_unique () const
    {
        return true;
    }

    /**
     * Get id of the entity.
     * @return NULL if its an anonymous entity, the unique entity id otherwise.
     */
    virtual const std::string * id () const 
    { 
        return NULL; 
    }
    
protected:
    /// an entity on a map
    placeable *Object;
    /// action associated to the entity
    action *Action;
};

/**
 * A uniquely identified entity. This may correspond to 
 * a unique object on the map (such as an individual 
 * character or door). It is also conceivable that the
 * same object is added as distinct instances multiple
 * times (i.e. items that will have the same appearence
 * on the map, but are backed by different rpg instances). 
 */
class named_entity : public entity
{
public:
    /**
     * Create an (uniquely) named entity.
     * @param object representation of the entity.
     * @param id name of the object.
     * @param unique set to false if the same object is already contained in a different entity.
     */
    named_entity (placeable *object, const std::string & id, const bool & unique = true) 
        : entity (object), Id (id), IsUnique (unique) { }
    
    /**
     * Destructor.
     */
    virtual ~named_entity ()
    {
        // make sure the object is not deleted, if it is not unique
        if (!IsUnique) Object = NULL;
    }
    
    /**
     * Return whether the %object representing this entity is
     * unique or not.
     * @return true if %object is unique, false otherwise.
     */
    bool is_unique () const
    {
        return IsUnique;
    }
    
    /**
     * Get Id of this entity.
     * @return Unique identifier for the contained object.
     */
    const std::string * id () const
    {
        return &Id;
    }
    
private:
    /// name of entity
    std::string Id;
    /// entity with same object exists elsewhere?
    bool IsUnique;
};

}

#endif // WORLD_ENTITY_H
