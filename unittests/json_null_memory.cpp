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

#include <cereal/types/json_null_memory.hpp>

using std::cout;
using std::endl;
using std::unique_ptr;
using std::string;

using cereal::make_nvp;

template<typename C>
bool eq_unique_ptr(const std::unique_ptr<C> &lhs, const std::unique_ptr<C> &rhs) {
  if (!lhs && !rhs) return true;
  if (!lhs || !rhs) return false;

  return *lhs == *rhs;
}

struct ObjWithNull {
  public:
    class Name {
      public:
        static constexpr char NULLABLE_STR[] = "nullable_str";
        static constexpr char NULLABLE_INT[] = "nullable_int";
        static constexpr char NULL_STR[] = "null_str";
        static constexpr char NULL_INT[] = "null_int";
    };

    unique_ptr<string> nullableStr;
    unique_ptr<int> nullableInt;
    unique_ptr<string> nullStr;
    unique_ptr<int> nullInt;

    ObjWithNull() { }

    ObjWithNull(string nullableStrIn, int nullableIntIn)
        : nullableStr(unique_ptr<string>(new string(nullableStrIn))),
          nullableInt(unique_ptr<int>(new int(nullableIntIn))),
          nullStr(nullptr),
          nullInt(nullptr) { }

    template<class Archive>
    void serialize(Archive &ar) {
      ar(make_nvp(Name::NULLABLE_STR, nullableStr),
         make_nvp(Name::NULLABLE_INT, nullableInt),
         make_nvp(Name::NULL_STR, nullStr),
         make_nvp(Name::NULL_INT, nullInt));
    }
};

template<typename C>
std::ostream &operator<<(std::ostream &os, const unique_ptr<C> &obj) {
  if (obj) {
    return os << *obj;
  } else {
    return os << "nullptr";
  }
}

std::ostream &operator<<(std::ostream &os, const ObjWithNull &obj) {
  os << "{ "
     << obj.nullableStr << ";"
     << obj.nullableInt << ";"
     << obj.nullStr << ";"
     << obj.nullInt
     << "}";
  return os;
}

inline bool operator==(const ObjWithNull& lhs, const ObjWithNull& rhs) {
  if (!eq_unique_ptr(lhs.nullableStr, rhs.nullableStr)) return false;
  if (!eq_unique_ptr(lhs.nullableInt, rhs.nullableInt)) return false;
  if (!eq_unique_ptr(lhs.nullStr, rhs.nullStr)) return false;
  if (!eq_unique_ptr(lhs.nullInt, rhs.nullInt)) return false;

  return true;
}

constexpr char ObjWithNull::Name::NULLABLE_STR[];
constexpr char ObjWithNull::Name::NULLABLE_INT[];
constexpr char ObjWithNull::Name::NULL_STR[];
constexpr char ObjWithNull::Name::NULL_INT[];

BOOST_AUTO_TEST_CASE( json_null_output )
{
  typedef ObjWithNull::Name Name;

  ObjWithNull objOut("someString", 2);

  std::stringstream ss;
  {
    cereal::JSONOutputArchive arOut(ss);
    objOut.serialize(arOut);
  }

  rapidjson::Document doc;
  doc.Parse(ss.str().c_str());

  BOOST_CHECK(doc[Name::NULLABLE_STR].IsString());
  BOOST_CHECK(doc[Name::NULLABLE_INT].IsInt());
  BOOST_CHECK_EQUAL(doc[Name::NULLABLE_INT].GetInt(), 2);
  BOOST_CHECK_EQUAL(doc[Name::NULLABLE_STR].GetString(), "someString");
  BOOST_CHECK(doc[Name::NULL_STR].IsNull());
  BOOST_CHECK(doc[Name::NULL_INT].IsNull());
}

BOOST_AUTO_TEST_CASE( json_null_input )
{
  ObjWithNull objOut("someString", 2);

  std::stringstream ss;
  {
    cereal::JSONOutputArchive arOut(ss);
    objOut.serialize(arOut);
  }

  ObjWithNull objIn;
  cereal::JSONInputArchive arIn(ss);
  objIn.serialize(arIn);

  BOOST_CHECK_EQUAL(objOut, objIn);
}
