#include "../testing.hpp"

#include <sl3/database.hpp>
#include <sl3/error.hpp>

#include <string>

SCENARIO ("Check RowCallback and Callback return values")
{
  sl3::Database db{":memory:"};

  REQUIRE_NOTHROW (db.execute ("CREATE TABLE tbltest (f INTEGER);"
                               "INSERT INTO tbltest VALUES (1) ;"
                               "INSERT INTO tbltest VALUES (2) ;"
                               "INSERT INTO tbltest VALUES (3) ;"));

  GIVEN ("a RowCallback that counts all calls")
  {
    struct CB : sl3::RowCallback
    {
      size_t counter{0};
      bool
      onRow (sl3::Columns /*unusedForNow*/)
      {
        ++counter;
        return true;
      }
    };

    CB cb;
    WHEN ("executed for the test data")
    {
      db.execute ("SELECT * FROM tbltest;", cb);
      THEN ("it was called for each of the 3 records")
      {
        CHECK (cb.counter == 3);
      }
    }
  }

  GIVEN ("a RowCallback that returns false after the second call")
  {
    struct : sl3::RowCallback
    {
      size_t counter{0};
      bool
      onRow (sl3::Columns /*unusedForNow*/)
      {
        ++counter;
        if (counter == 2)
          return false;
        return true;
      }
    } cb;

    WHEN ("executed for the test data")
    {
      db.execute ("SELECT * FROM tbltest;", cb);
      THEN ("it was called 2 time") { CHECK (cb.counter == 2); }
    }
  }

  GIVEN ("a Callback that counts all calls")
  {
    size_t counter{0};
    auto   cb = [&counter] (sl3::Columns /*ignoreForNow*/) -> bool {
      ++counter;
      return true;
    };

    WHEN ("executed for the test data")
    {
      db.execute ("SELECT * FROM tbltest;", cb);
      THEN ("it was called for each of the 3 records")
      {
        CHECK (counter == 3);
      }
    }
  }

  GIVEN ("a Callback that returns false after the second call")
  {
    size_t counter{0};
    auto   cb = [&counter] (sl3::Columns /*ignoreForNow*/) -> bool {
      ++counter;
      return counter == 2 ? false : true;
    };
    WHEN ("executed for the test data")
    {
      db.execute ("SELECT * FROM tbltest;", cb);
      THEN ("it was called 2 time") { CHECK (counter == 2); }
    }
  }
}

SCENARIO ("Check RowCallback start and end calls")
{
  sl3::Database db{":memory:"};

  REQUIRE_NOTHROW (db.execute ("CREATE TABLE tbltest (f INTEGER);"
                               "INSERT INTO tbltest VALUES (1) ;"
                               "INSERT INTO tbltest VALUES (2) ;"
                               "INSERT INTO tbltest VALUES (3) ;"));

  GIVEN ("a RowCallback that counts all calls and fantasi on start/end counts")
  {
    struct CB : sl3::RowCallback
    {
      size_t counter{0};
      size_t onstart{10000};
      size_t onend{10000};

      void
      onStart ()
      {
        onstart = counter;
      }

      void
      onEnd ()
      {
        onend = counter;
      }

      bool
      onRow (sl3::Columns /*unusedForNow*/)
      {
        ++counter;
        return true;
      }
    };

    CB cb;
    WHEN ("executed for the test data")
    {
      db.execute ("SELECT * FROM tbltest;", cb);
      THEN ("on start/end counts have logical values")
      {
        CHECK (cb.counter == 3);
        CHECK (cb.onstart == 0);
        CHECK (cb.onstart == 0);
        CHECK (cb.onend == 3);
      }
    }
  }
}

