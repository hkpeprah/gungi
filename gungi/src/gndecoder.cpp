// gndecoder.cpp                                                      -*-C++-*-
#include "gndecoder.hpp"

#include "gtypes.hpp"
#include "posn.hpp"
#include "logician.hpp"
#include "tower.hpp"
#include "util.hpp"

#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <utility>

namespace gungi {

                               // ================
                               // class GNMetadata
                               // ================

// CREATORS
GNMetadata::GNMetadata(void)
: m_event("")
, m_date("")
, m_location("")
, m_white("")
, m_black("")
{
  // DO NOTHING
}

// ACCESSORS
const std::string& GNMetadata::event(void) const {
  return m_event;
}

const std::string& GNMetadata::date(void) const {
  return m_date;
}

const std::string& GNMetadata::location(void) const {
  return m_location;
}

const std::string& GNMetadata::white(void) const {
  return m_white;
}

const std::string& GNMetadata::black(void) const {
  return m_black;
}

std::string GNMetadata::header(const Controller& controller) const {
  std::ostringstream oss;
  std::vector<std::pair<std::string, std::string> > pairs = {
    { "Event", m_event },
    { "Date", m_date },
    { "Location", m_location },
    { "White", m_white },
    { "Black", m_black }
  };

  for (std::pair<std::string, std::string> pair : pairs) {
    oss
      << "["
      << pair.first
      << " \""
      << pair.second
      << "\"]"
      << std::endl;
  }

  oss << "[Result \"";

  if (!controller.isOver()) {
    oss << "*";
  } else if (controller.isDraw()) {
    oss << "1/2 - 1/2";
  } else if (controller.isInCheckmate(BLACK)) {
    oss << "1 - 0";
  } else {
    oss << "0 - 1";
  }

  oss
    << "\"]"
    << std::endl;

  return oss.str();
}

// MANIPULATORS
void GNMetadata::setEvent(const std::string& event) {
  m_event = event;
}

bool GNMetadata::setDate(const std::string& date) {
  unsigned int year, month, day;
  if (sscanf(date.c_str(), "%u.%u.%u", &year, &month, &day)) {
    if (month > 12 || day > 31) {
      return false;
    }
    m_date = date;
    return true;
  }
  return false;
}

void GNMetadata::setLocation(const std::string& location) {
  m_location = location;
}

void GNMetadata::setWhite(const std::string& name) {
  m_white = name;
}

void GNMetadata::setBlack(const std::string& name) {
  m_black = name;
}


