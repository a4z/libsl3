
#include <a4testing.hpp>

#include <sl3/value.hpp>
#include <sl3/error.hpp>
#include "eqorder.hpp"
#include <functional>

namespace sl3
{
  namespace
  {

    using VauleRelation = std::function<bool(const Value&, const Value&)>;

    VauleRelation strong_eq = std::equal_to<sl3::Value>() ;
    VauleRelation strong_lt = std::less<Value>() ;

    VauleRelation weak_eq = sl3::weak_eq;
    VauleRelation weak_lt = sl3::weak_lt ;
  
    void
    create ()
    {
      Value a(100.0);
      Value b(a);
      Value c = a ;
      Value d = 100.0 ;

      BOOST_CHECK_EQUAL (a, 100.00) ;
      BOOST_CHECK_EQUAL (a.getType(), Type::Real) ;
      BOOST_CHECK_EQUAL (b, 100.00) ;
      BOOST_CHECK_EQUAL (b.getType(), Type::Real) ;
      BOOST_CHECK_EQUAL (c, 100.00) ;
      BOOST_CHECK_EQUAL (c.getType(), Type::Real) ;
      BOOST_CHECK_EQUAL (d, 100.00) ;
      BOOST_CHECK_EQUAL (d.getType(), Type::Real) ;

      Value e;
      BOOST_CHECK (e.isNull()) ;
      BOOST_CHECK_EQUAL (e.getType(), Type::Null) ;
      e = 100.00 ;
      BOOST_CHECK_EQUAL (e, 100.00) ;
      BOOST_CHECK_EQUAL (e.getType(), Type::Real) ;
      BOOST_CHECK (!e.isNull()) ;

    }


    void
    assing ()
    {

      Value a;
      BOOST_CHECK (a.isNull()) ;
      BOOST_CHECK_EQUAL (a.getType(), Type::Null) ;
      double real = 100.0 ;
      a = real ;
      BOOST_CHECK_EQUAL (a , real) ;
      BOOST_CHECK_EQUAL (a.getType(), Type::Real) ;
      BOOST_CHECK (!a.isNull()) ;

      Value b;
      b = a ;
      BOOST_CHECK_EQUAL (b , real) ;
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
      BOOST_CHECK_EQUAL (b, 1.0) ;

      Value c = std::move(b);
      BOOST_CHECK (b.isNull()) ;

      BOOST_CHECK_EQUAL (c.getType(), Type::Real) ;
      BOOST_CHECK_EQUAL (c, 1.0) ;

      Value d;
      d = std::move(c);

      BOOST_CHECK (c.isNull()) ;
      BOOST_CHECK_EQUAL (d.getType(), Type::Real) ;
      BOOST_CHECK_EQUAL (d, 1.0) ;


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

       BOOST_CHECK (realVal != std::string("foo"));
       BOOST_CHECK (!(realVal == Blob{}));

       int64_t i = 1;
       BOOST_CHECK (realVal == i);
       BOOST_CHECK (Value{2.0} != i);
     }


     void
     implicitConvert ()
     {
       Value a(100.0);

       BOOST_CHECK_NO_THROW({ int x = a  ; (void)x; })
       BOOST_CHECK_NO_THROW({ int64_t x = a  ; (void)x; })
       BOOST_CHECK_NO_THROW({ double x = a  ; (void)x; })
       BOOST_CHECK_THROW({ std::string x = a  ; (void)x; }, ErrTypeMisMatch);
       BOOST_CHECK_THROW({ Blob x = a  ; (void)x; }, ErrTypeMisMatch);

       {
         int x = a ;
         BOOST_CHECK_EQUAL(x , 100) ;
       }
       {
         int64_t x = a ;
         BOOST_CHECK_EQUAL(x , 100) ;
       }
       {
         double x = a ;
         BOOST_CHECK_EQUAL(x , 100.0) ;
       }


       Value b(100.001);
       BOOST_CHECK_THROW ({int x = b; (void)x;}, ErrTypeMisMatch);
       BOOST_CHECK_THROW({int64_t x = b; (void)x;}, ErrTypeMisMatch);


       BOOST_CHECK_THROW({double x = Value{}; (void)x;}, ErrNullValueAccess);
       BOOST_CHECK_THROW({double x = Value{"foo"}; (void)x;}, ErrTypeMisMatch);

       {
           int64_t tobig = std::numeric_limits<int64_t>::min ();
           BOOST_CHECK_THROW ({double x = Value{tobig}; (void)x;},
                              ErrOutOfRange);
         }



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
          .addTest ("implicitConvert", implicitConvert)
          .addTest ("getValue", getValue)
    );
  }
}

