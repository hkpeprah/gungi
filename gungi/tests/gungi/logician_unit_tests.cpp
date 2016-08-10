// logician_unit_tests.cpp                                            -*-C++-*-
#include "logician.hpp"

#include "builder.hpp"
#include "gtypes.hpp"
#include "player.hpp"
#include "tower.hpp"
#include "unit.hpp"
#include "util.hpp"

#include <CppUTest/TestHarness.h>

#include <cstdint>
#include <vector>

using namespace gungi;

class LogicianFixture {
  // Fixture for testing the logic controller code.

public:
  // STATIC CLASS MEMBERS
  static unsigned int hasFront(const SharedUnitPtrVector& units,
                               piece_id_t                 front) {
    unsigned int count = 0;

    for (const SharedUnitPtr& unit : units) {
      if (unit->front() == front) {
        count++;
      }
    }

    return count;
  }

  static unsigned int hasBack(const SharedUnitPtrVector& units,
                              piece_id_t                 back) {
    unsigned int count = 0;

    for (const SharedUnitPtr& unit : units) {
      if (unit->back() == back) {
        count++;
      }
    }

    return count;
  }

  static unsigned int hasFrontAndBack(const SharedUnitPtrVector& units,
                                      piece_id_t                 front,
                                      piece_id_t                 back) {
    unsigned int count = 0;

    for (const SharedUnitPtr& unit : units) {
      if (unit->front() == front && unit->back() == back) {
        count++;
      }
    }

    return count;
  }
};

TEST_GROUP(LogicianTest) {
  void setup(void) {
    return;
  }

  void teardown(void) {
    return;
  }
};

TEST(LogicianTest, constructor_and_destrucator) {
  Logician logician;
}

TEST(LogicianTest, state_retuns_game_state_at_start) {
  Logician logician;

  Logician::game_state_t state = Logician::GAME_STATE_INITIAL_ARRANGEMENT |
                                 Logician::GAME_STATE_TURN_BLACK;

  CHECK_EQUAL(state, logician.state());

  Posn posn(0, 0);
  for (const Tower& tower : logician.board()) {
    CHECK_TRUE(tower.posn() == posn);

    if (posn.col() == k_BOARD_LENGTH - 1) {
      if (posn.row() == k_BOARD_LENGTH - 1) {
        continue;
      }
      posn = Posn(0, posn.row() + 1);
    } else {
      posn = Posn(posn.col() + 1, posn.row());
    }
  }
}

TEST(LogicianTest, black_returns_black_player) {
  Logician logician;

  CHECK_EQUAL(BLACK, logician.black().colour());
}

