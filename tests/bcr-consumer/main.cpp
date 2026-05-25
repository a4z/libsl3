#include <sl3/database.hpp>

int
main ()
{
  sl3::Database db{":memory:"};
  db.execute ("CREATE TABLE item(value INTEGER);"
              "INSERT INTO item VALUES (1);"
              "SELECT value FROM item;");
}
