// player.cpp                                                         -*-C++-*-
#include "player.hpp"

#include "gtypes.hpp"
#include "unit.hpp"
#include "util.hpp"

#include <algorithm>
#include <cstdlib>
#include <vector>

namespace gungi {

// CREATORS
Player::Player(colour_t colour)
: m_colour(colour)
, m_units()
, m_commander(nullptr)
{
  // DO NOTHING
}

Player::~Player(void) {
  // DO NOTHING
}

// MANIPULATORS
void Player::reset(void) {
  m_units.clear();
  m_commander = nullptr;
}

void Player::addUnit(Unit *unit, error_t& error) {
  GASSERT(unit);

  SharedUnitPtr ptr(unit, NilDeleter<Unit>());
  addUnit(ptr, error);
}

void Player::addUnit(SharedUnitPtr& unit, error_t& error) {
  GASSERT(unit.get());

  if (Util::find(m_units, unit.get()) != m_units.end()) {
    error = GUNGI_ERROR_DUPLICATE;
  } else {
    if (unit->front() == GUNGI_PIECE_COMMANDER) {
      GASSERT(m_commander == nullptr);
      m_commander = unit;
    }

    unit->setColour(colour());

    error = GUNGI_ERROR_NONE;

    m_units.push_back(unit);
  }
}

void Player::removeUnit(Unit *unit, error_t& error) {
  GASSERT(unit);

  SharedUnitPtrVector::const_iterator it = Util::find(m_units, unit);
  if (it != m_units.end()) {
    error = GUNGI_ERROR_NONE;

    if ((*it)->front() == GUNGI_PIECE_COMMANDER) {
      m_commander.reset();
    }

    m_units.erase(it);
  } else {
    error = GUNGI_ERROR_NOT_A_MEMBER;
  }
}

// ACCESSORS
const SharedUnitPtr& Player::commander(void) const {
  return m_commander;
}

const colour_t& Player::colour(void) const {
  return m_colour;
}

SharedUnitPtrVector Player::activeUnits(void) const {
  SharedUnitPtrVector v;
  for (const SharedUnitPtr& ptr : m_units) {
    if (ptr->isActive()) {
      v.push_back(ptr);
    }
  }
  return v;
}

SharedUnitPtrVector Player::inactiveUnits(void) const {
  SharedUnitPtrVector v;
  for (const SharedUnitPtr& ptr : m_units) {
    if (!ptr->isActive()) {
      v.push_back(ptr);
    }
  }
  return v;
}

SharedUnitPtrVector& Player::units(void) {
  return m_units;
}

const SharedUnitPtrVector& Player::units(void) const {
  return m_units;
}

}  // close 'gungi' namespace
