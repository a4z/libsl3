/******************************************************************************
 ------------- Copyright (c) 2009-2017 H a r a l d  A c h i t z ---------------
 ---------- < h a r a l d dot a c h i t z at g m a i l dot c o m > ------------
 ---- This Source Code Form is subject to the terms of the Mozilla Public -----
 ---- License, v. 2.0. If a copy of the MPL was not distributed with this -----
 ---------- file, You can obtain one at http://mozilla.org/MPL/2.0/. ----------
 ******************************************************************************/

#include <sl3/command.hpp>

#include <functional>

#include <sqlite3.h>

#include "../sl3/connection.hpp"
#include <sl3/columns.hpp>
#include <sl3/database.hpp>
#include <sl3/error.hpp>

#include "utils.hpp"

namespace sl3
{
  namespace
  {
    sqlite3_stmt*
    createStmt (sqlite3* db, const std::string& sql)
    {
      if (db == nullptr)
        throw ErrNoConnection{};

      sqlite3_stmt* stmt       = nullptr;
      const char*   unussedSQL = nullptr;

      int rc = sqlite3_prepare_v2 (db, sql.c_str (), -1, &stmt, &unussedSQL);

      if (rc != SQLITE_OK)
        {
          SQLite3Error sl3error (rc, sqlite3_errmsg (db));
          throw sl3error;
        }

      return stmt;
    }

    DbValues
    createParameters (sqlite3_stmt* stmt)
    {
      const size_t paracount = as_size_t (sqlite3_bind_parameter_count (stmt));

      using container = DbValues::container_type;

      return paracount > 0 ? DbValues (container (paracount, Type::Variant))
                           : DbValues ();
    }

    void
    bind (sqlite3_stmt* stmt, DbValues& parameters)
    {
      int curParaNr = 0;
      for (auto& val : parameters)
        {
          curParaNr += 1; // sqlite starts at 1

          int rc;

          switch (val.type ())
            {
            case Type::Int:
              rc = sqlite3_bind_int64 (stmt, curParaNr, val.getInt ());
              break;

            case Type::Real:
              rc = sqlite3_bind_double (stmt, curParaNr, val.getReal ());
              break;

            case Type::Text:
              // note, i do not want \0 in the db so take size
              // SQLITE_TRANSIENT would copy the string , is unwanted here
              rc = sqlite3_bind_text (
                  stmt,
                  curParaNr,
                  val.getText ().c_str (),
                  static_cast<int> (val.getText ().size ()),
                  SQLITE_STATIC);

              break;

            case Type::Blob:
              rc = sqlite3_bind_blob (
                  stmt,
                  curParaNr,
                  &(val.getBlob ()[0]),
                  static_cast<int> (val.getBlob ().size ()),
                  SQLITE_STATIC);

              break;

            case Type::Null:
              rc = sqlite3_bind_null (stmt, curParaNr);
              break;

            default:
              throw ErrUnexpected (); // LCOV_EXCL_LINE
            }

          if (rc != SQLITE_OK)
            throw sl3::SQLite3Error (rc, ""); // LCOV_EXCL_LINE TODO ho to test
        }
    }

  } // ns

  Command::Command (Connection connection, const std::string& sql)
  : _connection (std::move (connection))
  , _stmt (createStmt (_connection->db (), sql))
  , _parameters (createParameters (_stmt))
  {
  }

  Command::Command (Connection         connection,
                    const std::string& sql,
                    DbValues           parameters)
  : _connection (std::move (connection))
  , _stmt (createStmt (_connection->db (), sql))
  , _parameters (std::move (parameters))
  {
    const size_t paracount = as_size_t (sqlite3_bind_parameter_count (_stmt));

    if (paracount != _parameters.size ())
      {
        throw ErrTypeMisMatch ("Incorrect parameter count");
      }
  }

  Command::Command (Command&& other)
  : _connection (std::move (other._connection))
  , _stmt (other._stmt)
  , _parameters (std::move (other._parameters))
  { // clear stm so that d'tor ot other does no action
    other._stmt = nullptr;
  }

