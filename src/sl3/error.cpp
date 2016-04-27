/******************************************************************************
 ------------- Copyright (c) 2009-2016 H a r a l d  A c h i t z ---------------
 ---------- < h a r a l d dot a c h i t z at g m a i l dot c o m > ------------
 ---- This Source Code Form is subject to the terms of the Mozilla Public -----
 ---- License, v. 2.0. If a copy of the MPL was not distributed with this -----
 ---------- file, You can obtain one at http://mozilla.org/MPL/2.0/. ----------
 ******************************************************************************/

#include <sl3/error.hpp>

#include <ostream>
#include <map>

namespace sl3
{

  namespace
  {
    std::string
    ErrCodeName (ErrCode ec)
    {
      switch (ec)
        {
        case ErrCode::SQL3Error:
          return "SQLite3Error";
          break;
        case ErrCode::NoConnection:
          return "NoConnection";
          break;
        case ErrCode::OutOfRange:
          return "OutOfRange";
          break;
        case ErrCode::TypeMisMatch:
          return "TypeMisMatch";
          break;
        case ErrCode::NullValueAccess:
          return "NullValueAccess";
          break;
        case ErrCode::UNEXPECTED:
          return "UNEXPECTED";
          break;
        }

      return std::to_string ((int) ec); //LCOV_EXCL_LINE
    }
  } //--------------------------------------------------------------------------


  Error::Error (ErrCode id, std::string info) :
      _id (id) ,
      _info (std::move (info))
  {
  }


  ErrCode
  Error::getId () const
  {
    return _id;
  }


  const std::string&
  Error::info () const
  {
    return _info;
  }


  void
  Error::toStream (std::ostream& os) const
  {
    os << "sl3::" << ErrCodeName (getId ()) << ":" << info ();
  }


  void
  raiseErrUnexpected (const std::string& msg)
  {
    throw ErrUnexpected (msg) ;
  }


  SQLite3Error::SQLite3Error( int sl3ec, const char* sl3msg )
  : ErrType<ErrCode::SQL3Error>( std::string("") )
  , m_sqlite_ec( sl3ec )
  , m_sqlite_msg ( sl3msg )
  {
  }


  SQLite3Error::SQLite3Error( int sl3ec, const char* sl3msg , const std::string& msg)
  : ErrType<ErrCode::SQL3Error>( msg )
  , m_sqlite_ec( sl3ec )
  , m_sqlite_msg ( sl3msg )
  {
  }


  int
  SQLite3Error::SQLiteErrorCode () const
  {
    return m_sqlite_ec ;
  }


  const std::string&
  SQLite3Error::SQLiteErrorMessage() const
  {
    return m_sqlite_msg ;
  }

  void
  SQLite3Error::toStream (std::ostream& os) const
  {

    os << "sl3::" << ErrCodeName (getId ()) <<
        "(" << m_sqlite_ec << ":" << m_sqlite_msg << "):"
        << info ();

  }


  std::ostream&
  operator<< (std::ostream& os, const Error& e)
  {
    e.toStream (os);
    return os;
  }


}
