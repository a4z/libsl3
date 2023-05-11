/******************************************************************************
 ------------- Copyright (c) 2009-2023 H a r a l d  A c h i t z ---------------
 ---------- < h a r a l d dot a c h i t z at g m a i l dot c o m > ------------
 ---- This Source Code Form is subject to the terms of the Mozilla Public -----
 ---- License, v. 2.0. If a copy of the MPL was not distributed with this -----
 ---------- file, You can obtain one at http://mozilla.org/MPL/2.0/. ----------
 ******************************************************************************/

#include <sl3/database.hpp>

#include <sqlite3.h>

#include "connection.hpp"

namespace
{
  sqlite3*
  opendb (const std::string& name, int openFlags)
  {
    if (openFlags == 0)
      {
        openFlags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
      }

    sqlite3* db    = nullptr;
    auto     sl3rc = sqlite3_open_v2 (name.c_str (), &db, openFlags, nullptr);

    if (sl3rc != SQLITE_OK)
      {
        using scope_guard
            = std::unique_ptr<sqlite3, decltype (&sqlite3_close)>;
        scope_guard guard{db, &sqlite3_close};
        throw sl3::SQLite3Error{sl3rc, sqlite3_errmsg (db)};
      }
    return db;
  }

}

namespace sl3
{

  std::string
  getErrStr (int errcode)
  {
    return std::string (sqlite3_errstr (errcode));
  }

  Database::Database (const std::string& name, int flags)
  : _connection{new internal::Connection{opendb (name, flags)}}
  {
    sqlite3_extended_result_codes (_connection->db (), true);
  }

  Database::Database (Database&& other) noexcept
  : _connection (std::move (other._connection))
  {
    // always have a connection, but an invalid one
    other._connection.reset (new internal::Connection{nullptr});
  }

  Database::~Database () noexcept
  {
    if (_connection != nullptr)
      {
        _connection->close ();
      }
  }

  Command
  Database::prepare (const std::string& sql)
  {
    return {_connection, sql};
  }

  Command
  Database::prepare (const std::string& sql, const DbValues& parameters)
  {
    return {_connection, sql, parameters};
  }

  void
  Database::execute (const std::string& sql)
  {
    execute (sql.c_str ());
  }

  void
  Database::execute (const char* sql)
  {
    _connection->ensureValid ();
    char* dbMsg = nullptr;

    int rc = sqlite3_exec (_connection->db (), sql, nullptr, nullptr, &dbMsg);

    if (rc != SQLITE_OK)
      {
        using scope_guard = std::unique_ptr<char, decltype (&sqlite3_free)>;
        scope_guard guard (dbMsg, &sqlite3_free);
        throw SQLite3Error{rc, dbMsg};
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
    prepare (sql).execute (std::move (cb));
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

    Callback cb = [&retVal] (Columns cols) -> bool {
      retVal = cols.getValue (0);
      return false; // exit after first row
    };

    Command cmd (_connection, sql);
    cmd.execute (cb);

    return retVal;
  }

  DbValue
  Database::selectValue (const std::string& sql, Type type)
  {
    DbValue retVal{type};

    Callback cb = [&retVal, type] (Columns cols) -> bool {
      retVal = cols.getValue (0, type);
      return false; // exit after first row
    };

    Command cmd (_connection, sql);
    cmd.execute (cb);

    return retVal;
  }

  int
  Database::getMostRecentErrCode ()
  {
    _connection->ensureValid ();
    return sqlite3_extended_errcode (_connection->db ());
  }

  std::string
  Database::getMostRecentErrMsg ()
  {
    _connection->ensureValid ();
    return std::string (sqlite3_errmsg (_connection->db ()));
  }

  std::size_t
  Database::getTotalChanges ()
  {
    _connection->ensureValid ();
    return static_cast<std::size_t> (
        sqlite3_total_changes (_connection->db ()));
  }

  std::size_t
  Database::getRecentlyChanged ()
  {
    _connection->ensureValid ();
    return static_cast<std::size_t> (sqlite3_changes (_connection->db ()));
  }

  int64_t
  Database::getLastInsertRowid ()
  {
    _connection->ensureValid ();
    return sqlite3_last_insert_rowid (_connection->db ());
  }

  sqlite3*
  Database::db ()
  {
    return _connection->db ();
  }

  auto
  Database::beginTransaction () -> Transaction
  {
    return Transaction{*this};
  }

  Database::Transaction::Transaction (Database& db)
  : _db (&db)
  {
    _db->execute ("BEGIN TRANSACTION");
  }

  Database::Transaction::Transaction (Transaction&& other) noexcept
  : _db (other._db)
  {
    other._db = nullptr; // ensure other does not commit in d'tor
  }

  Database::Transaction::~Transaction ()
  {
    if (_db)
      _db->execute ("ROLLBACK TRANSACTION");
  }

  void
  Database::Transaction::commit ()
  {
    if (_db)
      {
        _db->execute ("COMMIT TRANSACTION");
        _db = nullptr;
      }
  }

} // ns
