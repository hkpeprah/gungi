// util.hpp                                                           -*-C++-*-
#pragma once
//@DESCRIPTION:
//  This component provides a utility class function.
//
//@CLASSES:
//  'gungi::NilDeleter': no-op pointer deleter class.
//  'gungi::Util': gungi utility class.
#include "gtypes.hpp"
#include "posn.hpp"
#include "unit.hpp"

#include <string>

namespace gungi {

// FORWARD DECLARATION
class Unit;

                               // ================
                               // class NilDeleter
                               // ================

template<class T>
class NilDeleter {
  // No-op deleter class used with the 'std::shared_ptr' class.  Acts as a
  // no-op when called to deallocate the allocated pointer.

public:
  // CREATORS
  explicit NilDeleter(void);
    // Default constructor.

  ~NilDeleter(void);
    // Default destructor.

  // OPERATORS
  void operator()(T *ptr) const;
    // No-op deleter.  Does not de-allocate the given 'ptr'.
};


                                  // ==========
                                  // class Util
                                  // ==========

class Util {
  // Utility class for the 'gungi' library.

public:
  // STATIC CLASS MEMBERS
  static const piece_id_t k_FRONT_PIECES[];
    // Array of identifiers for the front pieces.

  static const piece_id_t k_BACK_PIECES[];
    // Array of identifiers for the back pieces.

public:
  // STATIC CLASS METHODS
  static bool isValidPiece(piece_id_t id);
    // Returns 'true' if the given 'id' is valid, otherwise 'false'.

  static bool isInvalidPiece(piece_id_t id);
    // Returns 'true' if the given 'id' is invalid, otherwise 'false'.  Note
    // that this is equivalent to '!isValidPiece()'.

  static bool isFrontPiece(piece_id_t id);
    // Returns 'true' if the given 'id' is the identifier for a front piece,
    // otherwise 'false'.  Note that 'isFrontPiece() == false' does not mean
    // that the given identifier is a back piece (the identifier may be
    // invalid).

  static bool isBackPiece(piece_id_t id);
    // Returns 'true' if the given 'id' is the identifier for a back piece,
    // otherwise 'false'.  Note that 'isBackPiece() == false' does not mean
    // that the given identifier is a front piece (the identifier may be
    // invalid).

  static unsigned char toLowerCase(unsigned char ch);
    // Returns the given character, 'ch', as the lowercase version.

  static std::string toLowerCase(const std::string& str);
    // Returns the given string, 'str', as all lowercase letters.

  static unsigned char toUpperCase(unsigned char ch);
    // Returns the given character, 'ch', as the uppercase version.

  static std::string toUpperCase(const std::string& str);
    // Returns the given string, 'str', as all uppercase letters.

  static SharedUnitPtrVector::iterator find(SharedUnitPtrVector& v,
                                            const Unit          *unit);
    // Returns an iterator to the shared poitner for the given 'unit' in the
    // given vector, 'v' if the unit exists with the vector, otherwise
    // returns 'v.end()'.

  static SharedUnitPtrVector::const_iterator find(
                                              const SharedUnitPtrVector& v,
                                              const Unit                *unit);
    // Returns a constant iterator to the shared pointer for the given 'unit'
    // in the given vector, 'v' if the unit exists within the vector,
    // otherwise returns 'v.end()'.

  static PosnSet::const_iterator find(const PosnSet& set, const Posn& posn);
    // Returns a constant iterator to the 'Posn' for the given 'posn' in the
    // given 'PosnSet', 'set', if the 'posn' is found, otherwise returns
    // 'set.end()'.

  static bool anyWalk(const Unit *unit,
                      tier_t      tier,
                      const Posn& start,
                      bool        invert = false);
    // Returns 'true' if there is any valid walk for the given 'unit' at the
    // given tower 'tier', starting at the given 'Posn', 'start', otherwise
    // 'false'.  The optionally specified 'invert' parameter determines if the
    // walk is top-down ('true'), otherwise bottom-up ('false'), defaults to
    // 'false'.

  static PosnSet allWalks(const Unit *unit,
                          tier_t      tier,
                          const Posn& start,
                          bool        invert = false);
    // Returns the set of all positions that the given 'unit' at the given
    // 'Posn', 'start', and tier, 'tier', can reach.  The optionally specified
    // 'invert' parameter determines if the walk is top-down ('true'),
    // otherwise bottom-up ('false'), defaults to 'false'.

  static PosnSet getWalk(const Unit *unit,
                         tier_t      tier,
                         const Posn& start,
                         const Posn& end,
                         error_t&    error,
                         bool        invert = false);
    // Returns a set of 'Posn' generated from walking from the given 'start'
    // position to the given 'end' position for the given 'unit' at the given
    // 'tier'.  Return an error status of 'GUNGI_ERROR_NONE' in the given
    // output parameter, 'error', if there was a successfully path found,
    // otherwise 'GUNGI_ERROR_NO_WALK'.  The optionally specified 'invert'
    // parameter determines if the walk is top-down ('true'), otherwise
    // bottom-up ('false'), defaults to 'false'.

  static PosnSet crossed(const Posn& a, const Posn& b);
    // Returns the set of 'Posn's that are crossed when drawing a straight line
    // from the given 'Posn', 'a', to the given 'Posn', 'b', excluding 'a' and
    // 'b' respectively.
};


// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                               // ================
                               // class NilDeleter
                               // ================

// CREATORS
template<class T>
inline NilDeleter<T>::NilDeleter(void) {
  // DO NOTHING
}

template<class T>
inline NilDeleter<T>::~NilDeleter(void) {
  // DO NOTHING
}

// OPERATORS
template<class T>
inline void NilDeleter<T>::operator()(T *ptr) const {
  (void)ptr;
}

}  // close 'gungi' namespace
