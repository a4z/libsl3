
#include <a4testing.hpp>

#include <sl3/value.hpp>
#include <sl3/error.hpp>

#include <functional>

namespace sl3
{
  namespace
  {

    void regular ()
    {
   
      // regular a == b, b != a 
      // constructed and compared 
  
      Value a{1} ;
      Value b{2} ;
      Value c = 2 ;
      Value d(2) ;

      Value e (d);
      Value f = e;
      
      BOOST_CHECK (a == a) ;
      BOOST_CHECK (!(a != a)) ;
      
      BOOST_CHECK (a != b) ;
      BOOST_CHECK (!(a == b)) ;
      BOOST_CHECK (b != a) ;
      BOOST_CHECK (!(b == a)) ;

      BOOST_CHECK (b == c) ;
      BOOST_CHECK (c == b) ;
      BOOST_CHECK (!(b != c)) ;
      BOOST_CHECK (!(c != b)) ;
   
      BOOST_CHECK (b == d) ;
      BOOST_CHECK (c == d) ;


      BOOST_CHECK (d == e) ;
      BOOST_CHECK (e == f) ;
    }

//http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4367.html#Definition
   
    void order ()
    {
    
    }


    void totalOrder ()
    {
    }



    a4TestAdd (a4test::suite ("intval")
          .addTest ("regular", regular)
          .addTest ("order", order)
          .addTest ("totalOrder", totalOrder)

                   );
  }
}

