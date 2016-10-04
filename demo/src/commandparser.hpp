// commandparser.hpp                                                  -*-C++-*-
#pragma once
//@DESCRIPTION:
//  This component provides a command-line parser for parsing the command-line
//  arguments given to the demo.
//
//@CLASSES:
//  'CommandParser': command-line parser class.
#include <gungi/gungi.hpp>

#include <fstream>
#include <string>

class CommandParser {
  // Command-line parser for the demo.

public:
  // INSTANCE MEMBERS
  std::string       progName;
                     // Program name.

  std::ifstream     inputFile;
                     // The input file to read from.

  std::ofstream     outputFile;
                     // The output file to write to.

  gungi::Controller controller;
                     // Game logic controller.

  unsigned int      moveCount;
                     // Number of moves loaded from the 'inputFile'.

  gungi::GNMetadata metadata;
                     // Game metadata.

public:
  // CREATORS
  CommandParser(int argc, char *argv[]);
    // Initializes an instance of the 'CommandParser' passing it the
    // comand-line argument count, 'argc', and command-line arguments, 'argv'.

  ~CommandParser(void);
    // Default destructor.

  // ACCESSORS
  std::string usage(void) const;
    // Returns a string specifying the program usage.
};
