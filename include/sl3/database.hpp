/******************************************************************************
 ------------- Copyright (c) 2009-2017 H a r a l d  A c h i t z ---------------
 ---------- < h a r a l d dot a c h i t z at g m a i l dot c o m > ------------
 ---- This Source Code Form is subject to the terms of the Mozilla Public -----
 ---- License, v. 2.0. If a copy of the MPL was not distributed with this -----
 ---------- file, You can obtain one at http://mozilla.org/MPL/2.0/. ----------
 ******************************************************************************/

#ifndef SL3_DATABASE_HPP_
#define SL3_DATABASE_HPP_

#include <memory>
#include <string>

#include <sl3/command.hpp>
#include <sl3/config.hpp>
#include <sl3/dataset.hpp>
#include <sl3/dbvalue.hpp>

struct sqlite3;

namespace sl3
{
  namespace internal
  {
    class Connection;
  }

  /**
   * \brief represents a SQLite3 Database
   *
   *
   * Encapsulate some/the most - useful methods for a SQLite3 database object.
   *
   * A Database is opened when a instance is created and close when the
   * instance
   * goes out of scope.
   *
   * \note for valgrind: http://www.sqlite.org/cvstrac/tktview?tn=3428 \n
   *  since I don't know where to put this note else I place it just here
   */
  class LIBSL3_API Database
  {
    Database (const Database&) = delete;
    Database& operator= (const Database&) = delete;
    Database& operator= (Database&&) = delete;

  public:
    /**
     * \brief Constructor
     *
     * Construction of this object opens, or creates a sqlite3 database.
     * The exact behavior can be fine tuned using the openFlags parameter.
     *
     * \param name database name.
     * \n If name is ":memory:" than the database will be an in memory
     * database.
     * \n If name has a size of 0 than the database will be private on disk
     * database.
     * \n Otherwise name will be interpreted as a file.
     *
     * \param openFlags if no flags are given, the defaults are
     * SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE.
     *
     * \sa https://www.sqlite.org/c3ref/open.html
     *
     * \throw sl3::SQLite3Error if the database can not be opened
     */
    Database (const std::string& name, int openFlags = 0);

    /**
     * \brief Destructor.
     */
    virtual ~Database () noexcept;

    /**
     * \brief Move constructor
     *
     * A Database is moveable
     */
    Database (Database&&) noexcept;

    /**
     * \brief create a SqlCommand instance.
     *
     * Parameters that the statement might contain will be automatically
     * deduced and created as DbVaraint values
     *
     * \param sql SQL statement
     * \return a Command instance
     */
    Command prepare (const std::string& sql);

    /**
     * \brief create a Command.
     *
     *  Given parameters will be used to set up the command parameters,
     *
     * \param sql     SQL statement
     * \param params  parameters
     *
     * \throw sl3::ErrTypeMisMatch if params count is wrong
     * \return a Command instance
     */
    Command prepare (const std::string& sql, const DbValues& params);

    /**
     * \brief Execute one or more SQL statements.
     *
     * \throw sl3::SQLite3Error in case of a problem.
     *
     * \param sql SQL Statements
     */
    void execute (const std::string& sql);

    /**
     * \brief Execute one or more SQL statements.
     *
     * Overload for const char to avoid a copy for larg command like
     * create database scripts which are often static  char*
     *
     * \throw sl3::SQLite3Error in case of a problem.
     *
     * \param sql SQL Statements
     */
    void execute (const char* sql);

    /**
     * \brief Execute one SQL statements and apply a SqlQueryHandler.
     *
     * \throw sl3::SQLite3Error in case of a problem.
     *
     * \param sql SQL Statements
     * \param cb RowCallback to handle query result
     */
    void execute (const std::string& sql, RowCallback& cb);

    /// shortcut for Command::Callback
    using Callback = Command::Callback;

    /**
     * \brief Execute one SQL statements and apply a QueryCallback.
     *
     * \throw sl3::SQLite3Error in case of a problem.
     *
     * \param sql SQL Statements
     * \param cb Callback to handle query result
     */
    void execute (const std::string& sql, Callback cb);

    /**
     * \brief Execute a SQL query and return the result.
     *
     * \throw sl3::SQLite3Error in case of a problem.
     *
     * \param sql SQL Statements
     * \return a Dataset with the result.
     */
    Dataset select (const std::string& sql);

    /**
     * \brief Execute a SQL query and returns the result .
     *
     *  If Types are
     *
     * \throw sl3::SQLite3Error in case of a problems.
     * \throw sl3::ErrTypeMisMatch in case of incorrect types.
     *
     * \param sql SQL Statements
     * \param types wanted types of the returned Dataset
     * \return a Dataset with the result.
     */
    Dataset select (const std::string& sql, const Types& types);

