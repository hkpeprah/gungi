// gungi_unit_tests.cpp                                               -*-C++-*-
#include <gungi/gungi.hpp>

#include <CppUTest/TestHarness.h>

#include <cstdint>

using namespace gungi;

TEST_GROUP(GungiLib) {
  void setup(void) {
    return;
  }

  void teardown(void) {
    gungi_games_clear();
  }
};

TEST(GungiLib, gungi_game_start_not_over) {
  unsigned int game_id;

  game_id = gungi_game_start();

  CHECK_FALSE(gungi_game_is_over(game_id));
  CHECK_FALSE(gungi_game_is_draw(game_id));
  CHECK_EQUAL(-1, gungi_game_get_winner(game_id));
}

TEST(GungiLib, gungi_game_start_multiple) {
  unsigned int game_id1;
  unsigned int game_id2;

  game_id1 = gungi_game_start();
  game_id2 = gungi_game_start();

  CHECK_TRUE(game_id1 != game_id2);
}

TEST(GungiLib, gungi_game_restart) {
  unsigned int game_id;

  game_id = gungi_game_start();

  CHECK_TRUE(gungi_game_restart(game_id));
}

TEST(GungiLib, gungi_game_is_over_in_progress) {
  unsigned int game_id;

  game_id = gungi_game_start();

  CHECK_FALSE(gungi_game_is_over(game_id));
}

TEST(GungiLib, gungi_game_is_draw_is_false) {
  unsigned int game_id;

  game_id = gungi_game_start();

  CHECK_FALSE(gungi_game_is_draw(game_id));
}

TEST(GungiLib, gungi_game_is_forced_recovery_is_false) {
  unsigned int game_id;

  game_id = gungi_game_start();

  CHECK_FALSE(gungi_game_is_forced_recovery(game_id));
}

TEST(GungiLib, gungi_game_is_forced_rearrangement_is_false) {
  unsigned int game_id;

  game_id = gungi_game_start();

  CHECK_FALSE(gungi_game_is_forced_rearrangement(game_id));
}

TEST(GungiLib, gungi_game_get_turn) {
  unsigned int game_id;

  game_id = gungi_game_start();

  CHECK_EQUAL(BLACK, gungi_game_get_turn(game_id));
}

TEST(GungiLib, gungi_game_get_winner) {
  unsigned int game_id;

  game_id = gungi_game_start();

  CHECK_EQUAL(-1, gungi_game_get_winner(game_id));
}

TEST(GungiLib, gungi_game_drop_unit) {
  unsigned int game_id;
  error_t error;

  game_id = gungi_game_start();
  error = gungi_game_drop_unit(game_id, "PG", 0, 8);

  CHECK_EQUAL(GUNGI_ERROR_NONE, error);
}
