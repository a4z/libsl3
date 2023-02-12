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
  as_size_t (ptrdiff_t val)
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
  // I keep that as a comment, since it's so funny, but not use it
  // template <typename T1, typename T2> struct common_typeype
  // {
  //   typedef typename std::conditional < std::numeric_limits<T1>::is_iec559
  //           &&std::numeric_limits<T2>::is_iec559,
  //       typename std::conditional<
  //           std::numeric_limits<
  //               T1>::digits<std::numeric_limits<T2>::digits, T2, T1>::type,
  //           typename std::conditional<std::numeric_limits<T1>::is_iec559,
  //                                     T1,
  //                                     T2>::type>::type type;
  // };

  template <typename T1, typename T2>
  bool
  is_less (const T1& a, const T2& b)
  {
    using common_type = std::common_type_t<T1, T2> ;
    return std::less<common_type>{}(
        static_cast<common_type> (a),
        static_cast<common_type> (b));
  }

  template <typename T1, typename T2>
  bool
  is_greater (const T1& a, const T2& b)
  {
    using common_type = std::common_type_t<T1, T2> ;
    return std::greater<common_type>{}(
        static_cast<common_type> (a),
        static_cast<common_type> (b));
  }

  template <typename T1, typename T2>
  bool
  is_less_equal (const T1& a, const T2& b)
  {
    using common_type = std::common_type_t<T1, T2> ;
    return std::less_equal<common_type>{}(
        static_cast<common_type> (a),
        static_cast<common_type> (b));
  }

  template <typename T1, typename T2>
  bool
  is_equal (const T1& a, const T2& b)
  {
    using common_type = std::common_type_t<T1, T2> ;
    return std::equal_to<common_type>{}(
        static_cast<common_type> (a),
        static_cast<common_type> (b));
  }

}
