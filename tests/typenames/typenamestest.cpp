#include "../testing.hpp"
#include <sl3/types.hpp>

#include <string>

#include <sstream>

SCENARIO ("check typenames")
{
  using namespace sl3;
  GIVEN ("the sqlite version libsl3 was comiled with")
  {
    std::string libUsedVersion = sl3::sqliteCompiledVersion ();

    WHEN ("using the typename function")
    {
      std::string appUsedVersion = sl3::sqliteRuntimeVersion ();

      THEN ("the short enum name is returned")
      {
        CHECK_EQ (typeName (Type::Int), "Int");
        CHECK_EQ (typeName (Type::Real), "Real");
        CHECK_EQ (typeName (Type::Text), "Text");
        CHECK_EQ (typeName (Type::Blob), "Blob");
        CHECK_EQ (typeName (Type::Null), "Null");
        CHECK_EQ (typeName (Type::Variant), "Variant");
      }
    }

    WHEN ("getting typnmes in a stream")
    {
      std::string appUsedVersion = sl3::sqliteRuntimeVersion ();

      THEN ("the full enum name in in the stream")
      {
        {
          std::stringstream ss;
          ss << Type::Int;
          CHECK_EQ (ss.str (), "Type::Int");
        }
        {
          std::stringstream ss;
          ss << Type::Real;
          CHECK_EQ (ss.str (), "Type::Real");
        }
        {
          std::stringstream ss;
          ss << Type::Text;
          CHECK_EQ (ss.str (), "Type::Text");
        }
        {
          std::stringstream ss;
          ss << Type::Blob;
          CHECK_EQ (ss.str (), "Type::Blob");
        }
        {
          std::stringstream ss;
          ss << Type::Variant;
          CHECK_EQ (ss.str (), "Type::Variant");
        }
        {
          std::stringstream ss;
          ss << Type::Null;
          CHECK_EQ (ss.str (), "Type::Null");
        }
      }
    }
  }
}
