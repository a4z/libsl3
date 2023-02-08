/******************************************************************************
 ------------- Copyright (c) 2009-2017 H a r a l d  A c h i t z ---------------
 ---------- < h a r a l d dot a c h i t z at g m a i l dot c o m > ------------
 ---- This Source Code Form is subject to the terms of the Mozilla Public -----
 ---- License, v. 2.0. If a copy of the MPL was not distributed with this -----
 ---------- file, You can obtain one at http://mozilla.org/MPL/2.0/. ----------
 ******************************************************************************/

#include <sl3/columns.hpp>
#include <sl3/error.hpp>

#include <cassert>
#include <sqlite3.h>

#include "utils.hpp"

namespace sl3
{
  Columns::Columns (sqlite3_stmt* stmt)
  : _stmt (stmt)
  {
  }

  int
  Columns::count () const
  {
    return sqlite3_column_count (_stmt);
  }

  std::string
  Columns::getName (int idx) const
  {
    if (idx < 0 || !(idx < count ()))
      throw ErrOutOfRange ("column index out of range");

    const char* name = sqlite3_column_name (_stmt, idx);
    return name ? std::string (name) : std::string ();
  }

  DbValue
  Columns::getValue (int idx) const
  {
    return getValue (idx, Type::Variant);
  }

  DbValue
  Columns::getValue (int idx, Type type) const
  {
    if (idx < 0 || !(idx < count ()))
      throw ErrOutOfRange ("column index out of range");

    switch (sqlite3_column_type (_stmt, idx))
      {
      case SQLITE_INTEGER:
        return DbValue (getInt64 (idx), type);
        break;

      case SQLITE_FLOAT:
        return DbValue (getReal (idx), type);
        break;

      case SQLITE_TEXT:
        return DbValue (getText (idx), type);
        break;

      case SQLITE_BLOB:
        return DbValue (getBlob (idx), type);
        break;

      case SQLITE_NULL:
        return DbValue (type);
        break;

      default: // LCOV_EXCL_START
        throw ErrUnexpected ("should never reach this line");
        break;
      }

    return DbValue (type); // for the lexer  LCOV_EXCL_STOP
  }

  std::vector<std::string>
  Columns::getNames () const
  {
    std::vector<std::string> names;
    names.reserve (as_size_t (count ()));
    for (int i = 0; i < count (); ++i)
      {
        names.emplace_back (getName (i));
      }

    return names;
  }

  DbValues
  Columns::getRow () const
  {
    DbValues::container_type v;
    for (int i = 0; i < count (); ++i)
      {
        v.push_back (getValue (i));
      }
    return DbValues (std::move (v));
  }

  DbValues
  Columns::getRow (const Types& types) const
  {
    if (types.size () != static_cast<size_t> (count ()))
      {
        throw ErrTypeMisMatch ("types.size () != getColumnCount ()");
      }

    DbValues::container_type v;
    for (int i = 0; i < count (); ++i)
      {
        v.push_back (getValue (i, types[as_size_t (i)]));
      }
    return DbValues (std::move (v));
  }

  Type
  Columns::getType (int idx) const
  {
    if (idx < 0 || !(idx < count ()))
      throw ErrOutOfRange ("column index out of range");

    auto type = Type::Variant;

    switch (sqlite3_column_type (_stmt, idx))
      {
      case SQLITE_INTEGER:
        type = Type::Int;
        break;

      case SQLITE_FLOAT:
        type = Type::Real;
        break;

      case SQLITE_TEXT:
        type = Type::Text;
        break;

      case SQLITE_BLOB:
        type = Type::Blob;
        break;

      case SQLITE_NULL:
        type = Type::Null;
        break;

      default:
        throw ErrUnexpected ("never reach"); // LCOV_EXCL_LINE
        break;                               // LCOV_EXCL_LINE
      }

    return type;
  }

  size_t
  Columns::getSize (int idx) const
  {
    if (idx < 0 || !(idx < count ()))
      throw ErrOutOfRange ("column index out of range");

    return as_size_t (sqlite3_column_bytes (_stmt, idx));
  }

  std::string
  Columns::getText (int idx) const
  {
    if (idx < 0 || !(idx < count ()))
      throw ErrOutOfRange ("column index out of range");

    const char* first
        = reinterpret_cast<const char*> (sqlite3_column_text (_stmt, idx));
    std::size_t s = as_size_t (sqlite3_column_bytes (_stmt, idx));
    return s > 0 ? std::string (first, s) : std::string ();
  }

  int
  Columns::getInt (int idx) const
  {
    if (idx < 0 || !(idx < count ()))
      throw ErrOutOfRange ("column index out of range");

    return sqlite3_column_int (_stmt, idx);
  }

  int64_t
  Columns::getInt64 (int idx) const
  {
    if (idx < 0 || !(idx < count ()))
      throw ErrOutOfRange ("column index out of range");

    return sqlite3_column_int64 (_stmt, idx);
  }

  double
  Columns::getReal (int idx) const
  {
    if (idx < 0 || !(idx < count ()))
      throw ErrOutOfRange ("column index out of range");

    return sqlite3_column_double (_stmt, idx);
  }

  Blob
  Columns::getBlob (int idx) const
  {
    if (idx < 0 || !(idx < count ()))
      throw ErrOutOfRange ("column index out of range");

    using value_type = Blob::value_type;
    const value_type* first
        = static_cast<const value_type*> (sqlite3_column_blob (_stmt, idx));
    std::size_t s = as_size_t (sqlite3_column_bytes (_stmt, idx));
    return s > 0 ? Blob (first, first + s) : Blob ();
  }

} // ns
