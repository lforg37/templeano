#ifndef TEMPLEANO_UTILS_HPP
#define TEMPLEANO_UTILS_HPP
#include <type_traits>

namespace templeano::utils {
namespace detail {

template <typename... SeqTypes> struct SeqSpec;

template <> struct SeqSpec<> {
  template <typename Predicate> using get_type_with_property = void;
};

template <typename SeqHead, typename... SeqTypes>
struct SeqSpec<SeqHead, SeqTypes...> {
  template <typename Predicate>
  using get_type_with_property =
      std::conditional_t<Predicate::template CheckFor<SeqHead>::value, SeqHead,
                         typename SeqSpec<SeqTypes...>::
                             template get_type_with_property<Predicate>>;
  using Head = SeqHead;
  static constexpr Head head{};
  template <typename Predicate>
  constexpr get_type_with_property<Predicate>
  get_for_predicate(Predicate) const {
    return {};
  }
};
} // namespace detail

template <typename... SeqTypes>
struct Seq : public detail::SeqSpec<SeqTypes...> {
  template <typename T>
  static constexpr auto prepend(T) -> Seq<T, SeqTypes...> {
    return {};
  }
  template <typename T> static constexpr auto append(T) -> Seq<SeqTypes..., T> {
    return {};
  }
  template <typename... OtherSeqT>
  static constexpr auto concat(Seq<OtherSeqT...>)
      -> Seq<SeqTypes..., OtherSeqT...> {
    return {};
  }
};

} // namespace templeano::utils

#endif // TEMPLEANO_UTILS_HPP
