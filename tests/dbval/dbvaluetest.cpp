#include <sl3/dbvalue.hpp>

#include <a4testing.hpp>

#include <sl3/error.hpp>
#include <sstream>
#include <string>
#include <cmath>

namespace sl3
{
  namespace
  {
    struct check
    {
      DbValue& _val;

      check (DbValue& val)
      : _val (val)
      {
      }

      void
      throwsMissMatchGet (const Types& types)
      {
        for (auto type : types)
          {
            if (type == Type::Int)
              {
                BOOST_CHECK_THROW (_val.getInt (), ErrTypeMisMatch);
              }
            else if (type == Type::Text)
              {
                BOOST_CHECK_THROW (_val.getText (), ErrTypeMisMatch);
              }
            else if (type == Type::Real)
              {
                BOOST_CHECK_THROW (_val.getReal (), ErrTypeMisMatch);
              }
            else if (type == Type::Blob)
              {
                BOOST_CHECK_THROW (_val.getBlob (), ErrTypeMisMatch);
              }
          }
      }

      void
      throwsNullAccess ()
      {
        BOOST_CHECK (_val.isNull ());
        BOOST_CHECK_THROW (_val.getInt (), ErrNullValueAccess);
        BOOST_CHECK_THROW (_val.getText (), ErrNullValueAccess);
        BOOST_CHECK_THROW (_val.getReal (), ErrNullValueAccess);
        BOOST_CHECK_THROW (_val.getBlob (), ErrNullValueAccess);
      }

      void
      typeAndStorage (Type type, Type storage)
      {
        BOOST_CHECK (_val.getType () == type);
        BOOST_CHECK (_val.getStorageType () == storage);
      }

      void
      throwsSetTypes (const Types& types)
      {
        for (auto type : types)
          {
            if (type == Type::Int)
              {
                BOOST_CHECK_THROW (_val.set (1), ErrTypeMisMatch);
                BOOST_CHECK_THROW (_val.set ((int64_t)1), ErrTypeMisMatch);
                BOOST_CHECK_THROW (_val = 1, ErrTypeMisMatch);
                BOOST_CHECK_THROW (_val = (int64_t)1, ErrTypeMisMatch);
              }
            else if (type == Type::Text)
              {
                BOOST_CHECK_THROW (_val.set ("foo"), ErrTypeMisMatch);
                BOOST_CHECK_THROW (_val = "bar", ErrTypeMisMatch);
              }
            else if (type == Type::Real)
              {
                BOOST_CHECK_THROW (_val.set (1.1), ErrTypeMisMatch);
                BOOST_CHECK_THROW (_val = 1.1, ErrTypeMisMatch);
              }
            else if (type == Type::Blob)
              {
                BOOST_CHECK_THROW (_val.set (Blob ()), ErrTypeMisMatch);
                BOOST_CHECK_THROW (_val = Blob (), ErrTypeMisMatch);
              }
          }
      }
    };

    void
    Creation ()
    {
      {
        DbValue dbval (Type::Variant);
        check (dbval).typeAndStorage (Type::Variant, Type::Null);
        check (dbval).throwsNullAccess ();
      }

      {
        DbValue dbval (Type::Null);
        check (dbval).typeAndStorage (Type::Variant, Type::Null);
        check (dbval).throwsNullAccess ();
      }

      {
        DbValue dbval (Type::Int);
        check (dbval).typeAndStorage (Type::Int, Type::Null);
        check (dbval).throwsNullAccess ();
        check (dbval).throwsSetTypes ({Type::Text, Type::Real, Type::Blob});
        BOOST_CHECK_NO_THROW (dbval = 12);
        check (dbval).throwsMissMatchGet (
            {Type::Text, Type::Real, Type::Blob});
        BOOST_CHECK (dbval.getInt () == 12);
      }

      {
        DbValue dbval (Type::Text);
        check (dbval).typeAndStorage (Type::Text, Type::Null);
        check (dbval).throwsNullAccess ();
        check (dbval).throwsSetTypes ({Type::Int, Type::Real, Type::Blob});
        BOOST_CHECK_NO_THROW (dbval = "some text");
        check (dbval).throwsMissMatchGet ({Type::Int, Type::Real, Type::Blob});
        BOOST_CHECK (dbval.getText () == "some text");
      }

      {
        DbValue dbval (Type::Real);
        check (dbval).typeAndStorage (Type::Real, Type::Null);
        check (dbval).throwsNullAccess ();
        check (dbval).throwsSetTypes ({// in works
                                       Type::Text,
                                       Type::Blob});
        BOOST_CHECK_NO_THROW (dbval = 1); // tets int assign to real
        BOOST_CHECK_NO_THROW (dbval = 2.1);
        check (dbval).throwsMissMatchGet ({Type::Int, Type::Text, Type::Blob});
        BOOST_CHECK (dbval.getReal () == 2.1);
      }

      {
        DbValue dbval (Type::Blob);
        check (dbval).typeAndStorage (Type::Blob, Type::Null);
        check (dbval).throwsNullAccess ();
        check (dbval).throwsSetTypes ({Type::Int, Type::Text, Type::Real});

        Blob b{1, 2, 3};
        BOOST_CHECK_NO_THROW (dbval = b);
        check (dbval).throwsMissMatchGet ({Type::Int, Type::Text, Type::Real});
        BOOST_CHECK (dbval.getBlob () == b);
      }

      //  BOOST_MESSAGE("done") ;
    } //-----------------------------------------------------------------------------