SCENARIO ("testing column names and index properties")
{
  sl3::Database db{":memory:"};

  GIVEN ("a statement that returns 2 columns with known names")
  {
    auto sql = "SELECT 1 AS first, 'hello' AS second;";
    REQUIRE_NOTHROW (db.execute (sql));

    WHEN ("accessing columns via index")
    {
      THEN ("valid indexes will not throw")
      {
        db.execute (sql, [] (sl3::Columns cols) {
          REQUIRE (cols.count () == 2);
          CHECK_NOTHROW ((void)cols.getValue (0));
          CHECK_NOTHROW ((void)cols.getValue (0, sl3::Type::Int));
          CHECK_NOTHROW ((void)cols.getValue (1));
          CHECK_NOTHROW ((void)cols.getValue (1, sl3::Type::Text));
          return false;
        });
      }

      THEN ("invalid indexes will throw throw out of range")
      {
        db.execute (sql, [] (sl3::Columns cols) {
          REQUIRE (cols.count () == 2);
          CHECK_THROWS_AS (cols.getValue (2), sl3::ErrOutOfRange);
          CHECK_THROWS_AS (cols.getValue (2, sl3::Type::Variant),
                           sl3::ErrOutOfRange);
          return false;
        });
      }
    }

    WHEN ("accessing the names via index")
    {
      THEN ("valid indexes return expected results")
      {
        db.execute (sql, [] (sl3::Columns cols) {
          REQUIRE (cols.count () == 2);
          CHECK (cols.getName (0) == "first");
          CHECK (cols.getName (1) == "second");
          auto names = cols.getNames ();
          CHECK (cols.getName (0) == names.at (0));
          CHECK (cols.getName (1) == names.at (1));
          return false;
        });
      }

      THEN ("invalid indexes throw out of range")
      {
        db.execute (sql, [] (sl3::Columns cols) {
          REQUIRE (cols.count () == 2);
          CHECK_THROWS_AS (cols.getName (2), sl3::ErrOutOfRange);
          return false;
        });
      }
    }
  }
}

SCENARIO ("all index accesses throw out of range for an invalud index")
{
  GIVEN ("a record with some fields")
  {
    sl3::Database db{":memory:"};
    auto          sql = "SELECT 1 as int, "
                        " 2.3 as real, "
                        " 'hello' as text, "
                        " x'1F' as blob, "
                        " NULL as noval; ";

    REQUIRE_NOTHROW (db.execute (sql));
    WHEN ("access any property with an invalid index")
    {
      THEN ("an ErrOutOfRange exception is thrown")
      {
        using namespace sl3;
        db.execute (sql, [] (Columns cols) {
          auto badIdx = cols.count (); // or bigge

          CHECK_THROWS_AS ((void)cols.getType (badIdx), ErrOutOfRange);
          CHECK_THROWS_AS ((void)cols.getValue (badIdx), ErrOutOfRange);
          CHECK_THROWS_AS ((void)cols.getValue (badIdx, Type::Variant),
                           ErrOutOfRange);
          CHECK_THROWS_AS ((void)cols.getName (badIdx), ErrOutOfRange);
          CHECK_THROWS_AS ((void)cols.getSize (badIdx), ErrOutOfRange);
          CHECK_THROWS_AS ((void)cols.getInt (badIdx), ErrOutOfRange);
          CHECK_THROWS_AS ((void)cols.getInt64 (badIdx), ErrOutOfRange);
          CHECK_THROWS_AS ((void)cols.getReal (badIdx), ErrOutOfRange);
          CHECK_THROWS_AS ((void)cols.getText (badIdx), ErrOutOfRange);
          CHECK_THROWS_AS ((void)cols.getBlob (badIdx), ErrOutOfRange);
          return false;
        });
      }
    }
  }
}

