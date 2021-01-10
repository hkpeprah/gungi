// logician.cpp                                                       -*-C++-*-
#include "logician.hpp"

#include "builder.hpp"
#include "gtypes.hpp"
#include "player.hpp"
#include "posn.hpp"
#include "tower.hpp"
#include "unit.hpp"
#include "util.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include <utility>

namespace {

static bool hasTower(std::shared_ptr<gungi::Unit>& unit);
  // Returns 'true' if the given 'unit' is in a tower, otherwise 'false'.

static gungi::PosnSet intersect(const gungi::PosnSet& a,
                                const gungi::PosnSet& b);
  // Computes the set intersection between the constant 'gungi::PosnSet'
  // references, 'a' and 'b', and returns the result.

static gungi::PosnSet difference(const gungi::PosnSet& a,
                                 const gungi::PosnSet& b);
  // Computes the set difference between the constant 'gungi::PosnSet'
  // references 'a' and 'b', and returns the result.

template <class InputIt, class UnaryPredicate>
unsigned int any_of_count(InputIt first, InputIt last, UnaryPredicate p) {
  unsigned int count = 0;
  while (first != last) {
    count += p(*first) ? 1 : 0;
    first++;
  }
  return count;
}

bool hasTower(std::shared_ptr<gungi::Unit>& unit) {
  return unit->tower() != NULL;
}

gungi::PosnSet intersect(const gungi::PosnSet& a, const gungi::PosnSet& b) {
  gungi::PosnSet temp;
  temp.reserve(a.size());
  std::set_intersection(a.begin(), a.end(), b.begin(), b.end(), temp.begin());
  return temp;
}

gungi::PosnSet difference(const gungi::PosnSet& a, const gungi::PosnSet& b) {
  gungi::PosnSet temp;
  temp.reserve(a.size());
  std::set_difference(a.begin(), a.end(), b.begin(), b.end(), temp.begin());
  return temp;
}

}  // close 'unnamed' namespace

namespace gungi {

                              // ===================
                              // class BoardRecorder
                              // ===================

// CREATORS
BoardRecorder::BoardRecorder(void)
: m_boardSize(Logician::k_BOARD_SIZE)
, m_positions()
{
  // DO NOTHING
}

BoardRecorder::~BoardRecorder(void) {
  // DO NOTHING
}

// ACCESSORS
const BoardRecorder::position_t *BoardRecorder::position(const std::vector<Tower>& board) const {
  for (const position_t& position : m_positions) {
    bool isBoardEqual = false;
    for (unsigned int i = 0; i < m_boardSize; i++) {
      const Tower& tower = board[i];
      const std::vector<piece_id_t>& pieces = position.board[i];

      if (tower.height() != pieces.size()) {
        // Early exit, since their sizes aren't equal.
        isBoardEqual = false;
        break;
      }

      for (unsigned int y = 0; y < tower.height(); y++) {
        if (tower.members()[i]->front() != pieces[i]) {
          // Eqarly exit, since the sides are not equal.
          isBoardEqual = false;
          break;
        }
      }

      if (!isBoardEqual) {
        break;
      }
    }

    if (isBoardEqual) {
      return &position;
    }
  }
  return NULL;
}

// MANIPULATORS
void BoardRecorder::reset(void) {
  m_positions.clear();
}

unsigned int BoardRecorder::record(const std::vector<Tower>& board) {
  position_t *pos = const_cast<position_t *>(position(board));
  if (!pos) {
    m_positions.resize(m_positions.size() + 1);
    pos = &m_positions.back();
    pos->board.resize(m_boardSize);
    pos->repetitions = 0;
    for (unsigned int i = 0; i < m_boardSize; i++) {
      for (const SharedUnitPtr& ptr : board[i].members()) {
        pos->board[i].push_back(ptr->front());
      }
    }
  }
  return pos->repetitions++;
}

// OPERATORS
unsigned int operator<<(BoardRecorder&            recorder,
                        const std::vector<Tower>& board) {
  return recorder.record(board);
}


