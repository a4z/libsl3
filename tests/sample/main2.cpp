#include <cassert>
#include <iostream>

#include <sl3/database.hpp>

int main()
{
  using namespace sl3;
  // define a db
  Database db(":memory:");
  // run commands against the db
  db.execute("CREATE TABLE tbl(f1 INTEGER, f2 TEXT, f3 REAL);");
  // create a command with parameters
  auto cmd = db.prepare("INSERT INTO tbl (f1, f2, f3) VALUES (?,?,?);",
                        DbValues({Type::Int,Type::Text, Type::Real})) ;

  //this will work,
  cmd.execute (parameters (1, "one", 1.1));

  // this will throw since "2" is a wrong type
  try
  {
      cmd.execute (parameters ("2", "two", 2.2));
  }
  catch (const Error& e)
  {
      std::cout << e << std::endl;
  }


}
