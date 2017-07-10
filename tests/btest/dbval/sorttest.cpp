
#include <a4testing.hpp>

#include <sl3/database.hpp>
#include <sl3/dbvalue.hpp>
#include <sl3/dbvalues.hpp>
#include <sl3/error.hpp>

#include <algorithm>
#include <iostream>
#include <vector>

using namespace sl3;

namespace sl3
{
  namespace
  {
  

    Database
    testdb ()
    {
      Database db{":memory:"};
      db.execute (
          "CREATE TABLE tbltest (intFld INTEGER,txtFld TEXT, dblFld real );"
          "INSERT INTO tbltest VALUES (2, 'aa', NULL) ;"
          "INSERT INTO tbltest VALUES (1, 'bb', 2.22) ;"
          "INSERT INTO tbltest VALUES (1, 'bb', 1.11) ;"
          "INSERT INTO tbltest VALUES (NULL, 'bb', 2.22) ;"
          "INSERT INTO tbltest VALUES (1, 'bb', 2.22) ;"
          "INSERT INTO tbltest VALUES (3, NULL, NULL) ;");

      return db;
    }

    bool
    sameDS (const Dataset& a, const Dataset& b)
    {
      if (a.size () != b.size ())
        return false;

      for (size_t i = 0; i < a.size (); ++i)
        {
          if (a[i].size () != b[i].size ())
            return false;

          for (size_t j = 0; j < a[i].size (); ++j)
            {
              if (a[i][j] != b[i][j])
                return false;
            }
        }
      return true;
    }


    auto  printDS = [](const Dataset& ds)
    {

      std::cout << ".................." << std::endl ;
      for (const auto& row: ds)
      {
        for (const auto& field : row)
          std::cout << field << " " ;

        std::cout << std::endl ;
      }

      std::cout << ".................." << std::endl ;
    };

    void
    sortRecord ()
    {

     (void)printDS ; // avoid not used as long I need this function here

      auto db = testdb ();
      auto ds = db.select ("SELECT * FROM tbltest;");

      ds.sort ({0});
      auto checkSQL = "SELECT * FROM tbltest ORDER BY intFld;";
      BOOST_CHECK (sameDS (ds, db.select (checkSQL)));

      ds.sort( {1, 2, 0});
      checkSQL = "SELECT * FROM tbltest ORDER BY txtFld, dblFld, intFld";
      
      auto dbsort = db.select (checkSQL);
      //printDS (ds);
      //printDS(dbsort) ;

      BOOST_CHECK (sameDS (ds, dbsort));
    }

    
    void
    sortTypedRecord ()
    {
      auto db = testdb ();
      Types types =  {Type::Int, Type::Text, Type::Real};
      auto ds = db.select ("SELECT * FROM tbltest;", types);

      ds.sort ({0});
      auto checkSQL = "SELECT * FROM tbltest ORDER BY intFld;";
      BOOST_CHECK (sameDS (ds, db.select (checkSQL, types)));

      ds.sort( {1, 2, 0});
      checkSQL = "SELECT * FROM tbltest ORDER BY txtFld, dblFld, intFld";
      
      auto dbsort = db.select (checkSQL, types);
      //printDS (ds);
      //printDS(dbsort) ;

      BOOST_CHECK (sameDS (ds, dbsort));
    }

    void
    sortLargeRecord ()
    {


      auto db = testdb ();

      
       db.execute (
          "INSERT INTO tbltest VALUES (21, 'aa', NULL) ;"
          "INSERT INTO tbltest VALUES (11, 'bb', 2.22) ;"
          "INSERT INTO tbltest VALUES (11, 'bb', 1.11) ;"
          "INSERT INTO tbltest VALUES (NULL, 'bb', 2.22) ;"
          "INSERT INTO tbltest VALUES (11, 'bb', 2.22) ;"
          "INSERT INTO tbltest VALUES (31, NULL, NULL) ;"
          "INSERT INTO tbltest VALUES (22, 'aa', NULL) ;"
          "INSERT INTO tbltest VALUES (12, 'bb', 2.242) ;"
          "INSERT INTO tbltest VALUES (12, 'bb', 1.1671) ;"
          "INSERT INTO tbltest VALUES (NULL, 'bb', 2.22) ;"
          "INSERT INTO tbltest VALUES (13, 'bb', 32.22) ;"
          "INSERT INTO tbltest VALUES (33, NULL, NULL) ;"
          "INSERT INTO tbltest VALUES (24, 'aa', NULL) ;"
          "INSERT INTO tbltest VALUES (14, 'abb', 2.242) ;"
          "INSERT INTO tbltest VALUES (15, 'sbb', 21.171) ;"
          "INSERT INTO tbltest VALUES (NULL, 'dbb', 32.22) ;"
          "INSERT INTO tbltest VALUES (16, 'bdb', 2.242) ;"
          "INSERT INTO tbltest VALUES (33, NULL, NULL) ;"
          "INSERT INTO tbltest VALUES (22, 'afa', NULL) ;"
          "INSERT INTO tbltest VALUES (14, 'bbf', 72.322) ;"
          "INSERT INTO tbltest VALUES (15, 'bbg', 1.11) ;"
          "INSERT INTO tbltest VALUES (NULL, 'hubb', 32.22) ;"
          "INSERT INTO tbltest VALUES (16, 'bbk', 72.22) ;"
          "INSERT INTO tbltest VALUES (38, NULL, NULL) ;"
          "INSERT INTO tbltest VALUES (29, 'aau', NULL) ;"
          "INSERT INTO tbltest VALUES (10, 'bbt', 32.22) ;"
          "INSERT INTO tbltest VALUES (10, 'bbr', 1.311) ;"
          "INSERT INTO tbltest VALUES (NULL, 'rbb', 22.22) ;"
          "INSERT INTO tbltest VALUES (1909, 'btb', 2.22) ;"
          "INSERT INTO tbltest VALUES (300, NULL, NULL) ;"
         
          );

     
      
      
      
      auto ds = db.select ("SELECT intFld FROM tbltest;");

      ds.sort ({0});
      auto checkSQL = "SELECT intFld FROM tbltest ORDER BY intFld;";
      BOOST_CHECK (sameDS (ds, db.select (checkSQL)));


      ds = db.select ("SELECT * FROM tbltest;");
      ds.sort( {1, 2, 0});
      checkSQL = "SELECT * FROM tbltest ORDER BY txtFld, dblFld, intFld";
      
      auto dbsort = db.select (checkSQL);
      //printDS (ds);
      //printDS(dbsort) ;

      BOOST_CHECK (sameDS (ds, dbsort));
    }



    a4TestAdd (a4test::suite ("sorting")
                   .addTest ("sortRecord", sortRecord)
                   .addTest ("sortTypedRecord", sortTypedRecord)
                   .addTest ("sortLargeRecord", sortLargeRecord)
                   );
  }
}
