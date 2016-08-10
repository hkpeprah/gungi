// tower_unit_tests.cpp                                               -*-C++-*-
#include "tower.hpp"

#include "builder.hpp"
#include "unit.hpp"

#include <CppUTest/TestHarness.h>

#include <cstdlib>

using namespace gungi;

const Builder builder;
const Posn posn(0, 1);
Unit pawn(GUNGI_PIECE_PAWN, GUNGI_PIECE_BRONZE, WHITE, builder);
Unit spy(GUNGI_PIECE_SPY, GUNGI_PIECE_CLANDESTINITE, WHITE, builder);
Unit bow(GUNGI_PIECE_BOW, GUNGI_PIECE_ARROW, WHITE, builder);
Unit pike(GUNGI_PIECE_PIKE, GUNGI_PIECE_SAMURAI, WHITE, builder);

TEST_GROUP(TowerTest) {
  void setup(void) {
    pawn.clearTower();
    spy.clearTower();
    bow.clearTower();
    pike.clearTower();
  }

  void teardown(void) {
    return;
  }
};

TEST(TowerTest, constructor_default) {
  Tower tower;

  CHECK_TRUE(tower.posn() == Posn(0, 0));
}

TEST(TowerTest, constructor_with_posn) {
  Tower tower(posn);

  CHECK_TRUE(tower.posn() == Posn(0, 1));
}

TEST(TowerTest, mark_dirty_marks_bit_as_dirty) {
  Tower tower(posn);

  tower.markDirty(Tower::TIER_EXCHANGE);

  CHECK_TRUE(tower.isDirty(Tower::TIER_EXCHANGE));
}

TEST(TowerTest, mark_clean_resets_single_bit) {
  Tower tower(posn);

  CHECK_FALSE(tower.isDirty(Tower::TIER_EXCHANGE));

  tower.markDirty(Tower::TIER_EXCHANGE);

  CHECK_TRUE(tower.isDirty(Tower::TIER_EXCHANGE));

  tower.markClean(Tower::TIER_EXCHANGE);

  CHECK_FALSE(tower.isDirty(Tower::TIER_EXCHANGE));
}

TEST(TowerTest, mark_clean_resets_all_bits) {
  Tower tower(posn);

  CHECK_FALSE(tower.isDirty(Tower::TIER_EXCHANGE));

  tower.markDirty(Tower::TIER_EXCHANGE);

  CHECK_TRUE(tower.isDirty(Tower::TIER_EXCHANGE));

  tower.markClean();

  CHECK_FALSE(tower.isDirty(Tower::TIER_EXCHANGE));
}

TEST(TowerTest, add_unit_to_tower_succeeds) {
  Tower tower(posn);
  error_t error;

  tower.add(&pawn, error);

  CHECK_EQUAL(1, tower.height());
  POINTERS_EQUAL(&tower, pawn.tower());
  CHECK_EQUAL(GUNGI_ERROR_NONE, error);
}

TEST(TowerTest, add_unit_to_tower_returns_duplicate_on_member) {
  Tower tower(posn);
  error_t error;

  tower.add(&pawn, error);

  CHECK_EQUAL(1, tower.height());
  POINTERS_EQUAL(&tower, pawn.tower());
  CHECK_EQUAL(GUNGI_ERROR_NONE, error);

  tower.add(&pawn, error);

  CHECK_EQUAL(1, tower.height());
  POINTERS_EQUAL(&tower, pawn.tower());
  CHECK_EQUAL(GUNGI_ERROR_DUPLICATE, error);

  Unit newPawn(pawn);
  CHECK_EQUAL(pawn.colour(), newPawn.colour());
  CHECK_EQUAL(pawn.front(), newPawn.front());

  tower.add(&newPawn, error);
  CHECK_EQUAL(1, tower.height());
  CHECK_EQUAL(GUNGI_ERROR_DUPLICATE, error);
}

TEST(TowerTest, add_unit_to_tower_returns_full_on_full_tower) {
  Tower tower(posn);
  error_t error;

  tower.add(&pawn, error);
  CHECK_EQUAL(GUNGI_ERROR_NONE, error);

  tower.add(&spy, error);
  CHECK_EQUAL(GUNGI_ERROR_NONE, error);

  tower.add(&bow, error);
  CHECK_EQUAL(GUNGI_ERROR_NONE, error);

  CHECK_EQUAL(Tower::k_MAX_HEIGHT, tower.height());
  CHECK_EQUAL(GUNGI_ERROR_NONE, error);

  tower.add(&pike, error);

  CHECK_EQUAL(Tower::k_MAX_HEIGHT, tower.height());
  CHECK_EQUAL(GUNGI_ERROR_FULL_TOWER, error);
  POINTERS_EQUAL((void *)NULL, pike.tower());
}

TEST(TowerTest, remove_succeeds_if_unit_member) {
  Tower tower(posn);
  error_t error;

  tower.add(&pawn, error);

  POINTERS_EQUAL(&tower, pawn.tower());

  tower.remove(&pawn, error);

  CHECK_EQUAL(0, tower.height());
  CHECK_EQUAL(GUNGI_ERROR_NONE, error);
  POINTERS_EQUAL((void *)NULL, pawn.tower());
}

