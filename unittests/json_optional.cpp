/*
  Copyright (c) 2015, Anton Rapetov
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
      * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
      * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
      * Neither the name of cereal nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL RANDOLPH VOORHIES AND SHANE GRANT BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "common.hpp"

#include <iostream>

#include <boost/test/unit_test.hpp>
#include <boost/utility/string_ref.hpp>

#include <boost/optional.hpp>
#include <boost/optional/optional_io.hpp>

#include <cereal/types/json_optional.hpp>

using std::cout;
using std::endl;
using std::unique_ptr;
using std::string;

using boost::optional;

using cereal::make_nvp;

struct Jsonable {
  public:
    class Name {
      public:
        static constexpr char STR1[] = "str1";
        static constexpr char INT1[] = "int1";
        static constexpr char OPTIONAL_STR[] = "optional_str1";
        static constexpr char OPTIONAL_INT[] = "optional_int1";
        static constexpr char OPTIONAL_STR2[] = "optional_str2";
        static constexpr char OPTIONAL_INT2[] = "optional_int2";
        static constexpr char STR2[] = "str2";
        static constexpr char INT2[] = "int2";
        static constexpr char STR3[] = "str3";
        static constexpr char INT3[] = "int3";
    };

    string str1;
    int int1 = 0;
    optional<string> optionalStr;
    optional<int> optionalInt;
    string str2;
    optional<string> optionalStr2;
    int int2 = 0;
    optional<int> optionalInt2;
    string str3;
    int int3 = 0;

    Jsonable() { }

    Jsonable(const string &strIn, int intIn)
        : str1(strIn),
          int1(intIn),
          optionalStr(strIn),
          optionalInt(intIn),
          str2(strIn),
          optionalStr2(boost::none),
          int2(intIn),
          optionalInt2(boost::none),
          str3(strIn),
          int3(intIn) { }

    template<class Archive>
    void serialize(Archive &ar) {
      ar(make_nvp(Name::STR1, str1),
         make_nvp(Name::INT1, int1),
         make_nvp(Name::OPTIONAL_STR, optionalStr),
         make_nvp(Name::OPTIONAL_INT, optionalInt),
         make_nvp(Name::STR2, str2),
         make_nvp(Name::OPTIONAL_STR2, optionalStr2),
         make_nvp(Name::INT2, int2),
         make_nvp(Name::OPTIONAL_INT2, optionalInt2),
         make_nvp(Name::STR3, str3),
         make_nvp(Name::INT3, int3));
    }
};

inline std::ostream &operator<<(std::ostream &os, const Jsonable &obj) {
  os << "{ "
     << obj.str1 << ";"
     << obj.int1 << ";"
     << obj.optionalStr << ";"
     << obj.optionalInt << ";"
     << obj.str2 << ";"
     << obj.optionalStr2 << ";"
     << obj.int2 << ";"
     << obj.optionalInt2
     << obj.str3 << ";"
     << obj.int3 << ";"
     << " }";
  return os;
}

inline bool operator==(const Jsonable &lhs, const Jsonable &rhs) {
    if(lhs.optionalStr != rhs.optionalStr) return false;
    if(lhs.optionalInt != rhs.optionalInt) return false;
    if(lhs.optionalStr2 != rhs.optionalStr2) return false;

    return lhs.optionalInt2 == rhs.optionalInt2;
}

constexpr char Jsonable::Name::STR1[];
constexpr char Jsonable::Name::INT1[];
constexpr char Jsonable::Name::OPTIONAL_STR[];
constexpr char Jsonable::Name::OPTIONAL_INT[];
constexpr char Jsonable::Name::OPTIONAL_STR2[];
constexpr char Jsonable::Name::OPTIONAL_INT2[];
constexpr char Jsonable::Name::STR2[];
constexpr char Jsonable::Name::INT2[];
constexpr char Jsonable::Name::STR3[];
constexpr char Jsonable::Name::INT3[];

BOOST_AUTO_TEST_CASE( json_optional_output )
{
  typedef Jsonable::Name Name;

  const string strVal = "someString";
  const int intVal = 2;

  Jsonable objOut("someString", 2);

  std::stringstream ss;
  {
    cereal::JSONOutputArchive arOut(ss);
    objOut.serialize(arOut);
  }

  rapidjson::Document doc;
  doc.Parse(ss.str().c_str());

  BOOST_CHECK(doc[Name::STR1].IsString());
  BOOST_CHECK(doc[Name::INT1].IsInt());
  BOOST_CHECK_EQUAL(doc[Name::STR1].GetString(), strVal);
  BOOST_CHECK_EQUAL(doc[Name::INT1].GetInt(), intVal);

  BOOST_CHECK(doc[Name::OPTIONAL_STR].IsString());
  BOOST_CHECK(doc[Name::OPTIONAL_INT].IsInt());

  BOOST_CHECK_EQUAL(doc[Name::OPTIONAL_STR].GetString(), strVal);
  BOOST_CHECK_EQUAL(doc[Name::OPTIONAL_INT].GetInt(), intVal);

  BOOST_CHECK(!doc.HasMember(Name::OPTIONAL_STR2));
  BOOST_CHECK(!doc.HasMember(Name::OPTIONAL_INT2));

  BOOST_CHECK(doc[Name::STR2].IsString());
  BOOST_CHECK(doc[Name::INT2].IsInt());
  BOOST_CHECK_EQUAL(doc[Name::STR2].GetString(), strVal);
  BOOST_CHECK_EQUAL(doc[Name::INT2].GetInt(), intVal);

  BOOST_CHECK(doc[Name::STR3].IsString());
  BOOST_CHECK(doc[Name::INT3].IsInt());
  BOOST_CHECK_EQUAL(doc[Name::STR3].GetString(), strVal);
  BOOST_CHECK_EQUAL(doc[Name::INT3].GetInt(), intVal);
}

BOOST_AUTO_TEST_CASE( json_optional_input )
{
  const string strVal = "someString";
  const int intVal = 2;

  Jsonable objOut(strVal, intVal);

  std::stringstream ss;
  {
    cereal::JSONOutputArchive arOut(ss);
    objOut.serialize(arOut);
  }

  Jsonable objIn;
  cereal::JSONInputArchive arIn(ss);
  objIn.serialize(arIn);

  BOOST_CHECK_EQUAL(objOut, objIn);
}