TEST(LogicianTest, black_initial_hand) {
  Logician logician;

  const SharedUnitPtrVector& units = logician.black().units();
  CHECK_EQUAL(k_START_PIECE_COUNT, units.size());

  CHECK_EQUAL(9, LogicianFixture::hasFront(units, GUNGI_PIECE_PAWN));
  CHECK_EQUAL(7, LogicianFixture::hasFrontAndBack(units, GUNGI_PIECE_PAWN, GUNGI_PIECE_BRONZE));
  CHECK_EQUAL(1, LogicianFixture::hasFrontAndBack(units, GUNGI_PIECE_PAWN, GUNGI_PIECE_SILVER));
  CHECK_EQUAL(1, LogicianFixture::hasFrontAndBack(units, GUNGI_PIECE_PAWN, GUNGI_PIECE_GOLD));

  CHECK_EQUAL(2, LogicianFixture::hasFront(units, GUNGI_PIECE_BOW));
  CHECK_EQUAL(2, LogicianFixture::hasFrontAndBack(units, GUNGI_PIECE_BOW, GUNGI_PIECE_ARROW));

  CHECK_EQUAL(1, LogicianFixture::hasFront(units, GUNGI_PIECE_PRODIGY));
  CHECK_EQUAL(1, LogicianFixture::hasFrontAndBack(units, GUNGI_PIECE_PRODIGY, GUNGI_PIECE_PHOENIX));

  CHECK_EQUAL(1, LogicianFixture::hasFront(units, GUNGI_PIECE_HIDDEN_DRAGON));
  CHECK_EQUAL(1, LogicianFixture::hasFrontAndBack(units, GUNGI_PIECE_HIDDEN_DRAGON, GUNGI_PIECE_DRAGON_KING));

  CHECK_EQUAL(1, LogicianFixture::hasFront(units, GUNGI_PIECE_FORTRESS));
  CHECK_EQUAL(1, LogicianFixture::hasFrontAndBack(units, GUNGI_PIECE_FORTRESS, GUNGI_PIECE_LANCE));

  CHECK_EQUAL(1, LogicianFixture::hasFront(units, GUNGI_PIECE_CATAPULT));
  CHECK_EQUAL(1, LogicianFixture::hasFrontAndBack(units, GUNGI_PIECE_CATAPULT, GUNGI_PIECE_LANCE));

  CHECK_EQUAL(3, LogicianFixture::hasFront(units, GUNGI_PIECE_SPY));
  CHECK_EQUAL(3, LogicianFixture::hasFrontAndBack(units, GUNGI_PIECE_SPY, GUNGI_PIECE_CLANDESTINITE));

  CHECK_EQUAL(2, LogicianFixture::hasFront(units, GUNGI_PIECE_SAMURAI));
  CHECK_EQUAL(2, LogicianFixture::hasFrontAndBack(units, GUNGI_PIECE_SAMURAI, GUNGI_PIECE_PIKE));

  CHECK_EQUAL(2, LogicianFixture::hasFront(units, GUNGI_PIECE_CAPTAIN));
  CHECK_EQUAL(2, LogicianFixture::hasFrontAndBack(units, GUNGI_PIECE_CAPTAIN, GUNGI_PIECE_PISTOL));

  CHECK_EQUAL(1, LogicianFixture::hasFront(units, GUNGI_PIECE_COMMANDER));
  CHECK_EQUAL(1, LogicianFixture::hasFrontAndBack(units, GUNGI_PIECE_COMMANDER, GUNGI_PIECE_NONE));
}

TEST(LogicianTest, white_returns_white_player) {
  Logician logician;

  CHECK_EQUAL(WHITE, logician.white().colour());
}

