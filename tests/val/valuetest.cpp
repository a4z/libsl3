
#include <a4testing.hpp>

#include <sl3/value.hpp>
#include <sl3/error.hpp>

#include "eqorder.hpp"

#include <functional>


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


    using VauleRelation = std::function<bool(const Value&, const Value&)>;

    VauleRelation strong_eq = std::equal_to<sl3::Value>() ;
    VauleRelation strong_lt = std::less<Value>() ;

    VauleRelation weak_eq = sl3::weak_eq;
    VauleRelation weak_lt = sl3::weak_lt ;



    void
    equality ()
    {
       Value a(100), b(100), c(100) ;
       using namespace eqo ;

       BOOST_CHECK (eq_reflexive (a, strong_eq));
       BOOST_CHECK (weak_reflexive (a,b, weak_eq));

       BOOST_CHECK (eq_symmetric (a,b, strong_eq));
       BOOST_CHECK (eq_symmetric (a,b, weak_eq));

       BOOST_CHECK (eq_transitive (a,b,c, strong_eq));
       BOOST_CHECK (eq_transitive (a,b,c, weak_eq));

    }



    void 
    strictTotalOrdered ()
    {
      using namespace eqo ;
      Value a(100), b(100), c(100) ;
      Value d(100), e(100.0), f("foo") ;

      BOOST_CHECK (strong_eq(Value{1}, Value{1}));
      BOOST_CHECK (!strong_eq(Value{1}, Value{1.0}));

      BOOST_CHECK (eq_reflexive (a, strong_eq));
      BOOST_CHECK (eq_symmetric (a,b, strong_eq));
      BOOST_CHECK (eq_transitive (a,b,c, strong_eq));

      BOOST_CHECK (irreflexive (a,b, strong_eq, strong_lt));
      BOOST_CHECK (lt_transitive (d,e,f, strong_lt));
      BOOST_CHECK (trichotomy (d,e, strong_eq, strong_lt));
    }


    void
    weakTotalOrdered ()
    {
      using namespace eqo ;
      Value a(100), b(100.0), c(100) ;
      Value d(100), e(200.0), f(300) ;

      BOOST_CHECK (weak_eq(Value{1}, Value{1}));
      BOOST_CHECK (weak_eq(Value{1}, Value{1.0}));

      BOOST_CHECK (weak_reflexive (a,a, weak_eq));
      BOOST_CHECK (eq_symmetric (a,b, weak_eq));
      BOOST_CHECK (eq_transitive (a,b,c, weak_eq));

      BOOST_CHECK (irreflexive (a,b, weak_eq, weak_lt));
      BOOST_CHECK (lt_transitive (d,e,f, weak_lt));
      BOOST_CHECK (trichotomy (d,e, weak_eq, weak_lt));

    }



    a4TestAdd (a4test::suite ("value")
          .addTest ("create", createValue)
          .addTest ("equality", equality)
          .addTest ("totalOrder", strictTotalOrdered)
          .addTest ("weakOrder", weakTotalOrdered)
                   );
  }
}

