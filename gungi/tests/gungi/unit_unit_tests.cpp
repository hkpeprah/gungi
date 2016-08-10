// unit_unit_tests.cpp                                                -*-C++-*-
#include "unit.hpp"

#include "builder.hpp"
#include "gtypes.hpp"
#include "tower.hpp"

#include <CppUTest/TestHarness.h>

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

using namespace gungi;

const Builder builder;

TEST_GROUP(UnitTest) {
  void setup(void) {
    return;
  }

  void teardown(void) {
    return;
  }
};

TEST(UnitTest, constructor_and_destructor) {
  Unit pawn(GUNGI_PIECE_PAWN, GUNGI_PIECE_NONE, BLACK, builder);
}

TEST(UnitTest, flip_returns_error_if_no_back) {
  Unit pawn(GUNGI_PIECE_PAWN, GUNGI_PIECE_NONE, BLACK, builder);
  error_t error;

  pawn.flip(error);
  CHECK_EQUAL(GUNGI_ERROR_NO_BACK, error);
}

TEST(UnitTest, flip_returns_no_error_on_success) {
  Unit u(GUNGI_PIECE_PAWN, GUNGI_PIECE_BRONZE, BLACK, builder);
  error_t error;

  CHECK_EQUAL(GUNGI_PIECE_PAWN, u.front());
  CHECK_EQUAL(GUNGI_PIECE_BRONZE, u.back());
  CHECK_EQUAL(GUNGI_EFFECT_FORCED_RECOVERY, u.effectField());
  CHECK_EQUAL(0, u.immunityField());

  u.flip(error);

  CHECK_EQUAL(GUNGI_ERROR_NONE, error);
  CHECK_EQUAL(GUNGI_PIECE_BRONZE, u.front());
  CHECK_EQUAL(GUNGI_PIECE_PAWN, u.back());
  CHECK_EQUAL(GUNGI_EFFECT_BETRAYAL, u.effectField());
  CHECK_EQUAL(0, u.immunityField());
}

TEST(UnitTest, set_colour_changes_colour) {
  Unit u(GUNGI_PIECE_PAWN, GUNGI_PIECE_NONE, BLACK, builder);

  CHECK_EQUAL(BLACK, u.colour());

  u.setColour(WHITE);

  CHECK_EQUAL(WHITE, u.colour());
}

TEST(UnitTest, set_tower_sets_tower) {
  Unit u(GUNGI_PIECE_PAWN, GUNGI_PIECE_NONE, BLACK, builder);
  Posn posn(0, 1);
  Tower tower(posn);

  CHECK_EQUAL((void *)NULL, u.tower());

  u.setTower(&tower);

  CHECK_EQUAL(&tower, u.tower());

  u.setTower(NULL);

  CHECK_EQUAL((void *)NULL, u.tower());
}

TEST(UnitTest, clear_tower_clears_tower) {
  Unit u(GUNGI_PIECE_PAWN, GUNGI_PIECE_NONE, BLACK, builder);
  Posn posn(0, 1);
  Tower tower(posn);

  CHECK_EQUAL((void *)NULL, u.tower());

  u.clearTower();

  CHECK_EQUAL((void *)NULL, u.tower());

  u.setTower(&tower);
  u.clearTower();

  CHECK_EQUAL((void *)NULL, u.tower());
}

TEST(UnitTest, add_effect_adds_given_effect_to_unit) {
  Unit u(GUNGI_PIECE_PIKE, GUNGI_PIECE_NONE, BLACK, builder);

  CHECK_EQUAL(0, u.effectField());

  u.addEffect(GUNGI_EFFECT_SUBSTITUTION);

  CHECK_EQUAL(GUNGI_EFFECT_SUBSTITUTION, u.effectField());
}

TEST(UnitTest, remove_effect_removes_given_effect_from_unit) {
  Unit u(GUNGI_PIECE_PIKE, GUNGI_PIECE_NONE, BLACK, builder);

  u.addEffect(GUNGI_EFFECT_BETRAYAL);

  CHECK_EQUAL(GUNGI_EFFECT_BETRAYAL, u.effectField());

  u.removeEffect(GUNGI_EFFECT_BETRAYAL);

  CHECK_EQUAL(0, u.effectField());
}

TEST(UnitTest, add_immunity_adds_given_immunity_to_unit) {
  Unit u(GUNGI_PIECE_PIKE, GUNGI_PIECE_NONE, BLACK, builder);

  CHECK_EQUAL(0, u.immunityField());

  u.addImmunity(GUNGI_EFFECT_SUBSTITUTION);

  CHECK_EQUAL(GUNGI_EFFECT_SUBSTITUTION, u.immunityField());
}