    void
    CreateVariant ()
    {
      DbValue dbval{Type::Variant};

      check (dbval).typeAndStorage (Type::Variant, Type::Null);
      check (dbval).throwsNullAccess ();

      {
        int val = 12;
        BOOST_CHECK_NO_THROW (dbval.set (val));
        BOOST_CHECK (dbval.getInt () == val);

        check (dbval).throwsMissMatchGet (
            {Type::Text, Type::Real, Type::Blob});

        check (dbval).typeAndStorage (Type::Variant, Type::Int);
      }
      {
        double val = 0.123;
        BOOST_CHECK_NO_THROW (dbval.set (val));
        BOOST_CHECK (dbval.getReal () == val);

        check (dbval).throwsMissMatchGet ({Type::Int, Type::Text, Type::Blob});

        check (dbval).typeAndStorage (Type::Variant, Type::Real);
      }
      {
        std::string val = "qwert";
        BOOST_CHECK_NO_THROW (dbval.set (val));
        BOOST_CHECK (dbval.getText () == val);
        check (dbval).throwsMissMatchGet ({Type::Int, Type::Real, Type::Blob});

        check (dbval).typeAndStorage (Type::Variant, Type::Text);
      }
      {
        Blob val = {2, 3, 4, 5, 6, 7};
        BOOST_CHECK_NO_THROW (dbval.set (val));
        BOOST_CHECK (dbval.getBlob () == val);
        check (dbval).throwsMissMatchGet ({Type::Int, Type::Real, Type::Text});

        check (dbval).typeAndStorage (Type::Variant, Type::Blob);
      }

    } //-----------------------------------------------------------------------------

    void
    CreateVariantWithValue ()
    {
      {
        DbValue dbval (123, Type::Variant);
        BOOST_CHECK (dbval.getType () == Type::Variant
                     && dbval.getStorageType () == Type::Int);
      }
      {
        DbValue dbval (1.23, Type::Variant);
        BOOST_CHECK (dbval.getType () == Type::Variant
                     && dbval.getStorageType () == Type::Real);
      }
      {
        DbValue dbval ("asdf", Type::Variant);
        BOOST_CHECK (dbval.getType () == Type::Variant
                     && dbval.getStorageType () == Type::Text);
      }
      {
        DbValue dbval (Blob ({1, 2, 3, 4}), Type::Variant);
        BOOST_CHECK (dbval.getType () == Type::Variant
                     && dbval.getStorageType () == Type::Blob);
      }

    } //-----------------------------------------------------------------------------

    void
    DontCreateWrongTypes ()
    {
      {
        BOOST_CHECK_THROW (DbValue dbval (Blob ({1, 2, 3, 4}), Type::Text),
                           ErrTypeMisMatch);
      }
      {
        BOOST_CHECK_THROW (DbValue dbval (1, Type::Text), ErrTypeMisMatch);
      }
      {
        BOOST_CHECK_THROW (DbValue dbval (1.1, Type::Text), ErrTypeMisMatch);
      }
      {
        BOOST_CHECK_THROW (DbValue dbval ("1", Type::Int), ErrTypeMisMatch);
      }
    }