TEST(TowerTest, remove_fails_with_not_a_member_if_unit_not_a_member) {
  Tower tower(posn);
  error_t error;

  tower.remove(&pawn, error);

  CHECK_EQUAL(GUNGI_ERROR_NOT_A_MEMBER, error);
}

TEST(TowerTest, reset_resets_the_tower) {
  Tower tower(posn);
  error_t error;

  tower.add(&pawn, error);

  CHECK_EQUAL(1, tower.height());

  tower.reset();

  CHECK_EQUAL(0, tower.height());
}

TEST(TowerTest, is_dirty_returns_false_on_unset) {
  Tower tower(posn);

  CHECK_FALSE(tower.isDirty(Tower::TIER_EXCHANGE));
}

TEST(TowerTest, height_returns_zero_on_empty) {
  Tower tower(posn);

  CHECK_EQUAL(0, tower.height());
}

TEST(TowerTest, height_returns_one_on_add) {
  Tower tower(posn);
  error_t error;

  CHECK_EQUAL(0, tower.height());

  tower.add(&pawn, error);

  CHECK_EQUAL(1, tower.height());
}

TEST(TowerTest, tier_returns_not_a_member_if_unit_not_a_member) {
  Tower tower(posn);
  error_t error;
  tier_t tier;

  tier = tower.tier(&pawn, error);

  CHECK_EQUAL(GUNGI_ERROR_NOT_A_MEMBER, error);
}

TEST(TowerTest, tier_returns_correct_tier_for_member) {
  Tower tower(posn);
  error_t error;
  tier_t tier;

  tower.add(&pawn, error);
  CHECK_EQUAL(GUNGI_ERROR_NONE, error);

  tower.add(&spy, error);
  CHECK_EQUAL(GUNGI_ERROR_NONE, error);

  tower.add(&bow, error);
  CHECK_EQUAL(GUNGI_ERROR_NONE, error);

  tier = tower.tier(&pawn, error);

  CHECK_EQUAL(0, tier);
  CHECK_EQUAL(GUNGI_ERROR_NONE, error);

  tier = tower.tier(&spy, error);

  CHECK_EQUAL(1, tier);
  CHECK_EQUAL(GUNGI_ERROR_NONE, error);

  tier = tower.tier(&bow, error);

  CHECK_EQUAL(2, tier);
  CHECK_EQUAL(GUNGI_ERROR_NONE, error);
}

TEST(TowerTest, at_test) {
  Tower tower(posn);
  error_t error;
  tier_t tier;

  const Unit *u = NULL;

  u = tower.at(0, error);

  CHECK_EQUAL(GUNGI_ERROR_OUT_OF_RANGE, error);
  POINTERS_EQUAL((void *)NULL, u);

  u = tower.at(1, error);

  CHECK_EQUAL(GUNGI_ERROR_OUT_OF_RANGE, error);
  POINTERS_EQUAL((void *)NULL, u);

  u = tower.at(2, error);

  CHECK_EQUAL(GUNGI_ERROR_OUT_OF_RANGE, error);
  POINTERS_EQUAL((void *)NULL, u);

  tower.add(&pawn, error);

  u = tower.at(0, error);

  CHECK_EQUAL(GUNGI_ERROR_NONE, error);
  POINTERS_EQUAL(&pawn, u);

  u = tower.at(1, error);

  CHECK_EQUAL(GUNGI_ERROR_OUT_OF_RANGE, error);
  POINTERS_EQUAL((void *)NULL, u);

  u = tower.at(2, error);

  CHECK_EQUAL(GUNGI_ERROR_OUT_OF_RANGE, error);
  POINTERS_EQUAL((void *)NULL, u);

  tower.add(&spy, error);
  CHECK_EQUAL(GUNGI_ERROR_NONE, error);

  u = tower.at(0, error);

  CHECK_EQUAL(GUNGI_ERROR_NONE, error);
  POINTERS_EQUAL(&pawn, u);

  u = tower.at(1, error);

  CHECK_EQUAL(GUNGI_ERROR_NONE, error);
  POINTERS_EQUAL(&spy, u);

  u = tower.at(2, error);

  CHECK_EQUAL(GUNGI_ERROR_OUT_OF_RANGE, error);
  POINTERS_EQUAL((void *)NULL, u);

  tower.add(&bow, error);

  u = tower.at(0, error);

  CHECK_EQUAL(GUNGI_ERROR_NONE, error);
  POINTERS_EQUAL(&pawn, u);

  u = tower.at(1, error);

  CHECK_EQUAL(GUNGI_ERROR_NONE, error);
  POINTERS_EQUAL(&spy, u);

  u = tower.at(2, error);

  CHECK_EQUAL(GUNGI_ERROR_NONE, error);
  POINTERS_EQUAL(&bow, u);
}

TEST(TowerTest, posn_returns_tower_posn) {
  Tower tower(posn);

  CHECK_TRUE(posn == tower.posn());
}

TEST(TowerTest, members_returns_empty_on_empty_tower) {
  Tower tower(posn);

  CHECK_EQUAL(0, tower.members().size());
}

TEST(TowerTest, members_returns_constant_vector) {
  Tower tower(posn);
  error_t error;
  tower.add(&pawn, error);

  const SharedUnitPtrVector& members = tower.members();
  CHECK_EQUAL(1, members.size());

  for (const SharedUnitPtr& ptr : members) {
    POINTERS_EQUAL(&pawn, ptr.get());
  }
}