SCENARIO ("getting automatic type info")
{
  GIVEN ("a test db")
  {
    sl3::Database db{":memory:"};

    WHEN ("selecting an integer value")
    {
      auto sql = "SELECT 1 ;";
      THEN ("columns will auto detect int")
      {
        db.execute (sql, [] (sl3::Columns cols) {
          CHECK (cols.getType (0) == sl3::Type::Int);
          return false;
        });
      }
    }
    WHEN ("selecting a real value")
    {
      auto sql = "SELECT 1.23 ;";
      THEN ("columns will auto detect real")
      {
        db.execute (sql, [] (sl3::Columns cols) {
          CHECK (cols.getType (0) == sl3::Type::Real);
          return false;
        });
      }
    }
    WHEN ("selecting a text value")
    {
      auto sql = "SELECT 'hello' ;";
      THEN ("columns will auto detect text")
      {
        db.execute (sql, [] (sl3::Columns cols) {
          CHECK (cols.getType (0) == sl3::Type::Text);
          return false;
        });
      }
    }
    WHEN ("selecting a blob value")
    {
      auto sql = "SELECT x'12345678ABCDEF';";
      THEN ("columns will auto detect blob")
      {
        db.execute (sql, [] (sl3::Columns cols) {
          CHECK (cols.getType (0) == sl3::Type::Blob);
          return false;
        });
      }
    }
    WHEN ("selecting a NULL")
    {
      auto sql = "SELECT NULL ;";
      THEN ("columns will auto detect NULL")
      {
        db.execute (sql, [] (sl3::Columns cols) {
          CHECK (cols.getType (0) == sl3::Type::Null);
          return false;
        });
      }
    }

    AND_WHEN ("accessing the type out of index")
    {
      auto sql = "SELECT NULL ;";
      THEN ("an out of range exception is thrown")
      {
        db.execute (sql, [] (sl3::Columns cols) {
          CHECK_THROWS_AS (cols.getType (1), sl3::ErrOutOfRange);
          return false;
        });
      }
    }
  }
}

SCENARIO ("getting DbValues from columns")
{
  GIVEN ("a record with known data and known types")
  {
    sl3::Database db{":memory:"};
    auto          sql = "SELECT 1 as int, "
                        " 2.3 as real, "
                        " 'hello' as text, "
                        " x'1F' as blob, "
                        " NULL as noval; ";

    WHEN ("verify the test data")
    {
      THEN ("fields and types are as expected")
      {
        REQUIRE_NOTHROW (db.execute (sql));
        db.execute (sql, [] (sl3::Columns cols) {
          CHECK (cols.getType (0) == sl3::Type::Int);
          CHECK (cols.getType (1) == sl3::Type::Real);
          CHECK (cols.getType (2) == sl3::Type::Text);
          CHECK (cols.getType (3) == sl3::Type::Blob);
          CHECK (cols.getType (4) == sl3::Type::Null);
          return false;
        });
      }
    }

    WHEN ("accessing values as they are")
    {
      THEN ("value type properties are set correct")
      {
        db.execute (sql, [] (sl3::Columns cols) {
          CHECK (cols.getValue (0).dbtype () == sl3::Type::Variant);
          CHECK (cols.getValue (0).type () == sl3::Type::Int);
          CHECK (cols.getValue (1).dbtype () == sl3::Type::Variant);
          CHECK (cols.getValue (1).type () == sl3::Type::Real);
          CHECK (cols.getValue (2).dbtype () == sl3::Type::Variant);
          CHECK (cols.getValue (2).type () == sl3::Type::Text);
          CHECK (cols.getValue (3).dbtype () == sl3::Type::Variant);
          CHECK (cols.getValue (3).type () == sl3::Type::Blob);
          CHECK (cols.getValue (4).dbtype () == sl3::Type::Variant);
          CHECK (cols.getValue (4).type () == sl3::Type::Null);
          return true;
        });
      }
    }

    WHEN ("accessing values with wanted types")
    {
      THEN ("requesting correct types will work")
      {
        using namespace sl3;
        db.execute (sql, [] (sl3::Columns cols) {
          auto t
              = {Type::Int, Type::Real, Type::Text, Type::Blob, Type::Variant};
          sl3::Types expectedTypes{t};
          REQUIRE (expectedTypes.size () == cols.count ());
          for (int i = 0; i < cols.count (); ++i)
            {
              auto type = expectedTypes[size_t (i)];
              auto val  = cols.getValue (i, type);
              CHECK (val.dbtype () == type);
              if (i == cols.count () - 1)
                CHECK (val.type () == Type::Null);
              else
                CHECK (val.type () == type);
            }

          return true;
        });
      }

      THEN ("requesting incorrect types will always throw")
      {
        using namespace sl3;
        db.execute (sql, [] (sl3::Columns cols) {
          CHECK_THROWS_AS ((void)cols.getValue (0, Type::Real),
                           ErrTypeMisMatch);
          CHECK_THROWS_AS ((void)cols.getValue (1, Type::Int),
                           ErrTypeMisMatch);
          CHECK_THROWS_AS ((void)cols.getValue (2, Type::Int),
                           ErrTypeMisMatch);
          CHECK_THROWS_AS ((void)cols.getValue (3, Type::Int),
                           ErrTypeMisMatch);
          return true;
        });
      }
    }
  }
}