    void
    CopyAndMove ()
    {
      {
        DbValue a (100);
        DbValue b = a;
        DbValue c (b);
        DbValue d (0);
        d = a;

        BOOST_CHECK (a.getInt () == b.getInt ());
        BOOST_CHECK (a.getInt () == c.getInt ());
        BOOST_CHECK (a.getInt () == d.getInt ());
      }

      {
        DbValue a ("move");
        DbValue b (std::move (a));

        BOOST_CHECK (a.isNull ());
        BOOST_CHECK (b.getText () == "move");
        BOOST_CHECK (b.getStorageType () == Type::Text);

        DbValue c = std::move (b);

        BOOST_CHECK (b.isNull ());
        BOOST_CHECK (c.getText () == "move");
        BOOST_CHECK (c.getStorageType () == Type::Text);

        DbValue d (std::string ("asd"));
        // DbValue d = std::string("asd");
        d = std::move (c);
        BOOST_CHECK (c.isNull ());
        BOOST_CHECK (d.getText () == "move");
        BOOST_CHECK (d.getStorageType () == Type::Text);
      }

      {
        DbValue a ("string");
        DbValue b (1);
        DbValue c (1.11);

        DbValue d{Type::Null};
        DbValue e{Type::Null};

        BOOST_CHECK_THROW (a = b, ErrTypeMisMatch);
        BOOST_CHECK_THROW (c = std::move (a), ErrTypeMisMatch);

        BOOST_CHECK_NO_THROW (d = a);
        BOOST_CHECK_THROW (b = d, ErrTypeMisMatch);
        BOOST_CHECK_THROW (b = std::move (d), ErrTypeMisMatch);
        e.set (2.2);
        BOOST_CHECK_NO_THROW (c = std::move (e));
      }
    } //-----------------------------------------------------------------------------

    void
    Compare ()
    {
      {
        auto intval = DbValue{1};
        BOOST_CHECK (intval.getInt() == 1);
        int64_t v64 = 1;
        BOOST_CHECK (intval.getInt() == v64);
        BOOST_CHECK (intval == DbValue(v64));      
        DbValue variant {v64, Type::Variant};
        BOOST_CHECK ((intval == variant) == false) ;
        v64 -= 1;
        BOOST_CHECK (intval.getInt() != v64);
        BOOST_CHECK (intval != DbValue(v64));

        BOOST_CHECK (intval != DbValue(Blob{}));
      }

      {
        double d1 = 0.2;
        double d2 = 1 / std::sqrt(5) / std::sqrt(5);
        BOOST_CHECK (d1 != d2); 
        auto realval = DbValue{d1};
        BOOST_CHECK (realval.getReal() != d2);
        BOOST_CHECK (realval == DbValue(d2));

        BOOST_CHECK (DbValue (0.20000002) != DbValue (0.20000001));
        BOOST_CHECK (realval != DbValue(std::string{"bar"}));
        BOOST_CHECK (realval != DbValue(Blob{}));
      }
    
      {
        DbValue a{1, Type::Variant} ;
        DbValue b{1.0, Type::Variant} ;
        BOOST_CHECK (a != b) ;
        BOOST_CHECK (weak_eq(a, b)) ;

        BOOST_CHECK (b != a) ;
        BOOST_CHECK (weak_eq(b, a)) ;

        DbValue blob{Blob {1,2}} ;

        BOOST_CHECK (a != blob) ;
        BOOST_CHECK (b != blob) ;
        BOOST_CHECK (blob != a) ;
        BOOST_CHECK (blob != a) ;
      }



      {
        auto txtval = DbValue{"foobar"};
        BOOST_CHECK (txtval.getText() == std::string{"foobar"});
        BOOST_CHECK (txtval == DbValue("foobar"));
        BOOST_CHECK (txtval != DbValue("bar"));

      }

      {
        auto blobval = DbValue{Blob{1, 2, 3}};
        std::vector<char> v       = {1, 2, 3};
        BOOST_CHECK (blobval.getBlob() == v);
        BOOST_CHECK (blobval == DbValue(v));
        BOOST_CHECK (blobval != DbValue(Blob{1, 2, 3, 4}) );

      }


      
      BOOST_CHECK (DbValue (Type::Variant) == DbValue (Type::Variant));

      BOOST_CHECK (DbValue (Type::Blob) != DbValue (Type::Variant));
      // Note, if this is true, than one has to be less as the other ....

    } //-----------------------------------------------------------------------------


