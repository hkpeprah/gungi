// logician.hpp                                                       -*-C++-*-
#pragma once
//@DESCRIPTION:
//  This component implements the logic controller for the 'Gungi' game.  This
//  is responsible for handling moves, effects, the early game mechancis (drops
//  at the start of the game), and checkmate conditions.
//
//  The board starts from the bottom left-corner at (0, 0) or (0) and moves
//  to the top-right corner at (8, 8) or (81):
//
//                   -----------------------------------------
//                   |   | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 |
//                   -----------------------------------------
//                   | 8 |   |   |   |   |   |   |   |   |   |
//                   -----------------------------------------
//                   | 7 |   |   |   |   |   |   |   |   |   |
//                   -----------------------------------------
//                   | 6 |   |   |   |   |   |   |   |   |   |
//                   -----------------------------------------
//                   | 5 |   |   |   |   |   |   |   |   |   |
//                   -----------------------------------------
//                   | 4 |   |   |   |   |   |   |   |   |   |
//                   -----------------------------------------
//                   | 3 |   |   |   |   |   |   |   |   |   |
//                   -----------------------------------------
//                   | 2 |   |   |   |   |   |   |   |   |   |
//                   -----------------------------------------
//                   | 1 |   |   |   |   |   |   |   |   |   |
//                   -----------------------------------------
//                   | 0 |   |   |   |   |   |   |   |   |   |
//                   -----------------------------------------
//
//  A row is called a 'rank', while a column is called a 'file'.  Note that the
//  top three ranks are black's territory, and the bottom three ranks are
//  white's territory.
//
//  The start of the game begins with the 'Initial Arrangement' in which each
//  player takes turns placing their units, as there is no mandated starting
//  position for the units.  Only 'Front' peices can be placed in the 'Initial
//  Arrangement'.  Each player must have a 'Pawn' in every 'file' by the end of
//  this phase.
//
//@CLASSES:
//  'gungi::BoardRecorder': class to record the board positions.
//  'gungi::Logician': logic controller class.
//
//@TYPES:
//  'gungi::Controller': alias for the 'Logician'.
#include "builder.hpp"
#include "gtypes.hpp"
#include "player.hpp"
#include "posn.hpp"
#include "tower.hpp"

#include <cstdint>
#include <iostream>
#include <memory>
#include <vector>

namespace gungi {

// FORWARD DECLARATION
class Unit;

                              // ===================
                              // class BoardRecorder
                              // ===================

class BoardRecorder {
  // Class to record the board positions.  This is necessary to ensure that the
  // same position does not repeat more than 'k_MAX_POSITION_REPETITIONS'.

public:
  // STRUCTURES
  typedef struct position_t {
    std::vector<std::vector<piece_id_t> > board;

    unsigned int repetitions;
  } position_t;

private:
  // INSTANCE MEMBERS
  const unsigned int      m_boardSize;
                           // Size of the board.

  std::vector<position_t> m_positions;
                           // Vector of board positions.

public:
  // CREATORS
  BoardRecorder(void);
    // Default constructor.

  ~BoardRecorder(void);
    // Default destructor.

  // ACCESSORS
  const position_t *position(const std::vector<Tower>& board) const;
    // Returns a pointer to the 'position_t' object that corresponds to the
    // given 'board' if the given 'board' matches any of the stored
    // positions, otherwise 'NULL'.

  // MANIPULATORS
  void reset(void);
    // Resets the recorder instance, clearing any stored positions.

  unsigned int record(const std::vector<Tower>& board);
    // Records the given 'board' position and returns the repetition count.

  // OPERATORS
  friend unsigned int operator<<(BoardRecorder&            recorder,
                                 const std::vector<Tower>& board);
    // Records the given 'board' position into the given 'recorder'.
};


                                // ==============
                                // class Logician
                                // ==============

class Logician {
  // Logic controller class for the 'Gungi' game.  Implements logic for making
  // moves, applying effects, start game mechanics, and checkmate conditions.

public:
  // ENUMERATIONS
  enum {
    // Enumeration for the different game states.
    GAME_STATE_INITIAL_ARRANGEMENT  = (1 << 0),
    GAME_STATE_TURN_BLACK           = (1 << 1),
    GAME_STATE_TURN_WHITE           = (1 << 2),
    GAME_STATE_CHECK                = (1 << 3),
    GAME_STATE_CHECKMATE            = (1 << 4),
    GAME_STATE_DRAW                 = (1 << 5),
  };

