// main.cpp                                                           -*-C++-*-
#include "app.hpp"
#include "commandparser.hpp"

int main(int argc, char *argv[]) {
  CommandParser parser(argc, argv);
  App app(parser);
  return app.run();
}
