#include <sl3/dbvalues.hpp>

#include <a4testing.hpp>

#include <sl3/error.hpp>


using namespace sl3;

namespace sl3 {
namespace {
  
  
void assingCheckSize ()
{
  bool unused{false};
  DbValues r1 ({DbValue (1)});
  DbValues r2 ({DbValue (2), DbValue ("b")});

  BOOST_CHECK_THROW (r1 = r2, ErrTypeMisMatch );
  BOOST_CHECK_THROW (r1 = std::move (r2), ErrTypeMisMatch );

  DbValues r3 ({DbValue (3), DbValue ("c")});

  BOOST_CHECK_NO_THROW (r3 = r2);

  BOOST_CHECK_NO_THROW (unused = r3.at(0) == r2.at(0));
  BOOST_CHECK_NO_THROW (unused = r3.at(1) == r2.at(1));

  (void)(unused) ;
}

void assingCheckType ()
{
  bool unused{false};
  {
    DbValues r1 ({DbValue ("a"), DbValue("a")});
    DbValues r2 ({DbValue (2), DbValue("b")});

    BOOST_CHECK_THROW(r1 = r2, ErrTypeMisMatch );
    BOOST_CHECK_THROW(r1 = std::move (r2), ErrTypeMisMatch );
  }

  {
    DbValues r1 ({DbValue ("a"), DbValue("a")});
    DbValues r2 ({DbValue (2, Type::Variant), DbValue("b")});

    BOOST_CHECK_THROW(r1 = r2, ErrTypeMisMatch );
    BOOST_CHECK_THROW(r1 = std::move (r2), ErrTypeMisMatch );
  }

  {
    DbValues r1 ({DbValue ("a"), DbValue("a")});
    DbValues r2 ({DbValue ("b", Type::Variant), DbValue("b")});

    BOOST_CHECK_NO_THROW(r1 = r2);
    BOOST_CHECK_NO_THROW(unused = r1.at(0).getText () == "b");
    BOOST_CHECK_NO_THROW(unused =  r1.at(1).getText () == "b");
  }


  {
    DbValues r1 ({DbValue ("a"), DbValue("a")});
    DbValues r2 ({DbValue ("b"), DbValue("a")});

    BOOST_CHECK_NO_THROW(r1 = r2);

    BOOST_CHECK_NO_THROW(unused = r1.at(0).getText () == "b");
    BOOST_CHECK_NO_THROW(unused = r1.at(1).getText () == "b");
  }

  {
    DbValues r1 ({DbValue ("a"), DbValue("a")});
    DbValues r2 ({DbValue ("b", Type::Variant), DbValue("b")});

    BOOST_CHECK_NO_THROW(r1 = std::move (r2));

    BOOST_CHECK_NO_THROW(unused = r1.at(0).getText () == "b");
    BOOST_CHECK_NO_THROW(unused = r1.at(1).getText () == "b");
  }


  {
    DbValues r1 ({DbValue ("a"), DbValue("a")});
    DbValues r2 ({DbValue ("b"), DbValue("a")});

    BOOST_CHECK_NO_THROW(r1 = std::move (r2));
    
    BOOST_CHECK_NO_THROW(r1.at(0).getText ());
    BOOST_CHECK_NO_THROW(r1.at(1).getText ());
  }


  (void)(unused) ;
}



a4TestAdd(
  a4test::suite("dbvalues")
  .addTest("assingCheckSize"  , assingCheckSize)
  .addTest("assingCheckType"  , assingCheckType)
) ;

}
}