    void
    lessCompare ()
    {
      BOOST_CHECK ( !(DbValue (Type::Int) < DbValue (Type::Int)));

      BOOST_CHECK ( (DbValue (Type::Text) < DbValue (1)));
      BOOST_CHECK ( !(DbValue (1) < DbValue (Type::Blob)));

      BOOST_CHECK ( (DbValue (1) < DbValue (1.1)));
      
      BOOST_CHECK ( !(DbValue (1) < DbValue (1)));
      BOOST_CHECK ( (DbValue (1) < DbValue ("foo")));

      BOOST_CHECK ( (DbValue (1) < DbValue (2)));
      BOOST_CHECK ( (DbValue (1, Type::Variant) < DbValue (2)));
      BOOST_CHECK ( (DbValue (1) < DbValue (2, Type::Variant)));
      BOOST_CHECK ( (DbValue (2) < DbValue (2, Type::Variant)));
      BOOST_CHECK ( !(DbValue (1, Type::Variant) < DbValue (1)));
      BOOST_CHECK ( !(DbValue (1, Type::Variant) == DbValue (1)));

      BOOST_CHECK (DbValue (Type::Blob) < DbValue (Type::Variant));
      BOOST_CHECK (DbValue (Type::Variant) < DbValue (1)); 
      BOOST_CHECK (DbValue (Type::Text) < DbValue (Type::Blob));


      // these have different types so that needs to be like that
      BOOST_CHECK ( (DbValue (1) < DbValue (1.0)));
      BOOST_CHECK ( !(DbValue (2) < DbValue (1.0)));
      BOOST_CHECK ( !(DbValue (1) == DbValue (1.0)) );
      BOOST_CHECK ( !(DbValue (1.0) < DbValue (1)));
      // same values with different type but variant
      BOOST_CHECK ( (DbValue (1, Type::Variant) < DbValue (1.0, Type::Variant)));
      BOOST_CHECK ( weak_eq(DbValue (1, Type::Variant), DbValue (1.0, Type::Variant)));
      BOOST_CHECK ( !(DbValue (1, Type::Variant) == DbValue (1.0, Type::Variant)) );
      BOOST_CHECK ( !(DbValue (1.0, Type::Variant) < DbValue (1, Type::Variant)));
      
      BOOST_CHECK ( (DbValue (1.0) < DbValue (1.1)));
      BOOST_CHECK ( (DbValue (1.0) < DbValue (1.1, Type::Variant)));
      BOOST_CHECK ( (DbValue (1.0, Type::Variant) < DbValue (1.1)));
      BOOST_CHECK ( (DbValue (1.0, Type::Variant) < DbValue (1.1, Type::Variant)));

      BOOST_CHECK ( (DbValue ("aa") < DbValue ("ab")));
      BOOST_CHECK ( !(DbValue ("aa") == DbValue ("ab")) );
      BOOST_CHECK ( !(DbValue ("ab") < DbValue ("aa")) );
      BOOST_CHECK ( !(DbValue ("ab") < DbValue (1)) );
      // same values with same type, variant, need to be bigger
      BOOST_CHECK ( !(DbValue ("aa", Type::Variant) < DbValue ("aa")));
      BOOST_CHECK ( !(DbValue ("aa", Type::Variant) == DbValue ("aa")));
      BOOST_CHECK ( (DbValue ("aa") < DbValue ("aa", Type::Variant)));


      Blob a{1,2} ;
      Blob b{3,4} ;

      BOOST_CHECK (DbValue{a} < DbValue{b});
      BOOST_CHECK (!(DbValue{b} < DbValue{a}));
     
// equal 
      BOOST_CHECK (!(DbValue{a} < DbValue{a}));
      BOOST_CHECK (!(DbValue(a, Type::Variant) < DbValue{a}));
      BOOST_CHECK ((DbValue{a} < DbValue(a, Type::Variant)));
      BOOST_CHECK (!(DbValue{a, Type::Variant} < DbValue{a, Type::Variant}));

      BOOST_CHECK (DbValue{1} < DbValue{b});
      
      DbValue ivariant {1, Type::Variant} ;
      DbValue bvariant {a, Type::Variant } ;

      BOOST_CHECK ( ivariant < DbValue{b});
      BOOST_CHECK ( ivariant < bvariant );
      BOOST_CHECK ( DbValue{1} < DbValue{b});
      BOOST_CHECK ( DbValue{1} < bvariant );



      BOOST_CHECK (  bvariant < (DbValue{b, Type::Variant}));

      BOOST_CHECK (!(DbValue{a, Type::Variant} < DbValue{1, Type::Variant}));

      BOOST_CHECK (!(DbValue{a} < DbValue{1, Type::Variant}));

      // BOOST_CHECK (DbValue (Type::Int) < DbValue (Type::Variant));
      // TODO is is not possible that
      // a < b == false and  a != b == false and a > b == false
      // if the type is like that, than do not use the standard <==> operators
      
    }


    
    void
    Eject ()
    {
      {
        std::string s;
        std::string initval{"foobar"};
        auto        dbval = DbValue{initval};
//        BOOST_CHECK (static_cast<std::string>(dbval.value()) == initval);
        BOOST_CHECK_THROW (dbval.ejectBlob (), ErrTypeMisMatch);
        BOOST_CHECK_NO_THROW (s = dbval.ejectText ());

        BOOST_CHECK (dbval.isNull ());
        BOOST_CHECK (s == initval);
        BOOST_CHECK_THROW (dbval.ejectText (), ErrNullValueAccess);
      }

      {
        Blob b;
        Blob initval{1, 2, 3};
        auto dbval = DbValue{initval};
//        BOOST_CHECK (static_cast<Blob>(dbval) == initval);
        BOOST_CHECK_THROW (dbval.ejectText (), ErrTypeMisMatch);
        BOOST_CHECK_NO_THROW (b = dbval.ejectBlob ());

        BOOST_CHECK (dbval.isNull ());
        BOOST_CHECK (b == initval);
        BOOST_CHECK_THROW (dbval.ejectBlob (), ErrNullValueAccess);
      }
    }