TEST(LogicianTest, white_initial_hand) {
  Logician logician;

  const SharedUnitPtrVector& units = logician.white().units();
  CHECK_EQUAL(k_START_PIECE_COUNT, units.size());

  CHECK_EQUAL(9, LogicianFixture::hasFront(units, GUNGI_PIECE_PAWN));
  CHECK_EQUAL(7, LogicianFixture::hasFrontAndBack(units, GUNGI_PIECE_PAWN, GUNGI_PIECE_BRONZE));
  CHECK_EQUAL(1, LogicianFixture::hasFrontAndBack(units, GUNGI_PIECE_PAWN, GUNGI_PIECE_SILVER));
  CHECK_EQUAL(1, LogicianFixture::hasFrontAndBack(units, GUNGI_PIECE_PAWN, GUNGI_PIECE_GOLD));

  CHECK_EQUAL(2, LogicianFixture::hasFront(units, GUNGI_PIECE_BOW));
  CHECK_EQUAL(2, LogicianFixture::hasFrontAndBack(units, GUNGI_PIECE_BOW, GUNGI_PIECE_ARROW));

  CHECK_EQUAL(1, LogicianFixture::hasFront(units, GUNGI_PIECE_PRODIGY));
  CHECK_EQUAL(1, LogicianFixture::hasFrontAndBack(units, GUNGI_PIECE_PRODIGY, GUNGI_PIECE_PHOENIX));

  CHECK_EQUAL(1, LogicianFixture::hasFront(units, GUNGI_PIECE_HIDDEN_DRAGON));
  CHECK_EQUAL(1, LogicianFixture::hasFrontAndBack(units, GUNGI_PIECE_HIDDEN_DRAGON, GUNGI_PIECE_DRAGON_KING));

  CHECK_EQUAL(1, LogicianFixture::hasFront(units, GUNGI_PIECE_FORTRESS));
  CHECK_EQUAL(1, LogicianFixture::hasFrontAndBack(units, GUNGI_PIECE_FORTRESS, GUNGI_PIECE_LANCE));

  CHECK_EQUAL(1, LogicianFixture::hasFront(units, GUNGI_PIECE_CATAPULT));
  CHECK_EQUAL(1, LogicianFixture::hasFrontAndBack(units, GUNGI_PIECE_CATAPULT, GUNGI_PIECE_LANCE));

  CHECK_EQUAL(3, LogicianFixture::hasFront(units, GUNGI_PIECE_SPY));
  CHECK_EQUAL(3, LogicianFixture::hasFrontAndBack(units, GUNGI_PIECE_SPY, GUNGI_PIECE_CLANDESTINITE));

  CHECK_EQUAL(2, LogicianFixture::hasFront(units, GUNGI_PIECE_SAMURAI));
  CHECK_EQUAL(2, LogicianFixture::hasFrontAndBack(units, GUNGI_PIECE_SAMURAI, GUNGI_PIECE_PIKE));

  CHECK_EQUAL(2, LogicianFixture::hasFront(units, GUNGI_PIECE_CAPTAIN));
  CHECK_EQUAL(2, LogicianFixture::hasFrontAndBack(units, GUNGI_PIECE_CAPTAIN, GUNGI_PIECE_PISTOL));

  CHECK_EQUAL(1, LogicianFixture::hasFront(units, GUNGI_PIECE_COMMANDER));
  CHECK_EQUAL(1, LogicianFixture::hasFrontAndBack(units, GUNGI_PIECE_COMMANDER, GUNGI_PIECE_NONE));
}

TEST(LogicianTest, pawn_in_every_file_enforced) {
  Logician logician;
  error_t error;

  CHECK_TRUE(logician.isPlayersTurn(BLACK));
  logician.dropUnit(GUNGI_PIECE_PAWN, GUNGI_PIECE_BRONZE, Posn(0, 8), error);
  CHECK_EQUAL(GUNGI_ERROR_NONE, error);

  CHECK_TRUE(logician.isPlayersTurn(WHITE));
  logician.dropUnit(GUNGI_PIECE_PAWN, GUNGI_PIECE_BRONZE, Posn(0, 0), error);
  CHECK_EQUAL(GUNGI_ERROR_NONE, error);

  CHECK_TRUE(logician.isPlayersTurn(BLACK));
  logician.dropUnit(GUNGI_PIECE_PAWN, GUNGI_PIECE_BRONZE, Posn(0, 8), error);
  CHECK_EQUAL(GUNGI_ERROR_PAWN_FILE, error);

  CHECK_TRUE(logician.isPlayersTurn(BLACK));
  logician.dropUnit(GUNGI_PIECE_COMMANDER, GUNGI_PIECE_NONE, Posn(0, 8), error);
  CHECK_EQUAL(GUNGI_ERROR_NONE, error);

  CHECK_TRUE(logician.isPlayersTurn(WHITE));
  logician.dropUnit(GUNGI_PIECE_PAWN, GUNGI_PIECE_SILVER, Posn(0, 0), error);
  CHECK_EQUAL(GUNGI_ERROR_PAWN_FILE, error);
}

