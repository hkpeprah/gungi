// posn.cpp                                                           -*-C++-*-
#include "posn.hpp"

#include "gtypes.hpp"
#include "util.hpp"

namespace gungi {

// CREATORS
Posn::Posn(void)
: m_col(0)
, m_row(0)
{
  // DO NOTHING
}

Posn::Posn(unsigned int col, unsigned int row)
: m_col(col)
, m_row(row)
{
  // DO NOTHING
}

Posn::Posn(const Posn& p) {
  m_col = p.col();
  m_row = p.row();
}

Posn::~Posn(void) {
  // DO NOTHING
}

// ACCESSORS
const unsigned int& Posn::col(void) const {
  return m_col;
}

const unsigned int& Posn::row(void) const {
  return m_row;
}

unsigned int Posn::index(void) const {
  return (k_BOARD_LENGTH * m_row) + m_col;
}

void Posn::index(unsigned int& col, unsigned int& row) const {
  col = m_col;
  row = m_row;
}

move_dir_t Posn::adjacent(const Posn& p2) const {
  if (m_row < k_BOARD_LENGTH - 1) {
    // Top-left
    if (m_col > 0) {
      if (Posn(m_col - 1, m_row + 1) == p2) {
        return GUNGI_MOVE_DIR_UP_LEFT;
      }
    }

    // Top
    if (Posn(m_col, m_row + 1) == p2) {
      return GUNGI_MOVE_DIR_UP;
    }

    // Top-right
    if (m_col < k_BOARD_LENGTH - 1) {
      if (Posn(m_col + 1, m_row + 1) == p2) {
        return GUNGI_MOVE_DIR_UP_RIGHT;
      }
    }
  }

  if (m_col > 0) {
    // Left
    if (Posn(m_col - 1, m_row) == p2) {
      return GUNGI_MOVE_DIR_LEFT;
    }
  }

  if (m_col < k_BOARD_LENGTH - 1) {
    // Right
    if (Posn(m_col + 1, m_row) == p2) {
      return GUNGI_MOVE_DIR_RIGHT;
    }
  }

  if (m_row > 0) {
    // Bottom-left
    if (m_col > 0) {
      if (Posn(m_col - 1, m_row - 1) == p2) {
        return GUNGI_MOVE_DIR_DOWN_LEFT;
      }
    }

    // Bottom
    if (Posn(m_col, m_row - 1) == p2) {
      return GUNGI_MOVE_DIR_DOWN;
    }

    // Bottom-right
    if (m_col < k_BOARD_LENGTH - 1) {
      if (Posn(m_col + 1, m_row - 1) == p2) {
        return GUNGI_MOVE_DIR_DOWN_RIGHT;
      }
    }
  }

  return GUNGI_MOVE_DIR_NONE;
}

const bool Posn::isValid(void) const {
  return m_col < k_BOARD_LENGTH && m_row < k_BOARD_LENGTH;
}

// MANIPULATORS
void Posn::up(bool invert) {
  if (invert) {
    m_row -= 1;
  } else {
    m_row += 1;
  }
}

void Posn::down(bool invert) {
  if (invert) {
    m_row += 1;
  } else {
    m_row -= 1;
  }
}

void Posn::left(bool invert) {
  if (invert) {
    m_col += 1;
  } else {
    m_col -= 1;
  }
}

void Posn::right(bool invert) {
  if (invert) {
    m_col -= 1;
  } else {
    m_col += 1;
  }
}

// OPERATORS
bool Posn::operator<(const Posn& posn) const {
  return index() < posn.index();
}

bool Posn::operator==(const Posn& posn) const {
  return (m_col == posn.m_col) && (m_row == posn.m_row);
}

bool Posn::operator!=(const Posn& posn) const {
  return !(*this == posn);
}

}  // close 'gungi' namespace