  // STRUCTURES
  typedef struct recover_t {
    // Pointer to the unit being recovered.
    SharedUnitPtr  unit;

    // Pointer to the player whose hand the recovered unit will be added to.
    Player  *player;
  } recover_t;

  // TYPE DEFINITIONS
  typedef unsigned int game_state_t;
    // Type definition for the bitfield that holds the game state.

public:
  // STATIC CLASS MEMBERS
  static const unsigned int k_NUM_PLAYERS = 2;

  static const unsigned int k_PIECE_COUNT = k_START_PIECE_COUNT * k_NUM_PLAYERS;

  static const unsigned int k_BOARD_SIZE = k_BOARD_LENGTH * k_BOARD_LENGTH;

  static const effect_bitfield_t k_MOBILE_RANGE_EXPANSION =
    GUNGI_EFFECT_MOBILE_RANGE_EXPANSION_1 | GUNGI_EFFECT_MOBILE_RANGE_EXPANSION_2;

private:
  // INSTANCE MEMBERS
  const Builder                        m_builder;
                                        // Constant builder used to create the
                                        // units.

  Player                               m_black;
                                        // Black player.

  Player                               m_white;
                                        // White player.

  SharedUnitPtrVector                  m_units;
                                        // Vector of shared pointers to units.

  std::vector<Tower>                   m_board;
                                        // Vector of towers that make up the
                                        // board.

  BoardRecorder                        m_boardRecorder;
                                        // Board position recording class.

  // STATE VARIABLES
  game_state_t                         m_gameState;
                                        // Current game state.

  SharedUnitPtr                        m_toRearrange;
                                        // Shared pointer for the unit that is
                                        // being rearranged.

  recover_t                            m_recovery;
                                        // Forced recovery information.

  unsigned int                         m_expansions[k_BOARD_SIZE];
                                        // Mobile range expansion bitfield.
                                        // Used to record which indices are in
                                        // the mobile range expansion for which
                                        // players.

  PosnSet                              m_escapeRoutes;
                                        // If the current player is in check,
                                        // contains the set of points that the
                                        // commander can move to in order to
                                        // escape check.

  PosnSet                              m_checkPoints;
                                        // If the current player is in check,
                                        // contains the set of points that a
                                        // non-commander units can be moved to
                                        // or dropped upon to get the commander
                                        // to escape check.

private:
  // PRIVATE ACCESSORS
  Player& next(void);
    // Returns a reference to the next player.

  Player& current(void);
    // Returns a reference to the 'Player' whose turn it is currently.

  PosnSet commanderEscapeRoutes(const Unit& com,
                                const Posn& commanderPosn) const;
    // Returns a set of 'Posn' objects that the given Commander, 'com', at the
    // given 'Posn', 'commanderPosn', can escape to in order to avoid a
    // checkmate.

  bool isReachableAfterMove(const Posn& posn, const Player& player) const;
    // Returns 'true' if the acitve units for the given 'player' could reach
    // the given 'posn' after an opposing unit has moved to the given 'posn',
    // otherwise 'false'.

  bool isDuplicateInFile(const Unit& unit, const Posn& posn) const;
    // Returns 'true' if there is a Unit of the same colour and identifier as
    // the given 'unit' in the file containing the given 'posn', otherwise
    // 'false'.

  // PRIVATE MANIPULATORS
  void reset(void);
    // Resets the logic controller, players, units, and game state.  Note that
    // after this function is called, 'Unit' pointers previously allocated will
    // be invalid.

  void resetPlayer(Player& player);
    // Resets the given 'player' by clearing its units; creating new ones in
    // the process.

  void captureUnit(SharedUnitPtr& ptr,
                   Player&        from,
                   Player&        to,
                   bool           remove = false);
    // Captures the unit specified by the given 'ptr', adding it to the
    // player's, to's, army, and removing it from the player's, from's, army.
    // If the optionally spcified 'remove' is 'true', also removes it from the
    // board.

