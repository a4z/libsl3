#include "../testing.hpp"
#include <sl3/dbvalue.hpp>

#include <algorithm>
#include <iterator>
#include <sstream>

struct check
{
  sl3::DbValue& _val;

  check (sl3::DbValue& val)
  : _val (val)
  {
  }

  void
  throwsMissMatchGet (const sl3::Types& types)
  {
    for (auto type : types)
      {
        if (type == sl3::Type::Int)
          {
            CHECK_THROWS_AS (_val.getInt (), sl3::ErrTypeMisMatch);
          }
        else if (type == sl3::Type::Text)
          {
            CHECK_THROWS_AS (_val.getText (), sl3::ErrTypeMisMatch);
          }
        else if (type == sl3::Type::Real)
          {
            CHECK_THROWS_AS (_val.getReal (), sl3::ErrTypeMisMatch);
          }
        else if (type == sl3::Type::Blob)
          {
            CHECK_THROWS_AS (_val.getBlob (), sl3::ErrTypeMisMatch);
          }
      }
  }

  void
  throwsNullAccess ()
  {
    CHECK (_val.isNull ());
    CHECK_THROWS_AS (_val.getInt (), sl3::ErrNullValueAccess);
    CHECK_THROWS_AS (_val.getText (), sl3::ErrNullValueAccess);
    CHECK_THROWS_AS (_val.getReal (), sl3::ErrNullValueAccess);
    CHECK_THROWS_AS (_val.getBlob (), sl3::ErrNullValueAccess);
    CHECK_THROWS_AS (_val.ejectText (), sl3::ErrNullValueAccess);
    CHECK_THROWS_AS (_val.ejectBlob (), sl3::ErrNullValueAccess);
  }

  void
  getTypedDefaults ()
  {
    if (_val.type () != sl3::Type::Int)
      {
        CHECK_EQ (_val.getInt (100), 100);
        CHECK_EQ (_val.getInt (int64_t{100}), 100);
      }

    if (_val.type () != sl3::Type::Real)
      CHECK_EQ (_val.getReal (2.1), 2.1);

    if (_val.type () != sl3::Type::Text)
      CHECK_EQ (_val.getText ("foo"), "foo");

    if (_val.type () != sl3::Type::Blob)
      CHECK_EQ (_val.getBlob (sl3::Blob{'a'}), sl3::Blob{'a'});
  }

  void
  getTypedDefaultsThrows (const sl3::Types& types)
  {
    for (auto t : types)
      {
        if (t == sl3::Type::Int)
          {
            CHECK_THROWS_AS ((void)_val.getInt (100), sl3::ErrTypeMisMatch);
            CHECK_THROWS_AS ((void)_val.getInt (int64_t{1}),
                             sl3::ErrTypeMisMatch);
          }
        if (t == sl3::Type::Real)
          CHECK_THROWS_AS ((void)_val.getReal (2.1), sl3::ErrTypeMisMatch);

        if (t == sl3::Type::Text)
          CHECK_THROWS_AS ((void)_val.getText ("foo"), sl3::ErrTypeMisMatch);

        if (t == sl3::Type::Blob)
          CHECK_THROWS_AS ((void)_val.getBlob (sl3::Blob{'a'}),
                           sl3::ErrTypeMisMatch);
      }
  }

  void
  getDefaults ()
  {
    if (_val.type () != sl3::Type::Int)
      {
        CHECK_EQ (_val.get (100), 100);
        CHECK_EQ (_val.get (int64_t{100}), 100);
      }

    if (_val.type () != sl3::Type::Real)
      CHECK_EQ (_val.get (2.1), 2.1);

    if (_val.type () != sl3::Type::Text)
      CHECK_EQ (_val.get ("foo"), "foo");

    if (_val.type () != sl3::Type::Blob)
      CHECK_EQ (_val.get (sl3::Blob{'a'}), sl3::Blob{'a'});
  }

  void
  typeAndStorage (sl3::Type type, sl3::Type storage)
  {
    CHECK (_val.dbtype () == type);
    CHECK (_val.type () == storage);
  }

