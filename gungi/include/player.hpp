// player.hpp                                                         -*-C++-*-
#pragma once
//@DESCRIPTION:
//  This component defines the player class.  A player is represented uniquely
//  by their colour.
//
//@CLASSES:
//  'gungi::Player': class representing a player.
#include "gtypes.hpp"
#include "unit.hpp"

#include <vector>

namespace gungi {

class Player {
  // Class definition for a 'gungi::Player' object.  A player is one of the two
  // people facing off in a game of Gungi, and is defined uniquely by their
  // colour.

private:
  // INSTANCE MEMBERS
  colour_t                  m_colour;
                             // Colour of this player.

  SharedUnitPtr             m_commander;
                             // Constant ointer to the player's commander.

  SharedUnitPtrVector       m_units;
                             // Units belonging to this player.  These may be
                             // in the player's hand or on the board.

public:
  // CREATORS
  Player(colour_t colour);
    // Default constructor.  Creates an instance of a player with the given
    // 'colour'.

  ~Player(void);
    // Default destructor.

  // MANIPULATORS
  void reset(void);
    // Resets the player instance: clears its units vector.

  void addUnit(Unit *unit, error_t& error);
    // Adds the given 'unit' to this player's unit vector.  Returns
    // 'GUNGI_ERROR_NONE' to the given output parameter, 'error', on success,
    // otherwise 'GUNGI_ERROR_DUPLICATE' if the given 'unit' is already in the
    // the underlying vector.

  void addUnit(SharedUnitPtr& unit, error_t& error);
    // Adds the given 'unit' to this player's unit vector.  Returns
    // 'GUNGI_ERROR_NONE' to the given output parameter, 'error', on success,
    // otherwise 'GUNGI_ERROR_DUPLICATE' if the given 'unit' is already in the
    // the underlying vector.

  void removeUnit(Unit *unit, error_t& error);
    // Removes the given 'unit' from this player's unit vector.  Returns
    // 'GUNGI_ERROR_NONE' to the given output parameter, 'error', on success,
    // otherwise 'GUNGI_ERROR_NOT_A_MEMBER' if the given 'unit' is not a unit
    // belonging to this player.

  // ACCESSORS
  const SharedUnitPtr& commander(void) const;
    // Returns a constant pointer to the player's commander.

  const colour_t& colour(void) const;
    // Returns this player's colour.

  SharedUnitPtrVector activeUnits(void) const;
    // Returns a vector of the player's active units.

  SharedUnitPtrVector inactiveUnits(void) const;
    // Returns a vector of the player's inactive units.

  SharedUnitPtrVector& units(void);
    // Returns a vector of the units belonging to this player.

  const SharedUnitPtrVector& units(void) const;
    // Returns a vector of the units belonging to this player.
};

}  // close 'gungi' namespace
