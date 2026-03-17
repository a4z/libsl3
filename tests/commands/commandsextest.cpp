#include "../testing.hpp"
#include <sl3/database.hpp>

#include <cstdint>
#include <stdexcept>
#include <string>

SCENARIO ("raising branch coverage on sl3::parameters")
{
  using namespace sl3;

  auto byte = [] (int v) { return std::byte{static_cast<unsigned char> (v)}; };

  GIVEN ("parameter packs with different argument categories")
  {
    Blob        blob{byte (1), byte (2), byte (3)};
    const char* ctext = "hello";
    int64_t     big   = 1LL << 40;

    WHEN ("creating empty and single-value packs")
    {
      auto empty      = parameters ();
      auto onlyInt    = parameters (1);
      auto onlyInt64  = parameters (big);
      auto onlyFloat  = parameters (1.25f);
      auto onlyDouble = parameters (2.5);
      auto onlyText   = parameters (std::string{"text"});
      auto onlyCText  = parameters (ctext);
      auto onlyBlob   = parameters (blob);
      auto onlyNull   = parameters (Type::Null);
      auto onlyVar    = parameters (Type::Variant);

      THEN ("the resulting values reflect the chosen constructor paths")
      {
        REQUIRE_EQ (empty.size (), std::size_t{0});

        REQUIRE_EQ (onlyInt.size (), std::size_t{1});
        CHECK_EQ (onlyInt[0].dbtype (), Type::Int);
        CHECK_EQ (onlyInt[0].getInt (), 1);

        REQUIRE_EQ (onlyInt64.size (), std::size_t{1});
        CHECK_EQ (onlyInt64[0].dbtype (), Type::Int);
        CHECK_EQ (onlyInt64[0].getInt (), big);

        REQUIRE_EQ (onlyFloat.size (), std::size_t{1});
        CHECK_EQ (onlyFloat[0].dbtype (), Type::Real);
        CHECK_EQ (onlyFloat[0].getReal (), doctest::Approx{1.25});

        REQUIRE_EQ (onlyDouble.size (), std::size_t{1});
        CHECK_EQ (onlyDouble[0].dbtype (), Type::Real);
        CHECK_EQ (onlyDouble[0].getReal (), doctest::Approx{2.5});

        REQUIRE_EQ (onlyText.size (), std::size_t{1});
        CHECK_EQ (onlyText[0].dbtype (), Type::Text);
        CHECK_EQ (onlyText[0].getText (), "text");

        REQUIRE_EQ (onlyCText.size (), std::size_t{1});
        CHECK_EQ (onlyCText[0].dbtype (), Type::Text);
        CHECK_EQ (onlyCText[0].getText (), "hello");

        REQUIRE_EQ (onlyBlob.size (), std::size_t{1});
        CHECK_EQ (onlyBlob[0].dbtype (), Type::Blob);
        CHECK_EQ (onlyBlob[0].getBlob (), blob);

        REQUIRE_EQ (onlyNull.size (), std::size_t{1});
        CHECK_EQ (onlyNull[0].dbtype (), Type::Variant);
        CHECK (onlyNull[0].isNull ());

        REQUIRE_EQ (onlyVar.size (), std::size_t{1});
        CHECK_EQ (onlyVar[0].dbtype (), Type::Variant);
        CHECK (onlyVar[0].isNull ());
      }
    }

    WHEN ("mixing lvalues, rvalues and repeated categories")
    {
      auto mixed = parameters (std::string{"alpha"},
                               ctext,
                               blob,
                               Blob{byte (4), byte (5)},
                               7,
                               big,
                               3.5f,
                               9.25,
                               Type::Null,
                               Type::Variant);

      THEN ("every slot keeps the expected type and value")
      {
        REQUIRE_EQ (mixed.size (), std::size_t{10});

        CHECK_EQ (mixed[0].dbtype (), Type::Text);
        CHECK_EQ (mixed[0].getText (), "alpha");

        CHECK_EQ (mixed[1].dbtype (), Type::Text);
        CHECK_EQ (mixed[1].getText (), "hello");

        CHECK_EQ (mixed[2].dbtype (), Type::Blob);
        CHECK_EQ (mixed[2].getBlob (), blob);

        CHECK_EQ (mixed[3].dbtype (), Type::Blob);
        CHECK_EQ (mixed[3].getBlob ().size (), std::size_t{2});

        CHECK_EQ (mixed[4].dbtype (), Type::Int);
        CHECK_EQ (mixed[4].getInt (), 7);

        CHECK_EQ (mixed[5].dbtype (), Type::Int);
        CHECK_EQ (mixed[5].getInt (), big);

        CHECK_EQ (mixed[6].dbtype (), Type::Real);
        CHECK_EQ (mixed[6].getReal (), doctest::Approx{3.5});

        CHECK_EQ (mixed[7].dbtype (), Type::Real);
        CHECK_EQ (mixed[7].getReal (), doctest::Approx{9.25});

        CHECK_EQ (mixed[8].dbtype (), Type::Variant);
        CHECK (mixed[8].isNull ());

        CHECK_EQ (mixed[9].dbtype (), Type::Variant);
        CHECK (mixed[9].isNull ());
      }
    }
  }
}

SCENARIO ("exploring throwing conversions passed to sl3::parameters")
{
  using namespace sl3;

  struct ThrowToInt
  {
    operator int () const { throw std::runtime_error{"int conversion failed"}; }
  };

  struct ThrowToText
  {
    operator std::string () const
    {
      throw std::runtime_error{"text conversion failed"};
    }
  };

  struct ThrowToBlob
  {
    operator Blob () const
    {
      throw std::runtime_error{"blob conversion failed"};
    }
  };

  GIVEN ("types that can convert to accepted DbValue inputs")
  {
    WHEN ("the conversion throws during parameter pack expansion")
    {
      THEN ("the exception escapes to the caller")
      {
        CHECK_THROWS_AS ((void)parameters (ThrowToInt{}), std::runtime_error);
        CHECK_THROWS_AS ((void)parameters (ThrowToText{}), std::runtime_error);
        CHECK_THROWS_AS ((void)parameters (ThrowToBlob{}), std::runtime_error);

        CHECK_THROWS_AS (
            (void)parameters (1, ThrowToText{}, std::string{"tail"}),
            std::runtime_error);
        CHECK_THROWS_AS (
            (void)parameters (std::string{"head"}, ThrowToBlob{}, 2.0),
            std::runtime_error);
      }
    }
  }
}
