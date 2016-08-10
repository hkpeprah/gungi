// gndecoder_unit_tests.cpp                                           -*-C++-*-
#include "gndecoder.hpp"

#include "gtypes.hpp"
#include "logician.hpp"
#include "tower.hpp"
#include "unit.hpp"
#include "util.hpp"

#include <CppUTest/TestHarness.h>

#include <sstream>
#include <string>

#include "scenarios.res"

using namespace gungi;

TEST_GROUP(GNDecoderTest) {
  void setup(void) {
    return;
  }

  void teardown(void) {
    return;
  }
};

TEST(GNDecoderTest, metadata_default_constructor) {
  GNMetadata md;

  CHECK_TRUE(md.event() == "");
  CHECK_TRUE(md.date() == "");
  CHECK_TRUE(md.location() == "");
  CHECK_TRUE(md.white() == "");
  CHECK_TRUE(md.black() == "");
}

TEST(GNDecoderTest, metadata_event) {
  GNMetadata md;
  std::string value("Selection");

  CHECK_TRUE(md.event() == "");

  md.setEvent(value);

  CHECK_TRUE(md.event() == value);
}

TEST(GNDecoderTest, metadata_date_valid) {
  GNMetadata md;
  std::string value("2016.09.31");

  CHECK_TRUE(md.date() == "");

  CHECK_TRUE(md.setDate(value));

  CHECK_TRUE(md.date() == value);
}

TEST(GNDecoderTest, metadata_date_invalid) {
  GNMetadata md;
  std::string value("2016.09.32");

  CHECK_TRUE(md.date() == "");

  CHECK_FALSE(md.setDate(value));

  CHECK_FALSE(md.date() == value);
  CHECK_TRUE(md.date() == "");
}

TEST(GNDecoderTest, metadata_location) {
  GNMetadata md;
  std::string value("NGL, Mitene Union");

  CHECK_TRUE(md.location() == "");

  md.setLocation(value);

  CHECK_TRUE(md.location() == value);
}

TEST(GNDecoderTest, metadata_white) {
  GNMetadata md;
  std::string value("Komugi");

  CHECK_TRUE(md.white() == "");

  md.setWhite(value);

  CHECK_TRUE(md.white() == value);
}

TEST(GNDecoderTest, metadata_black) {
  GNMetadata md;
  std::string value("Meruem");

  CHECK_TRUE(md.black() == "");

  md.setBlack(value);

  CHECK_TRUE(md.black() == value);
}

TEST(GNDecoderTest, metadata_header_full) {
  Controller controller;
  std::ostringstream expected;
  expected
    << "[Event \"Selection\"]"
    << std::endl
    << "[Date \"2013.10.30\"]"
    << std::endl
    << "[Location \"NGL, Mitene Union\"]"
    << std::endl
    << "[White \"Komugi\"]"
    << std::endl
    << "[Black \"Meruem\"]"
    << std::endl
    << "[Result \"*\"]"
    << std::endl;

  GNMetadata md;
  md.setEvent("Selection");
  md.setLocation("NGL, Mitene Union");
  md.setWhite("Komugi");
  md.setBlack("Meruem");

  CHECK_TRUE(md.setDate("2013.10.30"));

  CHECK_TRUE(expected.str() == md.header(controller));
}

TEST(GNDecoderTest, metadata_header_partial) {
  Controller controller;
  std::ostringstream expected;
  expected
    << "[Event \"Selection\"]"
    << std::endl
    << "[Date \"2013.10.30\"]"
    << std::endl
    << "[Location \"NGL, Mitene Union\"]"
    << std::endl
    << "[White \"\"]"
    << std::endl
    << "[Black \"\"]"
    << std::endl
    << "[Result \"*\"]"
    << std::endl;

  GNMetadata md;
  md.setEvent("Selection");
  md.setLocation("NGL, Mitene Union");

  CHECK_TRUE(md.setDate("2013.10.30"));

  CHECK_TRUE(expected.str() == md.header(controller));
}

