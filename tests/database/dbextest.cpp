#include "../testing.hpp"

#include <sl3.hpp>
#include <sqlite3.h>

SCENARIO ("creating an in memory database with custom name")
{
  GIVEN ("an im memory database with a custom name")
  {
    sl3::Database db{
        "", SQLITE_OPEN_MEMORY | SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE};

    WHEN ("creating a table and some test data")
    {
      db.execute (
          "CREATE TABLE tbltest (intFld INTEGER,txtFld TEXT, dblFld real );"
          "INSERT INTO tbltest VALUES (1, 'eins', 1.111) ;"
          "INSERT INTO tbltest VALUES (2, 'zwei', 2.22) ;"
          "INSERT INTO tbltest VALUES (3, NULL, NULL) ;");

      THEN ("I can verify that the table and some test data exists")
      {
        CHECK (db.getTotalChanges () == 3);
        CHECK_NOTHROW (db.execute (std::string{"DELETE FROM tbltest;"}));
        CHECK (db.getTotalChanges () == 6);
        CHECK (db.getRecentlyChanged () == 3);
      }
    }
  }
}
