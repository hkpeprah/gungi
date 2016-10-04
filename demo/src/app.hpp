// app.hpp                                                            -*-C++-*-
#pragma once
//@DESCRIPTION:
//  This component provides the main application interface.
//
//@CLASSES:
//  'App': demo application.
//
//@COMMANDS:
//  The commands for the application are listed below:
//         | Type             | Format                           |
//         | ---------------- | -------------------------------- |
//         | Drop             | 'd <piece> <posn>'               |
//         | Move             | 'm <posn>-<tier> <posn>'         |
//         | Immobile Strike  | 'i <posn>-<tier> <tier>'         |
//         | Force Recover    | 'fr'                             |
//         | No Force Recover | 'nfr'                            |
//         | Substitution     | 's <posn>-<tier> <posn>-<tier>'  |
//         | Tier Exchange    | 't <posn>-<tier> <tier>'         |
//  where '<posn>' is a 'row-col' pair, '<piece>' is the Gungi Notation
//  identifier.
#include "commandparser.hpp"

#include <gungi/gungi.hpp>

#include <sstream>
#include <string>

class App {
  // Demo application interface.

private:
  // INSTANCE MEMBERS
  CommandParser&      m_commandParser;
                       // Command-line parser.

  gungi::Controller&  m_controller;
                       // Logic controller.

  std::ostringstream  m_outputStream;
                       // Output string stream.

  unsigned int        m_moveCount;
                       // Count of moves that have been made.

  std::string         m_errorString;
                       // The error message stored in the application.

private:
  // PRIVATE MANIPULATORS
  void print(void);
    // Prints the application interface.

  void handleInput(const std::string& input);
    // Handles the given 'input' command.

public:
  // CREATORS
  App(CommandParser& commandParser);
    // Creates an application from the given command-line parser,
    // 'commandParser'.

  // MANIPULATORS
  int run(void);
    // Runs the application.  Returns an exit status representing the exit
    // status of the app.
};