TEST(GNDecoderTest, decode_invalid_header_name) {
  std::ostringstream oss;
  oss
    << "[Event \"Selection\"]"
    << std::endl
    << "[Date \"2013.10.30\"]"
    << std::endl
    << "[Locatio \"NGL, Mitene Union\"]"
    << std::endl
    << "[White \"\"]"
    << std::endl
    << "[Black \"\"]"
    << std::endl
    << "[Result \"*\"]"
    << std::endl;

  GNMetadata md;
  Controller controller;
  CHECK_FALSE(GNDecoder::decode(oss.str(), md, controller));
}

TEST(GNDecoderTest, decode_empty_is_valid) {
  GNMetadata md;
  Controller controller;
  CHECK_TRUE(GNDecoder::decode("", md, controller));
}

TEST(GNDecoderTest, decode_invalid_header_with_comments) {
  std::ostringstream oss;
  oss
    << "[Event \"Selection\"]"
    << std::endl
    << "[Date \"2013.10.30\"]"
    << std::endl
    << "[Location \"NGL, Mitene Union\"] (this comment doesn't end"
    << std::endl
    << "[White \"\"]"
    << std::endl
    << "[Black \"\"]"
    << std::endl
    << "[Result \"*\"]"
    << std::endl;

  GNMetadata md;
  Controller controller;
  CHECK_FALSE(GNDecoder::decode(oss.str(), md, controller));
}

TEST(GNDecoderTest, decode_invalid_header_with_comment_in_tag) {
  std::ostringstream oss;
  oss
    << "[Event \"Selection\"]"
    << std::endl
    << "[Date \"2013.10.30\"]"
    << std::endl
    << "[Location \"NGL, Mitene Union\"] (this comment does end)"
    << std::endl
    << "[White (this comment can't be here) \"\"]"
    << std::endl
    << "[Black \"\"]"
    << std::endl
    << "[Result \"*\"]"
    << std::endl;

  GNMetadata md;
  Controller controller;
  CHECK_FALSE(GNDecoder::decode(oss.str(), md, controller));
}

TEST(GNDecoderTest, decode_invalid_header_value_brack) {
  std::ostringstream oss;
  oss
    << "[Event \"Selection\"]"
    << std::endl
    << "[Date \"2013.10.30\"]"
    << std::endl
    << "[Location \"NGL, Mitene Union\""
    << std::endl
    << "[White \"\"]"
    << std::endl
    << "[Black \"\"]"
    << std::endl
    << "[Result \"*\"]"
    << std::endl;

  GNMetadata md;
  Controller controller;
  CHECK_FALSE(GNDecoder::decode(oss.str(), md, controller));
}

TEST(GNDecoderTest, decode_invalid_header_value_rquote) {
  std::ostringstream oss;
  oss
    << "[Event \"Selection\"]"
    << std::endl
    << "[Date 2013.10.30\"]"
    << std::endl
    << "[Location \"NGL, Mitene Union\"]"
    << std::endl
    << "[White \"\"]"
    << std::endl
    << "[Black \"\"]"
    << std::endl
    << "[Result \"*\"]"
    << std::endl;

  GNMetadata md;
  Controller controller;
  CHECK_FALSE(GNDecoder::decode(oss.str(), md, controller));
}

TEST(GNDecoderTest, decode_invalid_header_value_lquote) {
  std::ostringstream oss;
  oss
    << "[Event \"Selection\"]"
    << std::endl
    << "[Date \"2013.10.30]"
    << std::endl
    << "[Location \"NGL, Mitene Union\"]"
    << std::endl
    << "[White \"\"]"
    << std::endl
    << "[Black \"\"]"
    << std::endl
    << "[Result \"*\"]"
    << std::endl;

  GNMetadata md;
  Controller controller;
  CHECK_FALSE(GNDecoder::decode(oss.str(), md, controller));
}

TEST(GNDecoderTest, decode_invalid_header_date) {
  std::ostringstream oss;
  oss
    << "[Event \"Selection\"]"
    << std::endl
    << "[Date \"2013.10.32\"]"
    << std::endl
    << "[Location \"NGL, Mitene Union\"]"
    << std::endl
    << "[White \"\"]"
    << std::endl
    << "[Black \"\"]"
    << std::endl
    << "[Result \"*\"]"
    << std::endl;

  GNMetadata md;
  Controller controller;
  CHECK_FALSE(GNDecoder::decode(oss.str(), md, controller));
}

