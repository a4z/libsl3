/******************************************************************************
 ------------- Copyright (c) 2009-2023 H a r a l d  A c h i t z ---------------
 ---------- < h a r a l d dot a c h i t z at g m a i l dot c o m > ------------
 ---- This Source Code Form is subject to the terms of the Mozilla Public -----
 ---- License, v. 2.0. If a copy of the MPL was not distributed with this -----
 ---------- file, You can obtain one at http://mozilla.org/MPL/2.0/. ----------
 ******************************************************************************/

#ifndef SL3_SQLCOMMAND_HPP
#define SL3_SQLCOMMAND_HPP

#include <memory>
#include <string>

#include <sl3/config.hpp>
#include <sl3/dataset.hpp>
#include <sl3/dbvalue.hpp>
#include <sl3/rowcallback.hpp>

struct sqlite3;
struct sqlite3_stmt;

namespace sl3
{

  namespace internal
  {
    class Connection;
  }

  /**
   * \brief A compiled SQL command
   *
   *  This class holds a compiled SQL statement so that it can be reused.
   *
   *  A command can have parameters.
   *
   */
  class LIBSL3_API Command
  {
    friend class Database;
    using Connection = std::shared_ptr<internal::Connection>;

    Command (Connection connection, const std::string& sql);

    Command (Connection         connection,
             const std::string& sql,
             DbValues           parameters);

    Command ()                         = delete;
    Command (const Command&)           = delete;
    Command operator= (const Command&) = delete;
    Command operator= (Command&&)      = delete;

  public:
    /**
     * \brief Move constructor.
     *
     * A command is movable
     *
     */
    Command (Command&&);

    /**
     * \brief Destructor.
     *
     *  The destructor calls release.
     */
    ~Command (); // cause of unique pointer, check if protected is possible

    /**
     * \brief Run the Command and get the result
     *
     * Runs the command, applying possible already given parameters
     *  and returns the result in a Dataset.
     *
     *
     * \return A Dataset containing the query result
     */
    Dataset select ();

    /**
     * \brief Run the Command and get the result
     *
     * Runs the command, applying given parameters
     *  and returns the result in a Dataset.
     * If types are given, they are used for the returned Dataset.
     * If no
     * types are given all fields in the returned Dataset will be
     * of
     * Type::Variant
     *
     * \throw sl3::ErrTypeMisMatch if types are given which are invalid or
     * given parameters are of the wrong size.
     * \param parameters a list of parameters
     * \param types Types the Dataset shall use
     * \return A Dataset containing the query result
     */
    Dataset select (const DbValues& parameters, const Types& types = {});

    /**
     * \brief Run the Command and get the result
     *
     * Runs the command, applying possible given parameters
     * and returns the result in a Dataset in which given types are
     * used for the fields.
     *
     * \throw sl3::ErrTypeMisMatch if types are given which are invalid or
     * given parameters are of the wrong size.
     * \param parameters a list of parameters
     * \param types Types the Dataset shall use
     * \return A Dataset containing the query result
     */
    Dataset select (const Types& types, const DbValues& parameters = {});

    /**
     * \brief function object for handling a command result.
     *
     * Functor called for each  processed row of  Command or a
     * sql statement.
     *
     * \return false if processing the query result shall stop
     *       \n true otherwise
     */
    using Callback = std::function<bool (Columns)>;

    /**
     * \brief Execute the command
     *
     * Runs the current command.
     * If parameters are set,
     *
     */
    void execute ();

    /**
     * \brief Execute the command
     *
     * Applies given parameters and run the current command.
     *
     * \throw sl3::ErrTypeMisMatch given parameters are of the wrong size.
     * \param parameters a list of parameters
     */
    void execute (const DbValues& parameters);

    /**
     * \brief Execute the command applying given callback
     *
     * Applies given parameters and run the current command.
     *
     * \throw sl3::ErrTypeMisMatch given parameters are of the wrong size.
     * \param cb a callback
     * \param parameters a list of parameters
     */
    void execute (RowCallback& cb, const DbValues& parameters = {});

    /**
     * \brief Execute the command applying given callback
     *
     *  Applies given parameters and run the current command.
     *
     * \throw sl3::ErrTypeMisMatch given parameters are of the wrong size.
     * \param cb a callback
     * \param parameters a list of parameters
     */
    void execute (Callback cb, const DbValues& parameters = {});

    /**
     * \brief Parameters of command.
     *
     * \return reference to the parameters
     */
    DbValues& getParameters ();

    /**
     * \brief Parameters of command.
     *
     * \return const reference to the parameters
     */
    const DbValues& getParameters () const;

    /**
     * \brief get Parameter at given index.
     * \param idx index
     * \throw sl3::ErrOutOfRange if index is invalid
     * \return reference to the parameters at given index
     */
    DbValue& getParameter (int idx);

    /**
     * \brief get Parameter at given index.
     * \param idx index
     * \throw sl3::ErrOutOfRange if index is invalid
     * \return const reference to the parameters at given index
     */
    const DbValue& getParameter (int idx) const;

    /**
     * \brief Set parameter values
     *
     * The types of the given values must be compatible with the
     * current
     * ones.
     *
     * \param values new parameter values
     * \throw sl3::ErrTypeMisMatch if
     * size of values differs from the actual parameter size or DbValue
     * assignment throws.
     * \see DbValue
     *
     */
    void setParameters (const DbValues& values);

    /**
     * \brief Set new parameter values
     *
     * In contrast to setParameters, where the DbValue types must match,
     * this can be used to set other Types.
     *
     * \param values new parameter values
     * \throw sl3::ErrTypeMisMatch if size of values differs from the actual
     * one
     */
    void resetParameters (DbValues values);

    /**
     * \brief get a list of the parameter names
     *
     * If the command has no parameters, the list will be empty.
     * Parameters with no name will be an empty entry in the list.
     *
     * \return list of names
     */
    std::vector<std::string> getParameterNames () const;

  private:
    Connection    _connection;
    sqlite3_stmt* _stmt;
    DbValues      _parameters;
  };

  /**
   * \brief Syntax sugar to create command parameters
   *
   *  Creates DbValues with types based on the given arguments.
   * \code
   *  cmd.execute(parameters(1,"foo",3.3))
   * \endcode
   *  Will create DbValues of Type::Int, Type::Text and Type::Real
   *
   *  \tparam VALS variadic argument types
   *  \param vals variadic argument values
   *
   *  \return DbValues constructed by given arguments
   */
  template <typename... VALS>
  DbValues
  parameters (VALS&&... vals)
  {
    return {DbValue{vals}...};
  }
}

#endif
