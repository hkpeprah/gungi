// scenario_unit_tests.cpp                                            -*-C++-*-
#include "gndecoder.hpp"
#include "gtypes.hpp"
#include "logician.hpp"
#include "tower.hpp"
#include "unit.hpp"
#include "util.hpp"

#include <CppUTest/TestHarness.h>

#include <string>

using namespace gungi;

TEST_GROUP(ScenariosTest) {
  std::string scenarioOne;
  std::string scenarioTwo;

  void setup(void) {
#include "scenarios.res"
    scenarioOne = std::string(reinterpret_cast<char *>(scenario_one_gn), scenario_one_gn_len);
    scenarioTwo = std::string(reinterpret_cast<char *>(scenario_two_gn), scenario_two_gn_len);
  }

  void teardown(void) {
    return;
  }
};

TEST(ScenariosTest, scenario_one) {
  GNMetadata md;
  Controller controller;
  CHECK_TRUE(GNDecoder::decode(scenarioOne, md, controller));
  CHECK_TRUE(controller.isInitialArrangement());
  CHECK_TRUE(controller.isOver());
  CHECK_EQUAL(BLACK, controller.winner());
}

TEST(ScenariosTest, scenario_two) {
  GNMetadata md;
  Controller controller;
  CHECK_TRUE(GNDecoder::decode(scenarioTwo, md, controller));
  CHECK_FALSE(controller.isInitialArrangement());
  CHECK_TRUE(controller.isOver());
  CHECK_EQUAL(WHITE, controller.winner());
}