TEST(UnitTest, remove_immunity_removes_given_immunity_from_unit) {
  Unit u(GUNGI_PIECE_PIKE, GUNGI_PIECE_NONE, BLACK, builder);

  u.addImmunity(GUNGI_EFFECT_BETRAYAL);

  CHECK_EQUAL(GUNGI_EFFECT_BETRAYAL, u.immunityField());

  u.removeImmunity(GUNGI_EFFECT_BETRAYAL);

  CHECK_EQUAL(0, u.immunityField());
}

TEST(UnitTest, is_valid_returns_true_on_valid_constructor) {
  Unit pawn(GUNGI_PIECE_PAWN, GUNGI_PIECE_NONE, BLACK, builder);

  CHECK_TRUE(pawn.isValid());
}

TEST(UnitTest, is_valid_returns_false_on_failed_constructor) {
  Unit unit(GUNGI_PIECE_NONE, GUNGI_PIECE_NONE, BLACK, builder);

  CHECK_FALSE(unit.isValid());
}

TEST(UnitTest, is_active_returns_false_on_unit_not_in_tower) {
  Unit unit(GUNGI_PIECE_NONE, GUNGI_PIECE_NONE, BLACK, builder);

  CHECK_FALSE(unit.isActive());
}

TEST(UnitTest, is_active_returns_true_on_unit_in_tower) {
  Unit unit(GUNGI_PIECE_NONE, GUNGI_PIECE_NONE, BLACK, builder);
  Posn posn(0, 1);
  Tower tower(posn);

  unit.setTower(&tower);

  CHECK_TRUE(unit.isActive());
}

TEST(UnitTest, id_and_front_returns_unit_identifier) {
  Unit pawn(GUNGI_PIECE_PAWN, GUNGI_PIECE_NONE, BLACK, builder);

  CHECK_EQUAL(GUNGI_PIECE_PAWN, pawn.id());
  CHECK_EQUAL(GUNGI_PIECE_PAWN, pawn.front());
}

TEST(UnitTest, back_returns_unit_back_identifier) {
  Unit pawn(GUNGI_PIECE_PAWN, GUNGI_PIECE_NONE, BLACK, builder);

  CHECK_EQUAL(GUNGI_PIECE_NONE, pawn.back());
}

TEST(UnitTest, colour_returns_units_colour) {
  Unit pawn(GUNGI_PIECE_PAWN, GUNGI_PIECE_NONE, BLACK, builder);

  CHECK_EQUAL(BLACK, pawn.colour());
}

TEST(UnitTest, moveset_returns_pointer_to_unit_moves) {
  Unit pawn(GUNGI_PIECE_PAWN, GUNGI_PIECE_NONE, BLACK, builder);
  const_moveset_ptr_t moveset = pawn.moveset();

  CHECK_TRUE(moveset != NULL);
}

TEST(UnitTest, tower_returns_pointer_to_units_tower) {
  Unit pawn(GUNGI_PIECE_PAWN, GUNGI_PIECE_NONE, BLACK, builder);
  Posn posn(0, 1);
  Tower tower(posn);

  CHECK_EQUAL((void *)NULL, pawn.tower());

  pawn.setTower(&tower);

  CHECK_EQUAL(&tower, pawn.tower());
}

TEST(UnitTest, pawn_effect_and_immunities) {
  Unit pawn(GUNGI_PIECE_PAWN, GUNGI_PIECE_NONE, BLACK, builder);

  effect_bitfield_t effects = GUNGI_EFFECT_FORCED_RECOVERY;

  CHECK_EQUAL(0, pawn.immunityField());
  CHECK_EQUAL(effects, pawn.effectField());
}

TEST(UnitTest, bow_effects_and_immunities) {
  Unit bow(GUNGI_PIECE_BOW, GUNGI_PIECE_NONE, BLACK, builder);

  CHECK_EQUAL(0, bow.immunityField());
  CHECK_EQUAL(GUNGI_EFFECT_JUMP, bow.effectField());
}

