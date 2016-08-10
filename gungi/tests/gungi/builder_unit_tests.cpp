// builder_unit_tests.cpp                                             -*-C++-*-
#include "builder.hpp"

#include <CppUTest/TestHarness.h>

using namespace gungi;

TEST_GROUP(BuilderTest) {
  void setup(void) {
    return;
  }

  void teardown(void) {
    return;
  }
};

TEST(BuilderTest, constructor) {
  const Builder builder;
  (void)builder;
}

TEST(BuilderTest, effects_returns_correct_effect_for_units) {
  const Builder builder;
  error_t error;
  effect_bitfield_t bitfield;

  bitfield = builder.effects(GUNGI_PIECE_PAWN, error);
  CHECK_EQUAL(GUNGI_ERROR_NONE, error);
}

TEST(BuilderTest, effects_returns_none_on_invalid_unit) {
  const Builder builder;
  error_t error;
  effect_bitfield_t bitfield;

  bitfield = builder.effects(GUNGI_PIECE_NONE, error);
  CHECK_EQUAL(GUNGI_ERROR_INVALID_IDX, error);
  CHECK_EQUAL((effect_bitfield_t)GUNGI_EFFECT_NONE, bitfield);

  bitfield = builder.effects(GUNGI_NUM_PIECES, error);
  CHECK_EQUAL(GUNGI_ERROR_INVALID_IDX, error);
  CHECK_EQUAL((effect_bitfield_t)GUNGI_EFFECT_NONE, bitfield);
}

TEST(BuilderTest, immunities_return_correct_immunity_for_units) {
  const Builder builder;
  error_t error;
  effect_bitfield_t bitfield;

  bitfield = builder.immunities(GUNGI_PIECE_PAWN, error);
  CHECK_EQUAL(GUNGI_ERROR_NONE, error);
}

TEST(BuilderTest, immunities_returns_none_on_invalid_unit) {
  const Builder builder;
  error_t error;
  effect_bitfield_t bitfield;

  bitfield = builder.immunities(GUNGI_PIECE_NONE, error);
  CHECK_EQUAL(GUNGI_ERROR_INVALID_IDX, error);
  CHECK_EQUAL((effect_bitfield_t)GUNGI_EFFECT_NONE, bitfield);

  bitfield = builder.immunities(GUNGI_NUM_PIECES, error);
  CHECK_EQUAL(GUNGI_ERROR_INVALID_IDX, error);
  CHECK_EQUAL((effect_bitfield_t)GUNGI_EFFECT_NONE, bitfield);
}

TEST(BuilderTest, moveset_returns_correct_moves_for_units) {
  const Builder builder;
  error_t error;
  const_moveset_ptr_t ptr = NULL;

  ptr = builder.moveset(GUNGI_PIECE_PAWN, error);
  CHECK_EQUAL(GUNGI_ERROR_NONE, error);
  CHECK_TRUE(ptr != (void *)NULL);
}

TEST(BuilderTest, moveset_returns_none_on_none_unit) {
  const Builder builder;
  error_t error;
  const_moveset_ptr_t ptr = NULL;

  ptr = builder.moveset(GUNGI_PIECE_NONE, error);
  CHECK_EQUAL(GUNGI_ERROR_INVALID_IDX, error);
  POINTERS_EQUAL((void *)NULL, ptr);
}

TEST(BuilderTest, moveset_returns_none_on_invalid_unit) {
  const Builder builder;
  error_t error;
  const_moveset_ptr_t ptr = NULL;

  ptr = builder.moveset(GUNGI_NUM_PIECES, error);
  CHECK_EQUAL(GUNGI_ERROR_INVALID_IDX, error);
  POINTERS_EQUAL((void *)NULL, ptr);
}
