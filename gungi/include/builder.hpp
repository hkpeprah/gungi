// builder.hpp                                                        -*-C++-*-
#pragma once
//@DESCRIPTION:
//  This component provides a definition for a resource manager / unit buildewr
//  used to provide shared access to resources across the application, as well
//  as build the game units.
//
//@CLASSES:
//  'gungi::Builder': class definition for a builder.
#include "gtypes.hpp"

namespace gungi {

class Builder {
  // A 'gungi::Builder' is a resource manager to provide access to shared
  // resources across the application.  It also acts as a tool to create new
  // units.

public:
  // CREATORS
  explicit Builder(void);
    // Default constructor.

  ~Builder(void);
    // Default destructor.

  // ACCESSORS
  effect_bitfield_t effects(piece_id_t idx, error_t& err) const;
    // Returns the 'effect_t' bitfield for the unit defined by the given 'idx'.
    // Returns an error status of 'GUNGI_ERROR_NONE' to the given output
    // parameter 'err' if the given 'idx' is a valid identifier, otherwise
    // 'GUNGI_ERROR_INVALID_IDX'.

  effect_bitfield_t immunities(piece_id_t idx, error_t& err) const;
    // Returns the immunity bitfield for the unit defined by the given 'idx'.
    // Returns an error status of 'GUNGI_ERROR_NONE' to the given output
    // parameter 'err' if the given 'idx' is a valid identifier, otherwise
    // 'GUNGI_ERROR_INVALID_IDX'.

  const_moveset_ptr_t moveset(piece_id_t idx, error_t& err) const;
    // Returns a constant pointer to the moveset for the unit defined by the
    // given 'idx'.  Returns an error status of 'GUNGI_ERROR_NONE' to the
    // given output parameter 'err' if the given 'idx' is a valid identifier,
    // otherwise 'GUNGI_ERROR_INVALID_IDX'.
};

}  // close 'gungi' namespace
