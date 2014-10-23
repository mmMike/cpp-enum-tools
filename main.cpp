//==============================================================================
//
//            Copyright Michael Munzert (www.mm-log.com) 2014.
//        Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                   http://www.boost.org/LICENSE_1_0.txt)
//
//==============================================================================

#include "enum_tools.h"

#include <iostream>
#include <string>

DEF_ENUM(enum_1, foo, bar)
DEF_ENUM(enum_2, foo, bar, baz)
namespace ns {
  DEF_ENUM(enum_ns, foo, bar, baz)
}

int main(int, char **) {
  // enum sizes
  std::cout << "Size of enum_1: "  << enum_size<enum_1>::value << std::endl;
  std::cout << "Size of enum_2: "  << enum_size<enum_2>::value << std::endl;
  std::cout << "Size of enum_ns: " << enum_size<ns::enum_ns>::value << std::endl;

  // array with enum index
  enum_array<enum_1, std::string> strings_1 {"Foo", "Bar"};

  // Does not compile, and tells you that the count is not right.
  // enum_array<enum_1, std::string> strings_1 {"Foo"};

  // range-based for over the array
  for (const auto& i : strings_1) {
    std::cout << i << std::endl;
  }

  // range-based for over the enum
  for (const auto& e : enum_member<enum_1>()) {
    std::cout << strings_1.at(e) << std::endl;
  }

  // for-each with callback
  enum_for_each<ns::enum_ns>([](ns::enum_ns value){
    // index for given value
    std::cout << "Index: " << static_cast<int>(enum_index(value)) << std::endl;
  });

  // type trait to check for a managed enum
  std::cout << "string is managed enum: " << is_managed_enum<std::string>::value << std::endl;
  std::cout << "enum_1 is managed enum: " << is_managed_enum<enum_1>::value << std::endl;
}