  void updateMobileRangeExpansion(void);
    // Recomputes and updates the tracking of the mobile range expansions areas
    // on the board.

  void updateStateAfterTurn(error_t&   error,
                            piece_id_t dropped = GUNGI_PIECE_NONE);
    // Computes the next game state and updates it within the 'Logician'.  This
    // has the effect of updating whose turn it is, as well as information
    // needed for check / checkmate determination.  Returns 'GUNGI_ERROR_NONE'
    // in the given 'error' parameter if the state was computed successfully,
    // 'GUNGI_ERROR_BRONZE_CHECKMATE' if the state could not be updated as the
    // optionally given 'dropped' piece was a 'GUNGI_PIECE_BRONZE' and
    // checkmate was achieved, otherwise 'GUNGI_ERROR_PAWN_CHECKMATE' if the
    // state could not be updated as the optionally given 'dropped' piece was
    // a 'GUNGI_PIECE_PAWN' and checkmate was achieved.

public:
  // CREATORS
  explicit Logician(void);
    // Default constructor.

  ~Logician(void);
    // Default destructor.

  // ACCESSSORS
  const std::vector<Tower>& board(void) const;
    // Returns a pointer to the underlying 'Tower' array.

  int winner(void) const;
    // Returns 'BLACK' if black won, 'WHITE' if white won, '-1' if the game is
    // not over or if there was a draw.

  const Player& black(void) const;
    // Returns a constant reference to the 'BLACK' player.

  const Player& white(void) const;
    // Returns a constant reference to the 'WHITE' player.

  const game_state_t& state(void) const;
    // Returns the current game state.

  const Unit *forcedRearrangeUnit(void) const;
    // Returns a constant pointer to the unit being forced rearranged, if any.

  const Unit *unitAtPosn(const Posn& posn, tier_t tier) const;
    // Returns a constant pointer to the Unit at the given 'tier' in the tower
    // on the board at the given 'posn'.  Note that this may be 'NULL'.

  bool isDraw(void) const;
    // Returns 'true' if the game ended in a draw, otherwise 'false'.  Note
    // that this value is meaningless unless the game has ended.

  bool isOver(void) const;
    // Returns 'true' if the game is over, otherwise 'false'.

  bool isPlayersTurn(const colour_t& colour) const;
    // Returns 'true' if it is the turn of the player specified by the given
    // 'colour', otherwise 'false'.

  bool isPlayersTurn(const Player& player) const;
    // Returns 'true' if it is the turn of the given 'player', otherwise
    // 'false'.

  bool isInitialArrangement(void) const;
    // Returns 'true' if it is the initial arrangement, otherwise 'false'.

  bool isInCheck(void) const;
    // Returns 'true' if the current player is in check, otherwise 'false'.

  bool isInCheck(const colour_t& colour) const;
    // Returns 'true' if the player of the given 'colour' is in check,
    // otherwise 'false'.  This is shorthand for
    // 'isPlayersTurn(colour) && isInCheck()'.  Note that a player can only be
    // in check on their own turn.

  bool isInCheck(const Player& player) const;
    // Returns 'true' if the given 'player' is in check, otherwise 'false'.
    // This is shorthand for 'isPlayersTurn(player) && isInCheck()'.  Note that
    // a player can only be in check on their own turn.

  bool isInCheckmate(void) const;
    // Returns 'true' if the current player is in checkmate, otherwise 'false'.

  bool isInCheckmate(const colour_t& colour) const;
    // Returns 'true' if the player of the given 'colour' is in checkmate,
    // otherwise 'false'.  This is shorthand for
    // 'isPlayersTurn(colour) && isInCheckmate()'.  Note that a player can only
    // be in checkmate on their own turn.

  bool isInCheckmate(const Player& player) const;
    // Returns 'true' if the given 'player' is in checkmate, otherwise 'false'.
    // This is shorthand for 'isPlayersTurn(player) && isInCheckmate()'.  Note
    // that a player can only be in checkmate on their own turn.

  bool isInverted(const colour_t& colour) const;
    // Returns 'true' if the player specified by the given 'colour' has their
    // movements inverted, otherwise 'false'.

  bool isInverted(const Player& player) const;
    // Returns' true' if the movements of the 'player' are inverted.

