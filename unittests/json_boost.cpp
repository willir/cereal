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

#include <cereal/types/json_boost.hpp>

using cereal::JsonOptional;
using cereal::JsonOptNull;
using cereal::JsonNullable;
using cereal::make_nvp;
using std::cerr;
using std::cout;
using std::endl;
using std::unique_ptr;
using std::string;
using std::stringstream;

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

template <template <typename> class Cont>
struct Jsonable {
  public:
    string str1;
    int int1 = 0;
    Cont<string> optionalStr;
    Cont<int> optionalInt;
    string str2;
    Cont<string> optionalStr2;
    int int2 = 0;
    Cont<int> optionalInt2;
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

template <template <typename> class Cont>
inline std::ostream &operator<<(std::ostream &os, const Jsonable<Cont> &obj) {
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

template <template <typename> class Cont>
inline bool operator==(const Jsonable<Cont> &lhs, const Jsonable<Cont> &rhs) {
    if(lhs.optionalStr != rhs.optionalStr) return false;
    if(lhs.optionalInt != rhs.optionalInt) return false;
    if(lhs.optionalStr2 != rhs.optionalStr2) return false;

    return lhs.optionalInt2 == rhs.optionalInt2;
}

constexpr char Name::STR1[];
constexpr char Name::INT1[];
constexpr char Name::OPTIONAL_STR[];
constexpr char Name::OPTIONAL_INT[];
constexpr char Name::OPTIONAL_STR2[];
constexpr char Name::OPTIONAL_INT2[];
constexpr char Name::STR2[];
constexpr char Name::INT2[];
constexpr char Name::STR3[];
constexpr char Name::INT3[];

template <template <typename> class Cont>
static void testInput() {
  const string strVal = "someString";
  const int intVal = 2;

  Jsonable<Cont> objOut(strVal, intVal);

  std::stringstream ss;
  {
    cereal::JSONOutputArchive arOut(ss);
    objOut.serialize(arOut);
  }

  Jsonable<Cont> objIn;
  cereal::JSONInputArchive arIn(ss);
  objIn.serialize(arIn);

  BOOST_CHECK_EQUAL(objOut, objIn);
}

enum class ContType {
  NULLABLE, OPTIONAL, OPT_NULL
};

static void checkJson(const string &json, const string &strVal, int intVal, ContType contType) {
  rapidjson::Document doc;
  doc.Parse(json.c_str());

  BOOST_CHECK(doc[Name::STR1].IsString());
  BOOST_CHECK(doc[Name::INT1].IsInt());
  BOOST_CHECK_EQUAL(doc[Name::STR1].GetString(), strVal);
  BOOST_CHECK_EQUAL(doc[Name::INT1].GetInt(), intVal);

  BOOST_CHECK(doc[Name::OPTIONAL_STR].IsString());
  BOOST_CHECK(doc[Name::OPTIONAL_INT].IsInt());

  BOOST_CHECK_EQUAL(doc[Name::OPTIONAL_STR].GetString(), strVal);
  BOOST_CHECK_EQUAL(doc[Name::OPTIONAL_INT].GetInt(), intVal);

  switch (contType) {
  case ContType::NULLABLE:
    BOOST_CHECK(doc[Name::OPTIONAL_STR2].IsNull());
    BOOST_CHECK(doc[Name::OPTIONAL_INT2].IsNull());
    break;
  case ContType::OPTIONAL:
    BOOST_CHECK(!doc.HasMember(Name::OPTIONAL_STR2));
    BOOST_CHECK(!doc.HasMember(Name::OPTIONAL_INT2));
    break;
  case ContType::OPT_NULL:
    BOOST_CHECK(doc[Name::OPTIONAL_STR2].IsNull());
    BOOST_CHECK(!doc.HasMember(Name::OPTIONAL_INT2));
    break;
  }

  BOOST_CHECK(doc[Name::STR2].IsString());
  BOOST_CHECK(doc[Name::INT2].IsInt());
  BOOST_CHECK_EQUAL(doc[Name::STR2].GetString(), strVal);
  BOOST_CHECK_EQUAL(doc[Name::INT2].GetInt(), intVal);

  BOOST_CHECK(doc[Name::STR3].IsString());
  BOOST_CHECK(doc[Name::INT3].IsInt());
  BOOST_CHECK_EQUAL(doc[Name::STR3].GetString(), strVal);
  BOOST_CHECK_EQUAL(doc[Name::INT3].GetInt(), intVal);
}

BOOST_AUTO_TEST_CASE( json_optional_output )
{
  const string strVal = "someString";
  const int intVal = 2;

  Jsonable<JsonOptional> objOut("someString", 2);

  std::stringstream ss;
  {
    cereal::JSONOutputArchive arOut(ss);
    objOut.serialize(arOut);
  }

  checkJson(ss.str(), strVal, intVal, ContType::OPTIONAL);
}

BOOST_AUTO_TEST_CASE( json_optional_input )
{
  testInput<JsonOptional>();
}

BOOST_AUTO_TEST_CASE( json_nullable_output )
{
  const string strVal = "someString";
  const int intVal = 2;

  Jsonable<cereal::JsonNullable> objOut("someString", 2);

  std::stringstream ss;
  {
    cereal::JSONOutputArchive arOut(ss);
    objOut.serialize(arOut);
  }

  checkJson(ss.str(), strVal, intVal, ContType::NULLABLE);
}

BOOST_AUTO_TEST_CASE( json_nullable_input )
{
  testInput<JsonNullable>();
}


BOOST_AUTO_TEST_CASE( json_optnull_output )
{
  const string strVal = "someString";
  const int intVal = 2;

  Jsonable<cereal::JsonOptNull> objOut("someString", 2);

  objOut.optionalStr2 = cereal::jsonNull;

  std::stringstream ss;
  {
    cereal::JSONOutputArchive arOut(ss);
    objOut.serialize(arOut);
  }

  checkJson(ss.str(), strVal, intVal, ContType::OPT_NULL);
}

string serializeOptionalOfNullable(const JsonOptional<JsonNullable<string>> &obj) {
  std::stringstream ss;
  {
    cereal::JSONOutputArchive arOut(ss);
    arOut(make_nvp(Name::OPTIONAL_STR2, "asdasd"));
    arOut(make_nvp(Name::OPTIONAL_STR, obj));
  }
  return ss.str();
}

JsonOptional<JsonNullable<string>> parseOptionalOfNullable(const string &json) {
  JsonOptional<JsonNullable<string>> res;
  stringstream ss(json);

  cereal::JSONInputArchive arIn(ss);
  arIn(make_nvp(Name::OPTIONAL_STR, res));

  return res;
}

BOOST_AUTO_TEST_CASE( json_optional_of_nullable_input )
{
  JsonOptional<JsonNullable<std::string>> obj = boost::none;

  rapidjson::Document doc;
  string strOpt = serializeOptionalOfNullable(obj);
  doc.Parse(strOpt.c_str());
  BOOST_CHECK(!doc.HasMember(Name::OPTIONAL_STR));

  obj = cereal::JsonNullable<std::string>(boost::none);
  string strNull = serializeOptionalOfNullable(obj);
  doc.Parse(strNull.c_str());
  BOOST_CHECK(doc[Name::OPTIONAL_STR].IsNull());

  const string SOME_STR = "some_string_value";
  obj = cereal::JsonNullable<std::string>(SOME_STR);
  string str = serializeOptionalOfNullable(obj);
  doc.Parse(str.c_str());
  BOOST_CHECK(doc[Name::OPTIONAL_STR].IsString());
  BOOST_CHECK(doc[Name::OPTIONAL_STR].GetString() == SOME_STR);

  auto objOpt = parseOptionalOfNullable(strOpt);
  BOOST_CHECK(!objOpt.is_initialized());

  auto objNull = parseOptionalOfNullable(strNull);
  BOOST_CHECK(objNull.is_initialized());
  BOOST_CHECK(!objNull->is_initialized());

  auto objStr = parseOptionalOfNullable(str);
  BOOST_CHECK(objStr.is_initialized());
  BOOST_CHECK(objStr->is_initialized());
  BOOST_CHECK(objStr->value() == SOME_STR);
}

BOOST_AUTO_TEST_CASE( json_class_JsonOptional )
{
  JsonOptional<int> intOpt;
  BOOST_CHECK(!intOpt.is_initialized());
  BOOST_CHECK(intOpt.isAbsent());
  BOOST_CHECK_EQUAL(intOpt.value_or(4), 4);

  intOpt = 3;
  BOOST_CHECK(intOpt == 3);
  BOOST_CHECK(intOpt.is_initialized());
  BOOST_CHECK(!intOpt.isAbsent());
  BOOST_CHECK_EQUAL(intOpt.value_or(4), 3);

  intOpt = boost::none;
  BOOST_CHECK(!intOpt.is_initialized());
  BOOST_CHECK(intOpt.isAbsent());
  BOOST_CHECK_EQUAL(intOpt.value_or(4), 4);

  intOpt = JsonOptional<int>(3);
  BOOST_CHECK(intOpt == 3);
  BOOST_CHECK(intOpt.is_initialized());
  BOOST_CHECK(!intOpt.isAbsent());
  BOOST_CHECK_EQUAL(intOpt.value_or(4), 3);

  intOpt = cereal::jsonAbsent;
  BOOST_CHECK(!intOpt.is_initialized());
  BOOST_CHECK(intOpt.isAbsent());
  BOOST_CHECK_EQUAL(intOpt.value_or(4), 4);

  JsonOptional<int> intOpt2(2);
  BOOST_CHECK(intOpt2 == 2);
  BOOST_CHECK(intOpt2.is_initialized());
  BOOST_CHECK(!intOpt2.isAbsent());
  BOOST_CHECK_EQUAL(intOpt2.value_or(4), 2);
}

BOOST_AUTO_TEST_CASE( json_class_JsonNullable )
{
  JsonNullable<int> intNull;
  BOOST_CHECK(!intNull.is_initialized());
  BOOST_CHECK(intNull.isNull());
  BOOST_CHECK_EQUAL(intNull.value_or(4), 4);

  intNull = 3;
  BOOST_CHECK(intNull == 3);
  BOOST_CHECK(intNull.is_initialized());
  BOOST_CHECK(!intNull.isNull());
  BOOST_CHECK_EQUAL(intNull.value_or(4), 3);

  intNull = boost::none;
  BOOST_CHECK(!intNull.is_initialized());
  BOOST_CHECK(intNull.isNull());
  BOOST_CHECK_EQUAL(intNull.value_or(4), 4);

  intNull = JsonNullable<int>(3);
  BOOST_CHECK(intNull == 3);
  BOOST_CHECK(intNull.is_initialized());
  BOOST_CHECK(!intNull.isNull());
  BOOST_CHECK_EQUAL(intNull.value_or(4), 3);

  intNull = cereal::jsonNull;
  BOOST_CHECK(!intNull.is_initialized());
  BOOST_CHECK(intNull.isNull());
  BOOST_CHECK_EQUAL(intNull.value_or(4), 4);

  JsonNullable<int> intNull2(2);
  BOOST_CHECK(intNull2 == 2);
  BOOST_CHECK(intNull2.is_initialized());
  BOOST_CHECK(!intNull2.isNull());
  BOOST_CHECK_EQUAL(intNull2.value_or(4), 2);
}

BOOST_AUTO_TEST_CASE( json_class_JsonOptNull ) {
  JsonOptNull<int> obj;
  BOOST_CHECK(!obj.is_initialized());
  BOOST_CHECK(!obj.isNull());
  BOOST_CHECK(obj.isAbsent());
  BOOST_CHECK_EQUAL(obj.value_or(4), 4);

  obj = 3;
  BOOST_CHECK(obj == 3);
  BOOST_CHECK(obj.is_initialized());
  BOOST_CHECK(!obj.isNull());
  BOOST_CHECK(!obj.isAbsent());
  BOOST_CHECK_EQUAL(obj.value_or(4), 3);

  obj = cereal::jsonNull;
  BOOST_CHECK(!obj.is_initialized());
  BOOST_CHECK(obj.isNull());
  BOOST_CHECK(!obj.isAbsent());
  BOOST_CHECK_EQUAL(obj.value_or(4), 4);

  obj = boost::none;
  BOOST_CHECK(!obj.is_initialized());
  BOOST_CHECK(!obj.isNull());
  BOOST_CHECK(obj.isAbsent());
  BOOST_CHECK_EQUAL(obj.value_or(4), 4);

  obj = JsonOptNull<int>(3);
  BOOST_CHECK(obj == 3);
  BOOST_CHECK(obj.is_initialized());
  BOOST_CHECK(!obj.isNull());
  BOOST_CHECK_EQUAL(obj.value_or(4), 3);

  obj = cereal::jsonAbsent;
  BOOST_CHECK(!obj.is_initialized());
  BOOST_CHECK(!obj.isNull());
  BOOST_CHECK(obj.isAbsent());
  BOOST_CHECK_EQUAL(obj.value_or(4), 4);

  JsonOptNull<int> obj2(2);
  BOOST_CHECK(obj2 == 2);
  BOOST_CHECK(obj2.is_initialized());
  BOOST_CHECK(!obj2.isNull());
  BOOST_CHECK(!obj2.isAbsent());
  BOOST_CHECK_EQUAL(obj2.value_or(4), 2);
}

BOOST_AUTO_TEST_CASE( json_class_inter_copy )
{
  {
    JsonOptNull<int> obj(cereal::jsonNull);
    BOOST_CHECK(obj.toNullable().isNull());

    JsonNullable<int> obj2(std::move(obj));
    BOOST_CHECK(obj2.isNull());
  }

  {
    JsonOptNull<int> obj(cereal::jsonAbsent);
    BOOST_CHECK(obj.toOptional().isAbsent());

    JsonOptional<int> obj2(std::move(obj));
    BOOST_CHECK(obj2.isAbsent());
  }

  {
    JsonOptional<int> obj(cereal::jsonAbsent);

    JsonOptNull<int> obj2(obj);
    BOOST_CHECK(obj2.isAbsent());
    BOOST_CHECK(obj.isAbsent());

    JsonOptNull<int> obj3(std::move(obj));
    BOOST_CHECK(obj3.isAbsent());
  }

  {
    JsonNullable<int> obj(cereal::jsonNull);

    JsonOptNull<int> obj2(obj);
    BOOST_CHECK(obj2.isNull());
    BOOST_CHECK(obj.isNull());

    JsonOptNull<int> obj3(std::move(obj));
    BOOST_CHECK(obj3.isNull());
  }

  {
    JsonOptNull<int> obj = 5;
    BOOST_CHECK_EQUAL(obj.toNullable().value_or(4), 5);

    JsonNullable<int> obj2 = cereal::jsonNull;
    BOOST_CHECK(obj2.isNull());

    obj2 = obj;
    BOOST_CHECK_EQUAL(obj.value_or(4), 5);
    BOOST_CHECK_EQUAL(obj2.value_or(4), 5);

    obj2 = 3;
    BOOST_CHECK_EQUAL(obj2.value_or(4), 3);

    obj2 = std::move(obj);
    BOOST_CHECK_EQUAL(obj2.value_or(4), 5);
  }

  {
    JsonOptNull<int> obj = 5;
    BOOST_CHECK_EQUAL(obj.toOptional().value_or(4), 5);

    JsonOptional<int> obj2 = cereal::jsonAbsent;
    BOOST_CHECK(obj2.isAbsent());

    obj2 = obj;
    BOOST_CHECK_EQUAL(obj.value_or(4), 5);
    BOOST_CHECK_EQUAL(obj2.value_or(4), 5);

    obj2 = 3;
    BOOST_CHECK_EQUAL(obj2.value_or(4), 3);

    obj2 = std::move(obj);
    BOOST_CHECK_EQUAL(obj2.value_or(4), 5);
  }

  {
    JsonNullable<int> obj = 5;
    BOOST_CHECK_EQUAL(obj.value_or(4), 5);

    JsonOptNull<int> obj2 = cereal::jsonAbsent;
    BOOST_CHECK(obj2.isAbsent());

    obj2 = obj;
    BOOST_CHECK_EQUAL(obj.value_or(4), 5);
    BOOST_CHECK_EQUAL(obj2.value_or(4), 5);

    obj2 = 3;
    BOOST_CHECK_EQUAL(obj2.value_or(4), 3);

    obj2 = std::move(obj);
    BOOST_CHECK_EQUAL(obj2.value_or(4), 5);
  }

  {
    JsonOptional<int> obj = 5;
    BOOST_CHECK_EQUAL(obj.value_or(4), 5);

    JsonOptNull<int> obj2 = cereal::jsonNull;
    BOOST_CHECK(obj2.isNull());

    obj2 = obj;
    BOOST_CHECK_EQUAL(obj.value_or(4), 5);
    BOOST_CHECK_EQUAL(obj2.value_or(4), 5);

    obj2 = 3;
    BOOST_CHECK_EQUAL(obj2.value_or(4), 3);

    obj2 = std::move(obj);
    BOOST_CHECK_EQUAL(obj2.value_or(4), 5);
  }
}
