/******************************************************************************
 ------------- Copyright (c) 2009-2017 H a r a l d  A c h i t z ---------------
 ---------- < h a r a l d dot a c h i t z at g m a i l dot c o m > ------------
 ---- This Source Code Form is subject to the terms of the Mozilla Public -----
 ---- License, v. 2.0. If a copy of the MPL was not distributed with this -----
 ---------- file, You can obtain one at http://mozilla.org/MPL/2.0/. ----------
 ******************************************************************************/

#include <sl3/config.hpp>

#include <sl3/database.hpp>
#include <sl3/error.hpp>
#include <sqlite3.h>

namespace sl3
{
  //----------------------------------------------------------------------------
  // implement functions from config.h
  const char*
  sqliteCompiledVersion ()
  {
    return SQLITE_VERSION;
  }
  int
  sqliteCompiledVersionNumber ()
  {
    return SQLITE_VERSION_NUMBER;
  }

  const char*
  sqliteRuntimeVersion ()
  {
    return sqlite3_libversion ();
  }

  int
  sqliteRuntimeVersionNumber ()
  {
    return sqlite3_libversion_number ();
  }

  int
  sqliteThreadSafeCompileOption ()
  {
    return sqlite3_threadsafe ();
  }
  //----------------------------------------------------------------------------

} // ns