SCENARIO ("getting rows from columns")
{
  GIVEN ("a record with known data and known types")
  {
    sl3::Database db{":memory:"};
    auto          sql = "SELECT 1 as int, "
                        " 2.3 as real, "
                        " 'hello' as text, "
                        " x'1F' as blob, "
                        " NULL as noval; ";

    WHEN ("requesting just a row")
    {
      THEN ("fields and types are as expected")
      {
        REQUIRE_NOTHROW (db.execute (sql));
        db.execute (sql, [] (sl3::Columns cols) {
          auto row = cols.getRow ();
          REQUIRE (row.size () == cols.count ());
          CHECK (row.at (0).dbtype () == sl3::Type::Variant);
          CHECK (row.at (0).type () == sl3::Type::Int);
          CHECK (row.at (1).dbtype () == sl3::Type::Variant);
          CHECK (row.at (1).type () == sl3::Type::Real);
          CHECK (row.at (2).dbtype () == sl3::Type::Variant);
          CHECK (row.at (2).type () == sl3::Type::Text);
          CHECK (row.at (3).dbtype () == sl3::Type::Variant);
          CHECK (row.at (3).type () == sl3::Type::Blob);
          CHECK (row.at (4).dbtype () == sl3::Type::Variant);
          CHECK (row.at (4).type () == sl3::Type::Null);
          return false;
        });
      }
    }

    WHEN ("request a row with wrong count of types")
    {
      THEN ("exceptions are the consequenze")
      {
        REQUIRE_NOTHROW (db.execute (sql));
        db.execute (sql, [] (sl3::Columns cols) {
          using namespace sl3;
          auto toless
              = {Type::Variant, Type::Variant, Type::Variant, Type::Variant};
          auto tomany = {Type::Variant,
                         Type::Variant,
                         Type::Variant,
                         Type::Variant,
                         Type::Variant,
                         Type::Variant};
          CHECK_THROWS_AS (cols.getRow (toless), ErrTypeMisMatch);
          CHECK_THROWS_AS (cols.getRow (tomany), ErrTypeMisMatch);
          return false;
        });
      }
    }

    WHEN ("request a row with spezific correct  types")
    {
      THEN ("a row with those types is retunred")
      {
        REQUIRE_NOTHROW (db.execute (sql));
        db.execute (sql, [] (sl3::Columns cols) {
          using namespace sl3;
          auto types
              = {Type::Int, Type::Real, Type::Text, Type::Blob, Type::Variant};
          auto row = cols.getRow (types);
          CHECK (row.at (0).dbtype () == sl3::Type::Int);
          CHECK (row.at (0).type () == sl3::Type::Int);
          CHECK (row.at (1).dbtype () == sl3::Type::Real);
          CHECK (row.at (1).type () == sl3::Type::Real);
          CHECK (row.at (2).dbtype () == sl3::Type::Text);
          CHECK (row.at (2).type () == sl3::Type::Text);
          CHECK (row.at (3).dbtype () == sl3::Type::Blob);
          CHECK (row.at (3).type () == sl3::Type::Blob);
          CHECK (row.at (4).dbtype () == sl3::Type::Variant);
          CHECK (row.at (4).type () == sl3::Type::Null);

          return false;
        });
      }
    }
  }
}
