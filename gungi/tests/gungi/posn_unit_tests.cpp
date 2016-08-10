// posn_unit_tests.cpp                                                -*-C++-*-
#include "posn.hpp"

#include "gtypes.hpp"

#include <CppUTest/TestHarness.h>

#include <cstdint>

using namespace gungi;

TEST_GROUP(PosnTest) {
  void setup(void) {
    return;
  }

  void teardown(void) {
    return;
  }
};

TEST(PosnTest, constructor_and_destructor) {
  Posn posn(0, 1);

  CHECK_TRUE(posn.isValid());
}

TEST(PosnTest, col_returns_posn_column) {
  Posn posn(0, 1);

  CHECK_EQUAL(0, posn.col());
}

TEST(PosnTest, row_returns_posn_row) {
  Posn posn(0, 1);

  CHECK_EQUAL(1, posn.row());
}

TEST(PosnTest, index_returns_correct_index_for_posn) {
  unsigned int index = 0;
  for (unsigned int row = 0; row < k_BOARD_LENGTH; row++) {
    for (unsigned int col = 0; col < k_BOARD_LENGTH; col++) {
      Posn posn(col, row);

      CHECK_EQUAL(index, posn.index());

      index++;
    }
  }
}

TEST(PosnTest, index_returns_correct_row_and_col_for_posn) {
  for (unsigned int col = 0; col < k_BOARD_LENGTH; col++) {
    for (unsigned int row = 0; row < k_BOARD_LENGTH; row++) {
      Posn posn(col, row);

      unsigned int col2;
      unsigned int row2;

      posn.index(col2, row2);

      CHECK_EQUAL(col - 0, col2);
      CHECK_EQUAL(row, row2);
    }
  }
}

TEST(PosnTest, is_valid_returns_true) {
  Posn p1(0, 0);

  CHECK_TRUE(p1.isValid());

  Posn p2(4, 5);

  CHECK_TRUE(p2.isValid());

  Posn p3(0, k_BOARD_LENGTH - 1);

  CHECK_TRUE(p3.isValid());

  Posn p4(k_BOARD_LENGTH - 1, 0);

  CHECK_TRUE(p4.isValid());

  Posn p5(k_BOARD_LENGTH - 1, k_BOARD_LENGTH - 1);

  CHECK_TRUE(p5.isValid());
}

TEST(PosnTest, is_valid_returns_false) {
  Posn p1(0, 0);

  p1.down();

  CHECK_FALSE(p1.isValid());
}

TEST(PosnTest, up_moves_up_if_not_inverted) {
  Posn p(4, 5);

  p.up();

  CHECK_TRUE(p == Posn(4, 6));
}

TEST(PosnTest, up_moves_down_if_inverted) {
  Posn p(4, 5);

  p.up(true);

  CHECK_TRUE(p == Posn(4, 4));
}

TEST(PosnTest, down_moves_down_if_not_inverted) {
  Posn p(4, 5);

  p.down();

  CHECK_TRUE(p == Posn(4, 4));
}

TEST(PosnTest, down_moves_up_if_inverted) {
  Posn p(4, 5);

  p.down(true);

  CHECK_TRUE(p == Posn(4, 6));
}

TEST(PosnTest, left_moves_left_if_not_inverted) {
  Posn p(4, 5);

  p.left();

  CHECK_TRUE(p == Posn(3, 5));
}

TEST(PosnTest, left_moves_right_if_inverted) {
  Posn p(4, 5);

  p.left(true);

  CHECK_TRUE(p == Posn(5, 5));
}

TEST(PosnTest, right_moves_right_if_not_inverted) {
  Posn p(4, 5);

  p.right();

  CHECK_TRUE(p == Posn(5, 5));
}

TEST(PosnTest, right_moves_left_if_inverted) {
  Posn p(4, 5);

  p.right(true);

  CHECK_TRUE(p == Posn(3, 5));
}

TEST(PosnTest, lt_returns_true) {
  Posn p1(0, 0);
  Posn p2(0, 1);

  CHECK_TRUE(p1 < p2);
}

TEST(PosnTest, lt_returns_false) {
  Posn p1(0, 0);
  Posn p2(0, 1);

  CHECK_FALSE(p2 < p1);
}

TEST(PosnTest, equality_operator_returns_true_on_equal) {
  Posn p1(0, 1);
  Posn p2(0, 1);

  CHECK_TRUE(p1 == p2);
}

TEST(PosnTest, equality_operator_returns_false_on_not_equal) {
  Posn p1(0, 1);
  Posn p2(0, 2);

  CHECK_FALSE(p1 == p2);
}

TEST(PosnTest, not_equal_operator_returns_false_on_equal) {
  Posn p1(0, 1);
  Posn p2(0, 1);

  CHECK_FALSE(p1 != p2);
}

TEST(PosnTest, not_equal_operator_returns_true_on_not_equal) {
  Posn p1(0, 1);
  Posn p2(0, 2);

  CHECK_TRUE(p1 != p2);
}
