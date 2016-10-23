
#include <a4testing.hpp>

#include <sl3/value.hpp>
#include <sl3/error.hpp>
#include "eqorder.hpp"

#include <functional>
#include <climits>


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

      Value a(100);
      Value b(a);
      Value c = a ;
      Value d = 100 ;

      BOOST_CHECK_EQUAL (a, 100) ;
      BOOST_CHECK_EQUAL (a.getType(), Type::Int) ;
      BOOST_CHECK_EQUAL (b, 100) ;
      BOOST_CHECK_EQUAL (b.getType(), Type::Int) ;
      BOOST_CHECK_EQUAL (c, 100) ;
      BOOST_CHECK_EQUAL (c.getType(), Type::Int) ;
      BOOST_CHECK_EQUAL (d, 100) ;
      BOOST_CHECK_EQUAL (d.getType(), Type::Int) ;

      Value e;
      BOOST_CHECK (e.isNull()) ;
      BOOST_CHECK_EQUAL (e.getType(), Type::Null) ;
      e = 100 ;
      BOOST_CHECK_EQUAL (e, 100) ;
      BOOST_CHECK_EQUAL (e.getType(), Type::Int) ;
      BOOST_CHECK (!e.isNull()) ;


      int64_t i64 = 100 ;
      Value f{i64};
      BOOST_CHECK_EQUAL (f, i64) ;
      BOOST_CHECK_EQUAL (f.getType(), Type::Int) ;
      BOOST_CHECK (!f.isNull()) ;

    }


    void
    assing ()
    {

      Value a;
      BOOST_CHECK (a.isNull()) ;
      BOOST_CHECK_EQUAL (a.getType(), Type::Null) ;
      int64_t intval = 100 ;
      a = intval ;
      BOOST_CHECK_EQUAL (a , intval) ;
      BOOST_CHECK_EQUAL (a.getType(), Type::Int) ;
      BOOST_CHECK (!a.isNull()) ;

      Value b;
      b = a ;
      BOOST_CHECK_EQUAL (b , intval) ;
      BOOST_CHECK_EQUAL (b , a) ;

      b = Value{} ;
      BOOST_CHECK_EQUAL (b.getType(), Type::Null) ;
      a.setNull() ;
      BOOST_CHECK_EQUAL (b , a) ;

    }

    void
    move()
    {
      Value a{1};
      BOOST_CHECK (!a.isNull()) ;
      BOOST_CHECK_EQUAL (a.getType(), Type::Int) ;

      Value b{std::move(a)};
      BOOST_CHECK (a.isNull()) ;
      BOOST_CHECK_EQUAL (a.getType(), Type::Null) ;

      BOOST_CHECK_EQUAL (b.getType(), Type::Int) ;
      BOOST_CHECK_EQUAL (b, 1) ;

      Value c = std::move(b);
      BOOST_CHECK (b.isNull()) ;

      BOOST_CHECK_EQUAL (c.getType(), Type::Int) ;
      BOOST_CHECK_EQUAL (c, 1) ;

      Value d;
      d = std::move(c);

      BOOST_CHECK (c.isNull()) ;
      BOOST_CHECK_EQUAL (d.getType(), Type::Int) ;
      BOOST_CHECK_EQUAL (d, 1) ;


    }

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

       BOOST_CHECK (strong_eq(Value{1}, Value{1}));
       BOOST_CHECK (!strong_eq(Value{1}, Value{1.0}));

       Value a(100), b(100), c(100) ;
       Value d(100), e(200), f(300) ;

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

       Value a(100), b(100), c(100) ;
       Value d{1}, e(200), f(300) ;

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

       BOOST_CHECK (intVal != nullVal);
       BOOST_CHECK (intVal > nullVal);

       BOOST_CHECK (!(intVal < nullVal));

       BOOST_CHECK (intVal != realVal);
       BOOST_CHECK (intVal < realVal);

       BOOST_CHECK (!(Value{2} < Value{1.1}));
       BOOST_CHECK (!(Value{3} < Value{2}));
       BOOST_CHECK (!(Value{2} < Value{1.0}));

       BOOST_CHECK (intVal != textVal);
       BOOST_CHECK (intVal < textVal);

       BOOST_CHECK (intVal != blobVal);
       BOOST_CHECK (intVal < blobVal);


       BOOST_CHECK (intVal != std::string("foo"));
       BOOST_CHECK (!(intVal == Blob{}));

     }


     void
     implicitConvert ()
     {
       Value a(100);

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

       BOOST_CHECK_THROW({ int x = Value{}; (void)x; }, ErrNullValueAccess);
       BOOST_CHECK_THROW({ int64_t x = Value{}; (void)x; }, ErrNullValueAccess);

       BOOST_CHECK_THROW ({int x = Value{"x"}; (void)x;}, ErrTypeMisMatch);
       BOOST_CHECK_THROW({int x = Value{Blob{}}; (void)x;}, ErrTypeMisMatch);

       BOOST_CHECK_THROW({int64_t x = Value{"x"}; (void)x;}, ErrTypeMisMatch);
       BOOST_CHECK_THROW({int64_t x = Value{Blob{}}; (void)x;}, ErrTypeMisMatch);


       BOOST_CHECK_THROW ({int x = Value{"x"}; (void)x;}, ErrTypeMisMatch);


       {
         int64_t tobig = std::numeric_limits<int64_t>::max ();
         BOOST_CHECK_THROW ({int x = Value{tobig}; (void)x;}, ErrOutOfRange);
       }

       {
         int64_t toless = std::numeric_limits<int64_t>::min ();
         BOOST_CHECK_THROW ({int x = Value{toless}; (void)x;}, ErrOutOfRange);
       }

       {
         double tobig = std::numeric_limits<double>::max ();
         BOOST_CHECK_THROW ({int x = Value{tobig}; (void)x;}, ErrOutOfRange);
       }

       {
         double tobig = std::numeric_limits<double>::max ();
         BOOST_CHECK_THROW ({int64_t x = Value{tobig}; (void)x;},
                            ErrOutOfRange);
       }

     }

     void
     getValue ()
     {
       BOOST_CHECK_THROW (Value{}.int64(); , ErrNullValueAccess);
       BOOST_CHECK_THROW (Value{"x"}.int64(); , ErrTypeMisMatch);
       BOOST_CHECK_THROW (Value{1.0}.int64(); , ErrTypeMisMatch);
       BOOST_CHECK_NO_THROW (Value{0}.int64(); );
     }


    a4TestAdd (a4test::suite ("intval")
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

