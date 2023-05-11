/******************************************************************************
 ------------- Copyright (c) 2009-2023 H a r a l d  A c h i t z ---------------
 ---------- < h a r a l d dot a c h i t z at g m a i l dot c o m > ------------
 ---- This Source Code Form is subject to the terms of the Mozilla Public -----
 ---- License, v. 2.0. If a copy of the MPL was not distributed with this -----
 ---------- file, You can obtain one at http://mozilla.org/MPL/2.0/. ----------
 ******************************************************************************/

#include <sl3/types.hpp>

#include <ostream>

namespace sl3
{
  std::string
  typeName (Type type)
  {
    switch (type)
      {
      case Type::Null:
        return "Null";
        break;
      case Type::Int:
        return "Int";
        break;
      case Type::Real:
        return "Real";
        break;
      case Type::Text:
        return "Text";
        break;
      case Type::Blob:
        return "Blob";
        break;
      case Type::Variant:
        return "Variant";
        break;
      }

    return "unknown"; // LCOV_EXCL_LINE
  }

  std::ostream&
  operator<< (std::ostream& os, const Type& t)
  {
    os << "Type::" << typeName (t);
    return os;
  }

  void
  Types::swap (Types& other) noexcept
  {
    using std::swap;
    swap (_cont, other._cont);
  }
}
