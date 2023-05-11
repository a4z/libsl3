/******************************************************************************
 ------------- Copyright (c) 2009-2023 H a r a l d  A c h i t z ---------------
 ---------- < h a r a l d dot a c h i t z at g m a i l dot c o m > ------------
 ---- This Source Code Form is subject to the terms of the Mozilla Public -----
 ---- License, v. 2.0. If a copy of the MPL was not distributed with this -----
 ---------- file, You can obtain one at http://mozilla.org/MPL/2.0/. ----------
 ******************************************************************************/

#include <sl3/error.hpp>

#include <map>
#include <ostream>

namespace sl3
{
  std::ostream&
  operator<< (std::ostream& os, const Error& e)
  {
    os << "sl3::" << ErrCodeName (e.getId ()) << ":" << e.what ();
    return os;
  }
}
