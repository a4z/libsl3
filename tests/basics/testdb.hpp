/*
 * testdb.hpp
 *
 *  Created on: Oct 27, 2014
 *      Author: slk141
 */

#ifndef TESTDB_HPP_
#define TESTDB_HPP_

#include <map>

#include <sl3/database.hpp>

namespace sl3 {


// use this as a sample with the old code


/**
 * \brief Testing class
 *
 * some notes
 *
 * \test sl3::Database
 */
class TestDb : public sl3::Database
{
public:

  TestDb(const std::string& name = ":memory:");


  /**
   * \brief Create a named SqlCommand and keep it
   *
   * The created command will be stored with the given name on the
   * actual instance.
   * \sa getCommand
   *
   * \param name the name
   * \param sql sql statement
   * \parame DbValues, wanted types for a parameter command
   * \return reference to SqlCommand object if created successful
   *
   * \throw SQLite3Error if sql is invalid
   * \throw ErrUnexpected if the name is already in use
   *
   */
  Command& createStoredCommand(const std::string& name,
                                  const std::string& sql,
                                  const DbValues&  = {});

  /**
   * \brief Request a StoredCommand
   *
   *
   * \param name StoredCommand name
   * \return Pointer to StoredCommand object
   *        \n 0 if no StoredCommand with passed name exists.
   */
  Command& getCommand(const std::string& name);


  /**
   * \brief delete a StoredCommand
   * \param name name of the query
   * \return true if the query was removed
   * \n false if there is no query with the submitted name
   */
  bool eraseCommand(const std::string& name) ;

  /**
   * \brief removes all StoredQueries
   *
   * Destroys all StoredCommand objects the Database holds.
   */
  void eraseCommands( ) ;


private:
  std::map<std::string, Command> _commands ;


};

}

#endif /* TESTDB_HPP_ */
