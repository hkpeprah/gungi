// gtypes.hpp                                                         -*-C++-*-
#pragma once
//@DESCRIPTION:
//  This component provides the base types and identifiers used throughout the
//  Gungi game library.  Used for identifying pieces, specifying moves, and
//  piece effects.
//
//@TYPES:
//  'gungi::colour_t': enumeration specifying the player colours.
//  'gungi::piece_id_t': enumeration specifying the identifiers for pieces.
//  'gungi::move_dir_t': enumeration specifying the different move directions.
//  'gungi::move_mod_t': enumeration specifying how to interpret a move.
//  'gungi::move_t': structure representing a move which is a mod and dir pair.
//  'gungi::move_vector_t': type definition for a vector of moves.
//  'gungi::moveset_t': type definition for the piece moves in a tower.
//  'gungi::moveset_ptr_t': type definition for a pointer to a move vector.
//  'gungi::const_moveset_ptr_t': constant 'gungi::moveset_ptr'.
//  'gungi::effect_t': enumeration specifying the identifiers for effects.
//  'gungi::effect_bitfield_t': type definition for effect bitfields
//  'gungi::tier_t': type definition for tiers in a tower.
//  'gungi::error_t': enumeration for the various different error codes.
//
//@CONSTANTS:
//  'gungi::k_MAX_POSITION_REPETITIONS': constant for max position repetitions.
//  'gungi::k_MAX_TOWER_SIZE': constant for the max size of a tower.
//  'gungi::k_START_PIECE_COUNT': constant for a player's starting piece count.
//  'gungi::k_BOARD_LENGTH': length of the gungi board.
//  'gungi::k_UNIT_EFFECT': array of unit effect bitfields.
//  'gungi::k_UNIT_IMMUNITY': array of unit immunity bitfields.
//  'gungi::k_UNIT_MOVES': array of unit movesets.
//  'gungi::k_START_HAND': starting hand for a player.
#include <vector>