TEST(UnitTest, prodigy_effects_and_immunities) {
  Unit prodigy(GUNGI_PIECE_PRODIGY, GUNGI_PIECE_NONE, BLACK, builder);

  effect_bitfield_t immunities = GUNGI_EFFECT_MOBILE_RANGE_EXPANSION_1 |
                                 GUNGI_EFFECT_MOBILE_RANGE_EXPANSION_2;

  CHECK_EQUAL(immunities, prodigy.immunityField());
  CHECK_EQUAL(0, prodigy.effectField());
}

TEST(UnitTest, hidden_dragon_effects_and_immunities) {
  Unit hidden_dragon(GUNGI_PIECE_HIDDEN_DRAGON, GUNGI_PIECE_NONE, BLACK, builder);

  effect_bitfield_t immunities = GUNGI_EFFECT_MOBILE_RANGE_EXPANSION_1 |
                                 GUNGI_EFFECT_MOBILE_RANGE_EXPANSION_2;

  CHECK_EQUAL(immunities, hidden_dragon.immunityField());
  CHECK_EQUAL(0, hidden_dragon.effectField());
}

TEST(UnitTest, fortress_effects_and_immunities) {
  Unit fortress(GUNGI_PIECE_FORTRESS, GUNGI_PIECE_NONE, BLACK, builder);

  effect_bitfield_t effects = GUNGI_EFFECT_MOBILE_RANGE_EXPANSION_1 |
                              GUNGI_EFFECT_PASSIVE | GUNGI_EFFECT_LAND_LINK |
                              GUNGI_EFFECT_NO_STACK;

  CHECK_EQUAL(0, fortress.immunityField());
  CHECK_EQUAL(effects, fortress.effectField());
}

TEST(UnitTest, catapult_effects_and_immunities) {
  Unit catapult(GUNGI_PIECE_CATAPULT, GUNGI_PIECE_NONE, BLACK, builder);

  effect_bitfield_t effects = GUNGI_EFFECT_MOBILE_RANGE_EXPANSION_2 |
                              GUNGI_EFFECT_LAND_LINK | GUNGI_EFFECT_NO_STACK;

  CHECK_EQUAL(0, catapult.immunityField());
  CHECK_EQUAL(effects, catapult.effectField());
}

TEST(UnitTest, spy_effects_and_immunities) {
  Unit spy(GUNGI_PIECE_SPY, GUNGI_PIECE_NONE, BLACK, builder);

  effect_bitfield_t effects = GUNGI_EFFECT_LAND_LINK |
                              GUNGI_EFFECT_FORCED_RECOVERY |
                              GUNGI_EFFECT_BACK_DROP_ONLY |
                              GUNGI_EFFECT_JUMP;

  CHECK_EQUAL(0, spy.immunityField());
  CHECK_EQUAL(effects, spy.effectField());
}

TEST(UnitTest, samurai_effects_and_immunities) {
  Unit samurai(GUNGI_PIECE_SAMURAI, GUNGI_PIECE_NONE, BLACK, builder);

  effect_bitfield_t effects = GUNGI_EFFECT_SUBSTITUTION;

  CHECK_EQUAL(0, samurai.immunityField());
  CHECK_EQUAL(effects, samurai.effectField());
}

TEST(UnitTest, captain_effects_and_immunities) {
  Unit captain(GUNGI_PIECE_CAPTAIN, GUNGI_PIECE_NONE, BLACK, builder);

  effect_bitfield_t effects = GUNGI_EFFECT_1_3_TIER_EXCHANGE;

  CHECK_EQUAL(0, captain.immunityField());
  CHECK_EQUAL(effects, captain.effectField());
}

TEST(UnitTest, commander_effects_and_immunities) {
  Unit commander(GUNGI_PIECE_COMMANDER, GUNGI_PIECE_NONE, BLACK, builder);

  effect_bitfield_t effects = GUNGI_EFFECT_NO_TOWER;

  effect_bitfield_t immunities = GUNGI_EFFECT_MOBILE_RANGE_EXPANSION_1 |
                                 GUNGI_EFFECT_MOBILE_RANGE_EXPANSION_2 |
                                 GUNGI_EFFECT_1_3_TIER_EXCHANGE;

  CHECK_EQUAL(immunities, commander.immunityField());
  CHECK_EQUAL(effects, commander.effectField());
}

TEST(UnitTest, bronze_effects_and_immunities) {
  Unit bronze(GUNGI_PIECE_BRONZE, GUNGI_PIECE_NONE, BLACK, builder);

  effect_bitfield_t effects = GUNGI_EFFECT_BETRAYAL;

  CHECK_EQUAL(0, bronze.immunityField());
  CHECK_EQUAL(effects, bronze.effectField());
}

