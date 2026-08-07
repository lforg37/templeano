#ifndef TEMPLEANO_TEMPLEANO_HPP
#define TEMPLEANO_TEMPLEANO_HPP

#include <type_traits>

namespace templeano {
/* -----
 Building the natural integers:
 1. Zero is a natural integer
------- */
struct Zero {
  static constexpr bool is_number{true};
};

// Successor of a natural integer is a natural integer

template <typename T> struct Successor {
  static constexpr bool is_number{T::is_number};
};

template <typename T>
concept PeanoInteger = T::is_number;

// Helper class for more concise code
namespace detail {
template <PeanoInteger T> struct IsNonNullPeanoInteger : std::false_type {};

template <typename T>
struct IsNonNullPeanoInteger<Successor<T>> : std::true_type {};
} // namespace detail

template <typename T>
concept NonNullPeanoInteger = detail::IsNonNullPeanoInteger<T>::value;

// If n is a natural integer then n = n
template <PeanoInteger L, PeanoInteger R> constexpr bool is_eq_v{false};

template <PeanoInteger I> constexpr bool is_eq_v<I, I>{true};

template <PeanoInteger L, PeanoInteger R> constexpr bool operator==(L, R) {
  return is_eq_v<L, R>;
}

template <PeanoInteger T = Zero> constexpr Successor<T> successor(T = {}) {
  return {};
}

/*
                          Basic operations

*/

/*
    Addition: forall natural integer n, n + 0 = 0 + n = n
    Forall natural integers n0 and n1: S(n0) + S(n1) = S(S(n0)) + n1
*/

constexpr Zero operator+(Zero, Zero) { return {}; }

template <NonNullPeanoInteger T> constexpr T operator+(Zero, T) { return {}; }

template <NonNullPeanoInteger T> constexpr T operator+(T, Zero) { return {}; }

template <PeanoInteger T1, PeanoInteger T2>
constexpr auto operator+(Successor<T1>, Successor<T2>) {
  return Successor<Successor<T1>>{} + T2{};
}

template <typename L, typename R>
using add_t = decltype(std::declval<L>() + std::declval<R>());

/*
    Ordering: same logic as the addition
*/
constexpr bool operator<=(Zero, Zero) { return true; }

template <NonNullPeanoInteger T> constexpr bool operator<=(T, Zero) {
  return false;
}

template <NonNullPeanoInteger T> constexpr bool operator<=(Zero, T) {
  return true;
}

template <typename TL, typename TR>
constexpr bool operator<=(Successor<TL>, Successor<TR>) {
  return TL{} <= TR{};
}

template <typename L, typename R> constexpr bool is_leq_v = L{} <= R{};

constexpr bool operator<(Zero, Zero) { return false; }

template <NonNullPeanoInteger T> constexpr bool operator<(T, Zero) {
  return false;
}

template <NonNullPeanoInteger T> constexpr bool operator<(Zero, T) {
  return true;
}

template <typename TL, typename TR>
constexpr bool operator<(Successor<TL>, Successor<TR>) {
  return TL{} < TR{};
}

template <typename L, typename R> constexpr bool is_lower_v = L{} < R{};

/*
    Subtraction: same logic as the addition
*/

template <typename T> constexpr T operator-(T, Zero) { return {}; }

template <typename TL, typename TR>
constexpr auto operator-(Successor<TL>, Successor<TR>) {
  static_assert(is_leq_v<TR, TL>, "Invalid natural integer subtraction");
  return TL{} - TR{};
}

template <typename L, typename R>
using sub_t = decltype(std::declval<L>() - std::declval<R>());

using One = Successor<Zero>;
constexpr Zero zero{};
constexpr One one{};

/*
    Product: forall n, zero*n = *zero = 0
    For all n0, n1: S(n0) * S(n1) = S(n1) + n0 * S(n1)
*/
template <PeanoInteger T> constexpr Zero operator*(Zero, T) { return {}; }

template <NonNullPeanoInteger T> constexpr Zero operator*(T, Zero) {
  return {};
}

template <PeanoInteger T1, PeanoInteger T2>
constexpr auto operator*(Successor<T1>, Successor<T2>) {
  // This is "the good order" for the addition:
  // accumulating term stays left and addition
  // implementation reduce the right term to add it to
  // the left.
  // Reverse order would move the accumulating term
  // back and forth at each new element add.
  return T1{} * Successor<T2>{} + Successor<T2>{};
}

template <typename L, typename R>
using prod_t = decltype(std::declval<L>() * std::declval<R>());

template <typename T> using square_t = prod_t<T, T>;

/*
    Division: for n0, n1:
    - if n0 < n1:
      n0 / n1 = n0
    - else:
      n0 / n1 = 1 + (no-n1) / n1
*/
template <PeanoInteger L, NonNullPeanoInteger R>
constexpr auto operator/(L, R) {
  if constexpr (is_lower_v<L, R>) {
    return Zero{};
  } else {
    return One{} + ((L{} - R{}) / R{});
  }
}

template <typename L, typename R>
using div_t = decltype(std::declval<L>() / std::declval<R>());

// ----- remainder ----------------------------------------------------

template <typename L, typename RPred>
constexpr auto operator%(L l, Successor<RPred> r) {
  return l - (l / r) * r;
}

template <typename L, typename R>
using mod_t = decltype(std::declval<L>() % std::declval<R>());

/*
                          Positional encoding
*/

// test invalid
// helper::BinaryEncodingScheme::Encoding<One, detail::Two> testInvalid{};

/*
                                            Helper for decoder / encoder
*/

// Some tests

namespace helper {} // namespace helper

} // namespace templeano
#endif // TEMPLEANO_TEMPLEANO_HPP
