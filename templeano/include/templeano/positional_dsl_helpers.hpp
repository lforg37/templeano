#ifndef TEMPLEANO_POSITIONAL_DSL_HELPERS_HPP
#define TEMPLEANO_POSITIONAL_DSL_HELPERS_HPP

#include "templeano/positional.hpp"
#include <algorithm>

namespace templeano::helpers {
template <std::size_t N> struct StringLiteral {
  static constexpr std::size_t Len = N;
  wchar_t value[Len]{};

  constexpr StringLiteral(const wchar_t (&str)[N]) {
    std::copy_n(str, Len, value);
  }
};

template <typename T> constexpr bool is_string_literal_v{false};

template <size_t Len>
constexpr bool is_string_literal_v<StringLiteral<Len>>{true};

template <typename T>
concept SomeStringLiteral = is_string_literal_v<T>;

template <SomeStringLiteral auto Lit, std::size_t... Indexes>
constexpr auto explode_impl(std::index_sequence<Indexes...>)
    -> ::templeano::detail::CharSeq<Lit.value[Indexes]...> {
  return {};
}

template <SomeStringLiteral auto Lit> constexpr auto explode() {
  return explode_impl<Lit>(std::make_index_sequence<Lit.Len - 1>());
}

template <SomeStringLiteral auto Lit> constexpr auto get_encoder() {
  return encoder_from_charseq(explode<Lit>());
}

namespace udl {
template <StringLiteral SL> constexpr auto operator""_cseq() {
  return explode<SL>();
}

template <StringLiteral SL> constexpr auto operator""_digits() {
  return explode<SL>();
}

template <StringLiteral SL> constexpr auto operator""_pes() {
  return encoder_from_charseq(explode<SL>());
}
} // namespace udl
} // namespace templeano::helpers
#endif // TEMPLEANO_POSITIONAL_DSL_HELPERS_HPP
