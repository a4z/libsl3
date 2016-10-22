
#include <a4testing.hpp>

#include <sl3/value.hpp>
#include <sl3/error.hpp>
#include "eqorder.hpp"
#include <functional>

#include <ostream>

  std::ostream& operator<< (std::ostream& stm, const sl3::Blob& blob)
  {
    for ( auto b: blob)
      stm << b ;
    return stm ;
  }

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
      Value a(Blob({0,0,0}));
      Value b(a);
      Value c = a ;
      Value d = Blob({0,0,0}) ;

      BOOST_CHECK (a == Blob({0,0,0})) ;
      BOOST_CHECK  (a.getType()== Type::Blob) ;
      BOOST_CHECK  (b== Blob({0,0,0})) ;
      BOOST_CHECK  (b.getType()== Type::Blob) ;
      BOOST_CHECK  (c== Blob({0,0,0})) ;
      BOOST_CHECK  (c.getType()== Type::Blob) ;
      BOOST_CHECK  (d== Blob({0,0,0})) ;
      BOOST_CHECK  (d.getType()== Type::Blob) ;

      Value e;
      BOOST_CHECK (e.isNull()) ;
      BOOST_CHECK_EQUAL (e.getType(), Type::Null) ;
      e = Blob({0,0,0}) ;
      BOOST_CHECK (e == Blob({0,0,0})) ;
      BOOST_CHECK_EQUAL (e.getType(), Type::Blob) ;
      BOOST_CHECK (!e.isNull()) ;

    }


    void
    assing ()
    {

      Value a;
      BOOST_CHECK (a.isNull()) ;
      BOOST_CHECK_EQUAL (a.getType(), Type::Null) ;
      Blob blob = {1,2,3};
      a = blob ;
      BOOST_CHECK (a == blob) ;
      BOOST_CHECK_EQUAL (a.getType(), Type::Blob) ;
      BOOST_CHECK (!a.isNull()) ;

      Value b;
      b = a ;
      BOOST_CHECK (b == blob) ;
      BOOST_CHECK (b == a) ;

    }

    void
    move()
    {
      Value a{Blob{0,1,0}};
      BOOST_CHECK (!a.isNull()) ;
      BOOST_CHECK_EQUAL (a.getType(), Type::Blob) ;

      Value b{std::move(a)};
      BOOST_CHECK (a.isNull()) ;
      BOOST_CHECK_EQUAL (a.getType(), Type::Null) ;

      BOOST_CHECK_EQUAL (b.getType(), Type::Blob) ;
      BOOST_CHECK (b == Blob({0,1,0})) ;

      Value c = std::move(b);
      BOOST_CHECK (b.isNull()) ;

      BOOST_CHECK_EQUAL (c.getType(), Type::Blob) ;
      BOOST_CHECK  (c == Blob({0,1,0})) ;

      Value d;
      d = std::move(c);

      BOOST_CHECK (c.isNull()) ;
      BOOST_CHECK_EQUAL (d.getType(), Type::Blob) ;
      BOOST_CHECK (d == Blob({0,1,0})) ;


    }

    void
     equality ()
     {
        Value a(Blob{0,1,0}), b(Blob{0,1,0}), c(Blob{0,1,0}) ;
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

       BOOST_CHECK (strong_eq(Value{Blob{0,1,0}}, Value{Blob{0,1,0}}));
       BOOST_CHECK (!strong_eq(Value{Blob{0,1,0}}, Value{Blob{1,1,1}}));

       Value a(Blob{0,1,0}), b(Blob{0,1,0}), c(Blob{0,1,0}) ;
       Value d(Blob{0}), e(Blob{0,0}), f(Blob{0,0,0}) ;

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
       // makes no sense with blob

       // but I use this for the eject test

       Blob blob = Blob{0,1};
       Value val (blob);

       BOOST_CHECK_THROW(val.ejectText(), ErrTypeMisMatch);

       auto blob2 =  val.ejectBlob() ;

       BOOST_CHECK (blob2  == blob );

       BOOST_CHECK (val.isNull() );

       BOOST_CHECK_THROW(val.ejectBlob(), ErrNullValueAccess);

     }



     void
     compareWithOthers ()
     {
       Value nullVal ;
       Value intVal (1) ;
       Value realVal (1.0) ;
       Value textVal ("a");
       Value blobVal (Blob{});

       BOOST_CHECK (blobVal != nullVal);
       BOOST_CHECK (blobVal > nullVal);

       BOOST_CHECK (blobVal != intVal);
       BOOST_CHECK (blobVal > intVal);

       BOOST_CHECK (blobVal != realVal);
       BOOST_CHECK (blobVal > realVal);

       BOOST_CHECK (blobVal != textVal);
       BOOST_CHECK (blobVal > textVal);


     }


    a4TestAdd (a4test::suite ("blobval")
          .addTest ("create", create)
          .addTest ("assing", assing)
          .addTest ("move", move)
          .addTest ("equality", equality)
          .addTest ("strictTotalOrdered", strictTotalOrdered)
          .addTest ("weakTotalOrdered", weakTotalOrdered)
          .addTest ("compareWithOthers", compareWithOthers)

                   );
  }
}