    void
    Assign ()
    {
      {
        auto intval = DbValue{1};
        auto strval = DbValue{"foo"};
        BOOST_CHECK_THROW (intval = strval, ErrTypeMisMatch);

        auto oterintval = DbValue{2};

        BOOST_CHECK_NO_THROW (oterintval = intval);

        BOOST_CHECK (oterintval.getInt () == intval.getInt ());

        DbValue dvb (Blob{1, 2, 3}, Type::Variant);
        dvb = Blob{};
        BOOST_CHECK (dvb.getBlob () == Blob ());

        dvb = int64_t (123);
        BOOST_CHECK (dvb.getInt () == 123);

        dvb = DbValue (Type::Variant);
        BOOST_CHECK (dvb.isNull ());

        dvb = DbValue (Blob{1, 2});
        BOOST_CHECK (dvb.getBlob () == Blob ({1, 2}));
      }
    }

    void
    getWithDefault ()
    {
      auto intval  = DbValue{1};
      auto dblval  = DbValue{1.1};
      auto strval  = DbValue{"foo"};
      auto nullval = DbValue{Type::Null};

      auto blob    = Blob{1, 2, 3};
      auto blobval = DbValue{blob};

      BOOST_CHECK (intval.get ("") == "");
      BOOST_CHECK (intval.get (0) == 1);
      BOOST_CHECK (dblval.get (0) == 0);
      BOOST_CHECK (dblval.get (0.0) == 1.1);

      BOOST_CHECK (strval.get (0) == 0);
      BOOST_CHECK (strval.get ("") == "foo");

      BOOST_CHECK (nullval.get ("") == "");
      BOOST_CHECK (nullval.get (0) == 0);

      BOOST_CHECK (blobval.get (0.0) == 0);
      BOOST_CHECK (blobval.get (Blob{}) == blob);

      BOOST_CHECK (blobval.get ((int64_t)0) == 0);
      BOOST_CHECK (intval.get ((int64_t)0) == 1);
      BOOST_CHECK (dblval.get (blob) == blob);
    }

    void
    toStream ()
    {
      auto intval  = DbValue{1};
      auto dblval  = DbValue{1.1};
      auto strval  = DbValue{"foo"};
      auto nullval = DbValue{Type::Null};
      auto blobval = DbValue{Blob{1, 2, 3}};

      std::ostringstream stm;

      stm << intval;
      BOOST_CHECK (stm.str () == "1");
      stm.str (std::string ());

      stm << dblval;
      BOOST_CHECK (stm.str ().substr (0, 3) == "1.1");
      stm.str (std::string ());

      stm << strval;
      BOOST_CHECK (stm.str () == "foo");
      stm.str (std::string ());

      stm << nullval;
      BOOST_CHECK (stm.str () == "<NULL>");
      stm.str (std::string ());

      stm << blobval;
      BOOST_CHECK (stm.str () == "<BLOB>");
      stm.str (std::string ());
    }

