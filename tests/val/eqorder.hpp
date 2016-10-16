#pragma once

#include <functional>
#include <boost/test/test_tools.hpp>

namespace sl3
{
  namespace eqo
  {
    template <typename T>
    inline bool
    eq_reflexive (const T& a, std::function<bool(const T&, const T&)> equal)
    {
      return equal (a, a);
    }

    template <typename T>
    inline bool
    weak_reflexive (const T& a,
                    const T& b,
                    std::function<bool(const T&, const T&)> weakeq)
    {
      if (a == b)
        return weakeq (a, b);

      BOOST_ERROR (std::string (__FUNCTION__)
                   + " wrong input, expected: a == b");
      return false;
    }

    template <typename T>
    inline bool
    eq_symmetric (const T& a,
                  const T& b,
                  std::function<bool(const T&, const T&)> equal)
    {
      if (equal (a, b))
        return equal (b, a);

      BOOST_ERROR (std::string (__FUNCTION__)
                   + " wrong input, expected: a == b");
      return false;
    }

    // transitive: a == b && b == c implies a == c
    template <typename T>
    inline bool
    eq_transitive (const T& a,
                   const T& b,
                   const T& c,
                   std::function<bool(const T&, const T&)> equal)
    {
      if (!equal (a, b))
        {
          BOOST_ERROR (std::string (__FUNCTION__)
                       + " wrong input, expected: equal (a, b)");
          return false;
        }
      if (!equal (b, c))
        {
          BOOST_ERROR (std::string (__FUNCTION__)
                       + " wrong input, expected: equal (b, c)");
          return false;
        }

      return equal (a, c);
    }

    // Transitive law: r(a, b) && r(b, c) implies r(a, c)
    // From that we can easily show that !(r(a, b) && r(b, a))
    template <typename T>
    inline bool
    lt_transitive (const T& a,
                   const T& b,
                   const T& c,
                   std::function<bool(const T&, const T&)> r)
    {
      if (!r (a, b))
        {
          BOOST_ERROR (std::string (__FUNCTION__)
                       + " wrong input, expected: r (a, b)");
          return false;
        }
      if (!r (b, c))
        {
          BOOST_ERROR (std::string (__FUNCTION__)
                       + " wrong input, expected: r (b, c)");
          return false;
        }

      return r (a, c) && !(r (a, b) && r (b, a));
    }

    // symmetry of eq, eq(a, b) implies eq(b, a) and that implies !r(b, a).
    template <typename T>
    inline bool
    irreflexive (const T& a,
                 const T& b,
                 std::function<bool(const T&, const T&)> eq,
                 std::function<bool(const T&, const T&)> r)
    {
      if (!eq_symmetric (a, b, eq))
        {
          BOOST_ERROR (std::string (__FUNCTION__)
                       + " , expected eq_symmetric (a,b)");
          return false;
        }

      return !r (a, b) && !r (b, a);
    }

    // Trichotomy law: !eq(a, b) implies r(a, b) || r(b, a)
    template <typename T>
    inline bool
    trichotomy (const T& a,
                const T& b,
                std::function<bool(const T&, const T&)> equal,
                std::function<bool(const T&, const T&)> less)
    {
      if (!equal (a, b))
        {
          return less (a, b) || less (b, a);
        }

      BOOST_ERROR (std::string (__FUNCTION__)
                   + " wrong input, expected: a != b");
      return false;
    }

    /*
     regular type


     1. Equality: in order to use find and other related algorithms STL
     requires operator== to be defined and assumes certain properties of it.

     2. Total ordering: sorting and binary searching that allow us to find
     equal elements fast and on which sorted associative containers are based
     require operator< and assume that it possesses certain properties.
     Total ordering must be consistent with equality.

     3. Copying and assignment: in order to put things into containers and to
     move them with the help of different mutating algorithms STL assumes the
     existence of copy constructors, assignment and related operations.
     They must be consistent with equality.


     here I can add if it is move constructable / assignable,
     and this with nothrow, it is optimal....

     */

    /*
     strong order

     reflexive: a == a
     symmetric: a == b implies b == a
     transitive: a == b && b == c implies a == c


     Irreflexive law: a == b implies !(a < b)
     From this law we can easily derive that a == b implies !(b < a).
     Indeed, by symmetry of equality a == b implies b == a and that implies
     that !(b < a).

     Transitive law: a < b && b < c implies a < c
     From this we can easily derive that !(a < b && b < a).
     Indeed, if a < b && b < a then by transitivity a < a and that contradicts
     the irreflexive law.
     In other words, irreflexivity and transitivity imply anti-symmetry.

     And, finally Trichotomy law: a != b implies a < b || b < a


     */

    /*
     weak order

     Reflexive: a == b implies eq(a, b)
     Symmetric: eq(a, b) implies eq(b, a)
     Transitive: eq(a, b) && eq(b, c) implies eq(a, c)

     Then a relation r(a, b) is a strict weak ordering if it obeys the
     following laws:
     Irreflexive law: eq(a, b) implies !r(a, b)
     From that we can easily show that eq(a, b) implies !r(b, a).
     Indeed, by symmetry of eq, eq(a, b) implies eq(b, a) and that implies
     !r(b, a).

     Transitive law: r(a, b) && r(b, c) implies r(a, c)
     From that we can easily show that !(r(a, b) && r(b, a)).
     Indeed, if r(a, b) && r(b, a) then by transitivity r(a, a) and that
     contradicts the irreflexive law.
     As with strict total ordering, irreflexivity and transitivity imply
     anti-symmetry.

     And, finally, Trichotomy law: !eq(a, b) implies r(a, b) || r(b, a)

     */
  }
}