TEST(UnitTest, silver_effects_and_immunities) {
  Unit silver(GUNGI_PIECE_SILVER, GUNGI_PIECE_NONE, BLACK, builder);

  CHECK_EQUAL(0, silver.immunityField());
  CHECK_EQUAL(0, silver.effectField());
}

TEST(UnitTest, gold_effects_and_immunities) {
  Unit gold(GUNGI_PIECE_GOLD, GUNGI_PIECE_NONE, BLACK, builder);

  CHECK_EQUAL(0, gold.immunityField());
  CHECK_EQUAL(0, gold.effectField());
}

TEST(UnitTest, arrow_effects_and_immunities) {
  Unit arrow(GUNGI_PIECE_ARROW, GUNGI_PIECE_NONE, BLACK, builder);

  CHECK_EQUAL(0, arrow.immunityField());
  CHECK_EQUAL(0, arrow.effectField());
}

TEST(UnitTest, phoenix_effects_and_immunities) {
  Unit phoenix(GUNGI_PIECE_PHOENIX, GUNGI_PIECE_NONE, BLACK, builder);

  effect_bitfield_t immunities = GUNGI_EFFECT_MOBILE_RANGE_EXPANSION_1 |
                                 GUNGI_EFFECT_MOBILE_RANGE_EXPANSION_2;

  CHECK_EQUAL(immunities, phoenix.immunityField());
  CHECK_EQUAL(0, phoenix.effectField());
}

TEST(UnitTest, lance_effects_and_immunities) {
  Unit lance(GUNGI_PIECE_LANCE, GUNGI_PIECE_NONE, BLACK, builder);

  effect_bitfield_t effects = GUNGI_EFFECT_FORCED_REARRANGEMENT |
                              GUNGI_EFFECT_FORCED_RECOVERY;

  CHECK_EQUAL(0, lance.immunityField());
  CHECK_EQUAL(effects, lance.effectField());
}

TEST(UnitTest, clandestinite_effects_and_immunities) {
  Unit clandestinite(GUNGI_PIECE_CLANDESTINITE, GUNGI_PIECE_NONE, BLACK, builder);

  effect_bitfield_t effects = GUNGI_EFFECT_LAND_LINK |
                              GUNGI_EFFECT_FRONT_DROP_ONLY |
                              GUNGI_EFFECT_JUMP;

  CHECK_EQUAL(0, clandestinite.immunityField());
  CHECK_EQUAL(effects, clandestinite.effectField());
}

TEST(UnitTest, pike_effects_and_immunities) {
  Unit pike(GUNGI_PIECE_PIKE, GUNGI_PIECE_NONE, BLACK, builder);

  CHECK_EQUAL(0, pike.immunityField());
  CHECK_EQUAL(0, pike.effectField());
}

TEST(UnitTest, pistol_effects_and_immunities) {
  Unit pistol(GUNGI_PIECE_PISTOL, GUNGI_PIECE_NONE, BLACK, builder);

  CHECK_EQUAL(0, pistol.immunityField());
  CHECK_EQUAL(0, pistol.effectField());
}

TEST(UnitTest, dragon_king_effects_and_immunities) {
  Unit dragon_king(GUNGI_PIECE_DRAGON_KING, GUNGI_PIECE_NONE, BLACK, builder);

  effect_bitfield_t immunities = GUNGI_EFFECT_MOBILE_RANGE_EXPANSION_1 |
                                 GUNGI_EFFECT_MOBILE_RANGE_EXPANSION_2;

  CHECK_EQUAL(immunities, dragon_king.immunityField());
  CHECK_EQUAL(0, dragon_king.effectField());
}

TEST(UnitTest, unit_code_returns_unit_code) {
  Unit pawn(GUNGI_PIECE_PAWN, GUNGI_PIECE_BRONZE, BLACK, builder);
  CHECK_TRUE(pawn.code() == "PZ");

  Unit dragon_king(GUNGI_PIECE_DRAGON_KING, GUNGI_PIECE_NONE, BLACK, builder);
  CHECK_TRUE(dragon_king.code() == "K-");
}

TEST(UnitTest, output_stream_operator_returns_formatted_output) {
  std::ostringstream oss;
  Unit pawn(GUNGI_PIECE_PAWN, GUNGI_PIECE_BRONZE, BLACK, builder);
  oss << pawn;
  CHECK_TRUE(oss.str() == "Pawn(colour=black, back=Bronze, tower=0x0)");
}