TEST(LogicianTest, commander_cannot_be_dropped_on) {
  Logician logician;
  error_t error;

  CHECK_TRUE(logician.isPlayersTurn(BLACK));
  logician.dropUnit(GUNGI_PIECE_COMMANDER, GUNGI_PIECE_NONE, Posn(0, 8), error);
  CHECK_EQUAL(GUNGI_ERROR_NONE, error);

  CHECK_TRUE(logician.isPlayersTurn(WHITE));
  logician.dropUnit(GUNGI_PIECE_COMMANDER, GUNGI_PIECE_NONE, Posn(0, 0), error);
  CHECK_EQUAL(GUNGI_ERROR_NONE, error);

  CHECK_TRUE(logician.isPlayersTurn(BLACK));
  logician.dropUnit(GUNGI_PIECE_PAWN, GUNGI_PIECE_BRONZE, Posn(0, 8), error);
  CHECK_EQUAL(GUNGI_ERROR_NO_TOWER, error);

  CHECK_TRUE(logician.isPlayersTurn(BLACK));
  logician.dropUnit(GUNGI_PIECE_PAWN, GUNGI_PIECE_BRONZE, Posn(1, 8), error);
  CHECK_EQUAL(GUNGI_ERROR_NONE, error);

  CHECK_TRUE(logician.isPlayersTurn(WHITE));
  logician.dropUnit(GUNGI_PIECE_PAWN, GUNGI_PIECE_GOLD, Posn(0, 0), error);
  CHECK_EQUAL(GUNGI_ERROR_NO_TOWER, error);
}

TEST(LogicianTest, drop_same_unit_in_tower) {
  Logician logician;
  error_t error;

  CHECK_TRUE(logician.isPlayersTurn(BLACK));
  logician.dropUnit(GUNGI_PIECE_CAPTAIN, GUNGI_PIECE_PISTOL, Posn(0, 8), error);
  CHECK_EQUAL(GUNGI_ERROR_NONE, error);

  CHECK_TRUE(logician.isPlayersTurn(WHITE));
  logician.dropUnit(GUNGI_PIECE_CAPTAIN, GUNGI_PIECE_PISTOL, Posn(0, 0), error);
  CHECK_EQUAL(GUNGI_ERROR_NONE, error);

  CHECK_TRUE(logician.isPlayersTurn(BLACK));
  logician.dropUnit(GUNGI_PIECE_CAPTAIN, GUNGI_PIECE_PISTOL, Posn(0, 8), error);
  CHECK_EQUAL(GUNGI_ERROR_DUPLICATE, error);

  CHECK_TRUE(logician.isPlayersTurn(BLACK));
  logician.dropUnit(GUNGI_PIECE_CAPTAIN, GUNGI_PIECE_PISTOL, Posn(1, 8), error);
  CHECK_EQUAL(GUNGI_ERROR_NONE, error);

  CHECK_TRUE(logician.isPlayersTurn(WHITE));
  logician.dropUnit(GUNGI_PIECE_CAPTAIN, GUNGI_PIECE_PISTOL, Posn(0, 0), error);
  CHECK_EQUAL(GUNGI_ERROR_DUPLICATE, error);

  CHECK_TRUE(logician.isPlayersTurn(WHITE));
  logician.dropUnit(GUNGI_PIECE_CAPTAIN, GUNGI_PIECE_PISTOL, Posn(1, 0), error);
  CHECK_EQUAL(GUNGI_ERROR_NONE, error);
}

TEST(LogicianTest, drop_in_full_tower) {
  Posn posns[] = { Posn(0, 8), Posn(0, 0) };

  colour_t colours[] = {
    BLACK,
    WHITE
  };

  piece_id_t fronts[] = {
    GUNGI_PIECE_CAPTAIN,
    GUNGI_PIECE_PAWN,
    GUNGI_PIECE_PRODIGY
  };

  piece_id_t backs[] = {
    GUNGI_PIECE_PISTOL,
    GUNGI_PIECE_BRONZE,
    GUNGI_PIECE_PHOENIX
  };

  Logician logician;
  error_t error;

  for (int i = 0; i < 6; i++) {
    CHECK_TRUE(logician.isPlayersTurn(colours[i % 2]));
    logician.dropUnit(fronts[i / 2], backs[i / 2], posns[i % 2], error);
    CHECK_EQUAL(GUNGI_ERROR_NONE, error);
  }

  CHECK_TRUE(logician.isPlayersTurn(BLACK));
  logician.dropUnit(GUNGI_PIECE_SAMURAI, GUNGI_PIECE_PIKE, Posn(0, 8), error);
  CHECK_EQUAL(GUNGI_ERROR_FULL_TOWER, error);

  CHECK_TRUE(logician.isPlayersTurn(BLACK));
  logician.dropUnit(GUNGI_PIECE_SAMURAI, GUNGI_PIECE_PIKE, Posn(1, 8), error);
  CHECK_EQUAL(GUNGI_ERROR_NONE, error);

  CHECK_TRUE(logician.isPlayersTurn(WHITE));
  logician.dropUnit(GUNGI_PIECE_SAMURAI, GUNGI_PIECE_PIKE, Posn(0, 0), error);
  CHECK_EQUAL(GUNGI_ERROR_FULL_TOWER, error);

  CHECK_TRUE(logician.isPlayersTurn(WHITE));
  logician.dropUnit(GUNGI_PIECE_SAMURAI, GUNGI_PIECE_PIKE, Posn(1, 0), error);
  CHECK_EQUAL(GUNGI_ERROR_NONE, error);
}

