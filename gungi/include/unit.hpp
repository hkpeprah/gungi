// unit.hpp                                                           -*-C++-*-
#pragma once
//@DESCRIPTION:
//  This component provides a definition for a unit in Gungi.  A unit is a
//  piece on the Gungi board (e.g. 'Commander' or 'Samurai').
//
//@CLASSES:
//  'gungi::Unit': class definition for a unit in 'Gungi'.
#include "builder.hpp"
#include "gtypes.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace gungi {

// FORWARD DECLARATION
class Tower;

class Unit {
  // A 'gungi::Unit' represents a unit or piece in Gungi, defined by both
  // their colour and identifier.

private:
  // INSTANCE MEMBERS
  const Builder&       m_builder;
                        // Builder used to construct units and provide access
                        // to shared resources.

  piece_id_t           m_identifier;
                        // The identifier for this unit.  Specifies which kind
                        // of unit this is.

  piece_id_t           m_back;
                        // The identifier which specifies which unit this unit
                        // becomes when it is flipped to its backside.

  colour_t             m_colour;
                        // The colour of this unit.

  effect_bitfield_t    m_effectBitField;
                        // Bitfield which specifies what kind of effects this
                        // unit has.

  effect_bitfield_t    m_immuneBitField;
                        // Bitfield of the effects this unit is immune to.

  const_moveset_ptr_t  m_movesPtr;
                        // Pointer to a set of moves this unit can make.

  Tower const         *m_tower;
                        // Pointer to the tower this unit belongs to.

private:
  // PRIVATE MANIPULATORS
  void reset(void);
    // Resets the piece by loading the effects, immunities, and moves for the
    // piece identified by the 'm_identifier' member.

public:
  // CREATORS
  explicit Unit(piece_id_t      front,
                piece_id_t      back,
                colour_t        colour,
                const Builder&  builder);
    // Creates an instance of a 'Unit' initializing it to correspond to the
    // unit defined by the given 'front' identifier, with the given 'back'
    // identifier as the back piece with the given 'colour'.  Uses the given
    // 'builder' to access any shared resources.  Note that if the given
    // 'front' piece is an invalid identifier, this instance will be marked
    // invalid, and a subsequent call to 'isValid()' will return 'false'.

  ~Unit(void);
    // Default destructor.

  // MANIPULATORS
  void flip(error_t& err);
    // Flips this unit to its back piece, re-populating it using the underlying
    // builder.  Returns a status of 'GUNGI_ERROR_NONE' in the given output
    // parameter 'err', if the flip succeeds.  Returns a status of
    // 'GUNGI_ERROR_NO_BACK' if the unit does not have a back side.

  void setColour(colour_t colour);
    // Sets the colour of this unit to the given 'colour'.

  void setTower(Tower const *towerPtr);
    // Sets the tower of this unit to the given 'towerPtr' pointer.

  void clearTower(void);
    // Clears the tower of this unit.

  void addEffect(effect_t effect);
    // Adds the given 'effect' to this unit.

  void removeEffect(effect_t effect);
    // Removes the given 'effect' from this unit.

  void addImmunity(effect_t effect);
    // Adds the given 'effect' to this unit as an immunity.

  void removeImmunity(effect_t effect);
    // Removes the given 'effect' as an immunity this unit has.

  // ACCESSORS
  bool isValid(void) const;
    // Returns 'true' if this is a valid 'Unit', otherwise 'false'.

  bool isActive(void) const;
    // Returns 'true' if this unit is active, otherwise 'false'.  This is
    // equivalent to 'tower() != NULL'.

  const piece_id_t& id(void) const;
    // Returns the identifier of this unit.

  const piece_id_t& front(void) const;
    // Returns the identifier of the front piece of this unit.

  const piece_id_t& back(void) const;
    // Returns the identifier of the back piece of this unit.

  const colour_t& colour(void) const;
    // Returns the colour of this unit.

  const_moveset_ptr_t moveset(void) const;
    // Returns a constant reference to the moveset for this unit.

  Tower const *tower(void) const;
    // Returns a constant pointer to the tower this unit belongs to.  This
    // value is 'NULL' if this unit is not in a tower.

  effect_bitfield_t effectField(void) const;
    // Returns the bitfield of the effects this unit has.

  effect_bitfield_t immunityField(void) const;
    // Returns the bitfield of effects this unit is immune to.

  std::string code(void) const;
    // Returns the unit code.

  // OPERATORS
  friend std::ostream& operator<<(std::ostream& os, const Unit& unit);
    // Writes the given 'unit' out to the given output stream, 'os', and
    // returns the modified stream.
};

typedef std::shared_ptr<Unit> SharedUnitPtr;

typedef std::vector<SharedUnitPtr> SharedUnitPtrVector;

}  // close 'gungi' namespace