  bool isInTerritory(const Posn& posn, const colour_t& colour) const;
    // Returns 'true' if the given 'posn' is in the territory of the player
    // specified by the given 'colour', otherwise 'false'.

  bool isInTerritory(const Posn& posn, const Player& player) const;
    // Returns 'true' if the given 'posn' is in the territory of the given
    // 'player', otherwise 'false'.

  bool isInMobileRangeExpansion(const Posn& posn, colour_t colour) const;
    // Returns 'true' if the given 'posn' is in the mobile range expansion
    // region for the player with the given 'colour', otherwise 'false'.

  bool isForcedRearrangement(void) const;
    // Returns 'true' if a forced rearrangement must be performed, otherwise
    // 'false'.

  bool isForcedRearrangeForPlayer(const colour_t& colour) const;
    // Returns 'true' if it is the turn of the player corresponding to the
    // given 'colour', and they must perform a forced rearrangement, otherwise
    // 'false'.

  bool isForcedRearrangeForPlayer(const Player& player) const;
    // Returns 'true' if it is the turn of the given 'player', and they must
    // perform a forced rearrangement, otherwise 'false'.

  bool isForcedRecovery(void) const;
    // Returns 'true' if a forced recovery must be performed, otherwise
    // 'false'.  By performed, it means that the player must either opt to
    // recover or not recover their unit.

  bool isForcedRecoveryForPlayer(const colour_t& colour) const;
    // Returns 'true' if it is the turn of the player corresponding to the
    // given 'colour' and they must perform a forced recovery.

  bool isForcedRecoveryForPlayer(const Player& player) const;
    // Returns 'true' if it is the turn of the given 'player' and they must
    // perform a forced recovery.

  bool isValidDrop(const Posn& posn, const Unit& unit, error_t& error) const;
    // Returns 'true' if the given 'unit' can be dropped on the given 'posn',
    // otherwise 'false'.  Stores in the given output parameter, 'error', an
    // error value indicating why the drop is invalid if returning 'false',
    // otherwise 'GUNGI_ERROR_NONE'.  The error statuses are as follows:
    // 'GUNGI_ERROR_GAME_OVER' if the game has ended in a winner or a draw
    // thus no moves are valid, 'GUNGI_ERROR_INVALID_UNIT' if the given
    // 'unit' is in a tower or if there is a different unit to force rearrange
    // for that player first, 'GUNGI_ERROR_CHECK' if the player of the given
    // 'unit' is in check and the drop would not move the player out of check,
    // 'GUNGI_ERROR_TERRITORY' if the given 'unit' must be dropped within the
    // player's territory, 'GUNGI_ERROR_FULL_TOWER' if the tower is full,
    // 'GUNGI_ERROR_PAWN_FILE' if there is already a pawn in the file,
    // 'GUNGI_ERROR_BRONZE_FILE' if there is already a bronze in the file,
    // 'GUNGI_ERROR_LAND_LINK' if the unit can only be dropped on pieces with
    // the effect 'GUNGI_EFFECT_LAND_LINK', 'GUNGI_ERROR_NO_TOWER' if the
    // drop would be on a Commander, 'GUNGI_ERROR_NO_STACK' if the given
    // unit is a Catapult attempting to drop on a non-empty Tower,
    // 'GUNGI_ERROR_NO_STACK' if the given unit is a a Fortress attempting to
    // drop on a non-empty Tower, 'GUNGI_ERROR_BACK_ONLY' if the given 'unit'
    // is a front piece and would be dropped on a Spy,
    // 'GUNGI_ERROR_FRONT_ONLY' if the given 'unit' is a back piece and would
    // be dropped on a Clandestinite, 'GUNGI_ERROR_DUPLICATE' if the given
    // 'unit' is attempting to drop into a tower that already contains a unit
    // of the same team and front identifier, or 'GUNGI_ERROR_INVALID_STATE' if
    // player has to indicate if they're recovering a unit.