TEST(LogicianTest, drop_prevent_catapult_or_fortress_on_tower) {
  Logician logician;
  error_t error;

  CHECK_TRUE(logician.isPlayersTurn(BLACK));
  logician.dropUnit(GUNGI_PIECE_PAWN, GUNGI_PIECE_BRONZE, Posn(0, 8), error);
  CHECK_EQUAL(GUNGI_ERROR_NONE, error);

  CHECK_TRUE(logician.isPlayersTurn(WHITE));
  logician.dropUnit(GUNGI_PIECE_PAWN, GUNGI_PIECE_BRONZE, Posn(0, 0), error);
  CHECK_EQUAL(GUNGI_ERROR_NONE, error);

  CHECK_TRUE(logician.isPlayersTurn(BLACK));
  logician.dropUnit(GUNGI_PIECE_CATAPULT, GUNGI_PIECE_LANCE, Posn(0, 8), error);
  CHECK_EQUAL(GUNGI_ERROR_NO_STACK, error);

  CHECK_TRUE(logician.isPlayersTurn(BLACK));
  logician.dropUnit(GUNGI_PIECE_FORTRESS, GUNGI_PIECE_LANCE, Posn(0, 8), error);
  CHECK_EQUAL(GUNGI_ERROR_NO_STACK, error);

  CHECK_TRUE(logician.isPlayersTurn(BLACK));
  logician.dropUnit(GUNGI_PIECE_CATAPULT, GUNGI_PIECE_LANCE, Posn(0, 7), error);
  CHECK_EQUAL(GUNGI_ERROR_NONE, error);

  CHECK_TRUE(logician.isPlayersTurn(WHITE));
  logician.dropUnit(GUNGI_PIECE_CATAPULT, GUNGI_PIECE_LANCE, Posn(0, 0), error);
  CHECK_EQUAL(GUNGI_ERROR_NO_STACK, error);

  CHECK_TRUE(logician.isPlayersTurn(WHITE));
  logician.dropUnit(GUNGI_PIECE_FORTRESS, GUNGI_PIECE_LANCE, Posn(0, 0), error);
  CHECK_EQUAL(GUNGI_ERROR_NO_STACK, error);

  CHECK_TRUE(logician.isPlayersTurn(WHITE));
  logician.dropUnit(GUNGI_PIECE_CATAPULT, GUNGI_PIECE_LANCE, Posn(0, 1), error);
  CHECK_EQUAL(GUNGI_ERROR_NONE, error);
}

TEST(LogicianTest, drop_not_in_territory) {
  Logician logician;
  error_t error;

  CHECK_TRUE(logician.isPlayersTurn(BLACK));
  logician.dropUnit(GUNGI_PIECE_PAWN, GUNGI_PIECE_BRONZE, Posn(0, 0), error);
  CHECK_EQUAL(GUNGI_ERROR_TERRITORY, error);
}

