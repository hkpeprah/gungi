// tower.hpp                                                          -*-C++-*-
#pragma once
//@DESCRIPTION:
//  This component provides a definition for a tower in Gungi.  A tower is a
//  set of units stacked upon one another.
//
//@CLASSES:
//  'gungi::Tower': class definition for a tower in 'Gungi'.
#include "gtypes.hpp"
#include "posn.hpp"
#include "unit.hpp"

#include <vector>

namespace gungi {

class Tower {
  // A 'gungi::Tower' represents a tower in Gungi.

public:
  // STATIC CLASS MEMBERS
  static const unsigned int k_MAX_HEIGHT = k_MAX_TOWER_SIZE;
    // Maximum height of the tower.

public:
  // ENUMERATIONS
  typedef enum dirty_bit_t {
    TIER_EXCHANGE = (1 << 0),
  } dirty_bit_t;

private:
  // INSTANCE MEMBERS
  Posn                m_posn;
                       // The position of this tower on the board.

  int                 m_bits;
                       // Bits for keeping track of effects that have been
                       // applied within the tower.

  SharedUnitPtrVector m_units;
                       // Vector of pointers to the 'Unit' objects in this
                       // tower.

public:
  // CREATORS
  Tower(void);
    // Default constructor.

  explicit Tower(const Posn& posn);
    // Creates a tower at the given position, 'posn'.

  ~Tower(void);
    // Default destructor.

  // MANIPULATORS
  void markDirty(const dirty_bit_t& bit);
    // Sets the given 'bit' in the Tower's effect tracking bitfield.

  void markClean(const dirty_bit_t& bit);
    // Resets the given 'bit' in the tower's effect tracking bitfield.

  void markClean(void);
    // Resets all bits in the tower's effect tracking bitfield.

  void add(Unit *unit, error_t& err);
    // Adds the given 'unit' to this tower.  Returns an error status code of
    // 'GUNGI_ERROR_NONE' to the given output parameter 'err' if the unit was
    // added successfully.  Returns an error status code of
    // 'GUNGI_ERROR_FULL_TOWER' to the given output parameter 'err' if the
    // tower is of height 'k_MAX_HEIGHT'.  Returns an error status of
    // 'GUNGI_ERROR_DUPLICATE' to the given output parameter 'err' if the
    // given 'unit' is already a member of the tower.

  void add(SharedUnitPtr& unit, error_t& err);
    // Adds the given 'unit' to this tower.  Returns an error status code of
    // 'GUNGI_ERROR_NONE' to the given output parameter 'err' if the unit was
    // added successfully.  Returns an error status code of
    // 'GUNGI_ERROR_FULL_TOWER' to the given output parameter 'err' if the
    // tower is of height 'k_MAX_HEIGHT'.  Returns an error status of
    // 'GUNGI_ERROR_DUPLICATE' to the given output parameter 'err' if the
    // given 'unit' is already a member of the tower.

  void remove(const Unit *unit, error_t& err);
    // Removes the given 'unit' from this tower.  Returns an error status code
    // of 'GUNGI_ERROR_NONE' to the given output parameter 'err' if the unit
    // was removed successfully.  Returns an error status code of
    // 'GUNGI_ERROR_NOT_A_MEMBER' if the given unit is not a member of this
    // tower.

  void reset(void);
    // Resets the tower.

  // ACCESSORS
  bool isDuplicate(const Unit *unit) const;
    // Returns 'true' if the given 'unit' is already a member of this tower as
    // determined by its colour and front identifier.

  bool isDirty(const dirty_bit_t& bit) const;
    // Returns 'true' if the given 'bit' is set in the Tower's underlying
    // effect tracking bitfield, otherwise 'false'.

  unsigned int height(void) const;
    // Returns the height of this tower.

  tier_t tier(const Unit *unit, error_t& err) const;
    // Returns the tier the given unit is on: an unsigned integer between
    // [0, k_MAX_HEIGHT - 1] inclusive.  Returns an error status code of
    // 'GUNGI_ERROR_NONE' to the given output parameter 'err' if the unit
    // is in the tower, otherwise 'GUNGI_ERROR_NOT_A_MEMBER'.

  Unit *at(tier_t tier, error_t& err);
    // Returns a constant reference to the unit at the given 'tier' in the
    // tower.  Returns an error status code of 'GUNGI_ERROR_NONE' to the given
    // output parameter 'err' if there is a unit at the given tier, otherwise
    // 'GUNGI_ERROR_OUT_OF_RANGE'.

  const Unit *at(tier_t tier, error_t& err) const;
    // Returns a constant reference to the unit at the given 'tier' in the
    // tower.  Returns an error status code of 'GUNGI_ERROR_NONE' to the given
    // output parameter 'err' if there is a unit at the given tier, otherwise
    // 'GUNGI_ERROR_OUT_OF_RANGE'.

  const Posn& posn(void) const;
    // Returns the position of this tower.

  SharedUnitPtrVector& members(void);
    // Returns a reference to the members of htis tower.

  const SharedUnitPtrVector& members(void) const;
    // Returns a constant reference to the members of this tower.
};

}  // close 'gungi' namespace
