// gtypes.cpp                                                         -*-C++-*-
#include "gtypes.hpp"

#include "util.hpp"

#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <exception>
#include <iostream>
#include <string>
#include <vector>

namespace gungi {

static const char *s_ERROR_MESSAGES[] = {
  [GUNGI_ERROR_NONE]             = "",
  [GUNGI_ERROR_INVALID_IDX]      = "Not a valid unit identifier.",
  [GUNGI_ERROR_NO_BACK]          = "Unit does not have a back side.",
  [GUNGI_ERROR_FULL_TOWER]       = "Tower is full.",
  [GUNGI_ERROR_NOT_A_MEMBER]     = "Unit is not a member of the tower.",
  [GUNGI_ERROR_OUT_OF_RANGE]     = "No unit at the given tower tier.",
  [GUNGI_ERROR_DUPLICATE]        = "Cannot add same unit to tower twice.",
  [GUNGI_ERROR_NO_WALK]          = "Can't reach position.",
  [GUNGI_ERROR_GAME_OVER]        = "Game is over.",
  [GUNGI_ERROR_INVALID_UNIT]     = "Unit is not valid.",
  [GUNGI_ERROR_CHECK]            = "Move/drop is not valid for escaping check.",
  [GUNGI_ERROR_TERRITORY]        = "Can only drop in territory.",
  [GUNGI_ERROR_NOT_TOP]          = "Can only move a unit at the top of a tower.",
  [GUNGI_ERROR_PAWN_FILE]        = "Cannot drop Pawn into same file as another Pawn.",
  [GUNGI_ERROR_BRONZE_FILE]      = "Cannot put Bronze into same file as another Bronze.",
  [GUNGI_ERROR_LAND_LINK]        = "Can only drop on units with 'Land Link' effect.",
  [GUNGI_ERROR_SAME_TEAM]        = "Cannot attack a unit on the same team.",
  [GUNGI_ERROR_PAWN_CHECKMATE]   = "Pawn cannot be used to achieve checkmate.",
  [GUNGI_ERROR_BRONZE_CHECKMATE] = "Bronze cannot be used to achieve checkmate.",
  [GUNGI_ERROR_NO_TOWER]         = "Tower cannot be built on unit.",
  [GUNGI_ERROR_BACK_ONLY]        = "Only back pieces can be dropped on unit.",
  [GUNGI_ERROR_FRONT_ONLY]       = "Only front pieces can be dropped on unit.",
  [GUNGI_ERROR_IMMUNE]           = "Unit is immune to effect.",
  [GUNGI_ERROR_INVALID_EXCHANGE] = "Tier exchange is invalid.",
  [GUNGI_ERROR_INVALID_SUB]      = "Substitution is invalid.",
  [GUNGI_ERROR_NO_STACK]         = "Unit cannot be dropped on another unit.",
  [GUNGI_ERROR_NOT_TURN]         = "It is not this player's turn.",
  [GUNGI_ERROR_DROPS_ONLY]       = "Only drops allowed during initial arrangement.",
  [GUNGI_ERROR_INVALID_STATE]    = "Move cannot be performed in game's current state."
};

static const char *s_UNIT_NAMES[] = {
  [GUNGI_PIECE_PAWN]          = "Pawn",
  [GUNGI_PIECE_BOW]           = "Bow",
  [GUNGI_PIECE_PRODIGY]       = "Prodigy",
  [GUNGI_PIECE_HIDDEN_DRAGON] = "Hidden Dragon",
  [GUNGI_PIECE_FORTRESS]      = "Fortress",
  [GUNGI_PIECE_CATAPULT]      = "Catapult",
  [GUNGI_PIECE_SPY]           = "Spy",
  [GUNGI_PIECE_SAMURAI]       = "Samurai",
  [GUNGI_PIECE_CAPTAIN]       = "Captain",
  [GUNGI_PIECE_COMMANDER]     = "Commander",
  [GUNGI_PIECE_BRONZE]        = "Bronze",
  [GUNGI_PIECE_SILVER]        = "Silver",
  [GUNGI_PIECE_GOLD]          = "Gold",
  [GUNGI_PIECE_ARROW]         = "Arrow",
  [GUNGI_PIECE_PHOENIX]       = "Phoenix",
  [GUNGI_PIECE_DRAGON_KING]   = "Dragon King",
  [GUNGI_PIECE_LANCE]         = "Lance",
  [GUNGI_PIECE_CLANDESTINITE] = "Clandestinite",
  [GUNGI_PIECE_PIKE]          = "Pike",
  [GUNGI_PIECE_PISTOL]        = "Pistol"
};

// CONSTANT VARIABLES
const int k_START_HAND[12][3] = {
  { GUNGI_PIECE_PAWN,          GUNGI_PIECE_BRONZE,        7 },
  { GUNGI_PIECE_PAWN,          GUNGI_PIECE_SILVER,        1 },
  { GUNGI_PIECE_PAWN,          GUNGI_PIECE_GOLD,          1 },
  { GUNGI_PIECE_BOW,           GUNGI_PIECE_ARROW,         2 },
  { GUNGI_PIECE_PRODIGY,       GUNGI_PIECE_PHOENIX,       1 },
  { GUNGI_PIECE_HIDDEN_DRAGON, GUNGI_PIECE_DRAGON_KING,   1 },
  { GUNGI_PIECE_FORTRESS,      GUNGI_PIECE_LANCE,         1 },
  { GUNGI_PIECE_CATAPULT,      GUNGI_PIECE_LANCE,         1 },
  { GUNGI_PIECE_SPY,           GUNGI_PIECE_CLANDESTINITE, 3 },
  { GUNGI_PIECE_SAMURAI,       GUNGI_PIECE_PIKE,          2 },
  { GUNGI_PIECE_CAPTAIN,       GUNGI_PIECE_PISTOL,        2 },
  { GUNGI_PIECE_COMMANDER,     GUNGI_PIECE_NONE,          1 },
};

const effect_bitfield_t k_UNIT_EFFECT[GUNGI_NUM_PIECES] = {
  [GUNGI_PIECE_PAWN]          = GUNGI_EFFECT_FORCED_RECOVERY,
  [GUNGI_PIECE_BOW]           = GUNGI_EFFECT_JUMP,
  [GUNGI_PIECE_PRODIGY]       = GUNGI_EFFECT_NONE,
  [GUNGI_PIECE_HIDDEN_DRAGON] = GUNGI_EFFECT_NONE,
  [GUNGI_PIECE_FORTRESS]      = GUNGI_EFFECT_LAND_LINK |
                                GUNGI_EFFECT_MOBILE_RANGE_EXPANSION_1 |
                                GUNGI_EFFECT_PASSIVE |
                                GUNGI_EFFECT_NO_STACK,
  [GUNGI_PIECE_CATAPULT]      = GUNGI_EFFECT_LAND_LINK |
                                GUNGI_EFFECT_MOBILE_RANGE_EXPANSION_2 |
                                GUNGI_EFFECT_NO_STACK,
  [GUNGI_PIECE_SPY]           = GUNGI_EFFECT_LAND_LINK |
                                GUNGI_EFFECT_FORCED_RECOVERY |
                                GUNGI_EFFECT_BACK_DROP_ONLY |
                                GUNGI_EFFECT_JUMP,
  [GUNGI_PIECE_SAMURAI]       = GUNGI_EFFECT_SUBSTITUTION,
  [GUNGI_PIECE_CAPTAIN]       = GUNGI_EFFECT_1_3_TIER_EXCHANGE,
  [GUNGI_PIECE_COMMANDER]     = GUNGI_EFFECT_NO_TOWER,
  [GUNGI_PIECE_BRONZE]        = GUNGI_EFFECT_BETRAYAL,
  [GUNGI_PIECE_SILVER]        = GUNGI_EFFECT_NONE,
  [GUNGI_PIECE_GOLD]          = GUNGI_EFFECT_NONE,
  [GUNGI_PIECE_ARROW]         = GUNGI_EFFECT_NONE,
  [GUNGI_PIECE_PHOENIX]       = GUNGI_EFFECT_NONE,
  [GUNGI_PIECE_LANCE]         = GUNGI_EFFECT_FORCED_REARRANGEMENT |
                                GUNGI_EFFECT_FORCED_RECOVERY,
  [GUNGI_PIECE_CLANDESTINITE] = GUNGI_EFFECT_LAND_LINK |
                                GUNGI_EFFECT_FRONT_DROP_ONLY |
                                GUNGI_EFFECT_JUMP,
  [GUNGI_PIECE_PIKE]          = GUNGI_EFFECT_NONE,
  [GUNGI_PIECE_PISTOL]        = GUNGI_EFFECT_NONE,
  [GUNGI_PIECE_DRAGON_KING]   = GUNGI_EFFECT_NONE
};

const effect_bitfield_t k_UNIT_IMMUNITY[GUNGI_NUM_PIECES] = {
  [GUNGI_PIECE_PAWN]          = GUNGI_EFFECT_NONE,
  [GUNGI_PIECE_BOW]           = GUNGI_EFFECT_NONE,
  [GUNGI_PIECE_PRODIGY]       = GUNGI_EFFECT_MOBILE_RANGE_EXPANSION_1 |
                                GUNGI_EFFECT_MOBILE_RANGE_EXPANSION_2,
  [GUNGI_PIECE_HIDDEN_DRAGON] = GUNGI_EFFECT_MOBILE_RANGE_EXPANSION_1 |
                                GUNGI_EFFECT_MOBILE_RANGE_EXPANSION_2,
  [GUNGI_PIECE_FORTRESS]      = GUNGI_EFFECT_NONE,
  [GUNGI_PIECE_CATAPULT]      = GUNGI_EFFECT_NONE,
  [GUNGI_PIECE_SPY]           = GUNGI_EFFECT_NONE,
  [GUNGI_PIECE_SAMURAI]       = GUNGI_EFFECT_NONE,
  [GUNGI_PIECE_CAPTAIN]       = GUNGI_EFFECT_NONE,
  [GUNGI_PIECE_COMMANDER]     = GUNGI_EFFECT_MOBILE_RANGE_EXPANSION_1 |
                                GUNGI_EFFECT_MOBILE_RANGE_EXPANSION_2 |
                                GUNGI_EFFECT_1_3_TIER_EXCHANGE,
  [GUNGI_PIECE_BRONZE]        = GUNGI_EFFECT_NONE,
  [GUNGI_PIECE_SILVER]        = GUNGI_EFFECT_NONE,
  [GUNGI_PIECE_GOLD]          = GUNGI_EFFECT_NONE,
  [GUNGI_PIECE_ARROW]         = GUNGI_EFFECT_NONE,
  [GUNGI_PIECE_PHOENIX]       = GUNGI_EFFECT_MOBILE_RANGE_EXPANSION_1 |
                                GUNGI_EFFECT_MOBILE_RANGE_EXPANSION_2,
  [GUNGI_PIECE_LANCE]         = GUNGI_EFFECT_NONE,
  [GUNGI_PIECE_CLANDESTINITE] = GUNGI_EFFECT_NONE,
  [GUNGI_PIECE_PIKE]          = GUNGI_EFFECT_NONE,
  [GUNGI_PIECE_PISTOL]        = GUNGI_EFFECT_NONE,
  [GUNGI_PIECE_DRAGON_KING]   = GUNGI_EFFECT_MOBILE_RANGE_EXPANSION_1 |
                                GUNGI_EFFECT_MOBILE_RANGE_EXPANSION_2
};

const moveset_t k_UNIT_MOVES[GUNGI_NUM_PIECES] = {
  [GUNGI_PIECE_PAWN] = {
    [0] = {
      {
        { GUNGI_MOVE_DIR_UP, GUNGI_MOVE_MOD_NONE }
      }
    },
    [1] = {
      {
        { GUNGI_MOVE_DIR_UP, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_LEFT, GUNGI_MOVE_MOD_NONE },
        { GUNGI_MOVE_DIR_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_RIGHT, GUNGI_MOVE_MOD_NONE },
        { GUNGI_MOVE_DIR_RIGHT, GUNGI_MOVE_MOD_NONE }
      }
    },
    [2] = {
      {
        { GUNGI_MOVE_DIR_UP_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_UP_RIGHT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_LEFT, GUNGI_MOVE_MOD_NONE },
        { GUNGI_MOVE_DIR_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_RIGHT, GUNGI_MOVE_MOD_NONE },
        { GUNGI_MOVE_DIR_RIGHT, GUNGI_MOVE_MOD_NONE }
      }
    }
  },
  [GUNGI_PIECE_BOW] = {
    [0] = {
      {
        { GUNGI_MOVE_DIR_UP, GUNGI_MOVE_MOD_NONE },
        { GUNGI_MOVE_DIR_UP, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_LEFT, GUNGI_MOVE_MOD_NONE },
        { GUNGI_MOVE_DIR_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_RIGHT, GUNGI_MOVE_MOD_NONE },
        { GUNGI_MOVE_DIR_RIGHT, GUNGI_MOVE_MOD_NONE }
      }
    },
    [1] = {
      {
        { GUNGI_MOVE_DIR_UP, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_UP_LEFT, GUNGI_MOVE_MOD_NONE },
        { GUNGI_MOVE_DIR_UP_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_UP_RIGHT, GUNGI_MOVE_MOD_NONE },
        { GUNGI_MOVE_DIR_UP_RIGHT, GUNGI_MOVE_MOD_NONE }
      }
    },
    [2] = {
      {
        { GUNGI_MOVE_DIR_UP_LEFT, GUNGI_MOVE_MOD_NONE },
        { GUNGI_MOVE_DIR_UP_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_UP_RIGHT, GUNGI_MOVE_MOD_NONE },
        { GUNGI_MOVE_DIR_UP_RIGHT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_LEFT, GUNGI_MOVE_MOD_NONE },
        { GUNGI_MOVE_DIR_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_RIGHT, GUNGI_MOVE_MOD_NONE },
        { GUNGI_MOVE_DIR_RIGHT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN, GUNGI_MOVE_MOD_NONE },
        { GUNGI_MOVE_DIR_DOWN, GUNGI_MOVE_MOD_NONE }
      }
    }
  },
  [GUNGI_PIECE_PRODIGY] = {
    [0] = {
      {
        { GUNGI_MOVE_DIR_UP_LEFT, GUNGI_MOVE_MOD_UNLIMITED }
      },
      {
        { GUNGI_MOVE_DIR_UP_RIGHT, GUNGI_MOVE_MOD_UNLIMITED }
      },
      {
        { GUNGI_MOVE_DIR_DOWN_LEFT, GUNGI_MOVE_MOD_UNLIMITED }
      },
      {
        { GUNGI_MOVE_DIR_DOWN_RIGHT, GUNGI_MOVE_MOD_UNLIMITED }
      }
    },
    [1] = {
      {
        { GUNGI_MOVE_DIR_UP, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_RIGHT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN, GUNGI_MOVE_MOD_NONE }
      }
    },
    [2] = {
      {
        { GUNGI_MOVE_DIR_UP, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_RIGHT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN, GUNGI_MOVE_MOD_NONE }
      }
    }
  },
  [GUNGI_PIECE_HIDDEN_DRAGON] = {
    [0] = {
      {
        { GUNGI_MOVE_DIR_UP, GUNGI_MOVE_MOD_UNLIMITED }
      },
      {
        { GUNGI_MOVE_DIR_LEFT, GUNGI_MOVE_MOD_UNLIMITED }
      },
      {
        { GUNGI_MOVE_DIR_RIGHT, GUNGI_MOVE_MOD_UNLIMITED }
      },
      {
        { GUNGI_MOVE_DIR_DOWN, GUNGI_MOVE_MOD_UNLIMITED }
      }
    },
    [1] = {
      {
        { GUNGI_MOVE_DIR_UP_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_UP_RIGHT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN_RIGHT, GUNGI_MOVE_MOD_NONE }
      }
    },
    [2] = {
      {
        { GUNGI_MOVE_DIR_UP_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_UP_RIGHT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN_RIGHT, GUNGI_MOVE_MOD_NONE }
      }
    }
  },
  [GUNGI_PIECE_FORTRESS] = {
    [0] = {
    },
    [1] = {
    },
    [2] = {
    }
  },
  [GUNGI_PIECE_CATAPULT] = {
    [0] = {
    },
    [1] = {
    },
    [2] = {
    }
  },
  [GUNGI_PIECE_SPY] = {
    [0] = {
      {
        { GUNGI_MOVE_DIR_UP_LEFT, GUNGI_MOVE_MOD_NONE },
        { GUNGI_MOVE_DIR_UP, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_UP_RIGHT, GUNGI_MOVE_MOD_NONE },
        { GUNGI_MOVE_DIR_UP, GUNGI_MOVE_MOD_NONE }
      }
    },
    [1] = {
      {
        { GUNGI_MOVE_DIR_UP_LEFT, GUNGI_MOVE_MOD_NONE },
        { GUNGI_MOVE_DIR_UP, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_UP_RIGHT, GUNGI_MOVE_MOD_NONE },
        { GUNGI_MOVE_DIR_UP, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_UP_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_UP_RIGHT, GUNGI_MOVE_MOD_NONE }
      }
    },
    [2] = {
      {
        { GUNGI_MOVE_DIR_UP_LEFT, GUNGI_MOVE_MOD_NONE },
        { GUNGI_MOVE_DIR_UP, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_UP_RIGHT, GUNGI_MOVE_MOD_NONE },
        { GUNGI_MOVE_DIR_UP, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_UP_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_UP_RIGHT, GUNGI_MOVE_MOD_NONE }
      }
    }
  },
  [GUNGI_PIECE_SAMURAI] = {
    [0] = {
      {
        { GUNGI_MOVE_DIR_UP, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_RIGHT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_UP_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_UP_RIGHT, GUNGI_MOVE_MOD_NONE }
      }
    },
    [1] = {
      {
        { GUNGI_MOVE_DIR_UP, GUNGI_MOVE_MOD_NONE },
        { GUNGI_MOVE_DIR_UP, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_UP_RIGHT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_UP_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_RIGHT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN, GUNGI_MOVE_MOD_NONE },
        { GUNGI_MOVE_DIR_DOWN, GUNGI_MOVE_MOD_NONE }
      }
    },
    [2] = {
      {
        { GUNGI_MOVE_DIR_UP, GUNGI_MOVE_MOD_NONE },
        { GUNGI_MOVE_DIR_UP, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_UP_RIGHT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_UP_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_RIGHT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN, GUNGI_MOVE_MOD_NONE },
        { GUNGI_MOVE_DIR_DOWN, GUNGI_MOVE_MOD_NONE }
      }
    }
  },
  [GUNGI_PIECE_CAPTAIN] = {
    [0] = {
      {
        { GUNGI_MOVE_DIR_UP, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_UP_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_UP_RIGHT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN_RIGHT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN_LEFT, GUNGI_MOVE_MOD_NONE }
      }
    },
    [1] = {
      {
        { GUNGI_MOVE_DIR_UP, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_UP_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_UP_RIGHT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN_RIGHT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN_LEFT, GUNGI_MOVE_MOD_NONE }
      }
    },
    [2] = {
      {
        { GUNGI_MOVE_DIR_UP_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_UP_RIGHT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_UP_RIGHT, GUNGI_MOVE_MOD_NONE },
        { GUNGI_MOVE_DIR_UP_RIGHT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_UP_LEFT, GUNGI_MOVE_MOD_NONE },
        { GUNGI_MOVE_DIR_UP_LEFT, GUNGI_MOVE_MOD_NONE } 
      },
      {
        { GUNGI_MOVE_DIR_LEFT, GUNGI_MOVE_MOD_NONE },
        { GUNGI_MOVE_DIR_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_RIGHT, GUNGI_MOVE_MOD_NONE },
        { GUNGI_MOVE_DIR_RIGHT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN_RIGHT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN_LEFT, GUNGI_MOVE_MOD_NONE }
      }
    }
  },
  [GUNGI_PIECE_COMMANDER] = {
    [0] = {
      {
        { GUNGI_MOVE_DIR_UP, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_RIGHT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_UP_RIGHT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_UP_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN_RIGHT, GUNGI_MOVE_MOD_NONE }
      }
    },
    [1] = {
      {
        { GUNGI_MOVE_DIR_UP, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_RIGHT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_UP_RIGHT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_UP_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN_RIGHT, GUNGI_MOVE_MOD_NONE }
      }
    },
    [2] = {
      {
        { GUNGI_MOVE_DIR_UP, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_RIGHT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_UP_RIGHT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_UP_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN_RIGHT, GUNGI_MOVE_MOD_NONE }
      }
    }
  },
  [GUNGI_PIECE_BRONZE] = {
    [0] = {
      {
        { GUNGI_MOVE_DIR_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_RIGHT, GUNGI_MOVE_MOD_NONE }
      }
    },
    [1] = {
      {
        { GUNGI_MOVE_DIR_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_RIGHT, GUNGI_MOVE_MOD_NONE }
      }
    },
    [2] = {
      {
        { GUNGI_MOVE_DIR_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_RIGHT, GUNGI_MOVE_MOD_NONE }
      }
    }
  },
  [GUNGI_PIECE_SILVER] = {
    [0] = {
      {
        { GUNGI_MOVE_DIR_UP, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_RIGHT, GUNGI_MOVE_MOD_NONE }
      }
    },
    [1] = {
      {
        { GUNGI_MOVE_DIR_UP_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_UP_RIGHT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN_RIGHT, GUNGI_MOVE_MOD_NONE }
      }
    },
    [2] = {
      {
        { GUNGI_MOVE_DIR_UP_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_UP_RIGHT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN_RIGHT, GUNGI_MOVE_MOD_NONE }
      }
    }
  },
  [GUNGI_PIECE_GOLD] = {
    [0] = {
      {
        { GUNGI_MOVE_DIR_UP, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_RIGHT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_UP_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_UP_RIGHT, GUNGI_MOVE_MOD_NONE }
      }
    },
    [1] = {
      {
        { GUNGI_MOVE_DIR_UP, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_RIGHT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_UP_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_UP_RIGHT, GUNGI_MOVE_MOD_NONE }
      }
    },
    [2] = {
      {
        { GUNGI_MOVE_DIR_UP, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_RIGHT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_UP_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_UP_RIGHT, GUNGI_MOVE_MOD_NONE }
      }
    }
  },
  [GUNGI_PIECE_ARROW] = {
    [0] = {
      {
        { GUNGI_MOVE_DIR_UP, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN_RIGHT, GUNGI_MOVE_MOD_NONE }
      }
    },
    [1] = {
      {
        { GUNGI_MOVE_DIR_UP, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN_LEFT, GUNGI_MOVE_MOD_NONE },
        { GUNGI_MOVE_DIR_DOWN_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN_RIGHT, GUNGI_MOVE_MOD_NONE },
        { GUNGI_MOVE_DIR_DOWN_RIGHT, GUNGI_MOVE_MOD_NONE }
      }
    },
    [2] = {
      {
        { GUNGI_MOVE_DIR_UP, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN_RIGHT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN_LEFT, GUNGI_MOVE_MOD_NONE },
        { GUNGI_MOVE_DIR_DOWN_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN_RIGHT, GUNGI_MOVE_MOD_NONE },
        { GUNGI_MOVE_DIR_DOWN_RIGHT, GUNGI_MOVE_MOD_NONE }
      }
    }
  },
  [GUNGI_PIECE_PHOENIX] = {
    [0] = {
      {
        { GUNGI_MOVE_DIR_UP_LEFT, GUNGI_MOVE_MOD_UNLIMITED }
      },
      {
        { GUNGI_MOVE_DIR_UP_RIGHT, GUNGI_MOVE_MOD_UNLIMITED }
      },
      {
        { GUNGI_MOVE_DIR_DOWN_LEFT, GUNGI_MOVE_MOD_UNLIMITED }
      },
      {
        { GUNGI_MOVE_DIR_DOWN_RIGHT, GUNGI_MOVE_MOD_UNLIMITED }
      },
      {
        { GUNGI_MOVE_DIR_UP, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_RIGHT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN, GUNGI_MOVE_MOD_NONE }
      }
    },
    [1] = {
      {
        { GUNGI_MOVE_DIR_UP, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_RIGHT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN, GUNGI_MOVE_MOD_NONE }
      }
    },
    [2] = {
      {
        { GUNGI_MOVE_DIR_UP, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_RIGHT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN, GUNGI_MOVE_MOD_NONE }
      }
    }
  },
  [GUNGI_PIECE_LANCE] = {
    [0] = {
      {
        { GUNGI_MOVE_DIR_UP, GUNGI_MOVE_MOD_UNLIMITED }
      }
    },
    [1] = {
      {
        { GUNGI_MOVE_DIR_UP_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_UP_RIGHT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN_RIGHT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN_LEFT, GUNGI_MOVE_MOD_NONE }
      }
    },
    [2] = {
      {
        { GUNGI_MOVE_DIR_UP_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_UP_RIGHT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN_RIGHT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN_LEFT, GUNGI_MOVE_MOD_NONE }
      }
    }
  },
  [GUNGI_PIECE_CLANDESTINITE] = {
    [0] = {
      {
        { GUNGI_MOVE_DIR_UP_LEFT, GUNGI_MOVE_MOD_NONE },
        { GUNGI_MOVE_DIR_UP, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_UP_RIGHT, GUNGI_MOVE_MOD_NONE },
        { GUNGI_MOVE_DIR_UP, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN, GUNGI_MOVE_MOD_NONE }
      }
    },
    [1] = {
      {
        { GUNGI_MOVE_DIR_UP_LEFT, GUNGI_MOVE_MOD_NONE },
        { GUNGI_MOVE_DIR_UP, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_UP_RIGHT, GUNGI_MOVE_MOD_NONE },
        { GUNGI_MOVE_DIR_UP, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_UP_RIGHT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_UP_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN, GUNGI_MOVE_MOD_NONE }
      }
    },
    [2] = {
      {
        { GUNGI_MOVE_DIR_UP_LEFT, GUNGI_MOVE_MOD_NONE },
        { GUNGI_MOVE_DIR_UP, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_UP_RIGHT, GUNGI_MOVE_MOD_NONE },
        { GUNGI_MOVE_DIR_UP, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_UP_RIGHT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_UP_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN_LEFT, GUNGI_MOVE_MOD_NONE },
        { GUNGI_MOVE_DIR_DOWN, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN_RIGHT, GUNGI_MOVE_MOD_NONE },
        { GUNGI_MOVE_DIR_DOWN, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN_LEFT, GUNGI_MOVE_MOD_NONE },
        { GUNGI_MOVE_DIR_DOWN_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN_RIGHT, GUNGI_MOVE_MOD_NONE },
        { GUNGI_MOVE_DIR_DOWN_RIGHT, GUNGI_MOVE_MOD_NONE }
      }
    }
  },
  [GUNGI_PIECE_PIKE] = {
    [0] = {
      {
        { GUNGI_MOVE_DIR_UP, GUNGI_MOVE_MOD_NONE },
        { GUNGI_MOVE_DIR_UP, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_UP, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_RIGHT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN, GUNGI_MOVE_MOD_NONE }
      }
    },
    [1] = {
      {
        { GUNGI_MOVE_DIR_UP_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_UP_RIGHT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN_RIGHT, GUNGI_MOVE_MOD_NONE }
      }
    },
    [2] = {
      {
        { GUNGI_MOVE_DIR_UP_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_UP_RIGHT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN_RIGHT, GUNGI_MOVE_MOD_NONE }
      }
    }
  },
  [GUNGI_PIECE_PISTOL] = {
    [0] = {
      {
        { GUNGI_MOVE_DIR_UP_RIGHT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_UP_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN_RIGHT, GUNGI_MOVE_MOD_NONE }
      }
    },
    [1] = {
      {
        { GUNGI_MOVE_DIR_UP, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_RIGHT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN, GUNGI_MOVE_MOD_NONE }
      }
    },
    [2] = {
      {
        { GUNGI_MOVE_DIR_UP, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_RIGHT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN, GUNGI_MOVE_MOD_NONE }
      }
    }
  },
  [GUNGI_PIECE_DRAGON_KING] = {
    [0] = {
      {
        { GUNGI_MOVE_DIR_UP, GUNGI_MOVE_MOD_UNLIMITED }
      },
      {
        { GUNGI_MOVE_DIR_LEFT, GUNGI_MOVE_MOD_UNLIMITED }
      },
      {
        { GUNGI_MOVE_DIR_RIGHT, GUNGI_MOVE_MOD_UNLIMITED }
      },
      {
        { GUNGI_MOVE_DIR_DOWN, GUNGI_MOVE_MOD_UNLIMITED }
      },
      {
        { GUNGI_MOVE_DIR_UP_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_UP_RIGHT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN_RIGHT, GUNGI_MOVE_MOD_NONE }
      }
    },
    [1] = {
      {
        { GUNGI_MOVE_DIR_UP_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_UP_RIGHT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN_RIGHT, GUNGI_MOVE_MOD_NONE }
      }
    },
    [2] = {
      {
        { GUNGI_MOVE_DIR_UP_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_UP_RIGHT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN_LEFT, GUNGI_MOVE_MOD_NONE }
      },
      {
        { GUNGI_MOVE_DIR_DOWN_RIGHT, GUNGI_MOVE_MOD_NONE }
      }
    }
  },
};

static const char *s_GN_IDENTIFIERS[] = {
  [GUNGI_PIECE_ARROW]         = "A",
  [GUNGI_PIECE_BOW]           = "B",
  [GUNGI_PIECE_BRONZE]        = "Z",
  [GUNGI_PIECE_CAPTAIN]       = "C",
  [GUNGI_PIECE_CATAPULT]      = "T",
  [GUNGI_PIECE_CLANDESTINITE] = "N",
  [GUNGI_PIECE_COMMANDER]     = "O",
  [GUNGI_PIECE_DRAGON_KING]   = "K",
  [GUNGI_PIECE_FORTRESS]      = "F",
  [GUNGI_PIECE_GOLD]          = "G",
  [GUNGI_PIECE_HIDDEN_DRAGON] = "H",
  [GUNGI_PIECE_LANCE]         = "L",
  [GUNGI_PIECE_PAWN]          = "P",
  [GUNGI_PIECE_PHOENIX]       = "X",
  [GUNGI_PIECE_PIKE]          = "E",
  [GUNGI_PIECE_PISTOL]        = "I",
  [GUNGI_PIECE_PRODIGY]       = "R",
  [GUNGI_PIECE_SAMURAI]       = "S",
  [GUNGI_PIECE_SILVER]        = "V",
  [GUNGI_PIECE_SPY]           = "Y"
};