TEST(LogicianTest, mobile_range_expansion_1) {
  {
    // Back of territory.
    Logician logician;
    error_t error;

    CHECK_TRUE(logician.isPlayersTurn(BLACK));
    logician.dropUnit(GUNGI_PIECE_FORTRESS, GUNGI_PIECE_LANCE, Posn(0, 8), error);
    CHECK_EQUAL(GUNGI_ERROR_NONE, error);

    for (unsigned int col = 0; col < k_BOARD_LENGTH; col++) {
      for (unsigned int row = 0; row < k_BOARD_LENGTH; row++) {
        Posn posn(col, row);
        bool shouldBeInRange = (row <= 8 && col == 0);
        CHECK_EQUAL(shouldBeInRange, logician.isInMobileRangeExpansion(posn, BLACK));
      }
    }
  }
  {
    // Middle of territory.
    Logician logician;
    error_t error;

    CHECK_TRUE(logician.isPlayersTurn(BLACK));
    logician.dropUnit(GUNGI_PIECE_FORTRESS, GUNGI_PIECE_LANCE, Posn(0, 7), error);
    CHECK_EQUAL(GUNGI_ERROR_NONE, error);

    for (unsigned int col = 0; col < k_BOARD_LENGTH; col++) {
      for (unsigned int row = 0; row < k_BOARD_LENGTH; row++) {
        Posn posn(col, row);
        bool shouldBeInRange = (row <= 7 && col == 0);
        CHECK_EQUAL(shouldBeInRange, logician.isInMobileRangeExpansion(posn, BLACK));
      }
    }
  }
  {
    // Edge of territory.
    Logician logician;
    error_t error;

    CHECK_TRUE(logician.isPlayersTurn(BLACK));
    logician.dropUnit(GUNGI_PIECE_FORTRESS, GUNGI_PIECE_LANCE, Posn(0, 6), error);
    CHECK_EQUAL(GUNGI_ERROR_NONE, error);

    for (unsigned int col = 0; col < k_BOARD_LENGTH; col++) {
      for (unsigned int row = 0; row < k_BOARD_LENGTH; row++) {
        Posn posn(col, row);
        bool shouldBeInRange = (row <= 6 && col == 0);
        CHECK_EQUAL(shouldBeInRange, logician.isInMobileRangeExpansion(posn, BLACK));
      }
    }
  }
  {
    // White
    Logician logician;
    error_t error;

    CHECK_TRUE(logician.isPlayersTurn(BLACK));
    logician.dropUnit(GUNGI_PIECE_FORTRESS, GUNGI_PIECE_LANCE, Posn(0, 6), error);
    CHECK_EQUAL(GUNGI_ERROR_NONE, error);

    CHECK_TRUE(logician.isPlayersTurn(WHITE));
    logician.dropUnit(GUNGI_PIECE_FORTRESS, GUNGI_PIECE_LANCE, Posn(0, 0), error);
    CHECK_EQUAL(GUNGI_ERROR_NONE, error);

    for (unsigned int col = 0; col < k_BOARD_LENGTH; col++) {
      for (unsigned int row = 0; row < k_BOARD_LENGTH; row++) {
        Posn posn(col, row);
        bool shouldBeInRange = (col == 0);
        CHECK_EQUAL(shouldBeInRange, logician.isInMobileRangeExpansion(posn, WHITE));
      }
    }
  }
}