                                // ==============
                                // class Logician
                                // ==============

// PRIVATE ACCESSORS
Player& Logician::next(void) {
  if (state() & GAME_STATE_TURN_WHITE) {
    return m_black;
  }
  return m_white;
}

Player& Logician::current(void) {
  if (state() & GAME_STATE_TURN_WHITE) {
    return m_white;
  }
  return m_black;
}

PosnSet Logician::commanderEscapeRoutes(const Unit& com,
                                        const Posn& commanderPosn) const {
  GASSERT(com.front() == GUNGI_PIECE_COMMANDER);

  error_t error;
  PosnSet escapes = (PosnSet) {
    Posn(commanderPosn.col() + 1, commanderPosn.row()),
    Posn(commanderPosn.col() - 1, commanderPosn.row()),
    Posn(commanderPosn.col(), commanderPosn.row() + 1),
    Posn(commanderPosn.col(), commanderPosn.row() - 1),
    Posn(commanderPosn.col() + 1, commanderPosn.row() + 1),
    Posn(commanderPosn.col() + 1, commanderPosn.row() - 1),
    Posn(commanderPosn.col() - 1, commanderPosn.row() + 1),
    Posn(commanderPosn.col() - 1, commanderPosn.row() - 1),
  };

  for (PosnSet::iterator it = escapes.begin(); it != escapes.end();) {
    if (!it->isValid() || m_board[it->index()].height() == k_MAX_TOWER_SIZE) {
      escapes.erase(it);
    } else {
      it++;
    }
  }

  const Player& player = com.colour() == BLACK ? black() : white();
  for (const SharedUnitPtr& sharedUnitPtr : player.units()) {
    const Unit *unit = sharedUnitPtr.get();
    if (unit->effectField() & GUNGI_EFFECT_SUBSTITUTION) {
      // This effect allows to swap with a Commander to the left, right, front,
      // or back of the current unit if the commander is in check, and the unit
      // is at the top of its respective tower.
      Tower const *tower = unit->tower();
      if (!tower) {
        continue;
      }

      tier_t tier = tower->tier(unit, error);
      GASSERT(error == GUNGI_ERROR_NONE);
      if (tier != tower->height() - 1) {
        // Can't swap as a Commander cannot be built on top of in a tower.
        continue;
      }

      const Posn& posn = tower->posn();
      switch (posn.adjacent(commanderPosn)) {
      case GUNGI_MOVE_DIR_UP:
      case GUNGI_MOVE_DIR_LEFT:
      case GUNGI_MOVE_DIR_RIGHT:
      case GUNGI_MOVE_DIR_DOWN:
        escapes.push_back(posn);
        break;
      default:
        break;
      }
    }
  }

  return escapes;
}

bool Logician::isReachableAfterMove(const Posn&   posn,
                                    const Player& player) const {
  error_t error;
  for (const SharedUnitPtr& sharedUnitPtr : player.activeUnits()) {
    const Unit *unit = sharedUnitPtr.get();
    Tower const *tower = unit->tower();
    if (tower->posn() == posn) {
      tier_t tier = tower->tier(unit, error);
      GASSERT(error == GUNGI_ERROR_NONE);

      if (tier != tower->height() - 1) {
        continue;
      }

      const Unit *topUnit = tower->at(tier + 1, error);
      if (!topUnit || topUnit->colour() != unit->colour()) {
        continue;
      }

      return true;
    } else {
      PosnSet walk;
      bool validWalk = isValidMove(posn, *unit, walk, error);
      if (validWalk) {
        return true;
      }
    }
  }
  return false;
}

bool Logician::isDuplicateInFile(const Unit& unit, const Posn& posn) const {
  char col = posn.col();
  for (unsigned int i = 0; i < k_BOARD_LENGTH; i++) {
    const Posn p(col, i);
    const Tower& tower = m_board[p.index()];
    for (const SharedUnitPtr& sharedUnitPtr : tower.members()) {
      if (sharedUnitPtr->colour() == unit.colour() &&
          sharedUnitPtr->front() == unit.front()) {
        return true;
      }
    }
  }
  return false;
}

// PRIVATE MANIPULATORS
void Logician::reset(void) {
  // Clear all the units that have been allocated thus far.
  m_units.clear();

  // Reset each player.
  resetPlayer(m_black);
  resetPlayer(m_white);

  // Reset the towers.
  for (unsigned int i = 0; i < k_BOARD_LENGTH; i++) {
    for (unsigned int j = 0; j < k_BOARD_LENGTH; j++) {
      m_board[j + i * k_BOARD_LENGTH] = Tower(Posn(j, i));
    }
  }

  m_gameState = GAME_STATE_TURN_BLACK | GAME_STATE_INITIAL_ARRANGEMENT;
  m_boardRecorder.reset();
  m_toRearrange.reset();
  m_recovery.unit.reset();
  memset(&m_recovery, 0, sizeof(m_recovery));
  memset(m_expansions, 0, k_BOARD_SIZE * sizeof(unsigned int));
  m_escapeRoutes.clear();
  m_checkPoints.clear();
}

void Logician::resetPlayer(Player& player) {
  // Clear the player's unit.
  player.reset();

  // Count to keep track of the units added to the player.
  size_t hand_size = 0;

  // Size of the hand.
  size_t size = sizeof(k_START_HAND) / sizeof(int[3]);
  error_t error;

  // Add the player's starting hand.
  for (size_t i = 0; i < size; i++) {
    piece_id_t front = static_cast<piece_id_t>(k_START_HAND[i][0]);
    piece_id_t back = static_cast<piece_id_t>(k_START_HAND[i][1]);
    unsigned int count = static_cast<unsigned int>(k_START_HAND[i][2]);

    while (count --> 0) {
      Unit *unit = new Unit(front, back, player.colour(), m_builder);
      std::shared_ptr<Unit> ptr(unit);

      player.addUnit(ptr, error);
      GASSERT(error == GUNGI_ERROR_NONE);

      m_units.push_back(ptr);

      hand_size++;
    }
  }

  // Validate that the hand size is correct.
  GASSERT(hand_size == k_START_PIECE_COUNT);
}

void Logician::captureUnit(SharedUnitPtr& ptr, Player& from, Player& to, bool remove) {
  error_t error;
  from.removeUnit(ptr.get(), error);
  GASSERT(error == GUNGI_ERROR_NONE);

  to.addUnit(ptr, error);
  GASSERT(error == GUNGI_ERROR_NONE);

  if (remove) {
    GASSERT(ptr->tower());

    Tower& tower = m_board[ptr->tower()->posn().index()];
    tower.remove(ptr.get(), error);
    GASSERT(error == GUNGI_ERROR_NONE);

    ptr->flip(error);
    GASSERT(error == GUNGI_ERROR_NONE);
  }
}

void Logician::updateMobileRangeExpansion(void) {
  // Zero out the current expansions.
  memset(m_expansions, 0, sizeof(unsigned int) * k_BOARD_SIZE);

  for (const SharedUnitPtr& sharedUnitPtr : m_units) {
    if (!sharedUnitPtr->tower()) {
      continue;
    } else if (!(sharedUnitPtr->effectField() & k_MOBILE_RANGE_EXPANSION)) {
      continue;
    }

    colour_t colour = sharedUnitPtr->colour();
    effect_bitfield_t effects = sharedUnitPtr->effectField();
    Posn posn = sharedUnitPtr->tower()->posn();
    bool inverted = isInverted(colour);

    if (effects & GUNGI_EFFECT_MOBILE_RANGE_EXPANSION_1) {
      // Fortress mobile range expansion.  Affects the pieces on-top of the
      // unit and in front of it, extending outside the territory in a straight
      // line.
      while (posn.isValid()) {
        m_expansions[posn.index()] |= colour;
        posn.up(inverted);
      }
    }

    if (effects & GUNGI_EFFECT_MOBILE_RANGE_EXPANSION_2) {
      // Catapult mobile range expansion.  Affects the pieces on top of the
      // unit and within the territory.  The shape formed is a diamond:
      // 1-3-5-3-5 centered around the current unit.
      std::vector<int> upAmounts = { 2, 1, 0, -1, -2 };
      std::vector<int> leftAmounts = { 0, 1, 2, 1, 0 };
      std::vector<int> rightAmounts = { 0, 2, 4, 2, 0 };
      for (unsigned int i = 0; i < upAmounts.size(); i++) {
        int up = upAmounts[i];
        Posn p = posn;
        while (up != 0) {
          if (up < 0) {
            p.down(inverted);
            up += 1;
          } else {
            p.up(inverted);
            up -= 1;
          }
        }

        if (!p.isValid() || !isInTerritory(p, colour)) {
          continue;
        }

        int left = leftAmounts[i];
        while (left > 0) {
          p.left(inverted);
          left--;
        }

        int right = rightAmounts[i];
        while (right >= 0) {
          if (p.isValid()) {
            m_expansions[p.index()] |= colour;
          }
          p.right(inverted);
          right--;
        }  // end while
      }  // end for
    }  // end if
  }
}

void Logician::updateStateAfterTurn(error_t& error, piece_id_t dropped) {
  GASSERT(!isOver() || isForcedRearrangement());

  if (isForcedRecovery()) {
    // Exit early if forced recovery.
    return;
  }

  // Update the mobile range expansion.
  updateMobileRangeExpansion();

  // Keep a copy of the original game state if it has to be unwound due to foul
  // play.
  const game_state_t originalState = state();

  // Set state to indicate that it is the next player's turn by flipping the
  // state turn colour bits.
  m_gameState ^= GAME_STATE_TURN_BLACK | GAME_STATE_TURN_WHITE;

  // Compute if we are exiting the 'Initial Arrangement'.  The 'Initial
  // Arrangement' is over when both players have placed all of their units.
  // This phase only happens once per game.
  unsigned int initialPlaced = 0;
  if (isInitialArrangement()) {
    initialPlaced = any_of_count(m_units.begin(), m_units.end(), hasTower);
    if (initialPlaced == k_PIECE_COUNT) {
      // All units have been placed, so the initial arrangement is over.
      m_gameState ^= GAME_STATE_INITIAL_ARRANGEMENT;
    } else if (initialPlaced == k_PIECE_COUNT - 1) {
      // All but the last unit have been placed, in which case we need to
      // check if the current player will be in check/checkmate on the next
      // turn after initial arrangement.
      m_gameState &= ~GAME_STATE_INITIAL_ARRANGEMENT;
    }
  }

  // Compute if the current player is in check.  One can only be in check or
  // checkmate on their own turn.
  bool inCheck = false;
  Player& nextPlayer = next();
  Player& currentPlayer = current();
  const SharedUnitPtr& commander = currentPlayer.commander();
  GASSERT(commander.get());

  Tower const *commanderTower = commander->tower();
  if (commanderTower == NULL) {
    // Initial arrangement, commander hasn't been placed yet, so we can skip
    // computing check.
    GASSERT(isInitialArrangement() || initialPlaced == k_PIECE_COUNT - 1);

    error = GUNGI_ERROR_NONE;

    m_gameState |= GAME_STATE_INITIAL_ARRANGEMENT;
    m_checkPoints = PosnSet();
    m_escapeRoutes = PosnSet();

    return;
  }

  // Keeps track of the position of the current player's commander.
  const Posn& target = commanderTower->posn();

  // Compute if there are any units that could exchange positions with the
  // commander.  If there are such units, then they could be used to escape
  // from check, so they must be verified as well.
  PosnSet escapes;
  if (initialPlaced == 0) {
    // Only out of initial arrangment are escapes considered.
    escapes = commanderEscapeRoutes(*commander.get(), target);
  }

  // This position set is used to record the intersection of the positions
  // needed for each unit that can attack the current's player commander on the
  // next turn; this is the minimal sized set of points such that if the
  // current player can move another unit to any of the given positions then
  // they can escape check.
  PosnSet checkPoints;

  // For each unit in the next player's army, check if that unit can hit the
  // commander or any of the positions that the commander can escape to.  If it
  // can directly attack the commander on the next turn, then the commander is
  // in check.
  for (const SharedUnitPtr& sharedUnitPtr : nextPlayer.activeUnits()) {
    const Unit *unit = sharedUnitPtr.get();
    Tower const *tower = unit->tower();

    // This is the start position from where the unit will conduct its attack.
    const Posn& start(tower->posn());

    // Keep track of the walk that the unit does to reach the commander, if
    // there is such a walk.
    PosnSet walk;
    bool validWalk = isValidMove(target, *unit, walk, error);
    walk.pop_back();

    // The current unit may not able to directly attack the commander, but we
    // still need to verify if it can attack the commander's escape routes.
    for (PosnSet::iterator it = escapes.begin(); it != escapes.end();) {
      PosnSet escapeWalk;
      if (*it == start) {
        // This escape route goes to the tower containing this unit.
        int tier = tower->tier(unit, error);
        GASSERT(error == GUNGI_ERROR_NONE);
        if (tier == tower->height() - 1) {
          // If this unit is the highest in the tower, we have to check if it
          // is the same team as the commander.  If so, this escape route is
          // not valid.
          if (commander->colour() == unit->colour()) {
            escapes.erase(it);
            continue;
          }
        } else if (tier == tower->height() - 2) {
          // If this unit is not the highest in the tower, we have to check if
          // the unit above this one in the tower is the same team as the
          // current unit.  If so, the commander would take that unit's spot,
          // meaning this unit can now perform an immobile strike, so this
          // escape route is invalid.
          const Unit *above = tower->at(tier + 1, error);
          if (above && above->colour() == unit->colour()) {
            escapes.erase(it);
            continue;
          }
        }
      } else if (isValidMove(*it, *unit, escapeWalk, error)) {
        // The unit can hit this escape route, so it is not a valid escape
        // route as the current player will still be in check.
        escapes.erase(it);
        continue;
      }
      it++;
    }

    if (!validWalk) {
      continue;
    }

    // Current player is in check as there exists a means for the current unit
    // to attack the current player's commander.
    checkPoints = inCheck ? intersect(checkPoints, walk) : walk;
    inCheck = true;
  }

  if (initialPlaced == k_PIECE_COUNT - 1) {
    // Reset back to initial arrangement, as we no longer need to check if the
    // next player could attack the current player on their next turn.
    m_gameState ^= GAME_STATE_INITIAL_ARRANGEMENT;
  }

  if (checkPoints.size() > 0) {
    // For each non-commander unit on the current player's team, check if the
    // unit can move to or be placed in a checkpoint without leaving the
    // commander in check.
    PosnSet availableCheckPoints;
    for (SharedUnitPtr& sharedUnitPtr : currentPlayer.units()) {
      Unit *unit = sharedUnitPtr.get();
      for (const Posn& posn : checkPoints) {
        if (unit->tower()) {
          if (isInitialArrangement()) {
            // Cannot move.
            continue;
          }

          // If the unit has a tower, then this is either a move or an attack;
          // in either case, try moving the unit to the new tower, and see
          // whether the commander is still in a position where it is in check.
          Unit *removed = NULL;
          Tower& unitTower = m_board[unit->tower()->posn().index()];
          Tower& targetTower = m_board[posn.index()];
          if (targetTower.height() == k_MAX_TOWER_SIZE) {
            removed = targetTower.at(k_MAX_TOWER_SIZE - 1, error);
            GASSERT(error == GUNGI_ERROR_NONE);

            targetTower.remove(removed, error);
            GASSERT(error == GUNGI_ERROR_NONE);
          }

          // Remove the current unit from it current tower and add it to the
          // tower that it is transitioning into.
          unitTower.remove(unit, error);
          GASSERT(error == GUNGI_ERROR_NONE);

          targetTower.add(unit, error);
          GASSERT(error == GUNGI_ERROR_NONE);

          // Check whether the commander is still in check after the move has
          // been made, by checking if there are any valid attacks on the
          // commander after the move.
          bool stillInCheck = false;
          for (const SharedUnitPtr& enemySharedUnitPtr : nextPlayer.units()) {
            const Unit *enemyUnit = enemySharedUnitPtr.get();
            PosnSet walk;
            if (enemyUnit->tower() &&
                isValidMove(target,
                            *enemyUnit,
                            walk,
                            error)) {
              stillInCheck = true;
              break;
            }
          }

          if (!stillInCheck) {
            availableCheckPoints.push_back(posn);
          }

          // Undo the tower removal for the current unit.
          targetTower.remove(unit, error);
          GASSERT(error == GUNGI_ERROR_NONE);

          unitTower.add(unit, error);
          GASSERT(error == GUNGI_ERROR_NONE);

          // Undo removal if it was done.
          if (removed) {
            targetTower.add(unit, error);
            GASSERT(error == GUNGI_ERROR_NONE);
          }
        } else if (isValidDrop(posn, *unit, error)) {
          // Unit doesn't have a tower, so this is a drop-check.
          availableCheckPoints.push_back(posn);
        }
      }

      // Remove the 'availableCheckPoints' from the 'checkPoints' set to reduce
      // the number of points needed to check on subsequent iterations.
      checkPoints = difference(checkPoints, availableCheckPoints);
    }

    // Reduce the set of 'checkPoints' to the ones that the current player can
    // actually hit with their units.
    checkPoints = availableCheckPoints;
  }

  // There are no points at which a drop or move can be made, nor any escapes
  // that the commander can take to avoid still being in check on the next
  // player's turn, so it is checkmate.
  if (inCheck) {
    m_gameState ^= GAME_STATE_CHECK;

    if (checkPoints.size() == 0 && escapes.size() == 0) {
      if (dropped == GUNGI_PIECE_PAWN) {
        // Cannot achieve checkmate with a pawn drop as it is considered foul
        // play.
        m_gameState = originalState;
        error = GUNGI_ERROR_PAWN_CHECKMATE;
        return;
      } else if (dropped == GUNGI_PIECE_BRONZE) {
        // Cannot achieve checkmate with a bronze drop as it is considered foul
        // play.
        m_gameState = originalState;
        error = GUNGI_ERROR_BRONZE_CHECKMATE;
        return;
      }

      m_gameState ^= GAME_STATE_CHECKMATE;
    }
  }

  // Check for repetitions here.  If there are max repetitions reached, the
  // game is over, but only if checkmate was not achieved.
  unsigned int repetitions = m_boardRecorder << m_board;
  if (!(m_gameState & GAME_STATE_CHECKMATE) &&
      repetitions == k_MAX_POSITION_REPETITIONS) {
    m_gameState ^= GAME_STATE_DRAW;
  }

  error = GUNGI_ERROR_NONE;
  m_escapeRoutes = escapes;
  m_checkPoints = checkPoints;
}

// CREATORS
Logician::Logician(void)
: m_builder()
, m_black(BLACK)
, m_white(WHITE)
, m_units()
, m_board(k_BOARD_SIZE)
, m_boardRecorder()
, m_gameState(Logician::GAME_STATE_INITIAL_ARRANGEMENT)
, m_toRearrange()
, m_recovery()
, m_expansions {
}
, m_escapeRoutes()
, m_checkPoints()
{
  reset();
}

Logician::~Logician(void) {
  // DO NOTHING
}

// ACCESSORS
const std::vector<Tower>& Logician::board(void) const {
  return m_board;
}

int Logician::winner(void) const {
  if (!isOver() || isDraw()) {
    return -1;
  }

  if (isInCheckmate(BLACK)) {
    return WHITE;
  }

  return BLACK;
}

const Player& Logician::black(void) const {
  return m_black;
}

const Player& Logician::white(void) const {
  return m_white;
}

const Logician::game_state_t& Logician::state(void) const {
  return m_gameState;
}

const Unit *Logician::forcedRearrangeUnit(void) const {
  return m_toRearrange.get();
}

const Unit *Logician::forcedRecoveryUnit(void) const {
  return m_recovery.unit.get();
}

const int Logician::forcedRecoveryColour(void) const {
  return (m_recovery.player ? m_recovery.player->colour() : -1);
}

const Tower *Logician::forcedRecoveryTower(void) const {
  return m_recovery.tower;
}

const Unit *Logician::unitAtPosn(const Posn& posn, tier_t tier) const {
  GASSERT(posn.isValid() && tier < k_MAX_TOWER_SIZE);
  const Tower& tower = m_board[posn.index()];
  error_t error;
  return tower.at(tier, error);
}

bool Logician::isDraw(void) const {
  return state() & GAME_STATE_DRAW;
}

bool Logician::isOver(void) const {
  return state() & (GAME_STATE_CHECKMATE | GAME_STATE_DRAW);
}

bool Logician::isPlayersTurn(const colour_t& colour) const {
  if (state() & GAME_STATE_TURN_WHITE) {
    return (colour == WHITE);
  } else if (state() & GAME_STATE_TURN_BLACK) {
    return (colour == BLACK);
  }
  return false;
}

bool Logician::isPlayersTurn(const Player& player) const {
  if (state() & GAME_STATE_TURN_WHITE) {
    return (&player == &(white()));
  } else if (state() & GAME_STATE_TURN_BLACK) {
    return (&player == &(black()));
  }
  return false;
}

bool Logician::isInitialArrangement(void) const {
  return state() & GAME_STATE_INITIAL_ARRANGEMENT;
}

bool Logician::isInCheck(void) const {
  return state() & GAME_STATE_CHECK;
}

bool Logician::isInCheck(const colour_t& colour) const {
  const Player& player = colour == BLACK ? black() : white();
  return isPlayersTurn(player) && isInCheck();
}

bool Logician::isInCheck(const Player& player) const {
  return isPlayersTurn(player) && isInCheck();
}

bool Logician::isInCheckmate(void) const {
  return state() & GAME_STATE_CHECKMATE;
}

bool Logician::isInCheckmate(const colour_t& colour) const {
  const Player& player = colour == BLACK ? black() : white();
  return isPlayersTurn(player) && isInCheckmate();
}

bool Logician::isInCheckmate(const Player& player) const {
  return isPlayersTurn(player) && isInCheckmate();
}

bool Logician::isInMobileRangeExpansion(const Posn& posn,
                                        colour_t    colour) const {
  return m_expansions[posn.index()] & colour;
}

bool Logician::isInverted(const colour_t& colour) const {
  const Player& player = colour == BLACK ? black() : white();
  return isInverted(player);
}

bool Logician::isInverted(const Player& player) const {
  return isInTerritory(Posn(k_BOARD_LENGTH - 1, k_BOARD_LENGTH - 1), player);
}

bool Logician::isInTerritory(const Posn& posn, const colour_t& colour) const {
  const Player& player = colour == BLACK ? black() : white();
  return isInTerritory(posn, player);
}

bool Logician::isInTerritory(const Posn& posn, const Player& player) const {
  unsigned int idx = posn.index();
  if (player.colour() == WHITE) {
    return idx < k_BOARD_LENGTH * 3;
  }
  return (idx >= (k_BOARD_SIZE - k_BOARD_LENGTH * 3) && idx < k_BOARD_SIZE);
}

bool Logician::isForcedRearrangement(void) const {
  return (m_toRearrange.get() != NULL);
}

bool Logician::isForcedRearrangeForPlayer(const colour_t& colour) const {
  return isPlayersTurn(colour) && isForcedRearrangement();
}

bool Logician::isForcedRearrangeForPlayer(const Player& player) const {
  return isPlayersTurn(player) && isForcedRearrangement();
}

bool Logician::isForcedRecovery(void) const {
  return (m_recovery.unit.get() != NULL);
}

bool Logician::isForcedRecoveryForPlayer(const colour_t& colour) const {
  return isPlayersTurn(colour) && isForcedRecovery();
}

bool Logician::isForcedRecoveryForPlayer(const Player& player) const {
  return isPlayersTurn(player) && isForcedRecovery();
}

bool Logician::isValidDrop(const Posn& posn,
                           const Unit& unit,
                           error_t&    error) const {
  GASSERT(posn.isValid());
  if (isForcedRecoveryForPlayer(unit.colour())) {
    // Can't do a drop while waiting for a forced recovery.
    error = GUNGI_ERROR_INVALID_STATE;
    return false;
  } else if (isOver()) {
    // No moves are valid after a player is in checkmate.
    error = GUNGI_ERROR_GAME_OVER;
    return false;
  } else if (unit.tower()) {
    // A unit that is currently in a tower cannot be dropped.
    error = GUNGI_ERROR_INVALID_UNIT;
    return false;
  }

  const Player& player = unit.colour() == BLACK ? black() : white();
  if (isInCheck(player.colour())) {
    // If it is the turn of the unit's player, and that player is in check,
    // then check if the drop point is a valid point to stop the check.
    if (Util::find(m_checkPoints, posn) != m_checkPoints.end()) {
      // If the drop point is not in the check points set, then the drop
      // point is invalid as the player will still be in check on the next
      // player's turn.
      error = GUNGI_ERROR_CHECK;
      return false;
    }

    // This is a move that moves that player out of check, so it is valid.
    error = GUNGI_ERROR_NONE;
    return true;
  }

  const bool initialArrangement = isInitialArrangement();
  const bool rearrange = isForcedRearrangeForPlayer(player);
  if ((rearrange || initialArrangement) && !isInTerritory(posn, player)) {
    // During initial arrangement or when doing a forced rearrangement, drops
    // can only be made within the player's own territory.
    error = GUNGI_ERROR_TERRITORY;
    return false;
  }

  if (rearrange && &unit != m_toRearrange.get()) {
    // There is a unit waiting to be forced rearrange, and it is not the same
    // as the given 'unit'.
    error = GUNGI_ERROR_INVALID_UNIT;
    return false;
  }

  const unsigned int idx = posn.index();
  const Tower &tower = m_board[idx];
  if (tower.height() == k_MAX_TOWER_SIZE) {
    error = GUNGI_ERROR_FULL_TOWER;
    return false;
  }

  if (unit.front() == GUNGI_PIECE_PAWN || unit.front() == GUNGI_PIECE_BRONZE) {
    if (isDuplicateInFile(unit, posn)) {
      error = unit.front() == GUNGI_PIECE_PAWN
        ? GUNGI_ERROR_PAWN_FILE
        : GUNGI_ERROR_BRONZE_FILE;
      return false;
    }
  }

  if (tower.height() == 0) {
    // No more rules need to be validated if the tower is empty.
    error = GUNGI_ERROR_NONE;
    return true;
  }

  const Unit *topTowerUnit = tower.at(tower.height() - 1, error);
  GASSERT(error == GUNGI_ERROR_NONE);
  if (!initialArrangement &&
      (!(topTowerUnit->effectField() & GUNGI_EFFECT_LAND_LINK) ||
       unit.immunityField() & GUNGI_EFFECT_LAND_LINK)) {
    // After the Initial Arrangement, only active pieces with the effect,
    // 'GUNGI_EFFECT_LAND_LINK' can be dropped on.
    error = GUNGI_ERROR_LAND_LINK;
    return false;
  }

  if (topTowerUnit->effectField() & GUNGI_EFFECT_NO_TOWER) {
    // If the unit at the top of the tower is a Commander, then a unit cannot
    // be dropped on top of it.
    error = GUNGI_ERROR_NO_TOWER;
    return false;
  } else if (unit.effectField() & GUNGI_EFFECT_NO_STACK) {
    error = GUNGI_ERROR_NO_STACK;
    return false;
  } else if (topTowerUnit->effectField() & GUNGI_EFFECT_BACK_DROP_ONLY &&
             Util::isFrontPiece(unit.front())) {
    // Only back pieces can be dropped on this tower's unit.
    error = GUNGI_ERROR_BACK_ONLY;
    return false;
  } else if (topTowerUnit->effectField() & GUNGI_EFFECT_FRONT_DROP_ONLY &&
             Util::isBackPiece(unit.front())) {
    // Only front pieces can be dropped on this tower's unit.
    error = GUNGI_ERROR_FRONT_ONLY;
    return false;
  }

  // Verify that there are no units of the same team and type as the unit being
  // added in the current tower.
  if (tower.isDuplicate(&unit)) {
    error = GUNGI_ERROR_DUPLICATE;
    return false;
  }

  error = GUNGI_ERROR_NONE;
  return true;
}

bool Logician::isValidMove(const Posn& target,
                           const Unit& unit,
                           PosnSet&    walk,
                           error_t&    error) const {
  GASSERT(target.isValid());

  if (isInitialArrangement()) {
    error = GUNGI_ERROR_DROPS_ONLY;
    return false;
  } else if (isForcedRearrangeForPlayer(unit.colour()) ||
             isForcedRecoveryForPlayer(unit.colour())) {
    error = GUNGI_ERROR_INVALID_STATE;
    return false;
  } else if (isOver()) {
    // No moves are valid after a player is in checkmate.
    error = GUNGI_ERROR_GAME_OVER;
    return false;
  } else if (!unit.tower()) {
    // A unit that is not in a tower (i.e. is in the player's hand) cannot be
    // made to move.
    error = GUNGI_ERROR_INVALID_UNIT;
    return false;
  }

  const Player& player = unit.colour() == BLACK ? black() : white();
  if (isInCheck(player)) {
    // If it is the turn of the unit's player, and that player is in check,
    // then check if the move point is a valid point to stop the check.
    if (Util::find(m_checkPoints, target) != m_checkPoints.end()) {
      // If the move point is not in the check points set, then the move
      // point is invalid as the player will still be in check on the next
      // player's turn.
      error = GUNGI_ERROR_CHECK;
      return false;
    }

    // This is a move that moves that player out of check, so it is valid.
    error = GUNGI_ERROR_NONE;
    return true;
  }

  // Reference to the tower that the unit wants to move into.
  const Tower& targetTower = m_board[target.index()];
  const unsigned int targetHeight = targetTower.height();
  if (targetHeight > 0) {
    const Unit *topTowerUnit = targetTower.at(targetHeight - 1, error);
    GASSERT(error == GUNGI_ERROR_NONE);

    if (topTowerUnit->colour() == unit.colour() &&
        targetHeight == k_MAX_TOWER_SIZE) {
      // If the top unit is the same team as the unit that wants to move into
      // the tower, and the tower is full, then the unit cannot move into the
      // tower.  Even if the tower is full, if the teams aren't the same, it
      // is a valid move, as the moving unit will replace the unit at the top.
      error = GUNGI_ERROR_FULL_TOWER;
      return false;
    }
  }

  if (unit.front() == GUNGI_PIECE_BRONZE && isDuplicateInFile(unit, target)) {
    // Cannot move a Bronze unit into the same file as another Brozne of the
    // same team.
    error = GUNGI_ERROR_BRONZE_FILE;
    return false;
  }

  // The current tower the unit is in is the starting position for the move.
  const Posn& start = unit.tower()->posn();
  tier_t tier = unit.tower()->tier(&unit, error);
  GASSERT(error == GUNGI_ERROR_NONE);

  if (tier != unit.tower()->height() - 1) {
    // Must be the top unit in order to move.
    error = GUNGI_ERROR_NOT_TOP;
    return false;
  }

  // Have to keep track whether the movements are inverted, as the directions
  // are different depending on the player.
  const bool inverted = isInverted(player);
  colour_t enemyColour = player.colour() == WHITE ? BLACK : WHITE;

  for (tier_t t = tier; t < tier + 1 && t <= k_MAX_TOWER_SIZE - 1; t++) {
    walk = Util::getWalk(&unit, t, start, target, error, inverted);

    if (error == GUNGI_ERROR_NONE) {
      // If the walk is valid, have to check that for the points crossed by the
      // walk that if any of the towers at those positions are an enemy tower
      // in the enemy's mobile range expansion, as we cannot jump over them.
      PosnSet crossed = Util::crossed(start, target);

      // Keep track of whether the walk is valid.
      bool valid = true;

      if (crossed.size() > 0) {
        for (const Posn& posn : crossed) {
          const Tower& tower = m_board[posn.index()];
          if (tower.height() == 0) {
            continue;
          } else if (unit.effectField() & GUNGI_EFFECT_JUMP) {
            // Can jump over other units, but not enemy units that are in the
            // enemy's mobile range expansion effect.
            if (tower.members().back()->colour() == enemyColour) {
              if (isInMobileRangeExpansion(posn, enemyColour)) {
                // Can't jump over this.
                valid = false;
                break;
              }
            }
          } else {
            valid = false;
            break;
          }
        }
      }

      if (valid) {
        if (unit.front() == GUNGI_PIECE_COMMANDER) {
          // If the moving unit is a Commander, additionally have to check
          // that the move would not put the Commander into check.
          const Player& enemy = enemyColour == WHITE ? white() : black();
          if (isReachableAfterMove(target, enemy)) {
            error = GUNGI_ERROR_CHECK;
            return false;
          }
        }
        error = GUNGI_ERROR_NONE;
        return true;
      }
    }

    if (!isInMobileRangeExpansion(start, player.colour())) {
      // If the unit is not in the mobile range expansion effect, then we do
      // not need to check the subsequent tier.
      break;
    }

    if (unit.immunityField() & k_MOBILE_RANGE_EXPANSION) {
      // If the unit is immune to the mobile range expansion effect, then we do
      // not need to check the subsequent tier.
      break;
    }

    walk = PosnSet();
  }

  // If we reach here, then there is no valid path for the unit to move to the
  // target position.
  error = GUNGI_ERROR_NO_WALK;
  return false;
}

bool Logician::isValidImmobileStrike(tier_t      tier,
                                     const Unit& unit,
                                     error_t&    error) const {
  GASSERT(tier < k_MAX_TOWER_SIZE);

  if (isInitialArrangement()) {
    error = GUNGI_ERROR_DROPS_ONLY;
    return false;
  } else if (isForcedRearrangeForPlayer(unit.colour()) ||
             isForcedRecoveryForPlayer(unit.colour())) {
    error = GUNGI_ERROR_INVALID_STATE;
    return false;
  } else if (isOver()) {
    // No moves are valid after a player is in checkmate.
    error = GUNGI_ERROR_GAME_OVER;
    return false;
  } else if (!unit.tower()) {
    // A unit that is not in a tower (i.e. is in the player's hand) cannot be
    // made to move.
    error = GUNGI_ERROR_INVALID_UNIT;
    return false;
  }

  const Unit *target = unit.tower()->at(tier, error);
  if (!target) {
    error = GUNGI_ERROR_OUT_OF_RANGE;
    return false;
  } else if (target->colour() == unit.colour()) {
    // Cannot attack a unit on the same team.
    error = GUNGI_ERROR_SAME_TEAM;
    return false;
  }

  const int unitTier = unit.tower()->tier(&unit, error);
  GASSERT(error == GUNGI_ERROR_NONE);

  const unsigned int diff = std::abs(static_cast<int>(tier) - unitTier);
  if (diff > 1) {
    // Unit cannot reach the target from its position.
    error = GUNGI_ERROR_OUT_OF_RANGE;
    return false;
  }

  if (isInCheck(unit.colour()) &&
      (Util::find(m_checkPoints, unit.tower()->posn()) == m_checkPoints.end() ||
       tier != unit.tower()->height() - 1)) {
    // For an immobile strike to move the current player out of check, ti must
    // be the case that it exists within our check points list, and the unit
    // being attacked sits at the highest tier in the tower.
    error = GUNGI_ERROR_CHECK;
    return false;
  }

  error = GUNGI_ERROR_NONE;
  return true;
}

bool Logician::isValidExchange(const effect_t& exchange,
                               const Unit&     unit,
                               const Unit&     target,
                               error_t&        error) const {
  if (isInitialArrangement()) {
    error = GUNGI_ERROR_DROPS_ONLY;
    return false;
  } else if (isForcedRearrangeForPlayer(unit.colour()) ||
             isForcedRecoveryForPlayer(unit.colour())) {
    error = GUNGI_ERROR_INVALID_STATE;
    return false;
  }

  if (!unit.tower() || !target.tower()) {
    error = GUNGI_ERROR_INVALID_UNIT;
    return false;
  }

  if (exchange == GUNGI_EFFECT_1_3_TIER_EXCHANGE && unit.effectField() & exchange) {
    if (target.immunityField() & GUNGI_EFFECT_1_3_TIER_EXCHANGE) {
      error = GUNGI_ERROR_IMMUNE;
      return false;
    }

    if (isInCheck(unit.colour())) {
      // A substitution can never help us get out of check, so it will always
      // be implicitly invalid.
      error = GUNGI_ERROR_CHECK;
      return false;
    }

    const Tower* tower = unit.tower();
    if (tower != target.tower() ||
        unit.colour() != target.colour() ||
        tower->isDirty(Tower::TIER_EXCHANGE)) {
      // Units must be in the same tower, and be on the same team, and a tier
      // exchange cannot be performed in the same tower consecutively.
      error = GUNGI_ERROR_INVALID_EXCHANGE;
      return false;
    }

    int unitTier = tower->tier(&unit, error);
    GASSERT(error == GUNGI_ERROR_NONE);

    int targetTier = tower->tier(&target, error);
    GASSERT(error == GUNGI_ERROR_NONE);

    if (std::abs(unitTier - targetTier) <= 1) {
      // Can only be performed if the exchange is between tiers 1 and 3.
      error = GUNGI_ERROR_INVALID_EXCHANGE;
      return false;
    }

    if (unitTier == k_MAX_TOWER_SIZE - 1 &&
        (target.front() == GUNGI_PIECE_CATAPULT ||
         target.front() == GUNGI_PIECE_FORTRESS)) {
      // Cannot exchange with the Catapult or Fortress if it would move them to
      // the top of the tower.
      error = GUNGI_ERROR_INVALID_EXCHANGE;
      return false;
    }

    error = GUNGI_ERROR_NONE;
    return true;
  } else if (exchange == GUNGI_EFFECT_SUBSTITUTION && unit.effectField() & exchange) {
    if (target.immunityField() & GUNGI_EFFECT_SUBSTITUTION) {
      error = GUNGI_ERROR_IMMUNE;
      return false;
    }

    // Target must be a Commander on the same team as the unit that is invoking
    // the substitution effect.  The Commander must also be in check to invoke
    // the effect.  The unit must also be at the highest tier in its tower.
    if (target.colour() != unit.colour() ||
        target.front() != GUNGI_PIECE_COMMANDER ||
        !isInCheck(unit.colour())) {
      error = GUNGI_ERROR_INVALID_SUB;
      return false;
    }

    // Since the moves have been precomputed, we simply need to confirm that
    // the unit is at the highest tier in its tower, and that the unit's
    // position is in the escapes list.
    tier_t tier = unit.tower()->tier(&unit, error);
    GASSERT(error == GUNGI_ERROR_NONE);
    if (Util::find(m_escapeRoutes, unit.tower()->posn()) != m_escapeRoutes.end() &&
        tier == unit.tower()->height() - 1) {
        error = GUNGI_ERROR_NONE;
        return true;
    }

    error = GUNGI_ERROR_INVALID_SUB;
    return false;
  }

  error = GUNGI_ERROR_INVALID_UNIT;
  return false;
}

// MANIPULATORS
void Logician::newGame(void) {
  reset();
}

void Logician::moveUnit(const Posn& from,
                        tier_t      tier,
                        const Posn& to,
                        error_t&    error) {
  Tower& tower = m_board[from.index()];
  Unit *unit = tower.at(tier, error);
  if (error != GUNGI_ERROR_NONE) {
    return;
  }

  moveUnit(*unit, to, error);
}

void Logician::moveUnit(const Unit& unit, const Posn& posn, error_t& error) {
  if (!isPlayersTurn(unit.colour())) {
    error = GUNGI_ERROR_NOT_TURN;
    return;
  }

  PosnSet walk;
  if (!isValidMove(posn, unit, walk, error)) {
    return;
  }

  SharedUnitPtrVector::iterator it = Util::find(m_units, &unit);
  GASSERT(it != m_units.end());

  Tower& tower = m_board[posn.index()];
  SharedUnitPtr& unitPtr = *it;

  SharedUnitPtrVector capturedUnits;
  SharedUnitPtrVector betrayedUnits;

  if (tower.height() > 0 && tower.members().back()->colour() != unit.colour()) {
    // Capture the unit at the top of the tower.
    SharedUnitPtrVector::iterator it = Util::find(m_units,
                                                  tower.members().back().get());
    captureUnit(*it, next(), current(), true);
    capturedUnits.push_back(*it);

    if (unit.effectField() & GUNGI_EFFECT_BETRAYAL) {
      // Units below in the tower betray their team and align with the current
      // player.
      for (SharedUnitPtr& sharedUnitPtr : tower.members()) {
        if (sharedUnitPtr->colour() != unit.colour()) {
          captureUnit(sharedUnitPtr, next(), current());
          betrayedUnits.push_back(sharedUnitPtr);
        }
      }
    }
  }

  if (unit.effectField() & GUNGI_EFFECT_FORCED_RECOVERY) {
    tier_t tier = unit.tower()->tier(&unit, error);
    GASSERT(error == GUNGI_ERROR_NONE);

    const Posn& unitPosn = unit.tower()->posn();
    if (!Util::anyWalk(&unit, tier, unitPosn, isInverted(unit.colour()))) {
      m_recovery.unit = unitPtr;
      m_recovery.player = capturedUnits.size() ? &next() : &current();
      m_recovery.tower = &tower;
    }

    if (isInMobileRangeExpansion(unitPosn, unit.colour()) &&
        tier < k_MAX_TOWER_SIZE - 1 &&
        !Util::anyWalk(&unit, tier + 1, unitPosn, isInverted(unit.colour()))) {
      m_recovery.unit = unitPtr;
      m_recovery.player = capturedUnits.size() ? &next() : &current();
      m_recovery.tower = &tower;
    }
  }

  // Move unit into the tower, but first remove from current tower.
  GASSERT(unit.tower());
  Tower& unitTower = m_board[unit.tower()->posn().index()];
  unitTower.remove(unitPtr.get(), error);
  GASSERT(error == GUNGI_ERROR_NONE);

  // Add the unit to its new tower.
  tower.add(unitPtr, error);
  GASSERT(error == GUNGI_ERROR_NONE);

  if (unit.front() == GUNGI_PIECE_BRONZE) {
    // This is a special case in which a Bronze moves.  A Bronze cannot move
    // into a position in which it would put the opposing player into check.

    // There should never be a unit to recover here.
    GASSERT(!isForcedRecovery());

    updateStateAfterTurn(error, unit.front());

    if (error == GUNGI_ERROR_NONE) {
      return;
    }

    error_t tempError;

    // Undo the captures and betrayals.
    for (SharedUnitPtr& sharedUnitPtr : capturedUnits) {
      captureUnit(sharedUnitPtr, current(), next());
      tower.add(sharedUnitPtr, tempError);
      GASSERT(tempError == GUNGI_ERROR_NONE);
    }

    for (SharedUnitPtr& sharedUnitPtr : betrayedUnits) {
      captureUnit(sharedUnitPtr, current(), next());
    }

    // Undo the move of the Bronze unit.
    tower.remove(unitPtr.get(), tempError);
    GASSERT(tempError == GUNGI_ERROR_NONE);

    unitTower.add(unitPtr, tempError);
    GASSERT(tempError == GUNGI_ERROR_NONE);
  } else {
    updateStateAfterTurn(error);
    GASSERT(error == GUNGI_ERROR_NONE);
  }
}

void Logician::dropUnit(piece_id_t  front,
                        piece_id_t  back,
                        const Posn& to,
                        error_t&    error) {
  Player& p = current();
  Unit *unit = NULL;
  for (SharedUnitPtr& ptr : p.units()) {
    if (ptr->front() == front && ptr->back() == back && !ptr->tower()) {
      unit = ptr.get();
      break;
    }
  }

  if (unit) {
    dropUnit(*unit, to, error);
  } else {
    error = GUNGI_ERROR_INVALID_UNIT;
  }
}

void Logician::dropUnit(const Unit& unit, const Posn& posn, error_t& error) {
  if (!isPlayersTurn(unit.colour())) {
    error = GUNGI_ERROR_NOT_TURN;
    return;
  }

  if (!isValidDrop(posn, unit, error)) {
    return;
  }

  Tower& tower = m_board[posn.index()];
  SharedUnitPtrVector::iterator it = Util::find(m_units, &unit);
  GASSERT(it != m_units.end());

  tower.add(*it, error);
  GASSERT(error == GUNGI_ERROR_NONE);

  m_toRearrange.reset();
  updateStateAfterTurn(error, unit.front());

  if (error != GUNGI_ERROR_NONE) {
    // Foul play created by a drop that yielded a checkmate, so undo the drop,
    // and return.
    error_t tempError;
    tower.remove(it->get(), tempError);
    GASSERT(tempError == GUNGI_ERROR_NONE);
  }
}

void Logician::exchangeUnits(effect_t    effect,
                             const Posn& from,
                             tier_t      fromTier,
                             const Posn& to,
                             tier_t      toTier,
                             error_t&    error) {
  Tower& fromTower = m_board[from.index()];
  Unit *a = fromTower.at(fromTier, error);
  if (error != GUNGI_ERROR_NONE) {
    return;
  }

  Tower& toTower = m_board[to.index()];
  Unit *b = toTower.at(toTier, error);
  if (error != GUNGI_ERROR_NONE) {
    return;
  }

  exchangeUnits(effect, *a, *b, error);
}

void Logician::exchangeUnits(effect_t    exchange,
                             const Unit& a,
                             const Unit& b,
                             error_t&    error) {
  if (!isPlayersTurn(a.colour())) {
    error = GUNGI_ERROR_NOT_TURN;
    return;
  }

  if (!isValidExchange(exchange, a, b, error)) {
    return;
  }

  Tower& unitTower = m_board[a.tower()->posn().index()];
  if (exchange == GUNGI_EFFECT_SUBSTITUTION) {
    Tower& targetTower = m_board[b.tower()->posn().index()];
    SharedUnitPtr& unit = unitTower.members().back();
    SharedUnitPtr& target = targetTower.members().back();
    unit.swap(target);
    unit->setTower(&targetTower);
    target->setTower(&unitTower);
  } else if (exchange == GUNGI_EFFECT_1_3_TIER_EXCHANGE) {
    SharedUnitPtrVector& members = unitTower.members();
    members.back().swap(members.front());
  } else {
    GASSERT(false);
  }

  updateStateAfterTurn(error);
  GASSERT(error == GUNGI_ERROR_NONE);
}

void Logician::immobileStrike(const Posn& posn,
                              tier_t      tier,
                              tier_t      target,
                              error_t&    error) {
  Tower& tower = m_board[posn.index()];
  Unit *unit = tower.at(tier, error);
  if (error != GUNGI_ERROR_NONE) {
    return;
  }

  immobileStrike(*unit, target, error);
}

void Logician::immobileStrike(const Unit& unit,
                              tier_t      target,
                              error_t&    error) {
  if (!isPlayersTurn(unit.colour())) {
    error = GUNGI_ERROR_NOT_TURN;
    return;
  }

  if (!isValidImmobileStrike(target, unit, error)) {
    return;
  }

  Tower& tower = m_board[unit.tower()->posn().index()];
  Unit *enemy = tower.at(target, error);
  GASSERT(error == GUNGI_ERROR_NONE);

  SharedUnitPtrVector::iterator it = Util::find(m_units, &unit);
  captureUnit(*it, next(), current(), true);

  updateStateAfterTurn(error);
  GASSERT(error == GUNGI_ERROR_NONE);
}

void Logician::forceRecover(bool recover, error_t& error) {
  if (!isForcedRecovery()) {
    error = GUNGI_ERROR_INVALID_STATE;
    return;
  }

  if (recover) {
    // Always remove from the current player.  It will be added back in the
    // event the current player is recovering and it was not after a capture.
    Player& player = current();
    player.removeUnit(m_recovery.unit.get(), error);
    GASSERT(error == GUNGI_ERROR_NONE);

    m_recovery.player->addUnit(m_recovery.unit.get(), error);
    GASSERT(error == GUNGI_ERROR_NONE);

    // Remove the unit from the board.
    Tower& tower = m_board[m_recovery.unit->tower()->posn().index()];
    tower.remove(m_recovery.unit.get(), error);
    GASSERT(error == GUNGI_ERROR_NONE);
  }

  m_recovery.unit.reset();
  memset(&m_recovery, 0, sizeof(m_recovery));

  updateStateAfterTurn(error);
  GASSERT(error == GUNGI_ERROR_NONE);
}

// OPERATORS
std::ostream& operator<<(std::ostream& os, const Logician& logician) {
  const std::vector<Tower>& towers = logician.board();
  error_t error;
  const unsigned int len = k_BOARD_LENGTH * 6 + 1;

  os
    << std::endl
    << std::string(3, ' ')
    << 0;

  for (unsigned int col = 1; col < k_BOARD_LENGTH; col++) {
    os
      << std::string(5, ' ')
      << col;
  }

  os << std::endl;

  for (int row = k_BOARD_LENGTH - 1; row >= 0; row--) {
    // Print the board row by row.
    os << std::string(len, '-') << std::endl;
    for (tier_t tier = k_MAX_TOWER_SIZE - 1; tier >= 0; tier--) {
      // For each row, we have to print each tier on their own separate line,
      // starting with the topmost tier.
      os << "|";
      for (unsigned int col = 0; col < k_BOARD_LENGTH; col++) {
        // At each tier, print the unit that is at the given tier, with the
        // given column an row.
        const Posn posn(col, row);
        const Tower& tower = towers[posn.index()];
        const Unit *unit = tower.at(tier, error);
        if (!unit) {
          // No unit, so fill the space.
          os << std::string(4, ' ');
        } else {
          os
            << std::right
            << " "
            << (unit->colour() == BLACK ? "B" : "W")
            << unit->code();
        }
        os << " |";
        if (tier == k_MAX_TOWER_SIZE / 2 && col == k_BOARD_LENGTH - 1) {
          os
            << " "
            << row;
        }
      }
      os << std::endl;
    }
  }

  os
    << std::string(len, '-')
    << std::endl;

  return os;
}

}  // close 'gungi' namespace
