//==============================================================================
//
//            Copyright Michael Munzert (www.mm-log.com) 2014.
//        Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                   http://www.boost.org/LICENSE_1_0.txt)
//
//==============================================================================

#ifndef ENUM_TOOLS_H_
#define ENUM_TOOLS_H_

#include <boost/preprocessor.hpp>
#include <boost/optional.hpp>

#include <algorithm>
#include <array>
#include <functional>
#include <type_traits>
#include <typeinfo>
#include <utility>

//==============================================================================

/**
 * Macro to setup an enum completely.
 * First parameter is the name, following are the states as plain text.
 */
#define DEF_ENUM(name, ...)                                                    \
  enum class name : uint8_t { __VA_ARGS__ };                                   \
  SETUP_ENUM_LENGTH(name, BOOST_PP_VARIADIC_SIZE(__VA_ARGS__))                 \
  SETUP_ENUM_TRAIT(name)

/**
 * Once an enum class is defined, this macro makes the size publicly available.
 * Needed by enum_array. Already included in DEF_ENUM.
 */
#define SETUP_ENUM_LENGTH(enum_type, length)                                   \
  inline constexpr int enum_size_helper(enum_type*) { return length; }

/**
 * This macro makes the type trait is_managed_enum publicly available.
 * Already included in DEF_ENUM.
 */
#define SETUP_ENUM_TRAIT(enum_type)                                            \
  inline constexpr bool is_managed_enum_helper(enum_type*) { return true; }

//==============================================================================

/**
 * Type trait to get the size of the enum type.
 */
template<typename Enum>
struct enum_size
    : std::integral_constant<std::size_t, enum_size_helper((Enum*)nullptr)> {};

//==============================================================================

// not managed types/enums should return false
template <typename T>
inline constexpr bool is_managed_enum_helper(T*) { return false; }

/**
 * Type trait to identify managed enums.
 */
template <typename Enum>
struct is_managed_enum
    : std::integral_constant<bool, is_managed_enum_helper((Enum*)nullptr)> {};

//==============================================================================

/**
 * Function to iterate over all elements of an enum with a callback.
 */
template<typename Enum>
void enum_for_each(const std::function<void(Enum e)> &fct) {
  for (int i = 0; i < enum_size<Enum>::value; ++i) {
    fct(static_cast<Enum>(i));
  }
}

//==============================================================================

/**
 * Get the size of the enum type for a given enum value.
 */
struct enum_size_for_value {
  template<typename Enum>
  unsigned int operator()(Enum) const {
    return enum_size<Enum>::value;
  }
};

//==============================================================================

/**
 * Get the index for a given enum value.
 */
template<typename Enum>
typename std::underlying_type<Enum>::type enum_index(Enum e) {
  return static_cast<typename std::underlying_type<Enum>::type>(e);
}

//==============================================================================

/**
 * Get the enum value for a given index.
 */
template<typename Enum>
Enum enum_value(typename std::underlying_type<Enum>::type i) {
  if (i >= enum_size<Enum>::value) {
    throw std::out_of_range("enum conversion out of range, value: "
                            + to_string(i));
  }
  return static_cast<Enum>(i);
}

//==============================================================================
/**
 * Modeled on an std::array with length given by the size of an enum.
 * Needs to be initialized with the correct number of arguments, otherwise the
 * compiler will complain.
 */
template<typename Enum, typename T>
class enum_array {
public:
  typedef typename std::array<T, enum_size<Enum>::value>::const_iterator
          const_iterator;
  typedef typename std::array<T, enum_size<Enum>::value>::iterator iterator;
  typedef T value_type;

  //--------------------------------------
  template<typename ... Args>
  enum_array(Args ... list) {
    static_assert(enum_size<Enum>::value == sizeof...(Args) ||
                  // needed for default constructor (used in nested types)
                  sizeof...(Args) == 0,
        "Wrong number of arguments in enum array instantiation!");

    _data = { {std::forward<Args>(list)...} };
  }

  //--------------------------------------
  enum_array(value_type default_value) {
    _data.fill(default_value);
  }

  //--------------------------------------
  template<typename otherEnum>
  enum_array(const enum_array<otherEnum, value_type> &other ) {
    static_assert(enum_size<Enum>::value == enum_size<otherEnum>::value,
        "Wrong number of arguments in enum array copy constructor!");

    _data(other._data);
  }

  //--------------------------------------
  template<typename otherEnum>
  enum_array(enum_array<otherEnum, value_type> &&other ) {
    static_assert(enum_size<Enum>::value == enum_size<otherEnum>::value,
        "Wrong number of arguments in enum array move constructor!");

    _data(std::move(other._data));
  }