namespace gungi {

#define GASSERT(cond, ...) gassert(__FILE__, __LINE__, #cond, (cond), ## __VA_ARGS__)

const unsigned int k_MAX_POSITION_REPETITIONS = 4;
  // Maximum number of times the position on a board can repeat.

const unsigned int k_MAX_TOWER_SIZE = 3;
  // The maximum size a tower can be in Gungi.

const unsigned int k_START_PIECE_COUNT = 23;
  // Count of the total number of pieces each player starts with at the start
  // of the game.

const unsigned int k_BOARD_LENGTH = 9;
  // Length of the board.  Since the board is square, the total size of the
  // board is 'k_BOARD_LENGTH * k_BOARD_LENGTH'.

typedef enum colour_t {
  // The different player colours.
  WHITE = (1 << 0),
  BLACK = (1 << 1),
} colour_t;

typedef enum piece_id_t {
  GUNGI_PIECE_NONE = -1,
  GUNGI_PIECE_PAWN = 0,
  GUNGI_PIECE_BOW,
  GUNGI_PIECE_PRODIGY,
  GUNGI_PIECE_HIDDEN_DRAGON,
  GUNGI_PIECE_FORTRESS,
  GUNGI_PIECE_CATAPULT,
  GUNGI_PIECE_SPY,
  GUNGI_PIECE_SAMURAI,
  GUNGI_PIECE_CAPTAIN,
  GUNGI_PIECE_COMMANDER,
  GUNGI_PIECE_BRONZE,
  GUNGI_PIECE_SILVER,
  GUNGI_PIECE_GOLD,
  GUNGI_PIECE_ARROW,
  GUNGI_PIECE_PHOENIX,
  GUNGI_PIECE_DRAGON_KING,
  GUNGI_PIECE_LANCE,
  GUNGI_PIECE_CLANDESTINITE,
  GUNGI_PIECE_PIKE,
  GUNGI_PIECE_PISTOL,
  GUNGI_NUM_PIECES
} piece_id_t;

typedef enum move_dir_t {
  // Placeholder.
  GUNGI_MOVE_DIR_NONE  = 0,

  // Moves are written as bit fields.  So diagonal movement can be written as
  // 'GUNGI_MOVE_DIR_UP | GUNGI_MOVE_DIR_LEFT', which would correspond to up
  // and to the left (left diagonal).
  GUNGI_MOVE_DIR_UP    = (1 << 1),
  GUNGI_MOVE_DIR_DOWN  = (1 << 2),
  GUNGI_MOVE_DIR_LEFT  = (1 << 3),
  GUNGI_MOVE_DIR_RIGHT = (1 << 4),

  // Combination movements from the above list.
  GUNGI_MOVE_DIR_UP_LEFT    = GUNGI_MOVE_DIR_UP   | GUNGI_MOVE_DIR_LEFT,
  GUNGI_MOVE_DIR_UP_RIGHT   = GUNGI_MOVE_DIR_UP   | GUNGI_MOVE_DIR_RIGHT,
  GUNGI_MOVE_DIR_DOWN_LEFT  = GUNGI_MOVE_DIR_DOWN | GUNGI_MOVE_DIR_LEFT,
  GUNGI_MOVE_DIR_DOWN_RIGHT = GUNGI_MOVE_DIR_DOWN | GUNGI_MOVE_DIR_RIGHT
} move_dir_t;

typedef enum move_mod_t {
  // No modifier for the movement.
  GUNGI_MOVE_MOD_NONE,

  // Movement is unlimited in the given movement direction.
  GUNGI_MOVE_MOD_UNLIMITED
} move_mod_t;

typedef struct move_t {
  // Direction of the movement.
  move_dir_t move_direction;

  // Modifier for the particular movement.
  move_mod_t move_modifier;
} move_t;

typedef std::vector<std::vector<move_t> > move_vector_t;
  // Type definition for a vector of moves.  A piece has multiple moves it can
  // make based on its height within a tower.  This vector is used to contain
  // the moves at a particular height.

typedef move_vector_t moveset_t[k_MAX_TOWER_SIZE];
  // Type definition for a set of moves.  A piece has a different list of moves
  // based on its height within a tower; the set is the entire collection of
  // the available moves for every height.

typedef move_vector_t* moveset_ptr_t;
  // Type definition for a pointer to a move vector.

typedef move_vector_t const * const_moveset_ptr_t;
  // Type definition for a pointer to an array of constant move vectors.

typedef enum effect_t {
  // Placeholder.
  GUNGI_EFFECT_NONE                     = 0,

  // Standard set of effects.
  GUNGI_EFFECT_LAND_LINK                = (1 << 1),
  GUNGI_EFFECT_MOBILE_RANGE_EXPANSION_1 = (1 << 2), // Fortress
  GUNGI_EFFECT_MOBILE_RANGE_EXPANSION_2 = (1 << 3), // Catapult
  GUNGI_EFFECT_1_3_TIER_EXCHANGE        = (1 << 4),
  GUNGI_EFFECT_SUBSTITUTION             = (1 << 5),
  GUNGI_EFFECT_BETRAYAL                 = (1 << 6),
  GUNGI_EFFECT_FORCED_RECOVERY          = (1 << 7),
  GUNGI_EFFECT_FORCED_REARRANGEMENT     = (1 << 8),

  // Pieces with the no tower effect cannot be stacked upon.
  GUNGI_EFFECT_NO_TOWER                 = (1 << 9),

  // Pieces with the passive effect cannot attack.
  GUNGI_EFFECT_PASSIVE                  = (1 << 10),

  // Pieces with the 'no stack' effect cannot stack on other units.
  GUNGI_EFFECT_NO_STACK                 = (1 << 11),

  // Pieces with this effect can only have 'front' pieces dropped onto them.
  GUNGI_EFFECT_FRONT_DROP_ONLY          = (1 << 12), // Clandestinite

  // Pieces with this effect can only have 'back' pieces dropped onto them.
  GUNGI_EFFECT_BACK_DROP_ONLY           = (1 << 13), // Spy

  // Piece can jump over other pieces.
  GUNGI_EFFECT_JUMP                     = (1 << 14), // Spy, Bow, Clandestinite
} effect_t;

typedef unsigned int effect_bitfield_t;
  // Type definition for a bitfield of 'effect_t's.

typedef int tier_t;
  // Type definition for a tier.

typedef enum error_t {
  GUNGI_ERROR_NONE = 0,
  GUNGI_ERROR_INVALID_IDX,
  GUNGI_ERROR_NO_BACK,
  GUNGI_ERROR_FULL_TOWER,
  GUNGI_ERROR_NOT_A_MEMBER,
  GUNGI_ERROR_OUT_OF_RANGE,
  GUNGI_ERROR_DUPLICATE,
  GUNGI_ERROR_NO_WALK,
  GUNGI_ERROR_GAME_OVER,
  GUNGI_ERROR_INVALID_UNIT,
  GUNGI_ERROR_CHECK,
  GUNGI_ERROR_TERRITORY,
  GUNGI_ERROR_NOT_TOP,
  GUNGI_ERROR_PAWN_FILE,
  GUNGI_ERROR_BRONZE_FILE,
  GUNGI_ERROR_LAND_LINK,
  GUNGI_ERROR_SAME_TEAM,
  GUNGI_ERROR_PAWN_CHECKMATE,
  GUNGI_ERROR_BRONZE_CHECKMATE,
  GUNGI_ERROR_NO_TOWER,
  GUNGI_ERROR_BACK_ONLY,
  GUNGI_ERROR_FRONT_ONLY,
  GUNGI_ERROR_IMMUNE,
  GUNGI_ERROR_INVALID_EXCHANGE,
  GUNGI_ERROR_INVALID_SUB,
  GUNGI_ERROR_NO_STACK,
  GUNGI_ERROR_NOT_TURN,
  GUNGI_ERROR_DROPS_ONLY,
  GUNGI_ERROR_INVALID_STATE,
  GUNGI_NUM_ERRORS
} error_t;

extern const effect_bitfield_t k_UNIT_EFFECT[GUNGI_NUM_PIECES];
  // Effect bitfields specifying the effects each unit has.

extern const effect_bitfield_t k_UNIT_IMMUNITY[GUNGI_NUM_PIECES];
  // Effect bitfields specifying immunities for each unit.

extern const moveset_t k_UNIT_MOVES[GUNGI_NUM_PIECES];
  // Moveset for each unit.

extern const int k_START_HAND[12][3];
  // Multi-dimensional array specifying the number of front pieces of a
  // particular unit in the starting hand with the specified back.