  Command::~Command ()
  {
    if (_stmt) // its not a moved from zombi
      {
        if (_connection->isValid ()) // otherwise database will have done this
          {
            sqlite3_finalize (_stmt);
          }
      }
  }

  Dataset
  Command::select ()
  {
    return select (DbValues (), Types ());
  }

  Dataset
  Command::select (const Types& types, const DbValues& parameters)
  {
    return select (parameters, types);
  }

  Dataset
  Command::select (const DbValues& parameters, const Types& types)
  {
    Dataset  ds{std::move (types)};
    Callback fillds = [&ds] (Columns columns) -> bool {
      if (ds._names.size () == 0)
        {
          const int typeCount = static_cast<int> (ds._fieldtypes.size ());

          if (typeCount == 0)
            {
              using container_type = Types::container_type;
              container_type c (as_size_t (columns.count ()), Type::Variant);
              Types          fieldtypes{c};
              ds._fieldtypes.swap (fieldtypes);
            }
          else if (typeCount != columns.count ())
            {
              throw ErrTypeMisMatch (
                  "DbValuesTypeList.size != queryrow.getColumnCount()");
            }
          ds._names = columns.getNames ();
        }

      // this will throw if a type does not match.
      ds._cont.emplace_back (columns.getRow (ds._fieldtypes));

      return true;
    };

    execute (fillds, parameters);
    return ds;
  }

  void
  Command::execute ()
  {
    execute (DbValues ());
  }

  void
  Command::execute (const DbValues& parameters)
  {
    execute ([] (Columns) -> bool { return true; }, parameters);
  }

  void
  Command::execute (RowCallback& cb, const DbValues& parameters)
  {
    cb.onStart ();

    auto cbf = [&cb] (Columns cols) -> bool { return cb.onRow (cols); };

    execute (cbf, parameters);

    cb.onEnd ();
  }

  void
  Command::execute (Callback callback, const DbValues& parameters)
  {
    _connection->ensureValid ();

    if (parameters.size () > 0)
      setParameters (parameters);

    bind (_stmt, _parameters);

    // use this to ensure a reset of _stmt
    using ResetGuard
        = std::unique_ptr<sqlite3_stmt, decltype (&sqlite3_reset)>;
    ResetGuard resetGuard (_stmt, &sqlite3_reset);

    bool loop = true;
    while (loop)
      {
        int rc = sqlite3_step (_stmt);

        switch (rc)
          {
          case SQLITE_OK:
          case SQLITE_DONE:
            {
              loop = false;
              break;
            }
          case SQLITE_ROW:
            {
              loop = callback (Columns{_stmt});
              break;
            }

          default:
            {
              auto         db = sqlite3_db_handle (_stmt);
              SQLite3Error sl3error (rc, sqlite3_errmsg (db));
              throw sl3error;
            }
          }
      }
  }

  DbValues&
  Command::getParameters ()
  {
    return _parameters;
  }

  const DbValues&
  Command::getParameters () const
  {
    return _parameters;
  }

  DbValue&
  Command::getParameter (int idx)
  {
    return _parameters.at (as_size_t (idx));
  }

  const DbValue&
  Command::getParameter (int idx) const
  {
    return _parameters.at (as_size_t (idx));
  }

  void
  Command::setParameters (const DbValues& values)
  {
    if (values.size () != _parameters.size ())
      {
        throw ErrTypeMisMatch ("parameter size incorrect");
      }

    for (size_t i = 0; i < values.size (); ++i)
      {
        _parameters[i] = values[i];
      }
  }

  void
  Command::resetParameters (DbValues values)
  {
    ASSERT_EXCEPT (values.size () == _parameters.size (), ErrTypeMisMatch);
    // auto tmp = values;
    _parameters.swap (values);
  }

  std::vector<std::string>
  Command::getParameterNames () const
  {
    std::vector<std::string> names;
    names.resize (_parameters.size ());

    for (unsigned int i = 0; i < _parameters.size (); ++i)
      {
        const char* chrname
            = sqlite3_bind_parameter_name (_stmt, as_int (i + 1));
        names[i] = chrname ? chrname : "";
      }
    return names;
  }

} // ns