  //--------------------------------------
  template<typename otherEnum>
  enum_array<Enum, value_type>& operator= (const enum_array<otherEnum, value_type> &other) {
    static_assert(enum_size<Enum>::value == enum_size<otherEnum>::value,
        "Wrong number of arguments in enum array copy assignment!");

    _data = other._data;
  }

  //--------------------------------------
  template<typename otherEnum>
  enum_array<Enum, value_type>& operator= (enum_array<otherEnum, value_type> &&other) {
    static_assert(enum_size<Enum>::value == enum_size<otherEnum>::value,
        "Wrong number of arguments in enum array move assignment!");

    _data = std::move(other._data);
  }

  //--------------------------------------
  constexpr static size_t size() {
    return enum_size<Enum>::value;
  }

  //--------------------------------------
  // read
  const value_type& at(int index) const {
    return _data.at(index);
  }

  const value_type& operator[](int index) const {
    return _data[index];
  }

  const value_type& at(Enum type) const {
    return _data.at(enum_index(type));
  }

  const value_type& operator[](Enum type) const {
    return _data[enum_index(type)];
  }

  value_type& at(int index) {
    return _data.at(index);
  }

  value_type& operator[](int index) {
    return _data[index];
  }

  value_type& at(Enum type) {
    return _data.at(enum_index(type));
  }

  value_type& operator[](Enum type) {
    return _data[enum_index(type)];
  }

  //--------------------------------------
  // write
  void set_value(int index, const value_type &value) {
      _data.at(index) = value;
    }

  void set_value(Enum type, const value_type &value) {
      _data.at(enum_index(type)) = value;
    }

  //--------------------------------------
  // iterator access
  iterator begin() {
    return _data.begin();
  }

  iterator end() {
    return _data.end();
  }

  const_iterator begin() const {
    return _data.cbegin();
  }

  const_iterator end() const {
    return _data.cend();
  }

  //--------------------------------------
  // algorithms
  template <typename Value, typename = typename std::enable_if<
              std::is_constructible<std::string, Value>::value>::type>
  Enum find(const Value &value) const {
    auto it = std::find(_data.begin(), _data.end(), value);
    if (it != _data.end()) {
      return static_cast<Enum>(std::distance(_data.begin(), it));
    } else {
      // We don't have a default value;
      throw std::invalid_argument(
          "Value \"" + std::string(value) +
            "\" not found for enum " + typeid(Enum).name());
    }
  }

  template <typename Value, typename = void, typename = typename std::enable_if<
              !std::is_constructible<std::string, Value>::value>::type>
  Enum find(const Value &value) const {
    auto it = std::find(_data.begin(), _data.end(), value);
    if (it != _data.end()) {
      return static_cast<Enum>(std::distance(_data.begin(), it));
    } else {
      // We don't have a default value;
      throw std::invalid_argument(
          std::string("Value not found for enum ") + typeid(Enum).name());
    }
  }

  Enum find(const value_type &value, Enum default_value) const {
    auto it = std::find(_data.begin(), _data.end(), value);
    if (it != _data.end()) {
      return static_cast<Enum>(std::distance(_data.begin(), it));
    } else {
      return default_value;
    }
  }

  boost::optional<Enum> find_optional(const value_type &value) const {
    auto it = std::find(_data.begin(), _data.end(), value);
    if (it != _data.end()) {
      return static_cast<Enum>(std::distance(_data.begin(), it));
    } else {
      return boost::optional<Enum>();
    }
  }

private:
  std::array<value_type, enum_size<Enum>::value> _data;
};

//==============================================================================
template<typename Enum>
class enum_member {
public:
  typedef typename std::array<Enum, enum_size<Enum>::value>::const_iterator
          const_iterator;

  enum_member() {
    for (int i = 0; i < enum_size<Enum>::value; ++i) {
      _data.at(i) = static_cast<Enum>(i);
    }
  }

  enum_member(const enum_member&) = delete;
  enum_member& operator=(const enum_member&) = delete;

  constexpr static size_t size() {
    return enum_size<Enum>::value;
  }

  //--------------------------------------
  // read values
  Enum at(int index) const {
    return _data.at(index);
  }

  Enum operator[](int index) const {
    return _data[index];
  }

  Enum at(Enum type) const {
    return _data.at(static_cast<int>(type));
  }

  Enum operator[](Enum type) const {
    return _data[static_cast<int>(type)];
  }

  const_iterator begin() const {
    return _data.cbegin();
  }

  const_iterator end() const {
    return _data.cend();
  }

private:
  std::array<Enum, enum_size<Enum>::value> _data;
};

//==============================================================================

#endif /* ENUM_TOOLS_H_ */