  void
  throwsSetTypes (const sl3::Types& types)
  {
    for (auto type : types)
      {
        if (type == sl3::Type::Int && _val.dbtype () != sl3::Type::Real)
          { // we convert int to real since this is no loss, no problem, but
            // TODO, is that really wanted
            // const int64_t one = 1;
            CHECK_THROWS_AS (_val.set (int{1}), sl3::ErrTypeMisMatch);
            CHECK_THROWS_AS (_val.set (int64_t{1}), sl3::ErrTypeMisMatch);
            CHECK_THROWS_AS (_val = int{1}, sl3::ErrTypeMisMatch);
            CHECK_THROWS_AS (_val = sl3::Value{1}, sl3::ErrTypeMisMatch);
            CHECK_THROWS_AS (_val = int64_t{1}, sl3::ErrTypeMisMatch);
          }
        else if (type == sl3::Type::Text)
          {
            CHECK_THROWS_AS (_val.set ("foo"), sl3::ErrTypeMisMatch);
            CHECK_THROWS_AS (_val = "bar", sl3::ErrTypeMisMatch);
            CHECK_THROWS_AS (_val = sl3::Value{"bz"}, sl3::ErrTypeMisMatch);
          }
        else if (type == sl3::Type::Real)
          {
            CHECK_THROWS_AS (_val.set (1.1), sl3::ErrTypeMisMatch);
            CHECK_THROWS_AS (_val = 1.1, sl3::ErrTypeMisMatch);
            CHECK_THROWS_AS (_val = sl3::Value{1.2}, sl3::ErrTypeMisMatch);
          }
        else if (type == sl3::Type::Blob)
          {
            CHECK_THROWS_AS (_val.set (sl3::Blob ()), sl3::ErrTypeMisMatch);
            CHECK_THROWS_AS (_val = sl3::Blob (), sl3::ErrTypeMisMatch);
            CHECK_THROWS_AS (_val = sl3::Value{sl3::Blob ()},
                             sl3::ErrTypeMisMatch);
          }
      }
  }
};

SCENARIO ("using value, basics")
{
  using namespace sl3;
  GIVEN ("list of types without value")
  {
    // test for types that construct to variant / null
    std::vector<DbValue> vals{
        DbValue{Type::Variant}, DbValue{Type::Null}, // useless but is variant
    };

    for (DbValue& dbval : vals)
      {
        WHEN ("using a value without value")
        {
          check (dbval).typeAndStorage (Type::Variant, Type::Null);

          THEN ("access any value property will throw")
          {
            check (dbval).throwsNullAccess ();
          }
          THEN ("access values with default ones return the default")
          {
            check (dbval).getTypedDefaults ();
            check (dbval).getDefaults ();
          }
        }
      }
  }

  GIVEN ("list of types")
  {
    Types types{{Type::Int, Type::Real, Type::Text, Type::Blob}};
    WHEN ("creating type tagged values of wrong type")
    {
      THEN ("this will throw type miss match")
      {
        CHECK_THROWS_AS (DbValue (1, types[1]), ErrTypeMisMatch);
        CHECK_THROWS_AS (DbValue (1.1, types[0]), ErrTypeMisMatch);
        CHECK_THROWS_AS (DbValue ("f", types[3]), ErrTypeMisMatch);
        CHECK_THROWS_AS (DbValue (Blob{}, types[2]), ErrTypeMisMatch);
      }
    }
  }

  GIVEN ("list of typed types with value")
  {
    Types types{{Type::Int, Type::Int, Type::Real, Type::Text, Type::Blob}};

    std::vector<DbValue> vals{
        DbValue{1, types[0]},
        DbValue{int64_t{1000}, types[1]},
        DbValue{2.1, types[2]},
        DbValue{"hi", types[3]},
        DbValue{Blob{{std::byte{'a'}, std::byte{'f'}}}, types[4]}};

    size_t sizeIdx = 0;

    for (DbValue& dbval : vals)
      {
        CHECK (types[sizeIdx] == dbval.dbtype ());
        sizeIdx += 1;

        WHEN ("extracting other types")
        {
          std::vector<Type> otherTypes;

          using namespace std;
          copy_if (begin (types),
                   end (types),
                   std::back_inserter (otherTypes),
                   [&dbval] (Type t) { return t != dbval.dbtype (); });

          THEN ("getting defaults of other types throws")
          {
            check (dbval).getTypedDefaultsThrows (otherTypes);
          }

          THEN ("getting any other types throws")
          {
            check (dbval).throwsSetTypes (otherTypes);
          }

          THEN ("setting any other types throws")
          {
            check (dbval).throwsSetTypes (otherTypes);
          }

          THEN ("access values with default ones return the default")
          {
            check (dbval).getDefaults ();
          }
        }

        WHEN ("copy a typed value to a variant one")
        {
          DbValue v{Type::Variant};
          THEN ("this always succeed")
          {
            v = dbval;
            CHECK (value_type_eq (v.value (), dbval.value ()));
          }
        }
      }
  }
}