                                // ===============
                                // class GNDecoder
                                // ===============

// PRIVATE STATIC CLASS MEMBERS
bool GNDecoder::decodeComment(std::stringstream& ss) {
  char ch;
  ss >> ch;

  if (ch == '#') {
    std::string str;
    std::getline(ss, str);
  } else if (ch == '(') {
    while (ch != ')' && !(ss.eof() || ss.fail())) {
      ss >> ch;
    }
    return (ch == ')');
  }

  return true;
}

bool GNDecoder::decodeHeader(std::stringstream& ss, GNMetadata& md) {
  while (ss.good()) {
    ss >> std::ws;
    char ch = ss.peek();

    if (ch == EOF) {
      break;
    } else if (ch == '#' || ch == '(') {
      if (!decodeComment(ss)) {
        return false;
      }
      continue;
    } else if (ch == '[') {
      // Decode header component.
      unsigned char lbrack;
      std::string name, value;
      ss >> lbrack >> name;
      if (ss.fail()) {
        return false;
      }

      // Read the value.  Since this may contain spaces, have to read from the
      // the starting quotation until the ending quotation is reached.
      ss >> std::ws;
      ss >> ch;
      if (ch != '"') {
        return false;
      }

      while (ss.good()) {
        ss >> std::noskipws >> ch;
        if (ch == '"') {
          // Verify that is is followdd by an rbrack.
          ss >> std::ws;
          ss >> ch;
          if (ch != ']') {
            return false;
          }
          break;
        }

        value.append(1, static_cast<char>(ch));
      }

      // Uppercase or lowercase are fine for the name.
      name = Util::toLowerCase(name);

      if (name == "event") {
        md.setEvent(value);
      } else if (name == "date") {
        if (!md.setDate(value)) {
          return false;
        }
      } else if (name == "location") {
        md.setLocation(value);
      } else if (name == "white") {
        md.setWhite(value);
      } else if (name == "black") {
        md.setBlack(value);
      } else if (name == "result") {
        // Ignore the result.   This does not have any barring on populating
        // the metadata.
        continue;
      } else {
        // Unknown name.
        return false;
      }
    } else {
      break;
    }
  }

  return true;
}

bool GNDecoder::decodeMove(std::string move, Controller& controller) {
  char front[2] = { 0 };
  char back[2] = { 0 };
  unsigned int row1, row2, col1, col2, tier1, tier2;
  colour_t colour = controller.isPlayersTurn(BLACK) ? BLACK : WHITE;
  const std::vector<Tower>& towers = controller.board();
  error_t error = GUNGI_ERROR_NONE;

  if (sscanf(move.c_str(),
             "%c%c<%u-%u-%u>%u-%u-%u",
             front,
             back,
             &col1,
             &row1,
             &tier1,
             &col2,
             &row2,
             &tier2) == 8) {
    // Move.
    Posn from(col1, row1);
    Posn to(col2, row2);
    if (!to.isValid() || !from.isValid()) {
      return false;
    } else if (towers[to.index()].height() != tier2 &&
               towers[to.index()].height() != tier2 + 1) {
      // Not moving into the top of the tower, so invalidate this parameter, as
      // it is not actually passed to the '.moveUnit()' call.
      return false;
    }

    piece_id_t frontPiece = gn_identifier_to_piece(front);
    piece_id_t backPiece = gn_identifier_to_piece(back);
    const Unit *unit = towers[from.index()].at(tier1, error);
    if (!unit ||
        unit->front() != frontPiece ||
        unit->back() != backPiece ||
        unit->colour() != colour) {
      // Have to validate the unit is the correct one as well, since this is
      // not checked by the controller call.
      return false;
    }

    controller.moveUnit(from, tier1, to, error);
  } else if (sscanf(move.c_str(),
                    "%c%c*%u-%u-%u",
                    front,
                    back,
                    &col1,
                    &row1,
                    &tier1) == 5) {
    // Drop.
    Posn to(col1, row1);
    if (to.isValid() && towers[to.index()].height() != tier1) {
      // Not moving into the top of the tower, so invalidate this parameter, as
      // it is not actually passed to the '.dropUnit()' call.
      return false;
    }

    piece_id_t frontPiece = gn_identifier_to_piece(front);
    piece_id_t backPiece = gn_identifier_to_piece(back);
    controller.dropUnit(frontPiece, backPiece, to, error);
  } else if (sscanf(move.c_str(),
                    "%c%c<%u-%u-%ux%u-%u-%u",
                    front,
                    back,
                    &col1,
                    &row1,
                    &tier1,
                    &col2,
                    &row2,
                    &tier2) == 8) {
    // Mobile strike.
    Posn from(col1, row1);
    Posn to(col2, row2);
    if (!to.isValid() || !from.isValid()) {
      return false;
    } else if (towers[to.index()].height() != tier2 + 1) {
      // Not moving into the top of the tower, so invalidate this parameter, as
      // it is not actually passed to the '.moveUnit()' call.
      return false;
    }

    piece_id_t frontPiece = gn_identifier_to_piece(front);
    piece_id_t backPiece = gn_identifier_to_piece(back);
    const Unit *unit = towers[from.index()].at(tier1, error);
    if (!unit ||
        unit->front() != frontPiece ||
        unit->back() != backPiece ||
        unit->colour() != colour) {
      // Have to validate the unit is the correct one as well, since this is
      // not checked by the controller call.
      return false;
    }

    controller.moveUnit(from, tier1, to, error);
  } else if (sscanf(move.c_str(),
                    "%c%c<%u-%u-%ux%u",
                    front,
                    back,
                    &col1,
                    &row1,
                    &tier1,
                    &tier2) == 6) {
    // Immobile strike.
    Posn posn(col1, row1);
    piece_id_t frontPiece = gn_identifier_to_piece(front);
    piece_id_t backPiece = gn_identifier_to_piece(back);
    const Unit *unit = towers[posn.index()].at(tier1, error);
    if (!unit ||
        unit->front() != frontPiece ||
        unit->back() != backPiece ||
        unit->colour() != colour) {
      // Have to validate the unit is the correct one that the movetext
      // specifies, since those parameters are not passed to the controller
      // call.
      return false;
    }

    controller.immobileStrike(posn, tier1, tier2, error);
  } else if (sscanf(move.c_str(),
                    "%c%c+%u-%u-%u",
                    front,
                    back,
                    &col1,
                    &row1,
                    &tier1) == 5) {
    // Forced recovery to self.
    Posn posn(col1, row1);
    piece_id_t frontPiece = gn_identifier_to_piece(front);
    piece_id_t backPiece = gn_identifier_to_piece(back);
    const Unit *unit = controller.forcedRearrangeUnit();
    if (!unit || !unit->tower()) {
      // No forced rearrangement.
      return false;
    }

    if (unit->front() != frontPiece ||
        unit->back() != backPiece ||
        unit->colour() == colour) {
      // Invalid unit being force recovered.
      return false;
    }

    const Tower *tower = unit->tower();
    if (tower->posn() != posn) {
      // Not the correct tower.
      return false;
    }

    const Unit *towerUnit = tower->at(tier1, error);
    if (towerUnit && unit != towerUnit) {
      // Not the unit at the given tier.
      return false;
    }

    controller.forceRecover(true /* recover */, error);
  } else if (sscanf(move.c_str(),
                    "%c%c^%u-%u-%u",
                    front,
                    back,
                    &col1,
                    &row1,
                    &tier1) == 5) {
    // Forced recovery to the opponent.
    Posn posn(col1, row1);
    piece_id_t frontPiece = gn_identifier_to_piece(front);
    piece_id_t backPiece = gn_identifier_to_piece(back);
    const Unit *unit = controller.forcedRearrangeUnit();
    if (!unit || !unit->tower()) {
      // No forced rearrangement.
      return false;
    }

    if (unit->front() != frontPiece ||
        unit->back() != backPiece ||
        unit->colour() == colour) {
      // Invalid unit being force recovered.
      return false;
    }

    const Tower *tower = unit->tower();
    if (tower->posn() != posn) {
      // Not the correct tower.
      return false;
    }

    const Unit *towerUnit = tower->at(tier1, error);
    if (towerUnit && unit != towerUnit) {
      // Not the unit at the given tier.
      return false;
    }

    controller.forceRecover(true /* recover */, error);
  } else if (sscanf(move.c_str(),
                    "%c%c=%u-%u-%u",
                    front,
                    back,
                    &col1,
                    &row1,
                    &tier1) == 5) {
    // No forced recovery.
    Posn posn(col1, row1);
    piece_id_t frontPiece = gn_identifier_to_piece(front);
    piece_id_t backPiece = gn_identifier_to_piece(back);
    const Unit *unit = controller.forcedRearrangeUnit();
    if (!unit || !unit->tower()) {
      // No forced rearrangement.
      return false;
    }

    if (unit->front() != frontPiece ||
        unit->back() != backPiece ||
        unit->colour() == colour) {
      // Invalid unit being force recovered.
      return false;
    }

    const Tower *tower = unit->tower();
    if (tower->posn() != posn) {
      // Not the correct tower.
      return false;
    }

    const Unit *towerUnit = tower->at(tier1, error);
    if (towerUnit && unit != towerUnit) {
      // Not the unit at the given tier.
      return false;
    }

    controller.forceRecover(false /* no recover */, error);
  } else if (sscanf(move.c_str(),
                    "%c%c<%u-%u-%u&%u-%u-%u",
                    front,
                    back,
                    &col1,
                    &row1,
                    &tier1,
                    &col2,
                    &row2,
                    &tier2) == 8) {
    // Substitution.
    Posn from(col1, row1);
    Posn to(col2, row2);
    if (!from.isValid()) {
      return false;
    }

    piece_id_t frontPiece = gn_identifier_to_piece(front);
    piece_id_t backPiece = gn_identifier_to_piece(back);
    const Unit *unit = towers[from.index()].at(tier1, error);
    if (!unit ||
        unit->front() != frontPiece ||
        unit->back() != backPiece ||
        unit->colour() != colour) {
      // Have to validate the unit is the correct one as well, since this is
      // not checked by the controller call.
      return false;
    }

    controller.exchangeUnits(GUNGI_EFFECT_SUBSTITUTION,
                             from,
                             tier1,
                             to,
                             tier2,
                             error);
  } else if (sscanf(move.c_str(),
                    "%c%c<%u-%u-%u&%u",
                    front,
                    back,
                    &col1,
                    &row1,
                    &tier1,
                    &tier2) == 6) {
    // 1-3 Tier Exchange.
    Posn from(col1, row1);
    Posn to(col2, row2);
    if (!from.isValid()) {
      return false;
    }

    piece_id_t frontPiece = gn_identifier_to_piece(front);
    piece_id_t backPiece = gn_identifier_to_piece(back);
    const Unit *unit = towers[from.index()].at(tier1, error);
    if (!unit ||
        unit->front() != frontPiece ||
        unit->back() != backPiece ||
        unit->colour() != colour) {
      // Have to validate the unit is the correct one as well, since this is
      // not checked by the controller call.
      return false;
    }

    controller.exchangeUnits(GUNGI_EFFECT_1_3_TIER_EXCHANGE,
                             from,
                             tier1,
                             to,
                             tier2,
                             error);
  } else {
    // Unknown move string or empty.
    return (move.find_first_not_of(" \t\n\r") == std::string::npos);
  }

  return (error == GUNGI_ERROR_NONE);
}

bool GNDecoder::decodeMovetext(std::stringstream& ss, Controller& controller) {
  unsigned int moveCount = 1;

  while (ss.good()) {
    ss >> std::ws;
    char ch = ss.peek();

    if (ch == EOF) {
      // Early exit, we hit EOF.
      break;
    }

    if (ch == '#' || ch == 'C') {
      decodeComment(ss);
      continue;
    }

    unsigned int count;
    std::string periods;
    std::string moveString;
    ss >> std::skipws >> count >> periods >> moveString;

    if (ss.fail()) {
      return false;
    } else if (count != moveCount++) {
      // Move indicator is not the correct one.
      return false;
    } else if (periods == ".") {
      if (!controller.isPlayersTurn(BLACK)) {
        // One period means it's black's turn, so this is poorly formatted.
        return false;
      }
    } else if (periods == "...") {
      if (!controller.isPlayersTurn(WHITE)) {
        // Three periods means it's white's turn, so this is poorly formatted.
        return false;
      }
    } else {
      // Otherwise the periods are poorly formatted.
      return false;
    }

    if (!decodeMove(moveString, controller)) {
      // Try to decode the move.
      return false;
    }

    ss >> std::ws;
    ch = ss.peek();
    if (ch == EOF) {
      // Early exit, we hit EOF.
      break;
    }

    if (ch == '#' || ch == 'C') {
      while (ch == '#' || ch == '(') {
        decodeComment(ss);
        ss >> std::ws;
        ch = ss.peek();
      }
    }

    if (ss.eof() || ss.fail()) {
      break;
    }

    ss >> std::ws;
    ch = ss.peek();
    if (ch >= '1' && ch <= '9') {
      // Start of another move indicator, so continue.
      continue;
    }

    ss >> std::skipws >> moveString;
    if (!decodeMove(moveString, controller)) {
      // Try to decode the move.
      return false;
    }
  }

  return true;
}

// STATIC CLASS MEMBERS
bool GNDecoder::decode(const std::string& gn, GNMetadata& md, Controller& controller) {
  std::stringstream ss(gn);
  if (!decodeHeader(ss, md)) {
    return false;
  }

  if (!decodeMovetext(ss, controller)) {
    return false;
  }

  return true;
}

}  // close 'gungi' namespace
