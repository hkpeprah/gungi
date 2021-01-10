// gungi.hpp                                                          -*-C++-*-
#pragma once
//@PURPOSE: Gungi library header.
#include "../../include/builder.hpp"
#include "../../include/gndecoder.hpp"
#include "../../include/gtypes.hpp"
#include "../../include/logician.hpp"
#include "../../include/player.hpp"
#include "../../include/posn.hpp"
#include "../../include/tower.hpp"
#include "../../include/unit.hpp"
#include "../../include/util.hpp"

#include <cstdint>

using namespace gungi;

extern "C" {

                                // =========
                                // FUNCTIONS
                                // =========

unsigned int gungi_game_start(void);
  // Starts a new game, returning a unique identifier that represents the game
  // that was started.

void gungi_game_clear(unsigned int game_id);
  // Clears all game state for the game specified by the given 'game_id'.

void gungi_games_clear(void);
  // Clears all game state for all active games.

bool gungi_game_restart(unsigned int game_id);
  // Restarts a game, specified by the given 'game_id', that is already in
  // progress, returning a boolean to indicate success or not.

bool gungi_game_is_over(unsigned int game_id);
  // Returns a boolean indicating whether the game, specified by the given
  // 'game_id', is over or not; returning 'true' if over, otherwise 'false'.

bool gungi_game_is_draw(unsigned int game_id);
  // Returns 'true' if the game, specified by the given 'game_id', has ended in
  // a draw, otherwise 'false' if the game is still in progress, or the game
  // has ended with a winner.

bool gungi_game_is_forced_recovery(unsigned int game_id);
  // Returns 'true' if the current player must perform a forced recovery,
  // otherwise false.

bool gungi_game_is_forced_rearrangement(unsigned int game_id);
  // Returns 'true' if the current player must perform a forced rearrangement,
  // otherwise 'false'.

int gungi_game_get_turn(unsigned int game_id);
  // Returns the identifier for the player whose turn it currently is for the
  // game identified by the given 'game_id'; either 'WHITE' or 'BLACK'.

int gungi_game_get_winner(unsigned int game_id);
  // Returns the identifier of the player that won; either 'WHITE' or 'BLACK'
  // for the game specified by the given 'game_id'.  In the case that the game
  // is still in progress, or has ended in a draw, then '-1' is returned
  // instead.

error_t gungi_game_drop_unit(unsigned int game_id,
                             const char *id,
                             unsigned int to_col,
                             unsigned int to_row);
  // Drops a unit, specified by the given 'id', for the game, specified by the
  // given 'game_id' onto the board position identified by the column 'to_col',
  // and row 'to_row'.

error_t gungi_game_move_unit(unsigned int game_id,
                             unsigned int from_col,
                             unsigned int from_row,
                             tier_t from_tier,
                             unsigned int to_col,
                             unsigned int to_row);
  // For the game specified by the given 'game_id', moves a unit from the
  // position identified by the column 'from_col' and row 'from_row' at tier
  // 'from_tier' to the top of the tower at the position identified by the
  // given column 'to_col' and row 'to_row'.

error_t gungi_game_substitution(unsigned int game_id,
                                unsigned int from_col,
                                unsigned int from_row,
                                tier_t from_tier,
                                unsigned int to_col,
                                unsigned int to_row,
                                tier_t to_tier);
  // For the game specified by the given 'game_id', performs a substitution
  // between the unit at the position identified by the column 'from_col' and
  // row 'from_row' at tier 'from_tier' with the unit at the position
  // identified by the column 'to_col' and row 'to_row' at tier 'to_tier'.

error_t gungi_game_exchange(unsigned int game_id,
                            unsigned int from_col,
                            unsigned int from_row,
                            tier_t from_tier,
                            unsigned int to_col,
                            unsigned int to_row,
                            tier_t to_tier);
  // For the game specified by the given 'game_id', performs a 1-3 Tier
  // Exchange between the unit at the position identified by the column
  // 'from_col' and row 'from_row' at tier 'from_tier' and the unit at the
  // same position, identified by 'to_col' and 'to_row', but at tier 'to_tier'.

error_t gungi_game_immobile_strike(unsigned int game_id,
                                   unsigned int from_col,
                                   unsigned int from_row,
                                   tier_t from_tier,
                                   tier_t target_tier);
  // For the game specified by the given 'game_id', performs an immobile strike
  // within a tower at a position identified by the given column 'from_col' and
  // row 'from_row'.  The unit at the specified tier 'from_tier' performs the
  // strike against the unit at the specified tier 'target_tier'.

error_t gungi_game_force_recover(unsigned int game_id, bool recover);
  // For the game specified by the given 'game_id', sets whether the user will
  // be performing a forced recovery, by setting 'recover' to 'true', or not
  // by setting 'recover' to 'false'.

}  // close extern