SCENARIO ("variant")
{
  using namespace sl3;
  GIVEN ("an variant value")
  {
    DbValue dbval{Type::Variant};

    WHEN ("setting to any value")
    {
      THEN ("this works")
      {
        check (dbval).typeAndStorage (Type::Variant, Type::Null);
        dbval = 1;
        CHECK_EQ (dbval.getInt (), 1);
        CHECK_EQ (dbval.getInt (0), 1);
        CHECK_EQ (dbval.get (0), 1);
        dbval = int64_t{1000};
        CHECK_EQ (dbval.getInt (), 1000);
        CHECK_EQ (dbval.getInt (int64_t{0}), 1000);
        CHECK_EQ (dbval.get (int64_t{0}), 1000);
        check (dbval).typeAndStorage (Type::Variant, Type::Int);
        dbval = 2.1;
        CHECK_EQ (dbval.getReal (), 2.1);
        CHECK_EQ (dbval.getReal (1.0), 2.1);
        CHECK_EQ (dbval.get (1.0), 2.1);
        check (dbval).typeAndStorage (Type::Variant, Type::Real);
        dbval = "foba";
        CHECK_EQ (dbval.getText (), "foba");
        CHECK_EQ (dbval.getText (""), "foba");
        CHECK_EQ (dbval.get (""), "foba");
        check (dbval).typeAndStorage (Type::Variant, Type::Text);
        dbval = Blob{'f'};
        CHECK_EQ (dbval.getBlob (Blob{'a'}), Blob{'f'});
        CHECK_EQ (dbval.get (Blob{'a'}), Blob{'f'});
        CHECK_EQ (dbval.getBlob (), Blob{'f'});
        check (dbval).typeAndStorage (Type::Variant, Type::Blob);

        dbval = Value (1);
        check (dbval).typeAndStorage (Type::Variant, Type::Int);
        dbval = Value ("foo");
        check (dbval).typeAndStorage (Type::Variant, Type::Text);

        dbval.setNull ();
        check (dbval).typeAndStorage (Type::Variant, Type::Null);
      }
    }
  }
}

SCENARIO ("assign values")
{
  using namespace sl3;
  GIVEN ("a variant value")
  {
    DbValue dbval{Type::Variant};

    WHEN ("assingin any other typed value")
    {
      THEN ("this works")
      {
        CHECK_NOTHROW (dbval = DbValue{"foo"});
        CHECK_NOTHROW (dbval = DbValue{Type::Real});
      }
    }
  }

  GIVEN ("a typed value")
  {
    DbValue dbval{Type::Int};

    WHEN ("assign any other typed value")
    {
      DbValue strval{"foo"};

      THEN ("this thorws")
      {
        CHECK_THROWS_AS (dbval = strval, ErrTypeMisMatch);
        CHECK_THROWS_AS (dbval = DbValue{Type::Real}, ErrTypeMisMatch);
      }
    }
    WHEN ("assign a comaptible varaint")
    {
      DbValue strval{12, Type::Variant};

      THEN ("this works")
      {
        CHECK_NOTHROW (dbval = strval);
        CHECK (dbval.getInt () == 12);
        CHECK_NOTHROW (dbval = DbValue{Type::Variant});
        CHECK (dbval.isNull ());
      }
    }
  }

  GIVEN ("a real value")
  {
    DbValue dbval{Type::Real};

    WHEN ("assign a integer")
    {
      int i = 3;

      THEN ("this works")
      {
        CHECK_NOTHROW (dbval = i);
        CHECK (dbval.getReal () == 3.0);
      }
    }
  }
}

SCENARIO ("to stream")
{
  using namespace sl3;
  GIVEN ("some dbvalues ")
  {
    using b      = std::byte;
    auto intval  = DbValue{1};
    auto dblval  = DbValue{1.1};
    auto strval  = DbValue{"foo"};
    auto nullval = DbValue{Type::Null};
    auto blobval = DbValue{Blob{b{1}, b{2}, b{3}}};

    WHEN ("dumping to a stream")
    {
      std::ostringstream stm;

      THEN ("the stream contains expected content")
      {
        stm << intval;
        CHECK (stm.str () == "1");
        stm.str (std::string ());

        stm << dblval;
        CHECK (stm.str ().substr (0, 3) == "1.1");
        stm.str (std::string ());

        stm << strval;
        CHECK (stm.str () == "foo");
        stm.str (std::string ());

        stm << nullval;
        CHECK (stm.str () == "<NULL>");
        stm.str (std::string ());

        stm << blobval;
        CHECK (stm.str () == "<BLOB>");
        stm.str (std::string ());
      }
    }
  }
}

SCENARIO ("sorting")
{
  using namespace sl3;
  GIVEN ("a typed integer and a typed real with the same value")
  {
    DbValue ival{1};
    DbValue rval{1.0};

    check (ival).typeAndStorage (Type::Int, Type::Int);
    check (rval).typeAndStorage (Type::Real, Type::Real);

    WHEN ("comparing them")
    {
      THEN ("they are value equal but not value type eaql")
      {
        CHECK (dbval_eq (ival, ival));
        CHECK (dbval_eq (ival, rval));
        CHECK_FALSE (dbval_type_eq (ival, rval));
        CHECK (dbval_type_eq (rval, rval));
        CHECK_FALSE (dbval_type_eq (ival, DbValue{1, Type::Variant}));
      }
      THEN ("they are value type less but not value less")
      {
        CHECK (dbval_type_lt (ival, rval));
        CHECK (dbval_type_lt (ival, DbValue{1, Type::Variant}));
        CHECK_FALSE (dbval_lt (ival, rval));
        CHECK_FALSE (dbval_type_lt (ival, DbValue{0, Type::Variant}));
        CHECK_FALSE (dbval_type_lt (ival, DbValue{0}));
      }
    }
  }
}
