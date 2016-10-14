
#include <a4testing.hpp>

#include <sl3/value.hpp>
#include <sl3/error.hpp>
#include <sstream>
#include <string>
#include <cmath>
#include <algorithm>


namespace sl3
{
  namespace
  {

    void createValue ()
    {
   
// regular a == b, b != a 

      {
       Value a ;
       Value b (a);
       Value c = b ;

       BOOST_CHECK_EQUAL (a.getType (), Type::Null);
       BOOST_CHECK_EQUAL (b.getType (), Type::Null);
       BOOST_CHECK_EQUAL (c.getType (), Type::Null);
       BOOST_CHECK (c.isNull ()) ; 
      }
      
       {
       Value a{1} ;
       Value b (a);
       Value c = b ;
       Value d = 1 ;
       BOOST_CHECK_EQUAL (a.getType (), Type::Int);

       BOOST_CHECK_EQUAL (b.getType (), Type::Int);
       BOOST_CHECK_EQUAL (c.getType (), Type::Int);
       BOOST_CHECK_EQUAL (d.getType (), Type::Int);
       BOOST_CHECK (!d.isNull ()) ;        
      }
     
      {
       Value a{1.1} ;
       Value b (a);
       Value c = b ;
       Value d = 1.1 ;
       BOOST_CHECK_EQUAL (a.getType (), Type::Real);
       BOOST_CHECK_EQUAL (b.getType (), Type::Real);
       BOOST_CHECK_EQUAL (c.getType (), Type::Real);
       BOOST_CHECK_EQUAL (d.getType (), Type::Real);
       BOOST_CHECK (!d.isNull ()) ;        
      }
  
      {
       Value a{std::string{}} ;
       Value b (a);
       Value c = b ;
       Value d = std::string{} ;
       BOOST_CHECK_EQUAL (a.getType (), Type::Text);
       BOOST_CHECK_EQUAL (b.getType (), Type::Text);
       BOOST_CHECK_EQUAL (c.getType (), Type::Text);
       BOOST_CHECK_EQUAL (d.getType (), Type::Text);
       BOOST_CHECK (!d.isNull ()) ;        
      }
  

       {
       Value a{Blob{}} ;
       Value b (a);
       Value c = b ;
       Value d = Blob{} ;
       BOOST_CHECK_EQUAL (a.getType (), Type::Blob);

       BOOST_CHECK_EQUAL (b.getType (), Type::Blob);
       BOOST_CHECK_EQUAL (c.getType (), Type::Blob);
       BOOST_CHECK_EQUAL (d.getType (), Type::Blob);
       BOOST_CHECK (!d.isNull ()) ;        
      }
    }


    void 
    compareEqual ()
    {

       {
       Value a{1} ;
       Value b (a);
       Value c = 1;

       BOOST_CHECK_EQUAL (a, b) ;
       BOOST_CHECK_EQUAL (a, c) ;

       BOOST_CHECK (a == Value(1.0));
       // this should be the same as SELECT 1 == 1.00000; // and this is true 

       BOOST_CHECK (a != Value(2));
       BOOST_CHECK (a != Value("a"));
       BOOST_CHECK (a != Value(Blob{1,2,3}));

      }

    }

    void 
    compareLess ()
    {

       {
       Value a{1} ;
       Value b (2);
       Value c = 3;


       BOOST_CHECK (a < b);
       BOOST_CHECK (b < c);
       BOOST_CHECK (a < c);

       BOOST_CHECK ( a < Value{1.0});
      
       }

// TODO this is great, put it on a better place

       auto trichotomous = [](const Value& a, const Value& b) -> bool {
         std::vector<bool> v = { a<b, a==b, b<a } ; 
         return std::count (std::begin (v), std::end (v), true) ;
       };



       BOOST_CHECK (trichotomous (Value(1), Value (2))) ;
       BOOST_CHECK (trichotomous (Value(2), Value (2))) ;
       BOOST_CHECK (trichotomous (Value(3), Value (2))) ;
       BOOST_CHECK (trichotomous (Value(1.0), Value (2))) ;
       BOOST_CHECK (trichotomous (Value(2.0), Value (2))) ;
       BOOST_CHECK (trichotomous (Value(2.0), Value (2))) ;
       BOOST_CHECK (trichotomous (Value(2.0), Value (2.0))) ;



//Reflexive: a == b implies eq(a, b)
//Symmetric: eq(a, b) implies eq(b, a)
//Transitive: eq(a, b) && eq(b, c) implies eq(a, c)


//(strict) weak order 	ordered partitions: a<b implies a<c or c<b
//(strict) total order 	trichotomous: exactly one of a<b, a=b, b<a
    }




// Irreflexive law: a == b implies !(a < b)
// Transitive law: a < b && b < c implies a < c
// Trichotomy law: a != b implies a < b || b < a 

    void 
    strictTotalOrdered ()
    {
      
      
    }



    a4TestAdd (a4test::suite ("value")
          .addTest ("create", createValue)
          .addTest ("equal", compareEqual)
          .addTest ("less", compareLess)
          .addTest ("totalOrder", strictTotalOrdered)

                   );
  }
}