TEST(LogicianTest, mobile_range_expansion_2) {
  {
    // Center of territory.
    Logician logician;
    error_t error;

    CHECK_TRUE(logician.isPlayersTurn(BLACK));
    logician.dropUnit(GUNGI_PIECE_CATAPULT, GUNGI_PIECE_LANCE, Posn(4, 7), error);
    CHECK_EQUAL(GUNGI_ERROR_NONE, error);

    PosnSet set = {
                  Posn(3, 8), Posn(4, 8), Posn(5, 8),
      Posn(2, 7), Posn(3, 7), Posn(4, 7), Posn(5, 7), Posn(6, 7),
                  Posn(3, 6), Posn(4, 6), Posn(5, 6)
    };

    for (unsigned int col = 0; col < k_BOARD_LENGTH; col++) {
      for (unsigned int row = 0; row < k_BOARD_LENGTH; row++) {
        Posn posn(col, row);
        bool shouldBeInRange = (Util::find(set, posn) != set.end());
        CHECK_EQUAL(shouldBeInRange, logician.isInMobileRangeExpansion(posn, BLACK));
      }
    }
  }
  {
    // Center edge of territory (top).
    Logician logician;
    error_t error;

    CHECK_TRUE(logician.isPlayersTurn(BLACK));
    logician.dropUnit(GUNGI_PIECE_CATAPULT, GUNGI_PIECE_LANCE, Posn(4, 8), error);
    CHECK_EQUAL(GUNGI_ERROR_NONE, error);

    PosnSet set = {
      Posn(2, 8), Posn(3, 8), Posn(4, 8), Posn(5, 8), Posn(6, 8),
                  Posn(3, 7), Posn(4, 7), Posn(5, 7),
                              Posn(4, 6)
    };

    for (unsigned int col = 0; col < k_BOARD_LENGTH; col++) {
      for (unsigned int row = 0; row < k_BOARD_LENGTH; row++) {
        Posn posn(col, row);
        bool shouldBeInRange = (Util::find(set, posn) != set.end());
        CHECK_EQUAL(shouldBeInRange, logician.isInMobileRangeExpansion(posn, BLACK));
      }
    }
  }
  {
    // Center edge of territory (bottom).
    Logician logician;
    error_t error;

    CHECK_TRUE(logician.isPlayersTurn(BLACK));
    logician.dropUnit(GUNGI_PIECE_CATAPULT, GUNGI_PIECE_LANCE, Posn(4, 6), error);
    CHECK_EQUAL(GUNGI_ERROR_NONE, error);

    PosnSet set = {
                              Posn(4, 8),
                  Posn(3, 7), Posn(4, 7), Posn(5, 7),
      Posn(2, 6), Posn(3, 6), Posn(4, 6), Posn(5, 6), Posn(6, 6)
    };

    for (unsigned int col = 0; col < k_BOARD_LENGTH; col++) {
      for (unsigned int row = 0; row < k_BOARD_LENGTH; row++) {
        Posn posn(col, row);
        bool shouldBeInRange = (Util::find(set, posn) != set.end());
        CHECK_EQUAL(shouldBeInRange, logician.isInMobileRangeExpansion(posn, BLACK));
      }
    }
  }
  {
    // Edge of territory (left).
    Logician logician;
    error_t error;

    CHECK_TRUE(logician.isPlayersTurn(BLACK));
    logician.dropUnit(GUNGI_PIECE_CATAPULT, GUNGI_PIECE_LANCE, Posn(0, 7), error);
    CHECK_EQUAL(GUNGI_ERROR_NONE, error);

    PosnSet set = {
      Posn(0, 8), Posn(1, 8),
      Posn(0, 7), Posn(1, 7), Posn(2, 7),
      Posn(0, 6), Posn(1, 6)
    };

    for (unsigned int col = 0; col < k_BOARD_LENGTH; col++) {
      for (unsigned int row = 0; row < k_BOARD_LENGTH; row++) {
        Posn posn(col, row);
        bool shouldBeInRange = (Util::find(set, posn) != set.end());
        CHECK_EQUAL(shouldBeInRange, logician.isInMobileRangeExpansion(posn, BLACK));
      }
    }
  }
  {
    // Almost edge of territory (left).
    Logician logician;
    error_t error;

    CHECK_TRUE(logician.isPlayersTurn(BLACK));
    logician.dropUnit(GUNGI_PIECE_CATAPULT, GUNGI_PIECE_LANCE, Posn(1, 7), error);
    CHECK_EQUAL(GUNGI_ERROR_NONE, error);

    PosnSet set = {
      Posn(0, 8), Posn(1, 8), Posn(2, 8),
      Posn(0, 7), Posn(1, 7), Posn(2, 7), Posn(3, 7),
      Posn(0, 6), Posn(1, 6), Posn(2, 6)
    };

    for (unsigned int col = 0; col < k_BOARD_LENGTH; col++) {
      for (unsigned int row = 0; row < k_BOARD_LENGTH; row++) {
        Posn posn(col, row);
        bool shouldBeInRange = (Util::find(set, posn) != set.end());
        CHECK_EQUAL(shouldBeInRange, logician.isInMobileRangeExpansion(posn, BLACK));
      }
    }
  }
  {
    // Edge of territory (right).
    Logician logician;
    error_t error;

    CHECK_TRUE(logician.isPlayersTurn(BLACK));
    logician.dropUnit(GUNGI_PIECE_CATAPULT, GUNGI_PIECE_LANCE, Posn(8, 7), error);
    CHECK_EQUAL(GUNGI_ERROR_NONE, error);

    PosnSet set = {
                  Posn(7, 8), Posn(8, 8),
      Posn(6, 7), Posn(7, 7), Posn(8, 7),
                  Posn(7, 6), Posn(8, 6)
    };

    for (unsigned int col = 0; col < k_BOARD_LENGTH; col++) {
      for (unsigned int row = 0; row < k_BOARD_LENGTH; row++) {
        Posn posn(col, row);
        bool shouldBeInRange = (Util::find(set, posn) != set.end());
        CHECK_EQUAL(shouldBeInRange, logician.isInMobileRangeExpansion(posn, BLACK));
      }
    }
  }
  {
    // Almost edge of territory (right).
    Logician logician;
    error_t error;

    CHECK_TRUE(logician.isPlayersTurn(BLACK));
    logician.dropUnit(GUNGI_PIECE_CATAPULT, GUNGI_PIECE_LANCE, Posn(7, 7), error);
    CHECK_EQUAL(GUNGI_ERROR_NONE, error);

    PosnSet set = {
                  Posn(6, 8), Posn(7, 8), Posn(8, 8),
      Posn(5, 7), Posn(6, 7), Posn(7, 7), Posn(8, 7),
                  Posn(6, 6), Posn(7, 6), Posn(8, 6)
    };

    for (unsigned int col = 0; col < k_BOARD_LENGTH; col++) {
      for (unsigned int row = 0; row < k_BOARD_LENGTH; row++) {
        Posn posn(col, row);
        bool shouldBeInRange = (Util::find(set, posn) != set.end());
        CHECK_EQUAL(shouldBeInRange, logician.isInMobileRangeExpansion(posn, BLACK));
      }
    }
  }
  {
    // White
    Logician logician;
    error_t error;

    CHECK_TRUE(logician.isPlayersTurn(BLACK));
    logician.dropUnit(GUNGI_PIECE_CATAPULT, GUNGI_PIECE_LANCE, Posn(4, 7), error);
    CHECK_EQUAL(GUNGI_ERROR_NONE, error);

    CHECK_TRUE(logician.isPlayersTurn(WHITE));
    logician.dropUnit(GUNGI_PIECE_CATAPULT, GUNGI_PIECE_LANCE, Posn(4, 1), error);
    CHECK_EQUAL(GUNGI_ERROR_NONE, error);

    PosnSet set = {
                  Posn(3, 2), Posn(4, 2), Posn(5, 2),
      Posn(2, 1), Posn(3, 1), Posn(4, 1), Posn(5, 1), Posn(6, 1),
                  Posn(3, 0), Posn(4, 0), Posn(5, 0)
    };

    for (unsigned int col = 0; col < k_BOARD_LENGTH; col++) {
      for (unsigned int row = 0; row < k_BOARD_LENGTH; row++) {
        Posn posn(col, row);
        bool shouldBeInRange = (Util::find(set, posn) != set.end());
        CHECK_EQUAL(shouldBeInRange, logician.isInMobileRangeExpansion(posn, WHITE));
      }
    }
  }
}