    void
    getDefault ()
    {
      {
        DbValue val{Type::Int};
        BOOST_CHECK (val.isNull ());
        BOOST_CHECK_EQUAL (val.getInt (1), 1);
        val = 2;
        BOOST_CHECK_EQUAL (val.getInt (1), 2);

        BOOST_CHECK_THROW (val.getText ("a"), ErrTypeMisMatch);
      }

      {
        DbValue val{Type::Real};
        BOOST_CHECK (val.isNull ());
        BOOST_CHECK_EQUAL (val.getReal (1.1), 1.1);
        val = 2.2;
        BOOST_CHECK_EQUAL (val.getReal (1.1), 2.2);

        BOOST_CHECK_THROW (val.getBlob (Blob ()), ErrTypeMisMatch);
      }

      {
        DbValue val{Type::Text};
        BOOST_CHECK (val.isNull ());
        BOOST_CHECK_EQUAL (val.getText ("foo"), "foo");
        val = "bar";
        BOOST_CHECK_EQUAL (val.getText ("foo"), "bar");

        BOOST_CHECK_THROW (val.getInt (1), ErrTypeMisMatch);
      }

      {
        DbValue val{Type::Blob};
        BOOST_CHECK (val.isNull ());
        Blob blob{1, 2};
        BOOST_CHECK (val.getBlob (blob) == blob);
        Blob blob2 = {1, 2, 3};
        val        = blob2;
        BOOST_CHECK (val.getBlob (blob) == blob2);

        BOOST_CHECK_THROW (val.getReal (1.1), ErrTypeMisMatch);
      }

      {
        DbValue val{Type::Variant};
        BOOST_CHECK (val.isNull ());
        BOOST_CHECK_EQUAL (val.getInt (1), 1);
        val = 2;
        BOOST_CHECK_EQUAL (val.getInt (1), 2);

        BOOST_CHECK_THROW (val.getText ("a"), ErrTypeMisMatch);
      }
    }

    void
    variantLikeGet ()
    {
      {
        int     v = 123;
        DbValue val{v};
        BOOST_REQUIRE_NO_THROW ((void)get<int64_t> (val));
        BOOST_CHECK (v == get<int64_t> (val));

        // may not compile,  ..
        // sl3::get<char>(val) ;
      }

      // TODO tolerance not in boost 1.54, Slackware 14.2
      //      {
      //        double  v = 123.0;
      //        DbValue val{v};
      //        BOOST_REQUIRE_NO_THROW ((void)get<double> (val));
      //        BOOST_TEST (v == get<double> (val),
      //                    boost::test_tools::tolerance (0.001));
      //      }

      {
        std::string v = "123";
        DbValue     val{v};
        BOOST_REQUIRE_NO_THROW ((void)get<std::string> (val));
        BOOST_CHECK (v == get<std::string> (val));
      }

      {
        Blob    v{1, 2, 3};
        DbValue val{v};
        BOOST_REQUIRE_NO_THROW ((void)get<Blob> (val));
        BOOST_CHECK (v == get<Blob> (val));
      }

      {
        double  v = 123.0;
        DbValue val{v};
        BOOST_CHECK_THROW (get<int64_t> (val), ErrTypeMisMatch);
      }
    }

    // ./sl3basics --on=dbvalue --log_level=all --run_test=whatever
    // --on=DbValGeneral --run_test=DbValGeneral/compare  --log_level=all

    a4TestAdd (a4test::suite ("dbval")
                   .addTest ("creation", Creation)
                   .addTest ("variant", CreateVariant)
                   .addTest ("variantvalue", CreateVariantWithValue)
                   .addTest ("wrongtypes", DontCreateWrongTypes)
                   .addTest ("copymove", CopyAndMove)
                   .addTest ("compare", Compare)
                   .addTest ("lesscompare", lessCompare)
                   .addTest ("eject", Eject)
                   .addTest ("assign", Assign)
                   .addTest ("getWithDefault", getWithDefault)
                   .addTest ("tostream", toStream)
                   .addTest ("default", getDefault)
                   .addTest ("variantLikeGet", variantLikeGet)

                   );
  }
}

