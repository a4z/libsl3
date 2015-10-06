/*
 * testdb.cpp
 *
 *  Created on: Oct 27, 2014
 *      Author: slk141
 */

#include "testdb.hpp"

namespace sl3 {

TestDb::TestDb(const std::string& name)
:Database(name)
{

}


Command&
TestDb::createStoredCommand(const std::string& name,
                              const std::string& sql,
                              const DbValues& parameters)
{
  auto result = parameters.size () < 1 ?
        _commands.emplace( name, prepare(sql)) :
        _commands.emplace( name, prepare(sql, parameters ));

  if(!result.second )
    throw ErrUnexpected("Name already in use");


  return result.first->second;
}




bool
TestDb::eraseCommand( const std::string& name )
{
  return _commands.erase(name) > 0 ;
}



void
TestDb::eraseCommands()
{
  _commands.clear();
}


Command&
TestDb::getCommand(const std::string& name)
{

  auto finder = _commands.find( name );

  if ( finder == _commands.end() )
    throw ErrUnexpected("stored command '" + name + "' not found");


  return finder->second;

}



}
