/******************************************************************************
 ------------- Copyright (c) 2009-2023 H a r a l d  A c h i t z ---------------
 ---------- < h a r a l d dot a c h i t z at g m a i l dot c o m > ------------
 ---- This Source Code Form is subject to the terms of the Mozilla Public -----
 ---- License, v. 2.0. If a copy of the MPL was not distributed with this -----
 ---------- file, You can obtain one at http://mozilla.org/MPL/2.0/. ----------
 ******************************************************************************/

#ifndef SL3_COLUMNS_HPP
#define SL3_COLUMNS_HPP

#include <sl3/config.hpp>
#include <sl3/dbvalues.hpp>

struct sqlite3_stmt;

namespace sl3
{

  // TODO add to docu
  // index access always checked, since docu says
  // 'if the column index is out of range, the result is undefined'
  // but if you fell lke preoptimization is requiresd,
  // feel free to access  the underlying sqlite3_stmt via
  // and adopt the index access!

  /**
   * \brief Class to access data of query results.
   *
   * A Columns instance is constructed by a Command and passed to the
   * callback which handles the results of a query.
   *
   * \see RowCallback
   * \see Command::Callback
   * \see Database::Callback
   */
  class LIBSL3_API Columns
  {
    friend class Command;

    Columns (sqlite3_stmt* stmt);

    // not be needed, even if they would not harm ..
    Columns& operator= (const Columns&) = delete;

    // not be needed, even if they would not harm ..
    Columns& operator= (Columns&&) = delete;

    Columns (const Columns&) = default;

  public:
    /**
     * \brief Move constructor
     * A column is movable
     */
    Columns (Columns&&) = default;

    /**
     * \brief Destructor
     *
     */
    ~Columns () = default;

    /**
     * \brief Number of columns in the statement.
     *
     * \return number of columns
     */
    int count () const;

    /**
     * \brief Column name of given index.
     *
     *
     * \throw sl3::ErrOutOfRange if idx is invalid,
     * \param idx wanted column index
     * \return column name or an empty string if index is invalid
     */
    std::string getName (int idx) const;

    /**
     * \brief Get columns names
     *
     *  Will return a list of size count() .
     *  Unnamed columns will be an ampty string
     *
     * \return the names
     */
    std::vector<std::string> getNames () const;

    /**
     * \brief Get the value at a given index
     *
     *  The returned DbValue::getType() will be a Type::Variant and
     *  DbValue::getStorageType() will be set to the sqlite3 storage type.
     *
     * \param idx wanted index
     * \throw sl3::ErrOutOfRange if idx is invalid,
     * \return DbValue of requested column
     */
    DbValue getValue (int idx) const;

    /**
     * \brief Get the value at a given index
     *
     *  The returned DbValue::getType() and DbValue::getStorageType()
     *  will be set to the given type.
     *
     * \param idx wanted index
     * \param type wanted type
     * \throw sl3::ErrTypeMisMatch if type is not the sqlite type
     * \throw sl3::ErrOutOfRange if idx is invalid,
     * \return DbValue of requested column
     */
    DbValue getValue (int idx, Type type) const;

    /**
     * \brief Get all columns at once
     *
     * All DbValue object will be of Type::Variant
     *
     * \return DbValues of the column values
     */
    DbValues getRow () const;

    /**
     * \brief Get all columns at once using the given types
     *
     * If a value does not math the given types an exception will be thown
     *
     * \param types wanted Types
     * \throw sl3::ErrTypeMisMatch in case of an incorrect type
     * \return DbValues of the column values
     */
    DbValues getRow (const Types& types) const;

    /**
     * \brief Get the sqlite type for a column
     *
     * If used, should be called before accessing the value of the column
     * at the given index, otherwise the typed access might set the type.
     *
     * This method can be used to check if a column isNull.
     *
     * \param idx wanted index
     * \throw sl3::ErrOutOfRange if idx is invalid
     * \return Type sqlite interprets the value
     */
    Type getType (int idx) const;

    /**
     * \brief Get the size of a column
     *
     * The size sqlite3 uses to store the value of the given field.
     *
     * \param idx wanted index
     * \throw sl3::ErrOutOfRange if idx is invalid
     * \return size sqlite uses for the column
     */
    size_t getSize (int idx) const;

    /**
     *  \brief Get the value of a column.
     *
     *  If a column is Null of of a different type, the sqlite3 conversion
     *  rules are applied.
     *
     *  \param idx column index
     *  \throw sl3::ErrOutOfRange if idx is invalid
     *  \return column value
     */
    std::string getText (int idx) const;

    /**
     *  \brief Get the value of a column.
     *
     *  If a column is Null of of a different type, the sqlite3 conversion
     *  rules are applied.
     *
     *  \param idx column index
     *  \throw sl3::ErrOutOfRange if idx is invalid
     *  \return column value
     */
    int getInt (int idx) const;

    /**
     *  \brief Get the value of a column.
     *
     *  If a column is Null of of a different type, the sqlite3 conversion
     *  rules are applied.
     *  \param idx column index
     *  \throw sl3::ErrOutOfRange if idx is invalid
     *  \return column value
     */
    int64_t getInt64 (int idx) const;

    /**
     *  \brief Get the value of a column.
     *
     *  If a column is Null of of a different type, the sqlite3 conversion
     *  rules are applied.
     *
     *  \param idx column index
     *  \throw sl3::ErrOutOfRange if idx is invalid
     *  \return column value
     */
    double getReal (int idx) const;

    /**
     *  \brief Get the value of a column.
     *
     *  If a column is Null of of a different type, the sqlite3 conversion
     *  rules are applied.
     *
     *  \param idx column index
     *  \throw sl3::ErrOutOfRange if idx is invalid
     *  \return column value
     */
    Blob getBlob (int idx) const;

    /**
     * \brief Get the underlying sqlite3_stmt
     *
     * User defined QueryCallbacks might have their own way to do things,
     * so this getter provides access to the underlying sqlite3_stmt.
     *
     * \return underlying sqlite3_stmt
     */
    sqlite3_stmt*
    get_stmt () const
    {
      return _stmt;
    }

  private:
    sqlite3_stmt* _stmt;
  };
}

#endif