    /**
     * \brief Select a single value form the database.
     *
     * This is a quick way to get single value results for queries like
     *  "SELECT COUNT(*) FROM someTable;" \n
     *
     * This function returns the first column of the first result row of the
     *  given query.
     * If query returns no rows, the return value will be null.
     *
     * \throw sl3::SQLite3Error in case of a problems.
     *
     * \param sql SQL statement
     * \return DbValue of the first available result.
     */
    DbValue selectValue (const std::string& sql);

    /**
     * \brief select a single typed value form the database.
     *
     * This is a quick way to get single value results for queries like
     *  "SELECT COUNT(*) FROM someTable;" \n
     *
     * This function returns the first column of the first result row of the
     *  given query.
     * If query returns no rows, the return value will be null.
     *
     * \throw sl3::ErrTypeMisMatch if return type differs from requested type
     * \throw sl3::SQLite3Error in case of a problems.
     * \param sql SQL statement
     * \param type resutl has to be of that Type
     *
     * \return DbValue of the first available result.
     */
    DbValue selectValue (const std::string& sql, Type type);

    /**
     * \brief Returns last SQLite3 extended result code
     *
     *  \note SQLite3 result code in case of a failure is triggered within a
     *   SQLite3Error.
     *
     *  The return value is only valid if the last call failed.
     *
     * \note This function returns extended result codes
     *
     * \return SQLite3 extended error code
     */
    int getMostRecentErrCode ();

    /**
     * \brief Returns most recent error message.
     *
     *
     * \return SQLite3 error message.
     */
    std::string getMostRecentErrMsg ();

    /**
      * \brief Wraps sqlite3_errstr() function
      *
      * The sqlite3_errstr() interface returns the English-language text
      * that describes the result code, as UTF
      * \param errcode err number
      * \return Count of changed rows
      */
    std::string getErrStr (int errcode);

    /**
     * \brief Returns number of row that have changed since database opening.
     *
     * Returns the number of rows that have been changed
     * Through successful SQL INSERT/UPDATE or DELETE statements since database
     *  was opened.
     *
     * \return Count of changed rows
     */
    std::size_t getTotalChanges ();

    /**
     * \brief Rows that have been changed from the most recent SQL statement.
     *
     * Returns the number of rows that have been changed from the most recent
     * successful SQL INSERT/UPDATE or DELETE statement.
     *
     * \return Count of changed rows
     */
    std::size_t getRecentlyChanged ();

    /**
     * \brief returns rowid  of the most recent successful INSERT statement
     *
     * Returns the rowid (ROWID, OID, or _ROWID_ or the column of
     * type INTEGER PRIMARY KEY )
     * of the most recent successful INSERT into the database.
     * \n If no successful INSERTs have ever occurred on that database,
     * zero is returned.
     *
     * \return rowid
     */
    int64_t getLastInsertRowid ();

    /**
     * \brief Transaction Guard
     *
     * Scope guard for transaction.
     * If an instance of this class goes out of scope and commit has
     * not been called, it will call Rollback.
     */
    class Transaction
    {
      sl3::Database& _db;
      bool           _commit;

      Transaction (Database&);
      friend class Database;

      Transaction (const Transaction&) = delete;
      Transaction& operator= (const Transaction&) = delete;
      Transaction& operator= (Transaction&&) = delete;

    public:
      /** \brief Move constructor
       *  A Transaction is movalble.
       */
      Transaction (Transaction&&);

      /** \brief Destructor
       *
       * Calls ROLLBACK if the Transaction commit has not been called.
       */
      ~Transaction ();

      /** \brief Commit the transaction
       *
       * Calls commit transaction.
       */
      void commit ();
    };

    /**
     * \brief Create a TransactionGuard
     * \return Transaction instance
     */
    Transaction beginTransaction ();

  protected:
    /**
     * \brief Access the underlying sqlite3 database.
     *
     * Derived classes may, if needed, access the pointer to
     * the sqlite3 database.
     *
     * \note: Do not change the state of database (call close on it).
     *
     * \return a handle to the underlying sqlite3 database;
     */
    sqlite3* db ();

  private:
    /**
     * \brief Define internal::Connection type.
     *
     * Only internal used
     */
    using ConnectionPtr = std::shared_ptr<internal::Connection>;

    /**
     * \brief Shared pointer for internal::Connection.
     *
     * Only internal used. A connection is shared with Command instances.
     *
     * If a database closes, the connection closes.
     * A Command knows about that and will throw an exception if used
     * after closing the database.
     */
    ConnectionPtr _connection;
  };
}

#endif /* ...DATABASE_HPP_ */
