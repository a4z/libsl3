/******************************************************************************
 ------------- Copyright (c) 2009-2017 H a r a l d  A c h i t z ---------------
 ---------- < h a r a l d dot a c h i t z at g m a i l dot c o m > ------------
 ---- This Source Code Form is subject to the terms of the Mozilla Public -----
 ---- License, v. 2.0. If a copy of the MPL was not distributed with this -----
 ---------- file, You can obtain one at http://mozilla.org/MPL/2.0/. ----------
 ******************************************************************************/

#ifndef SL3_CONNECTION_HPP_
#define SL3_CONNECTION_HPP_

#include <sl3/database.hpp>

struct sqlite3;

namespace sl3
{
  /// \cond HIDDEN_SYMBOLS
  namespace internal
  {
    /**
     * \internal
     * \brief Holds the sqlite3 connection pointer.
     *
     * This is the link between Database and Command , and only used internal
     *
     */
    class Connection
    {
      friend class sl3::Database;

      Connection (sqlite3* p);

    public:
      ~Connection ();

      /// return db pointer
      sqlite3* db ();

      /// return if connection is open
      bool isValid ();

      ///  throw ErrNoConnection if not valid
      void ensureValid ();

    private:
      Connection (Connection&&) = default;

      Connection (const Connection&) = delete;
      Connection& operator= (const Connection&) = delete;
      Connection& operator= (Connection&&) = delete;

      void close (); // called by the db

      sqlite3* sl3db;
    };
  }
  ///\endcond

  namespace internal
  {
    inline Connection::Connection (sqlite3* p)
    : sl3db (p)
    {
    }

    inline Connection::~Connection () { close (); }
    inline sqlite3*
    Connection::db ()
    {
      return sl3db;
    }

    inline bool
    Connection::isValid ()
    {
      return sl3db != nullptr;
    }

    inline void
    Connection::ensureValid ()
    {
      if (sl3db == nullptr)
        {
          throw ErrNoConnection ();
        }
    }

    inline void
    Connection::close ()
    {
      if (sl3db == nullptr)
        return;

      // total clean up to be sure nothing left.
      auto stm = sqlite3_next_stmt (sl3db, 0);
      while (stm != nullptr)
        {
          sqlite3_finalize (stm);
          stm = sqlite3_next_stmt (sl3db, 0);
        }

      // if  busy, use v2 for garbed collecting,
      if (sqlite3_close (sl3db) != SQLITE_OK)
        {                           // but that should 'never' happen :-)
          sqlite3_close_v2 (sl3db); // LCOV_EXCL_LINE
        }

      sl3db = nullptr;
    }

  } // ns internal
}

#endif /* ...DATABASE_HPP_ */
