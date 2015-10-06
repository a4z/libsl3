/******************************************************************************
 -------------- Copyright (c) 2009-2015 H a r a l d  A c h i t z --------------
 ---------- < h a r a l d dot a c h i t z at g m a i l dot c o m > ------------
 ---- This Source Code Form is subject to the terms of the Mozilla Public -----
 ---- License, v. 2.0. If a copy of the MPL was not distributed with this -----
 ---------- file, You can obtain one at http://mozilla.org/MPL/2.0/. ----------
 ******************************************************************************/

#include <sl3/database.hpp>

#include <sqlite3.h>

#include <sl3/columns.hpp>
#include <sl3/error.hpp>
#include "../sl3/connection.hpp"

namespace sl3
{


  Database::Database (const std::string& name, int openFlags) :
      _connection (nullptr)
  {

    if (openFlags == 0)
      {
        openFlags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
      }

    sqlite3* db = nullptr;
    auto sl3rc = sqlite3_open_v2 (name.c_str (), &db, openFlags, 0);

    if (sl3rc != SQLITE_OK)
      {
        SQLite3Error sl3error (sl3rc, sqlite3_errmsg (db));
        sqlite3_close (db);
        throw sl3error;
      }

    _connection.reset (new internal::Connection (db));

    sqlite3_extended_result_codes (_connection->db (), true);

  }

  Database::Database (Database&& other) noexcept
  : _connection(std::move(other._connection))
  {
    other._connection.reset();
  }


  Database::~Database () noexcept
  {
    if (_connection != nullptr)
      {
        _connection->close();
      }
  }



  Command
  Database::prepare (const std::string& sql)
  {
    return { _connection, sql};
  }


  Command
  Database::prepare (const std::string& sql, const DbValues& parameters)
  {
    return { _connection, sql, parameters};
  }


  void
  Database::execute (const std::string& sql)
  {
    execute (sql.c_str ());
  }


  void
  Database::execute (const char* sql)
  {
    char* dbMsg = 0;

    int rc = sqlite3_exec (_connection->db (), sql, 0, 0, &dbMsg);

    if (rc != SQLITE_OK)
      {
        SQLite3Error sl3error (rc, dbMsg);

        sqlite3_free (dbMsg);

        throw sl3error;
      }

  }


  void
  Database::execute (const std::string& sql, RowCallback& cb)
  {
    prepare (sql).execute (cb);
  }


  void
  Database::execute (const std::string& sql, Callback cb)
  {
    prepare (sql).execute (cb);
  }


  Dataset
  Database::select (const std::string& sql)
  {
    return Command (_connection, sql).select ();
  }


  Dataset
  Database::select (const std::string& sql, const Types& types)
  {

    return Command (_connection, sql).select (types);

  }


  DbValue
  Database::selectValue (const std::string& sql)
  {

    DbValue retVal (Type::Variant);

    auto cb = [&retVal](Columns cols) -> bool
      {
        retVal = cols.at (0);
        return false; //exit after first row
      };

    Command cmd (_connection, sql);
    cmd.execute (cb);

    return retVal;
  }


  DbValue
  Database::selectValue (const std::string& sql, Type type)
  {

    DbValue retVal (type);

    auto cb = [&retVal](Columns cols) -> bool
      {
        retVal = cols.at (0);
        return false; //exit after first row
      };

    Command cmd (_connection, sql);
    cmd.execute (cb);

    return retVal;
  }


  int
  Database::getMostRecentErrCode ()
  {
    return sqlite3_extended_errcode (_connection->db ());
  }


  std::string
  Database::getMostRecentErrMsg ()
  {
    return std::string (sqlite3_errmsg (_connection->db ()));
  }


  std::string
  Database::getErrStr (int errcode)
  {
    return std::string (sqlite3_errstr (errcode));
  }


  std::size_t
  Database::getTotalChanges ()
  {
    return sqlite3_total_changes (_connection->db ());
  }


  std::size_t
  Database::getRecentlyChanged ()
  {
    return sqlite3_changes (_connection->db ());
  }


  int64_t
  Database::getLastInsertRowid ()
  {
    return sqlite3_last_insert_rowid (_connection->db ());
  }


  sqlite3*
  Database::db ()
  {
    return _connection->db () ;
  }


  auto
  Database::beginTransaction ()
  -> Transaction
  {
    return Transaction { *this };
  }





  Database::Transaction::Transaction (Database& db) :
      _db (db),
      _commit (false)
  {
    _db.execute ("BEGIN TRANSACTION");
  }


  Database::Transaction::Transaction (Transaction&& other) :
      _db (other._db),
      _commit (other._commit)
  {
    other._commit = true; // ensure other does not commit in d'tor
  }


  Database::Transaction::~Transaction ()
  {
    if (!_commit)
      _db.execute ("ROLLBACK TRANSACTION");
  }


  void
  Database::Transaction::commit ()
  {
    _db.execute ("COMMIT TRANSACTION");
    _commit = true;
  }



}// ns