TEST(GNDecoderTest, decode_valid_header) {
  std::ostringstream oss;
  oss
    << "[Event \"Selection\"]"
    << std::endl
    << "[Date \"2013.10.30\"]"
    << std::endl
    << "[Location \"NGL, Mitene Union\"]"
    << std::endl
    << "[White \"Komugi\"]"
    << std::endl
    << "[Black \"Meruem\"]"
    << std::endl
    << "[Result \"*\"]"
    << std::endl;

  GNMetadata md;
  Controller controller;
  CHECK_TRUE(GNDecoder::decode(oss.str(), md, controller));
  CHECK_TRUE(md.event() == "Selection");
  CHECK_TRUE(md.date() == "2013.10.30");
  CHECK_TRUE(md.location() == "NGL, Mitene Union");
  CHECK_TRUE(md.white() == "Komugi");
  CHECK_TRUE(md.black() == "Meruem");
}

TEST(GNDecoderTest, decode_valid_header_with_comments) {
  std::ostringstream oss;
  oss
    << "# Starting off with a comment."
    << std::endl
    << "[Event \"Selection\"] (inline comment)"
    << std::endl
    << "[Date \"2013.10.30\"]"
    << std::endl
    << "[Location \"NGL, Mitene Union\"]"
    << std::endl
    << "[White \"Komugi\"] (another inline comment)"
    << std::endl
    << "[Black \"Meruem\"]"
    << std::endl
    << "# Throw in a comment here for good measure."
    << std::endl
    << "[Result \"*\"]"
    << std::endl;

  GNMetadata md;
  Controller controller;
  CHECK_TRUE(GNDecoder::decode(oss.str(), md, controller));
  CHECK_TRUE(md.event() == "Selection");
  CHECK_TRUE(md.date() == "2013.10.30");
  CHECK_TRUE(md.location() == "NGL, Mitene Union");
  CHECK_TRUE(md.white() == "Komugi");
  CHECK_TRUE(md.black() == "Meruem");
}

TEST(GNDecoderTest, decode_move_empty) {
  Controller controller;
  CHECK_TRUE(GNDecoder::decodeMove("", controller));
}

TEST(GNDecoderTest, decode_move_invalid) {
  Controller controller;
  {
    // Invalid piece.
    CHECK_FALSE(GNDecoder::decodeMove("PB*0-8-0", controller));
  }
  {
    // Not in territory.
    CHECK_FALSE(GNDecoder::decodeMove("PZ*0-0-0", controller));
  }
  {
    // Invalid tier for the drop.
    CHECK_FALSE(GNDecoder::decodeMove("PZ*0-8-1", controller));
  }
  {
    // Initial arrangement only.
    CHECK_FALSE(GNDecoder::decodeMove("PZ<0-8-0>0-7-0", controller));
  }
  {
    // Unknown move.
    CHECK_FALSE(GNDecoder::decodeMove("dafadill", controller));
  }
}

TEST(GNDecoderTest, decode_move_valid) {
  Controller controller;
  CHECK_TRUE(GNDecoder::decodeMove("PZ*0-8-0", controller));
  CHECK_TRUE(GNDecoder::decodeMove("PZ*0-0-0", controller));
}

TEST(GNDecoderTest, decode_invalid_missing_move_indicator) {
  std::ostringstream oss;
  oss
    << "[Event \"Selection\"]"
    << std::endl
    << "[Date \"2013.10.30\"]"
    << std::endl
    << "[Location \"NGL, Mitene Union\"]"
    << std::endl
    << "[White \"Komugi\"]"
    << std::endl
    << "[Black \"Meruem\"]"
    << std::endl
    << "[Result \"*\"]"
    << std::endl
    << "PZ*0-8-0 PZ*0-0-0"
    << std::endl;

  GNMetadata md;
  Controller controller;
  CHECK_FALSE(GNDecoder::decode(oss.str(), md, controller));
}

