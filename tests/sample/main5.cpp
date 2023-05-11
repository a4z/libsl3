#include <iostream>
#include <sl3/database.hpp>

int
main ()
{
  using namespace sl3;
  // define a db
  Database db (":memory:");
  // run commands against the db
  db.execute ("CREATE TABLE tbl(f1 INTEGER, f2 TEXT, f3 REAL);");
  // create a command with parameters
  auto cmd = db.prepare ("INSERT INTO tbl (f1, f2, f3) VALUES (?,?,?);");
  // no types so we use variants
  // insert Int, Text, Real
  cmd.execute (parameters (1, "one", 1.1));
  // insert Text, Text, Real
  cmd.execute (parameters ("some text", "two", 2.1));

  // this will throw since types in column0  are different
  try
    {
      Dataset ds = db.select ("SELECT * FROM tbl;",
                              {Type::Int, Type::Text, Type::Real});
    }
  catch (const Error& e)
    {
      std::cout << e << std::endl;
    }
}
