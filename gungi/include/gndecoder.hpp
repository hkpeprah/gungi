// gndecoder.hpp                                                      -*-C++-*-
#pragma once
//@DESCRIPTION:
//  This component provides utility classes for working with Gungi Notation
//  strings: decoding a string in order to update a Controller with the moves
//  specified by the notation string, and parsing metadata from a notation
//  string.
//
//@CLASSES:
//  'gungi::GNMetadata': metadata class for a Gungi Notation file.
//  'gungi::GNDecoder': decoder class to turn Gungi Notation into a game.
//
//@EXAMPLE:
//  ```
//  [Event "Selection"]
//  [Date "2013.10.30"]
//  [Location "NGL, Mitene Union"]
//  [White "Komugi"]
//  [Black "Meruem"]
//  [Result "*"]
//  # King's Selection match between Komugi and Meruem
//  # Witnessed by Neferpitou
//  1. PZ*0-8-0 PZ*0-0-0 2. PZ*8-8-0 PZ*1-1-0
//  ```
#include "gtypes.hpp"
#include "logician.hpp"

#include <string>
#include <sstream>

namespace gungi {

                               // ================
                               // class GNMetadata
                               // ================

class GNMetadata {
  // Game meta read from a '.gn' file.  This metadata is not essential to the
  // game, but can be used to build a header.

protected:
  // INSTANCE MEMBERS
  std::string  m_event;
                // Name of the event.

  std::string  m_date;
                // Date the event / match took place.

  std::string  m_location;
                // Location of the event.

  std::string  m_white;
                // Name of the white player.

  std::string  m_black;
                // Name of the black player.

public:
  // CREATORS
  GNMetadata(void);
    // Default constructor.

  // ACCESSORS
  const std::string& event(void) const;
    // Returns the event string.

  const std::string& date(void) const;
    // Returns the date string.

  const std::string& location(void) const;
    // Returns the location string.

  const std::string& white(void) const;
    // Returns the white player's name.

  const std::string& black(void) const;
    // Returns the black player's name.

  std::string header(const Controller& controller) const;
    // Returns the header string for the given 'controller' using the
    // underlying metadata.

  // MANIPULATORS
  void setEvent(const std::string& event);
    // Sets the event name to the given 'event'.

  bool setDate(const std::string& date);
    // Sets the date string to the given 'date'.  The date is expected to be
    // formatted as 'YYYY.MM.DD'.  Returns 'true' on success and sets the
    // underlying date, otherwise 'false' if the date is malformed.

  void setLocation(const std::string& location);
    // Sets the location string to the given 'location'.

  void setWhite(const std::string& name);
    // Sets the white player to the given 'name'.

  void setBlack(const std::string& name);
    // Sets the black player to the given 'name'.
};


                                // ===============
                                // class GNDecoder
                                // ===============

class GNDecoder {
  // Utility class for decoding '.gn' files.

private:
  // PRIVATE STATIC CLASS MEMBERS
  static bool decodeComment(std::stringstream& ss);
    // Decodes and drops comments from the given stringstream, 'ss'.  Returns
    // 'true' if the comment was successfully parsed, otherwise 'false'.

  static bool decodeHeader(std::stringstream& ss, GNMetadata& md);
    // Decodes the header from the given stringstream, 'ss', and populates the
    // given Gungi Notation metadata, 'md'.  Returns 'true' if was succesfully
    // decoded, otherwise 'false'.

  static bool decodeMovetext(std::stringstream& ss, Controller& controller);
    // Decodes the movetext from the given stringstream, 'ss', and applies the
    // moves to the given 'controller'.  Returns 'true' if successfully
    // decoded, otherwise 'false'.

public:
  // STATIC CLASS MEMBERS
  static bool decodeMove(std::string move, Controller& controller);
    // Decodes the given 'move', updating the given 'controller' based on the
    // move.  Returns 'true' if the move was valid, otherwise 'false'.

  static bool decode(const std::string& gn, GNMetadata& md, Controller& controller);
    // Decodes the given Gungi Notation contents, 'gn', updating the given
    // 'controller', and returning the metadata, if any, to the given output
    // parameter, 'md'.  Returns 'true' on decode success, otherwise 'false'.
};

}  // close 'gungi' namespace
