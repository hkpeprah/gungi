// gtypes_unit_tests.cpp                                              -*-C++-*-
#include "gtypes.hpp"

#include <CppUTest/TestHarness.h>

#include <cstring>

using namespace gungi;

TEST_GROUP(GtypesTest) {
  void setup(void) {
    return;
  }

  void teardown(void) {
    return;
  }
};

TEST(GtypesTest, error_to_string) {
  const char *error_str;

  error_str = error_to_string(GUNGI_ERROR_NONE);
  CHECK_EQUAL(0, strlen(error_str));

  error_t error = static_cast<error_t>(GUNGI_ERROR_NONE + 1);
  while (error < GUNGI_NUM_ERRORS) {
    error_str = error_to_string(error);
    CHECK_FALSE(0 == strlen(error_str));

    error = static_cast<error_t>(error + 1);
  }
}

TEST(GtypesTest, piece_to_string) {
  const char *pawn_str = piece_to_string(GUNGI_PIECE_PAWN);
  CHECK_EQUAL(0, strcmp("Pawn", pawn_str));

  const char *pistol_str = piece_to_string(GUNGI_PIECE_PISTOL);
  CHECK_EQUAL(0, strcmp("Pistol", pistol_str));
}

TEST(GtypesTest, piece_to_gn_identifier) {
  CHECK_EQUAL(0, strcmp("A", piece_to_gn_identifier(GUNGI_PIECE_ARROW)));
  CHECK_EQUAL(0, strcmp("B", piece_to_gn_identifier(GUNGI_PIECE_BOW)));
  CHECK_EQUAL(0, strcmp("Z", piece_to_gn_identifier(GUNGI_PIECE_BRONZE)));
  CHECK_EQUAL(0, strcmp("C", piece_to_gn_identifier(GUNGI_PIECE_CAPTAIN)));
  CHECK_EQUAL(0, strcmp("T", piece_to_gn_identifier(GUNGI_PIECE_CATAPULT)));
  CHECK_EQUAL(0, strcmp("N", piece_to_gn_identifier(GUNGI_PIECE_CLANDESTINITE)));
  CHECK_EQUAL(0, strcmp("O", piece_to_gn_identifier(GUNGI_PIECE_COMMANDER)));
  CHECK_EQUAL(0, strcmp("K", piece_to_gn_identifier(GUNGI_PIECE_DRAGON_KING)));
  CHECK_EQUAL(0, strcmp("F", piece_to_gn_identifier(GUNGI_PIECE_FORTRESS)));
  CHECK_EQUAL(0, strcmp("G", piece_to_gn_identifier(GUNGI_PIECE_GOLD)));
  CHECK_EQUAL(0, strcmp("H", piece_to_gn_identifier(GUNGI_PIECE_HIDDEN_DRAGON)));
  CHECK_EQUAL(0, strcmp("L", piece_to_gn_identifier(GUNGI_PIECE_LANCE)));
  CHECK_EQUAL(0, strcmp("P", piece_to_gn_identifier(GUNGI_PIECE_PAWN)));
  CHECK_EQUAL(0, strcmp("X", piece_to_gn_identifier(GUNGI_PIECE_PHOENIX)));
  CHECK_EQUAL(0, strcmp("E", piece_to_gn_identifier(GUNGI_PIECE_PIKE)));
  CHECK_EQUAL(0, strcmp("I", piece_to_gn_identifier(GUNGI_PIECE_PISTOL)));
  CHECK_EQUAL(0, strcmp("R", piece_to_gn_identifier(GUNGI_PIECE_PRODIGY)));
  CHECK_EQUAL(0, strcmp("S", piece_to_gn_identifier(GUNGI_PIECE_SAMURAI)));
  CHECK_EQUAL(0, strcmp("V", piece_to_gn_identifier(GUNGI_PIECE_SILVER)));
  CHECK_EQUAL(0, strcmp("Y", piece_to_gn_identifier(GUNGI_PIECE_SPY)));
  CHECK_EQUAL(0, strcmp("-", piece_to_gn_identifier(GUNGI_PIECE_NONE)));
}

TEST(GtypesTest, gn_identifier_to_piece) {
  CHECK_EQUAL(GUNGI_PIECE_PAWN, gn_identifier_to_piece("P"));
  CHECK_EQUAL(GUNGI_PIECE_NONE, gn_identifier_to_piece("M"));
}
