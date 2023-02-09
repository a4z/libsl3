#pragma once

#include <cassert>
#include <cstddef>
#include <limits>

namespace sl3
{

  inline size_t
  as_size_t (int val)
  {
    assert (val >= 0);
    return static_cast<size_t> (val);
  }

  inline size_t
  as_size_t (long val)
  {
    assert (val >= 0);
    return static_cast<size_t> (val);
  }

  inline int
  as_int (size_t val)
  {
    assert (val < std::numeric_limits<int>::max ());
    return static_cast<int> (val);
  }

  // gcc -Werror=conversion helpers
  template <typename T1, typename T2> struct higher_precision_type
  {
    typedef typename std::conditional < std::numeric_limits<T1>::is_iec559
            &&std::numeric_limits<T2>::is_iec559,
        typename std::conditional<
            std::numeric_limits<
                T1>::digits<std::numeric_limits<T2>::digits, T2, T1>::type,
            typename std::conditional<std::numeric_limits<T1>::is_iec559,
                                      T1,
                                      T2>::type>::type type;
  };

  template <typename T1, typename T2>
  bool
  is_less (const T1& a, const T2& b)
  {
    typedef typename higher_precision_type<T1, T2>::type higher_precision_t;
    return std::less<higher_precision_t>{}(
        static_cast<higher_precision_t> (a),
        static_cast<higher_precision_t> (b));
  }

  template <typename T1, typename T2>
  bool
  is_greater (const T1& a, const T2& b)
  {
    typedef typename higher_precision_type<T1, T2>::type higher_precision_t;
    return std::greater<higher_precision_t>{}(
        static_cast<higher_precision_t> (a),
        static_cast<higher_precision_t> (b));
  }

  template <typename T1, typename T2>
  bool
  is_less_equal (const T1& a, const T2& b)
  {
    typedef typename higher_precision_type<T1, T2>::type higher_precision_t;
    return std::less_equal<higher_precision_t>{}(
        static_cast<higher_precision_t> (a),
        static_cast<higher_precision_t> (b));
  }

  template <typename T1, typename T2>
  bool
  is_equal (const T1& a, const T2& b)
  {
    typedef typename higher_precision_type<T1, T2>::type higher_precision_t;
    return std::equal_to<higher_precision_t>{}(
        static_cast<higher_precision_t> (a),
        static_cast<higher_precision_t> (b));
  }

}
