// unit.cpp                                                           -*-C++-*-
#include "unit.hpp"

#include "builder.hpp"
#include "gtypes.hpp"
#include "tower.hpp"
#include "util.hpp"

#include <iostream>
#include <sstream>
#include <string>
#include <utility>

namespace gungi {

// PRIVATE MANIPULATORS
void Unit::reset(void) {
  error_t err;

  m_effectBitField = m_builder.effects(m_identifier, err);
  if (err != GUNGI_ERROR_NONE) {
    m_identifier = GUNGI_PIECE_NONE;
    return;
  }

  m_immuneBitField = m_builder.immunities(m_identifier, err);
  if (err != GUNGI_ERROR_NONE) {
    m_identifier = GUNGI_PIECE_NONE;
    return;
  }

  m_movesPtr = m_builder.moveset(m_identifier, err);
  if (err != GUNGI_ERROR_NONE) {
    m_identifier = GUNGI_PIECE_NONE;
    return;
  }
}

// CREATORS
Unit::Unit(piece_id_t     front,
           piece_id_t     back,
           colour_t       colour,
           const Builder& builder)
: m_builder(builder)
, m_identifier(front)
, m_back(back)
, m_colour(colour)
, m_effectBitField(0)
, m_immuneBitField(0)
, m_movesPtr(0)
, m_tower(0)
{
  reset();
}

Unit::~Unit(void) {
  // DO NOTHING
}

// MANIPULATORS
void Unit::flip(error_t& err) {
  if (m_back == GUNGI_PIECE_NONE) {
    err = GUNGI_ERROR_NO_BACK;
    return;
  }

  err = GUNGI_ERROR_NONE;

  std::swap(m_identifier, m_back);

  reset();
}

void Unit::setColour(colour_t colour) {
  m_colour = colour;
}

void Unit::setTower(Tower const *towerPtr) {
  m_tower = towerPtr;
}

void Unit::clearTower(void) {
  m_tower = NULL;
}

void Unit::addEffect(effect_t effect) {
  m_effectBitField |= effect;
}

void Unit::removeEffect(effect_t effect) {
  m_effectBitField ^= effect;
}

void Unit::addImmunity(effect_t effect) {
  m_immuneBitField |= effect;
}

void Unit::removeImmunity(effect_t effect) {
  m_immuneBitField ^= effect;
}

// ACCESSORS
bool Unit::isValid(void) const {
  return (m_identifier != GUNGI_PIECE_NONE);
}

bool Unit::isActive(void) const {
  return (m_tower != NULL);
}

const piece_id_t& Unit::id(void) const {
  return m_identifier;
}

const piece_id_t& Unit::front(void) const {
  return id();
}

const piece_id_t& Unit::back(void) const {
  return m_back;
}

const colour_t& Unit::colour(void) const {
  return m_colour;
}

const_moveset_ptr_t Unit::moveset(void) const {
  return m_movesPtr;
}

Tower const *Unit::tower(void) const {
  return m_tower;
}

effect_bitfield_t Unit::effectField(void) const {
  return m_effectBitField;
}

effect_bitfield_t Unit::immunityField(void) const {
  return m_immuneBitField;
}

std::string Unit::code(void) const {
  std::string id(piece_to_gn_identifier(front()));
  id += std::string(piece_to_gn_identifier(back()));
  return id;
}

// OPERATORS
std::ostream& operator<<(std::ostream& os, const Unit& unit) {
  os
    << piece_to_string(unit.front())
    << "(colour="
    << (unit.colour() == BLACK ? "black" : "white")
    << ", back="
    << (unit.back() == GUNGI_PIECE_NONE ? "" : piece_to_string(unit.back()))
    << ", tower="
    << unit.tower()
    << ")";
  return os;
}

}  // close 'gungi' namespace
