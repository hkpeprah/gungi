// posn.hpp                                                           -*-C++-*-
#pragma once
//@DESCRIPTION:
//  This component provides a definition for the 'Posn' class which represents
//  a position on the board.  A position is defined by a column, which is a
//  character, followed by the row, which is a number.
//
//@CLASSES:
//  'gungi::Posn': class representating a position on the board.
//
//@TYPES:
//  'gungi::PosnSet': set of 'Posn' objects.
#include "gtypes.hpp"

#include <iostream>
#include <vector>

namespace gungi {

class Posn {
  // A 'Posn' is a position on the grid defined by a character and number,
  // representing the column and row respectively (e.g. 'a1').

private:
  // INSTANCE MEMBERS
  unsigned int  m_col;
                 // Column of this position.

  unsigned int  m_row;
                 // Row of this position.

public:
  // CREATORS
  explicit Posn(void);
    // Default constructor.

  Posn(unsigned int col, unsigned int row);
    // Creates an instance of a 'Posn' with the given column, 'col', and the
    // given row, 'row'.

  Posn(const Posn& p);
    // Copy constructor.

  ~Posn(void);
    // Default destructor.

  // ACCESSORS
  const unsigned int& col(void) const;
    // Returns the column of the 'Posn' instance.

  const unsigned int& row(void) const;
    // Returns the row of the 'Posn' instance.

  unsigned int index(void) const;
    // Returns this 'Posn' instance interpreted as a index within a singular
    // array.

  void index(unsigned int& col, unsigned int& row) const;
    // Returns this 'Posn' instance interpreted as a 'row by column' in a
    // two-dimensional matrix.

  move_dir_t adjacent(const Posn& p2) const;
    // Returns the direction to which the given 'Posn', 'p2', is with respect
    // the instance 'Posn', provided 'p2' is adjacent, otherwise returns
    // 'GUNGI_MOVE_DIR_NONE'.

  const bool isValid(void) const;
    // Return 'true' if the 'Posn' is valid, otherwise 'false'.

  // MANIPULATORS
  void up(bool invert=false);
    // Moves the 'Posn' up a row.  Consumes an optional parameter, 'invert',
    // which if 'true' means up is down, down is up, left is right, and right
    // is down.

  void down(bool invert=false);
    // Moves the 'Posn' down a row.  Consumes an optional parameter, 'invert'
    // which if 'true' means up is down, down is up, left is right, and right
    // right is down.

  void left(bool invert=false);
    // Moves the 'Posn' left in the row.  Consumes an optional parameter,
    // 'invert', which if 'true' means up is down, down is up, left is right,
    // and right is down.

  void right(bool invert=false);
    // Moves the 'Posn' right in the row.  Consumes an optional parameter,
    // 'invert', which if 'true' means up is down, down is up, left is right,
    // and right is down.

  // OPERATORS
  bool operator<(const Posn& posn) const;
    // Returns 'true' if the given 'posn' index is less then the instance the
    // index of the instance 'Posn'.

  bool operator==(const Posn& posn) const;
    // Returns 'true' if the given 'posn' has the same column and row as the
    // instance 'Posn'.

  bool operator!=(const Posn& posn) const;
    // Returns 'true' if the given 'posn' is not in the same column and row as
    // the instance 'Posn'.

  friend std::ostream& operator<<(std::ostream& os, const Posn& posn);
    // Formats the given 'posn' and outputs it to the given output stream,
    // 'os'.  Returns the modified output stream.
};

typedef std::vector<Posn> PosnSet;
  // Set of 'Posn' objects.


// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

// OPERATORS
inline std::ostream& operator<<(std::ostream& os, const Posn& posn) {
  os << "Posn(" << posn.col() << ", " << posn.row() << ")";
  return os;
}

}  // close 'gungi' namespace
