cpp\-enum\-tools
================

Type traits and container classes to get compile\-time checked array indexing with enum classes for C\+\+11\. [Example usage](https://github.com/mmMike/cpp-enum-tools/blob/master/main.cpp)\.

Library
-------

* Header only, but relies on [Boost Preprocessor](http://www.boost.org/doc/libs/release/libs/preprocessor/) and [Boost Optional](http://www.boost.org/doc/libs/release/libs/optional/) \(which are also header only\)\.

* Distributed under the Boost Software License, Version 1\.0\.

Usage
-----

### DEF\_ENUM

Macro to define a managed enum at global or namespace scope\.

```
// Macro expands to the enum class 'Columns' and some type traits
DEF_ENUM(Columns, name, address, city, country)
```

### enum\_array

Wrapper template which gives type safe and simple access to a C\+\+11 ``std::array`` by members of a managed enum class\.

**Features**

* Compile time checked initializer list length\.

* Compile time checked copy and move assignments and construction\.

* Type safe element access\.

Main feature is the compile time check for using an initializer list\. This allows a checked storage of additional data for each enum member\. I\.e\. the compiler checks, if the size of the enum class equals the number of given values\. \(Cf\. array of enumerated type in Pascal/Delphi\.\)

**Example**

```
// Enum indexed array with std::string value_type
enum_array<Columns, std::string> Columntitle {"Name", "Address", "City", "Country"};

// Does not compile, and tells you that the count is not right.
enum_array<Columns, std::string> Columntitle_bad {"Name", "Address", "City"};
```

### enum\_member

Container to allow range\-based for loops over a managed enum class\.

**Example**

```
for (const auto& e : enum_member<Columns>()) {
  std::cout << Columntitle.at(e) << std::endl;
}
```

