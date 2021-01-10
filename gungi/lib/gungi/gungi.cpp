// gungi.cpp                                                          -*-C++-*-
#include "gungi.hpp"

#include <atomic>
#include <cstdint>
#include <cstring>
#include <limits>
#include <map>
#include <memory>
#include <string>

using namespace gungi;

extern "C" {

// Map of game identifiers to their controller instances.
static std::map<unsigned int, std::shared_ptr<Controller> > s_games;

// Index of the last game that was created.
static std::atomic<unsigned int> s_game_last_index = {0};


                                // =========
                                // FUNCTIONS
                                // =========

static Controller& get_controller(unsigned int game_id) {
  std::map<unsigned int, std::shared_ptr<Controller> >::iterator it;

  it = s_games.find(game_id);
  GASSERT(it != s_games.end());
  GASSERT(it->second != nullptr);

  return *it->second;
}

unsigned int gungi_game_start(void) {
  unsigned int next;
  unsigned int current;
  unsigned int count;

  count = 0;
  next = s_game_last_index;

  while (s_games.find(next) != s_games.end()) {
    if (next == std::numeric_limits<unsigned int>::max()) {
      // Wrap to the beginning.
      next = 0;
    } else {
      // Increment to the next.
      next += 1;
    }

    GASSERT(count != s_games.size());

    count++;
  }

  do {
      current = s_game_last_index;
  } while (!std::atomic_compare_exchange_weak(&s_game_last_index, &current, next));

  s_games[next] = std::shared_ptr<Controller>(new Controller);

  return next;
}

void gungi_game_clear(unsigned int game_id) {
  s_games.erase(game_id);
}

void gungi_games_clear(void) {
  s_games.clear();
}

bool gungi_game_restart(unsigned int game_id) {
  Controller& controller = get_controller(game_id);

  controller.newGame();

  return true;
}

bool gungi_game_is_over(unsigned int game_id) {
  Controller& controller = get_controller(game_id);
  return controller.isOver();
}

bool gungi_game_is_draw(unsigned int game_id) {
  Controller& controller = get_controller(game_id);
  return controller.isDraw();
}

bool gungi_game_is_forced_recovery(unsigned int game_id) {
  Controller& controller = get_controller(game_id);
  return controller.isForcedRecovery();
}

bool gungi_game_is_forced_rearrangement(unsigned int game_id) {
  Controller& controller = get_controller(game_id);
  return controller.isForcedRearrangement();
}

int gungi_game_get_turn(unsigned int game_id) {
  Controller& controller = get_controller(game_id);
  if (controller.state() & gungi::Logician::GAME_STATE_TURN_BLACK) {
    return BLACK;
  }
  return WHITE;
}

int gungi_game_get_winner(unsigned int game_id) {
  Controller& controller = get_controller(game_id);
  return controller.winner();
}

error_t gungi_game_drop_unit(unsigned int game_id,
                             const char *id,
                             unsigned int to_col,
                             unsigned int to_row) {
  Controller& controller = get_controller(game_id);
  error_t error;
  piece_id_t front;
  piece_id_t back;
  std::string stringId;
  const Posn to(to_col, to_row);

  GASSERT(strlen(id) >= 1);

  stringId = std::string(1, id[0]);
  front = gn_identifier_to_piece(stringId.c_str());

  if (strlen(id) == 2) {
      stringId = std::string(1, id[1]);
      back = gn_identifier_to_piece(stringId.c_str());
  } else {
      back = GUNGI_PIECE_NONE;
  }

  controller.dropUnit(front, back, to, error);

  return error;
}

error_t gungi_game_move_unit(unsigned int game_id,
                             unsigned int from_col,
                             unsigned int from_row,
                             tier_t from_tier,
                             unsigned int to_col,
                             unsigned int to_row) {
  Controller& controller = get_controller(game_id);
  error_t error;
  const Posn from(from_col, from_row);
  const Posn to(to_col, to_row);

  controller.moveUnit(from, from_tier, to, error);

  return error;
}

error_t gungi_game_substitution(unsigned int game_id,
                                unsigned int from_col,
                                unsigned int from_row,
                                tier_t from_tier,
                                unsigned int to_col,
                                unsigned int to_row,
                                tier_t to_tier) {
  Controller& controller = get_controller(game_id);
  Posn from(from_col, from_row);
  Posn to(to_col, to_row);
  effect_t effect = GUNGI_EFFECT_SUBSTITUTION;
  error_t error;

  controller.exchangeUnits(effect, from, from_tier, to, to_tier, error);

  return error;
}

error_t gungi_game_exchange(unsigned int game_id,
                            unsigned int from_col,
                            unsigned int from_row,
                            tier_t from_tier,
                            unsigned int to_col,
                            unsigned int to_row,
                            tier_t to_tier) {
  Controller& controller = get_controller(game_id);
  Posn from(from_col, from_row);
  Posn to(to_col, to_row);
  effect_t effect = GUNGI_EFFECT_1_3_TIER_EXCHANGE;
  error_t error;

  controller.exchangeUnits(effect, from, from_tier, to, to_tier, error);

  return error;
}

error_t gungi_game_immobile_strike(unsigned int game_id,
                                   unsigned int from_col,
                                   unsigned int from_row,
                                   tier_t from_tier,
                                   tier_t target_tier) {
  Controller& controller = get_controller(game_id);
  error_t error;
  Posn from(from_col, from_row);

  controller.immobileStrike(from, from_tier, target_tier, error);

  return error;
}

error_t gungi_game_force_recover(unsigned int game_id,
                                 bool recover) {
  Controller& controller = get_controller(game_id);
  error_t error;

  controller.forceRecover(recover, error);
  return error;
}

}  // close extern
