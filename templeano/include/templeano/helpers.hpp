#ifndef TEMPLEANO_HELPERS_HPP
#define TEMPLEANO_HELPERS_HPP

#include "templeano/peano.hpp"
#include <tuple>

/*
 Helper mechanic class to get the size_t representation of a number
*/
namespace templeano::helpers {
namespace detail {
template <typename T> struct CountBuilder;

template <typename... T1>
constexpr std::tuple<std::tuple<T1...>, T1...> augment(std::tuple<T1...> arg) {
  return std::tuple_cat(std::make_tuple(arg), arg);
}

template <typename... TupleArgs>
constexpr std::integral_constant<std::size_t, sizeof...(TupleArgs)>
tupleSize(std::tuple<TupleArgs...>) {
  return {};
}

template <> struct CountBuilder<Zero> {
  using type = std::tuple<>;
  static constexpr auto value =
      decltype(tupleSize(std::declval<type>()))::value;
};

template <typename T> struct CountBuilder<Successor<T>> {
  using type =
      decltype(augment(std::declval<typename CountBuilder<T>::type>()));
  static constexpr auto value =
      decltype(tupleSize(std::declval<type>()))::value;
};
} // namespace detail
template <PeanoInteger T = Zero> constexpr std::size_t value(T = {}) {
  return detail::CountBuilder<T>::value;
}

/*
 Helper mechanic class to get type encoding a given size_t value
*/
namespace detail {
template <typename T> struct DebugBuilderHelper;
template <> struct DebugBuilderHelper<std::integral_constant<std::size_t, 0>> {
  using type = Zero;
};
template <std::size_t value>
struct DebugBuilderHelper<std::integral_constant<std::size_t, value>> {
  using type = Successor<typename DebugBuilderHelper<
      std::integral_constant<std::size_t, value - 1>>::type>;
};
} // namespace detail

template <std::size_t value>
using PeanoEncoding = typename detail::DebugBuilderHelper<
    std::integral_constant<std::size_t, value>>::type;
} // namespace templeano::helpers
#endif // TEMPLEANO_HELPERS_HPP