  bool isValidMove(const Posn& target,
                   const Unit& unit,
                   PosnSet&    walk,
                   error_t&    error) const;
    // Returns 'true' if the given 'unit' can move to the given 'Posn',
    // 'target', otherwise 'false'.  Returns in the given output parameter,
    // 'walk', the path for the given 'unit' to reach the given 'target'.
    // Returns 'GUNGI_ERROR_NONE' in the given output parameter, 'error', on
    // success.  Other error statuses are: 'GUNGI_ERROR_GAME_OVER' if a
    // player is in checkmate or a draw so there are no more valid moves,
    // 'GUNGI_ERROR_INVALID_UNIT' if the given 'unit' is in a player's hand,
    // 'GUNGI_ERROR_CHECK' if the move would not move the current player out of
    // check, 'GUNGI_ERROR_FULL_TOWER' if the target tower is full, and the top
    // unit cannot be usurped, 'GUNGI_ERROR_BRONZE_FILE' if the given 'unit' is
    // a bronze unit and moving it would move it into a file with another
    // bronze of the same team, 'GUNGI_ERROR_NO_WALK' if there is no path for
    // the given 'unit' to reach the given 'target' position,
    // 'GUNGI_ERROR_DROPS_ONLY' if it is initial arrangement, or
    // 'GUNGI_ERROR_INVALID_STATE' if moves cannot be performed at this time.

  bool isValidImmobileStrike(tier_t      tier,
                             const Unit& unit,
                             error_t&    error) const;
    // Returns 'true' if the given 'unit' can perform an immobile strike on the
    // unit at the given 'tier_t', 'tier', in the unit's tower.  Returns an
    // error code of 'GUNGI_ERROR_NONE' to the given output parameter, 'error',
    // on success, 'GUNGI_ERROR_INVALID_UNIT' if the given 'unit' is a not a
    // member of a tower, 'GUNGI_ERROR_GAME_OVER' if the game is in a draw or
    // checkmate, 'GUNGI_ERROR_OUT_OF_RANGE' if there is no Unit at the given
    // 'tier' or if the Unit at the tier cannot be reached,
    // 'GUNGI_ERROR_SAME_TEAM' if the two units are the same colour,
    // 'GUNGI_ERROR_INVALID_STATE' if immobile strikes cannot be performed at
    // this time, 'GUNGI_ERROR_DROPS_ONLY' if it is still initial arrangement,
    // or 'GUNGI_ERROR_CHECK' if the player for the given 'unit' is in check,
    // and the strike would not move them out of check.

  bool isValidExchange(const effect_t& exchange,
                       const Unit&     unit,
                       const Unit&     target,
                       error_t&        error) const;
    // Returns 'true' if the given 'unit' can invoke the given 'exchange' (i.e.
    // 'GUNGI_EFFECT_SUBSTITUTION' or 'GUNGI_EFFECT_1_3_TIER_EXCHANGE') with
    // the given 'Unit', 'target', otherwise 'false'.  Returns
    // 'GUNGI_ERROR_NONE' if an exhcange exists and is valid,
    // 'GUNGI_ERROR_INVALID_UNIT' if the unit is not placed or does not have
    // an exchange effect, 'GUNGI_ERROR_IMMUNE' if the given 'target' is immune
    // to the exchange, 'GUNGI_ERROR_INVALID_EXCHANGE' if the effect is a
    // 'GUNGI_EFFECT_1_3_TIER_EXCHANGE' and it is invalid,
    // 'GUNGI_ERROR_INVALID_SUB' if the effect is a 'GUNGI_EFFECT_SUBSTITUTION'
    // and it is invalid, 'GUNGI_ERROR_CHECK' if the effect is a
    // 'GUNGI_EFFECT_SUBSTITUTION' and the unit's team's Commander is in check,
    // 'GUNGI_ERROR_DROPS_ONLY' if it is still initial arrangement, or
    // 'GUNGI_ERROR_INVALID_STATE' if exchanges cannot be performed at this
    // time.

  // MANIPULATAORS
  void newGame(void);
    // Sets up a new game.  'BLACK' goes first.

  void moveUnit(const Posn& from, tier_t tier, const Posn& to, error_t& error);
    // Moves the unit at the given 'tier' in the Tower at the given 'Posn',
    // 'from', to the given 'Posn', 'to'.  Returns an error status code of
    // 'GUNGI_ERROR_NONE' to the given 'error' on success, otherwise failure.

