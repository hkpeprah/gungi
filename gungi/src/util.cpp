// util.cpp                                                           -*-C++-*-
#include "util.hpp"

#include "gtypes.hpp"
#include "posn.hpp"
#include "unit.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <vector>

namespace gungi {

                                             // ==========
                                             // class Util
                                             // ==========

// STATIC CLASS MEMBERS
const piece_id_t Util::k_FRONT_PIECES[] = {
  GUNGI_PIECE_COMMANDER,
  GUNGI_PIECE_CAPTAIN,
  GUNGI_PIECE_SAMURAI,
  GUNGI_PIECE_SPY,
  GUNGI_PIECE_CATAPULT,
  GUNGI_PIECE_FORTRESS,
  GUNGI_PIECE_HIDDEN_DRAGON,
  GUNGI_PIECE_PRODIGY,
  GUNGI_PIECE_BOW,
  GUNGI_PIECE_PAWN,
  GUNGI_PIECE_NONE
};

const piece_id_t Util::k_BACK_PIECES[] = {
  GUNGI_PIECE_PISTOL,
  GUNGI_PIECE_PIKE,
  GUNGI_PIECE_CLANDESTINITE,
  GUNGI_PIECE_LANCE,
  GUNGI_PIECE_DRAGON_KING,
  GUNGI_PIECE_PHOENIX,
  GUNGI_PIECE_ARROW,
  GUNGI_PIECE_BRONZE,
  GUNGI_PIECE_SILVER,
  GUNGI_PIECE_GOLD,
  GUNGI_PIECE_NONE
};

// STATIC CLASS METHODS
bool Util::isValidPiece(piece_id_t id) {
  return (id > GUNGI_PIECE_NONE && id < GUNGI_NUM_PIECES);
}

bool Util::isInvalidPiece(piece_id_t id) {
  return !isValidPiece(id);
}

bool Util::isFrontPiece(piece_id_t id) {
  const size_t len = sizeof(k_FRONT_PIECES) / sizeof(k_FRONT_PIECES[0]) - 1;
  const piece_id_t *end = k_FRONT_PIECES + len;
  return std::find(k_FRONT_PIECES, end, id) != end;
}

bool Util::isBackPiece(piece_id_t id) {
  const size_t len = sizeof(k_BACK_PIECES) / sizeof(k_BACK_PIECES[0]) - 1;
  const piece_id_t *end = k_BACK_PIECES + len;
  return std::find(k_BACK_PIECES, end, id) != end;
}

unsigned char Util::toLowerCase(unsigned char ch) {
  if (ch >= 'A' && ch <= 'Z') {
    return (ch - 'A') + 'a';
  }
  return ch;
}

std::string Util::toLowerCase(const std::string& str) {
  std::string newstr(str);
  for (unsigned int i = 0; i < newstr.size(); i++) {
    newstr[i] = toLowerCase(newstr[i]);
  }
  return newstr;
}

unsigned char Util::toUpperCase(unsigned char ch) {
  if (ch >= 'a' && ch <= 'z') {
    return (ch - 'a') + 'A';
  }
  return ch;
}

std::string Util::toUpperCase(const std::string& str) {
  std::string newstr(str);
  for (unsigned int i = 0; i < newstr.size(); i++) {
    newstr[i] = toUpperCase(newstr[i]);
  }
  return newstr;
}

SharedUnitPtrVector::iterator Util::find(SharedUnitPtrVector& v,
                                         const Unit          *unit) {
  SharedUnitPtrVector::iterator it;
  for (it = v.begin(); it != v.end(); it++) {
    if ((*it).get() == unit) {
      return it;
    }
  }
  return it;
}

SharedUnitPtrVector::const_iterator Util::find(
                                             const SharedUnitPtrVector& v,
                                             const Unit                *unit) {
  SharedUnitPtrVector::const_iterator it;
  for (it = v.begin(); it != v.end(); it++) {
    if ((*it).get() == unit) {
      return it;
    }
  }
  return it;
}

PosnSet::const_iterator Util::find(const PosnSet& set, const Posn& posn) {
  return std::find(set.begin(), set.end(), posn);
}

bool Util::anyWalk(const Unit *unit,
                   tier_t      tier,
                   const Posn& start,
                   bool        invert) {
  GASSERT(unit);

  const move_vector_t move_vector = unit->moveset()[tier];
  for (const std::vector<move_t> moves : move_vector) {
    // Check the moves available at the tier.  Each portion of the move must be
    // executed in turn to walk towards the 'end' position.
    Posn pos = start;

    for (const move_t move : moves) {
      unsigned int dir = move.move_direction;
      move_mod_t mod = move.move_modifier;

      while (dir) {
        if (dir & GUNGI_MOVE_DIR_UP) {
          pos.up(invert);
          if (!mod) {
            dir ^= GUNGI_MOVE_DIR_UP;
          }
        }

        if (dir & GUNGI_MOVE_DIR_DOWN) {
          pos.down(invert);
          if (!mod) {
            dir ^= GUNGI_MOVE_DIR_DOWN;
          }
        }

        if (dir & GUNGI_MOVE_DIR_LEFT) {
          pos.left(invert);
          if (!mod) {
            dir ^= GUNGI_MOVE_DIR_LEFT;
          }
        }

        if (dir & GUNGI_MOVE_DIR_RIGHT) {
          pos.right(invert);
          if (!mod) {
            dir ^= GUNGI_MOVE_DIR_RIGHT;
          }
        }

        if (mod && pos.isValid()) {
          return true;
        } else if (!pos.isValid()) {
          break;
        }
      }
    }

    if (pos.isValid()) {
      return true;
    }
  }

  return false;
}

PosnSet Util::allWalks(const Unit *unit,
                       tier_t      tier,
                       const Posn& start,
                       bool        invert) {
  GASSERT(unit);
  PosnSet posns;

  const move_vector_t move_vector = unit->moveset()[tier];
  for (const std::vector<move_t> moves : move_vector) {
    // Check the moves available at the tier.  Each portion of the move must be
    // executed in turn to walk towards the 'end' position.
    Posn pos = start;

    for (const move_t move : moves) {
      unsigned int dir = move.move_direction;
      move_mod_t mod = move.move_modifier;

      while (dir) {
        if (dir & GUNGI_MOVE_DIR_UP) {
          pos.up(invert);
          if (!mod) {
            dir ^= GUNGI_MOVE_DIR_UP;
          }
        }

        if (dir & GUNGI_MOVE_DIR_DOWN) {
          pos.down(invert);
          if (!mod) {
            dir ^= GUNGI_MOVE_DIR_DOWN;
          }
        }

        if (dir & GUNGI_MOVE_DIR_LEFT) {
          pos.left(invert);
          if (!mod) {
            dir ^= GUNGI_MOVE_DIR_LEFT;
          }
        }

        if (dir & GUNGI_MOVE_DIR_RIGHT) {
          pos.right(invert);
          if (!mod) {
            dir ^= GUNGI_MOVE_DIR_RIGHT;
          }
        }

        if (mod && pos.isValid()) {
          posns.push_back(pos);
        } else if (!pos.isValid()) {
          break;
        }
      }
    }

    if (pos.isValid()) {
      posns.push_back(pos);
    }
  }

  return posns;
}

PosnSet Util::getWalk(const Unit *unit,
                      tier_t      tier,
                      const Posn& start,
                      const Posn& end,
                      error_t&    error,
                      bool        invert) {
  GASSERT(unit);

  const move_vector_t move_vector = unit->moveset()[tier];
  for (const std::vector<move_t> moves : move_vector) {
    // Check the moves available at the tier.  Each portion of the move must be
    // executed in turn to walk towards the 'end' position.
    PosnSet posn_set;
    Posn pos = start;
    posn_set.push_back(pos);

    for (const move_t move : moves) {
      unsigned int dir = move.move_direction;
      move_mod_t mod = move.move_modifier;

      while (dir) {
        if (dir & GUNGI_MOVE_DIR_UP) {
          pos.up(invert);
          if (!mod) {
            dir ^= GUNGI_MOVE_DIR_UP;
          }
        }

        if (dir & GUNGI_MOVE_DIR_DOWN) {
          pos.down(invert);
          if (!mod) {
            dir ^= GUNGI_MOVE_DIR_DOWN;
          }
        }

        if (dir & GUNGI_MOVE_DIR_LEFT) {
          pos.left(invert);
          if (!mod) {
            dir ^= GUNGI_MOVE_DIR_LEFT;
          }
        }

        if (dir & GUNGI_MOVE_DIR_RIGHT) {
          pos.right(invert);
          if (!mod) {
            dir ^= GUNGI_MOVE_DIR_RIGHT;
          }
        }

        posn_set.push_back(pos);
        if (pos == end || !pos.isValid()) {
          break;
        }
      }
    }

    if (pos != end) {
      continue;
    }

    // If the position is the ending position, then it mens that we have
    // successfully reached the target position, return the set of points
    // walked to reach the target.
    error = GUNGI_ERROR_NONE;
    return posn_set;
  }

  error = GUNGI_ERROR_NO_WALK;
  return PosnSet();
}

PosnSet Util::crossed(const Posn& a, const Posn& b) {
  unsigned int x0, y0;
  a.index(x0, y0);

  unsigned int x1, y1;
  b.index(x1, y1);

  int dx = std::abs(static_cast<int>(x0) - static_cast<int>(x1));
  int dy = std::abs(static_cast<int>(y0) - static_cast<int>(y1));

  unsigned int x = a.col();
  unsigned int y = a.row();

  int n = 1 + dx + dy;
  int x_inc = (x1 > x0 ? 1 : -1);
  int y_inc = (y1 > y0 ? 1 : -1);
  int error = (dx - dy);
  dx *= 2;
  dy *= 2;

  PosnSet posns;
  for (; n > 0; n--) {
    posns.push_back(Posn(x, y));

    if (error > 0) {
      x += x_inc;
      error -= dy;
    } else if (error < 0) {
      y += y_inc;
      error += dx;
    } else {
      x += x_inc;
      y += y_inc;
      n--;
    }
  }

  if (posns.begin() + 1 == posns.end()) {
    // Vector will throw an error if adding '1' to 'begin()' when there is only
    // one element.
    return PosnSet();
  }

  return PosnSet(posns.begin() + 1, posns.end() - 1);
}

}  // close 'gungi' namespace
