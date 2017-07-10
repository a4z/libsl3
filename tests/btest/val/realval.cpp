
#include <a4testing.hpp>

#include <sl3/value.hpp>
#include <sl3/error.hpp>
#include "eqorder.hpp"
#include <functional>

#ifdef _MSC_VER
#include <ciso646>
#endif


namespace sl3
{
  namespace
  {

    using VauleRelation = std::function<bool(const Value&, const Value&)>;

    VauleRelation strong_eq = std::equal_to<sl3::Value>() ;
    VauleRelation strong_lt = std::less<Value>() ;

    VauleRelation weak_eq = sl3::weak_eq;
    VauleRelation weak_lt = sl3::weak_lt ;
  
    using _v = Value ;

    void
    create ()
    {
      Value a(12.34);
      Value b = _v(12.34) ;

      BOOST_CHECK (a.real() == 12.34) ;
      BOOST_CHECK (a.getType()== Type::Real) ;
      BOOST_CHECK  (not a.isNull()) ;
      BOOST_CHECK (b.getType()== Type::Real) ;
      BOOST_CHECK  (not b.isNull()) ;

      Value c ;
      BOOST_CHECK (c.getType() == Type::Null) ;
      BOOST_CHECK  (c.isNull()) ;

      Value d{std::move(b)};
      BOOST_CHECK (d.getType()== Type::Real) ;
      BOOST_CHECK (not d.isNull()) ;


    }


    void
    assing ()
    {

      Value a;
      BOOST_CHECK (a.isNull()) ;
      BOOST_CHECK_EQUAL (a.getType(), Type::Null) ;
      double real = 100.0 ;
      a = real ;
      BOOST_CHECK_EQUAL (a.real() , real) ;
      BOOST_CHECK_EQUAL (a.getType(), Type::Real) ;
      BOOST_CHECK (!a.isNull()) ;

      Value b;
      b = a ;
      BOOST_CHECK_EQUAL (b.real() , real) ;
      BOOST_CHECK_EQUAL (b , a) ;

      b = Value{} ;
      BOOST_CHECK_EQUAL (b.getType(), Type::Null) ;
      a.setNull() ;
      BOOST_CHECK_EQUAL (b , a) ;

    }

    void
    move()
    {
      Value a{1.0};
      BOOST_CHECK (!a.isNull()) ;
      BOOST_CHECK_EQUAL (a.getType(), Type::Real) ;

      Value b{std::move(a)};
      BOOST_CHECK (a.isNull()) ;
      BOOST_CHECK_EQUAL (a.getType(), Type::Null) ;

      BOOST_CHECK_EQUAL (b.getType(), Type::Real) ;
      BOOST_CHECK_EQUAL (b.real(), 1.0) ;

      Value c = std::move(b);
      BOOST_CHECK (b.isNull()) ;

      BOOST_CHECK_EQUAL (c.getType(), Type::Real) ;
      BOOST_CHECK_EQUAL (c.real(), 1.0) ;

      Value d;
      d = std::move(c);

      BOOST_CHECK (c.isNull()) ;
      BOOST_CHECK_EQUAL (d.getType(), Type::Real) ;
      BOOST_CHECK_EQUAL (d.real(), 1.0) ;


    }

    void
     equality ()
     {
        Value a(100.0), b(100.0), c(100.0) ;
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

       BOOST_CHECK (strong_eq(Value{1.0}, Value{1.0}));
       BOOST_CHECK (!strong_eq(Value{1}, Value{1.0}));

       Value a(1.00), b(1.00), c(1.00) ;
       Value d(1.00), e(2.00), f(3.00) ;

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
       BOOST_CHECK (weak_eq(Value{1}, Value{1}));
       BOOST_CHECK (weak_eq(Value{1}, Value{1.0}));

       Value a(100.0), b(100.0), c(100.0) ;
       Value d{1.1}, e(2.00), f(300.2) ;

       BOOST_CHECK (weak_reflexive (a,a, weak_eq));
       BOOST_CHECK (eq_symmetric (a,b, weak_eq));
       BOOST_CHECK (eq_transitive (a,b,c, weak_eq));

       BOOST_CHECK (irreflexive (a,b, weak_eq, weak_lt));
       BOOST_CHECK (lt_transitive (d,e,f, weak_lt));
       BOOST_CHECK (trichotomy (d,e, weak_eq, weak_lt));

     }



     void
     compareWithOthers ()
     {
       Value nullVal ;
       Value intVal (1) ;
       Value realVal (1.0) ;
       Value textVal ("a");
       Value blobVal (Blob{});

       BOOST_CHECK (realVal != nullVal);
       BOOST_CHECK (realVal > nullVal);
       BOOST_CHECK (!(realVal < nullVal));

       BOOST_CHECK (realVal != intVal);
       BOOST_CHECK (realVal > intVal);

       BOOST_CHECK (Value{1.9} < Value{2});



       BOOST_CHECK (realVal != textVal);
       BOOST_CHECK (realVal < textVal);

       BOOST_CHECK (realVal != blobVal);
       BOOST_CHECK (realVal < blobVal);


     }


     void
     convert ()
     {
       int val=100;
       Value a(val);

       BOOST_CHECK_NO_THROW( static_cast<int>(a)  );
       BOOST_CHECK_NO_THROW( static_cast<int64_t>(a)  );
       BOOST_CHECK_NO_THROW( static_cast<double>(a)  );

       BOOST_CHECK_THROW( static_cast<std::string>(a), ErrTypeMisMatch);
       BOOST_CHECK_THROW(static_cast<Blob>(a), ErrTypeMisMatch);


       BOOST_CHECK_THROW(static_cast<int>(Value{}),
                         ErrNullValueAccess);

       BOOST_CHECK_THROW(static_cast<int64_t>(Value{}),
                         ErrNullValueAccess);



     }


     void
     getValue ()
     {
       BOOST_CHECK_THROW (Value{}.real(); , ErrNullValueAccess);
       BOOST_CHECK_THROW (Value{"x"}.real(); , ErrTypeMisMatch);
       BOOST_CHECK_THROW (Value{1}.real(); , ErrTypeMisMatch);
       BOOST_CHECK_NO_THROW (Value{1.0}.real(); );

     }


    a4TestAdd (a4test::suite ("realval")
          .addTest ("create", create)
          .addTest ("assing", assing)
          .addTest ("move", move)
          .addTest ("equality", equality)
          .addTest ("strictTotalOrdered", strictTotalOrdered)
          .addTest ("weakTotalOrdered", weakTotalOrdered)
          .addTest ("compareWithOthers", compareWithOthers)
          .addTest ("convert", convert)
          .addTest ("getValue", getValue)
    );
  }
}