  void moveUnit(const Unit& unit, const Posn& posn, error_t& error);
    // Moves the given 'unit' to the given 'posn', capturing the 'unit' at the
    // top of the tower at the given 'posn' if it is a different team,
    // otherwise moves without capture.  Return an error status of
    // 'GUNGI_ERROR_NONE' to the given 'error' on success,
    // 'GUNGI_ERROR_NOT_TURN' if it is not the turn of the player whose 'unit'
    // that is, otherwise the same error codes as 'isValidMove()'.

  void dropUnit(piece_id_t  front,
                piece_id_t  back,
                const Posn& to,
                error_t&    error);
    // Drops a 'Unit' from the current player's hand that has the given front,
    // 'front', and given back, 'back', to the given 'Posn', 'to'.  Returns an
    // error status of 'GUNGI_ERROR_NONE' to the given 'error' on success,
    // otherwise failure.

  void dropUnit(const Unit& unit, const Posn& posn, error_t& error);
    // Performs a drop, placing the given 'unit' in the tower at the given
    // 'posn'.  Returns an error status of 'GUNGI_ERROR_NONE' to the given
    // output parameter, 'error', on success.  Returns an error status of
    // 'GUNGI_ERROR_NOT_TURN' if it is not the player's turn,
    // 'GUNGI_ERROR_BRONZE_CHECKMATE' if dropped unit was a Bronze was used to
    // achieve checkmate, 'GUNGI_ERROR_PAWN_CHECKMATE' if the dropped unit was
    // a Pawn and was used to achieve checkmate, otherwise the same error codes
    // as 'isValidDrop()'.

  void exchangeUnits(effect_t    exchange,
                     const Posn& from,
                     tier_t      fromTier,
                     const Posn& to,
                     tier_t      toTier,
                     error_t&    error);
    // Performs the given effect 'exchange' between the unit at the given tier,
    // 'fromTier', in the tower at the given 'Posn', 'from', with the unit at
    // the given tier, 'toTier', in the tower at the the given 'Posn', 'to'.
    // Returns an error status of 'GUNGI_ERROR_NONE" to the given 'error'
    // output parameter on success, otherwise failure.

  void exchangeUnits(effect_t    exchange,
                     const Unit& a,
                     const Unit& b,
                     error_t&    error);
    // Performs the given effect 'exchange' between the units 'a', and 'b'.
    // Returns an error status of 'GUNGI_ERROR_NONE' to the given output
    // parameter, 'error', on success.  Returns an error status of
    // 'GUNGI_ERROR_NOT_TURN' to the given 'error' output parameter if it is
    // not the player's turn, otherwise the same error codes as would be
    // returned by 'isValidExchange()'.

  void immobileStrike(const Posn& posn,
                      tier_t      tier,
                      tier_t      target,
                      error_t&    error);
    // Performs an immobile strike having the unit at the given 'tier' in the
    // tower at the given 'posn' attack the unit at the given tier, 'target'
    // in the same error.  Returns 'GUNGI_ERROR_NONE' to the given output
    // parameter 'error' on success, otherwise the same error codes as those
    // returned from 'isValidImmobileStrike()'.  This performs a capture.

  void immobileStrike(const Unit& unit, tier_t target, error_t& error);
    // Performs an immobile strike where the given 'unit' attacks the unit at
    // the target tier, 'target', in its tower.  Returns an error code of
    // 'GUNGI_ERROR_NONE' to the given output parameter, 'error', on success,
    // 'GUNGI_ERROR_NOT_TURN' if it is not the player's turn, otherwise the
    // same error values as 'isValidImmobileStrike()'.  This performs a
    // capture.

  void forceRecover(bool recover, error_t& error);
    // Invokes a forced recovery for the current player if 'recover' is 'true',
    // otherwise does not.  Returns 'GUNGI_ERROR_NONE' on success, otherwise
    // 'GUNGI_ERROR_INVALID_STATE' if there is no active forced recovery.

  // OPERATORS
  friend std::ostream& operator<<(std::ostream& os, const Logician& logician);
    // Writes the underlying board of the given 'logician' out to the given
    // output stream, 'os'.  This will result in writing out the 9x9 grid with
    // three slots each for the tower's tiers.  Returns the modified output
    // stream.
};

typedef Logician Controller;

}  // close 'gungi' namespace
