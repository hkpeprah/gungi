// tower.cpp                                                          -*-C++-*-
#include "tower.hpp"

#include "gtypes.hpp"
#include "posn.hpp"
#include "unit.hpp"
#include "util.hpp"

#include <algorithm>
#include <iterator>
#include <memory>
#include <utility>
#include <vector>

namespace gungi {

// CREATORS
Tower::Tower(void)
: m_posn(0, 0)
, m_units()
, m_bits(0)
{
  // DO NOTHING
}

Tower::Tower(const Posn& posn)
: m_posn(posn)
, m_units()
, m_bits(0)
{
  // DO NOTHING
}

Tower::~Tower(void) {
  // DO NOTHING
}

// MANIPULATORS
void Tower::markDirty(const Tower::dirty_bit_t& bit) {
  m_bits |= bit;
}

void Tower::markClean(const Tower::dirty_bit_t& bit) {
  if (m_bits & bit) {
    m_bits ^= bit;
  }
}

void Tower::markClean(void) {
  m_bits = 0;
}

void Tower::add(Unit *unit, error_t& err) {
  GASSERT(unit);

  SharedUnitPtr ptr(unit, NilDeleter<Unit>());
  add(ptr, err);
}

void Tower::add(SharedUnitPtr& unit, error_t& err) {
  if (height() == k_MAX_HEIGHT) {
    err = GUNGI_ERROR_FULL_TOWER;
  } else if (isDuplicate(unit.get())) {
    err = GUNGI_ERROR_DUPLICATE;
  } else {
    m_units.push_back(unit);

    unit->setTower(this);

    err = GUNGI_ERROR_NONE;
  }
}

void Tower::remove(const Unit *unit, error_t& err) {
  GASSERT(unit);

  SharedUnitPtrVector::const_iterator it = Util::find(m_units, unit);
  if (it == m_units.end()) {
    err = GUNGI_ERROR_NOT_A_MEMBER;
  } else {
    (*it)->clearTower();

    m_units.erase(it);

    err = GUNGI_ERROR_NONE;
  }
}

void Tower::reset(void) {
  m_units.clear();
  markClean();
}

// ACCESSORS
bool Tower::isDuplicate(const Unit *unit) const {
  GASSERT(unit);
  for (const SharedUnitPtr& ptr : m_units) {
    if (ptr->front() == unit->front() && ptr->colour() == unit->colour()) {
      return true;
    }
  }
  return false;
}

bool Tower::isDirty(const Tower::dirty_bit_t& bit) const {
  return (m_bits & bit);
}

unsigned int Tower::height(void) const {
  return m_units.size();
}

tier_t Tower::tier(const Unit *unit, error_t& err) const {
  GASSERT(unit);

  SharedUnitPtrVector::const_iterator it = Util::find(m_units, unit);
  tier_t tier = std::distance(m_units.begin(), it);
  if (tier >= height()) {
    err = GUNGI_ERROR_NOT_A_MEMBER;
  } else {
    err = GUNGI_ERROR_NONE;
  }

  return tier;
}

Unit *Tower::at(tier_t tier, error_t& err) {
  return const_cast<Unit *>(
    static_cast<const Tower *>(this)->at(tier, err));
}

const Unit *Tower::at(tier_t tier, error_t& err) const {
  if (tier >= height()) {
    err = GUNGI_ERROR_OUT_OF_RANGE;
    return NULL;
  }

  err = GUNGI_ERROR_NONE;
  return m_units.at(tier).get();
}

const Posn& Tower::posn(void) const {
  return m_posn;
}

SharedUnitPtrVector& Tower::members(void) {
  return m_units;
}

const SharedUnitPtrVector& Tower::members(void) const {
  return m_units;
}

}  // close 'gungi' namespace