                                // =========
                                // FUNCTIONS
                                // =========

const char *error_to_string(error_t err);
  // Returns a string corresponding to the error message for the given 'err'.
  // Raises an exception if the given 'err' is not a valid error identifier.

const char *piece_to_string(piece_id_t piece);
  // Returns a string corresponding to the name for the given 'piece'.  Raises
  // an assertion if the given 'id' is not a valid piece identifier.

const char *piece_to_gn_identifier(piece_id_t piece);
  // Returns a string corresponding to the Gungi Notation identifier for the
  // given piece, 'piece'.

piece_id_t gn_identifier_to_piece(const char *id);
  // Returns the piece identifier for the given 'id' if found, otherwise
  // 'GUNGI_PIECE_NONE'.

void gassert(const char  *file,
             unsigned int line,
             const char  *condition,
             bool         status);
  // Triggers an assertion if the given 'status' is 'False', else 'True',
  // printing a message to standard error, formatted with the given 'file' and
  // given 'line'.

void gassert(const char  *file,
             unsigned int line,
             const char  *condition,
             bool         status,
             const char  *message,
             ...);
  // Triggers an assertion if the given 'status' is 'False', else 'True',
  // printing a message to standard error, formatted with the given 'file' and
  // given 'line'.

}  // close 'gungi' namespace
