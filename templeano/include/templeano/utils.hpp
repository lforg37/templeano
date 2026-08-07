#ifndef TEMPLEANO_UTILS_HPP
#define TEMPLEANO_UTILS_HPP
#include <type_traits>

namespace templeano::utils {
template <typename... SeqTypes> struct Seq;

template <> struct Seq<> {
  template <typename Predicate> using get_type_with_property = void;
};

template <typename SeqHead, typename... SeqTypes>
struct Seq<SeqHead, SeqTypes...> {
  template <typename Predicate>
  using get_type_with_property = std::conditional_t<
      Predicate::template CheckFor<SeqHead>::value, SeqHead,
      typename Seq<SeqTypes...>::template get_type_with_property<Predicate>>;
  using Head = SeqHead;
  static constexpr Head head{};
  template <typename Predicate>
  constexpr get_type_with_property<Predicate>
  get_for_predicate(Predicate) const {
    return {};
  }
};

template <typename... SeqL, typename... SeqR>
constexpr Seq<SeqL..., SeqR...> concat(Seq<SeqL...>, Seq<SeqR...>) {
  return {};
}

} // namespace templeano::utils

#endif // TEMPLEANO_UTILS_HPP