TEST(GNDecoderTest, decode_invalid_move_indicator) {
  std::ostringstream oss;
  oss
    << "[Event \"Selection\"]"
    << std::endl
    << "[Date \"2013.10.30\"]"
    << std::endl
    << "[Location \"NGL, Mitene Union\"]"
    << std::endl
    << "[White \"Komugi\"]"
    << std::endl
    << "[Black \"Meruem\"]"
    << std::endl
    << "[Result \"*\"]"
    << std::endl
    << "2. PZ*0-8-0 PZ*0-0-0"
    << std::endl;

  GNMetadata md;
  Controller controller;
  CHECK_FALSE(GNDecoder::decode(oss.str(), md, controller));
}

TEST(GNDecoderTest, decode_invalid_move) {
  std::ostringstream oss;
  oss
    << "[Event \"Selection\"]"
    << std::endl
    << "[Date \"2013.10.30\"]"
    << std::endl
    << "[Location \"NGL, Mitene Union\"]"
    << std::endl
    << "[White \"Komugi\"]"
    << std::endl
    << "[Black \"Meruem\"]"
    << std::endl
    << "[Result \"*\"]"
    << std::endl
    << "1. PZ*0-8"
    << std::endl;

  GNMetadata md;
  Controller controller;
  CHECK_FALSE(GNDecoder::decode(oss.str(), md, controller));
}

TEST(GNDecoderTest, decode_invalid_periods) {
  std::ostringstream oss;
  oss
    << "[Event \"Selection\"]"
    << std::endl
    << "[Date \"2013.10.30\"]"
    << std::endl
    << "[Location \"NGL, Mitene Union\"]"
    << std::endl
    << "[White \"Komugi\"]"
    << std::endl
    << "[Black \"Meruem\"]"
    << std::endl
    << "[Result \"*\"]"
    << std::endl
    << "1... PZ*0-8-0 PZ*0-0-0"
    << std::endl;

  GNMetadata md;
  Controller controller;
  CHECK_FALSE(GNDecoder::decode(oss.str(), md, controller));
}

TEST(GNDecoderTest, decode_valid) {
  std::ostringstream oss;
  oss
    << "[Event \"Selection\"]"
    << std::endl
    << "[Date \"2013.10.30\"]"
    << std::endl
    << "[Location \"NGL, Mitene Union\"]"
    << std::endl
    << "[White \"Komugi\"]"
    << std::endl
    << "[Black \"Meruem\"]"
    << std::endl
    << "[Result \"*\"]"
    << std::endl
    << "1. PZ*0-8-0 PZ*0-0-0 2. PZ*1-8-0 PZ*1-0-0"
    << std::endl;

  GNMetadata md;
  Controller controller;
  CHECK_TRUE(GNDecoder::decode(oss.str(), md, controller));
}

TEST(GNDecoderTest, decode_valid_only_move) {
  std::ostringstream oss;
  oss
    << "1. PZ*0-8-0 PZ*0-0-0 2. PZ*1-8-0 PZ*1-0-0"
    << std::endl;

  GNMetadata md;
  Controller controller;
  CHECK_TRUE(GNDecoder::decode(oss.str(), md, controller));
}

TEST(GNDecoderTest, decode_valid_multiline) {
  std::ostringstream oss;
  oss
    << "[Event \"Selection\"]"
    << std::endl
    << "[Date \"2016.09.05\"]"
    << std::endl
    << "[Location \"NGL, Mitene Union\"]"
    << std::endl
    << "[White \"Komugi\"]"
    << std::endl
    << "[Black \"Meruem\"]"
    << std::endl
    << "[Result \"*\"]"
    << std::endl
    << "1. PZ*0-8-0"
    << std::endl
    << "2... PZ*0-0-0"
    << std::endl;

  GNMetadata md;
  Controller controller;
  CHECK_TRUE(GNDecoder::decode(oss.str(), md, controller));
}

TEST(GNDecoderTest, decode_initial_arrangement) {
  const char *data = reinterpret_cast<char *>(initial_arrangement_gn);
  unsigned int data_len = initial_arrangement_gn_len;
  std::string gn(data, data_len);

  GNMetadata md;
  Controller controller;
  CHECK_TRUE(GNDecoder::decode(gn, md, controller));
  CHECK_FALSE(controller.isInitialArrangement());
}