                                // =========
                                // FUNCTIONS
                                // =========

const char *error_to_string(error_t err) {
  GASSERT(err < GUNGI_NUM_ERRORS);
  return s_ERROR_MESSAGES[err];
}

const char *piece_to_string(piece_id_t piece) {
  GASSERT(Util::isValidPiece(piece));
  return s_UNIT_NAMES[piece];
}

const char *piece_to_gn_identifier(piece_id_t piece) {
  if (!Util::isValidPiece(piece)) {
    return "-";
  }
  return s_GN_IDENTIFIERS[piece];
}

piece_id_t gn_identifier_to_piece(const char *id) {
  std::string strId(id);
  strId = Util::toUpperCase(strId);

  for (int i = GUNGI_PIECE_NONE + 1; i < GUNGI_NUM_PIECES; i++) {
    piece_id_t piece = static_cast<piece_id_t>(i);
    std::string pieceId(piece_to_gn_identifier(piece));
    if (strId == pieceId) {
      return piece;
    }
  }

  return GUNGI_PIECE_NONE;
}

void gassert(const char  *file,
             unsigned int line,
             const char  *condition,
             bool         status) {
  gassert(file, line, condition, status, "");
}

void gassert(const char  *file,
             unsigned int line,
             const char  *condition,
             bool         status,
             const char  *message,
             ...) {
  if (!status) {
    std::cerr
      << "Assertion '"
      << condition
      << "' failed in "
      << file
      << ":"
      << line;

    if (message) {
      va_list va;
      va_start(va, message);

      char buffer[strlen(message) + 500];
      vsprintf(buffer, message, va);

      std::cerr
        << ": "
        << buffer
        << std::endl;

      va_end(va);
    } else {
      std::cerr << std::endl;
    }

    std::terminate();
  }
}

}  // close 'gungi' namespace
