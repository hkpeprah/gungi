// player_unit_tests.cpp                                              -*-C++-*-
#include "player.hpp"

#include "gtypes.hpp"
#include "posn.hpp"
#include "tower.hpp"
#include "unit.hpp"

#include <CppUTest/TestHarness.h>

#include <cstdint>
#include <vector>

using namespace gungi;

TEST_GROUP(PlayerTest) {
  void setup(void) {
    return;
  }

  void teardown(void) {
    return;
  }
};

TEST(PlayerTest, constructor_and_destructor) {
  Player player1(BLACK);
  Player player2(WHITE);
}

TEST(PlayerTest, reset_clears_unit_vector) {
  const Builder builder;
  Player player(BLACK);
  Unit unit(GUNGI_PIECE_PAWN, GUNGI_PIECE_NONE, player.colour(), builder);

  CHECK_EQUAL(0, player.units().size());

  error_t error;
  player.addUnit(&unit, error);

  CHECK_EQUAL(1, player.units().size());

  player.reset();

  CHECK_EQUAL(0, player.units().size());
}

TEST(PlayerTest, add_unit_adds_unit) {
  const Builder builder;
  Player player(BLACK);
  Unit unit(GUNGI_PIECE_PAWN, GUNGI_PIECE_NONE, player.colour(), builder);

  error_t error;
  player.addUnit(&unit, error);

  CHECK_EQUAL(GUNGI_ERROR_NONE, error);
  CHECK_EQUAL(1, player.units().size());
}

TEST(PlayerTest, add_unit_returns_error_on_duplicate_add) {
  const Builder builder;
  Player player(BLACK);
  Unit unit(GUNGI_PIECE_PAWN, GUNGI_PIECE_NONE, player.colour(), builder);

  error_t error;
  player.addUnit(&unit, error);

  CHECK_EQUAL(GUNGI_ERROR_NONE, error);
  CHECK_EQUAL(1, player.units().size());

  player.addUnit(&unit, error);

  CHECK_EQUAL(GUNGI_ERROR_DUPLICATE, error);
  CHECK_EQUAL(1, player.units().size());
}

TEST(PlayerTest, remove_unit_removes_unit) {
  const Builder builder;
  Player player(BLACK);
  Unit unit(GUNGI_PIECE_PAWN, GUNGI_PIECE_NONE, player.colour(), builder);

  error_t error;
  player.addUnit(&unit, error);

  CHECK_EQUAL(GUNGI_ERROR_NONE, error);
  CHECK_EQUAL(1, player.units().size());

  player.removeUnit(&unit, error);

  CHECK_EQUAL(GUNGI_ERROR_NONE, error);
  CHECK_EQUAL(0, player.units().size());
}

TEST(PlayerTest, remove_unit_retunrs_error_on_non_member) {
  const Builder builder;
  Player player(BLACK);
  Unit unit(GUNGI_PIECE_PAWN, GUNGI_PIECE_NONE, player.colour(), builder);

  error_t error;
  player.removeUnit(&unit, error);

  CHECK_EQUAL(GUNGI_ERROR_NOT_A_MEMBER, error);
}

TEST(PlayerTest, colour_returns_players_colour) {
  Player player(BLACK);

  CHECK_EQUAL(BLACK, player.colour());
}

TEST(PlayerTest, active_units_returns_active_units_only) {
  Player player(BLACK);

  const Builder builder;
  Unit unit1(GUNGI_PIECE_PAWN, GUNGI_PIECE_NONE, player.colour(), builder);
  Unit unit2(GUNGI_PIECE_PAWN, GUNGI_PIECE_NONE, player.colour(), builder);

  Posn posn(0, 1);
  Tower tower(posn);
  unit1.setTower(&tower);

  CHECK_EQUAL(0, player.activeUnits().size());

  error_t error;
  player.addUnit(&unit1, error);

  CHECK_EQUAL(GUNGI_ERROR_NONE, error);
  CHECK_EQUAL(1, player.activeUnits().size());
  CHECK_TRUE(&unit1 == player.activeUnits()[0].get());

  player.addUnit(&unit2, error);

  CHECK_EQUAL(GUNGI_ERROR_NONE, error);
  CHECK_EQUAL(1, player.activeUnits().size());
  CHECK_TRUE(&unit1 == player.activeUnits()[0].get());

  unit2.setTower(&tower);
  CHECK_EQUAL(2, player.activeUnits().size());
}

TEST(PlayerTest, inactive_unit_returns_inactive_units_only) {
  Player player(BLACK);

  const Builder builder;
  Unit unit1(GUNGI_PIECE_PAWN, GUNGI_PIECE_NONE, player.colour(), builder);
  Unit unit2(GUNGI_PIECE_PAWN, GUNGI_PIECE_NONE, player.colour(), builder);

  Posn posn(0, 1);
  Tower tower(posn);
  unit1.setTower(&tower);

  CHECK_EQUAL(0, player.inactiveUnits().size());

  error_t error;
  player.addUnit(&unit1, error);

  CHECK_EQUAL(GUNGI_ERROR_NONE, error);
  CHECK_EQUAL(0, player.inactiveUnits().size());

  player.addUnit(&unit2, error);

  CHECK_EQUAL(GUNGI_ERROR_NONE, error);
  CHECK_EQUAL(1, player.inactiveUnits().size());
  CHECK_TRUE(&unit2 == player.inactiveUnits()[0].get());

  unit2.setTower(&tower);
  CHECK_EQUAL(0, player.inactiveUnits().size());
}

TEST(PlayerTest, units_returns_empty) {
  const Builder builder;
  Player player(BLACK);

  CHECK_EQUAL(0, player.units().size());
}
