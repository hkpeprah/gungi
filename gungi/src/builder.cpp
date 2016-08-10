// builder.cpp                                                        -*-C++-*-
#include "builder.hpp"

#include "gtypes.hpp"
#include "util.hpp"

#include <cstdlib>

namespace gungi {

// CREATORS
Builder::Builder(void) {
  // DO NOTHING
}

Builder::~Builder(void) {
  // DO NOTHING
}

// ACCESSORS
effect_bitfield_t Builder::effects(piece_id_t idx, error_t& err) const {
  if (Util::isInvalidPiece(idx)) {
    err = GUNGI_ERROR_INVALID_IDX;
    return GUNGI_EFFECT_NONE;
  }

  err = GUNGI_ERROR_NONE;
  return k_UNIT_EFFECT[idx];
}

effect_bitfield_t Builder::immunities(piece_id_t idx, error_t& err) const {
  if (Util::isInvalidPiece(idx)) {
    err = GUNGI_ERROR_INVALID_IDX;
    return GUNGI_EFFECT_NONE;
  }

  err = GUNGI_ERROR_NONE;
  return k_UNIT_IMMUNITY[idx];
}

const_moveset_ptr_t Builder::moveset(piece_id_t idx, error_t& err) const {
  if (Util::isInvalidPiece(idx)) {
    err = GUNGI_ERROR_INVALID_IDX;
    return NULL;
  }

  const_moveset_ptr_t ptr = k_UNIT_MOVES[idx];
  err = GUNGI_ERROR_NONE;
  return ptr;
}

}  // close 'gungi' namespace
