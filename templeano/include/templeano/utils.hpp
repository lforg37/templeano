#ifndef TEMPLEANO_UTILS_HPP
#define TEMPLEANO_UTILS_HPP
#include <type_traits>

namespace templeano::utils {

struct NotFound {};

using not_found_t = NotFound;
namespace detail {

template <typename... SeqTypes> struct SeqSpec;

template <> struct SeqSpec<> {
  template <typename Predicate> using get_type_with_property = not_found_t;
  static constexpr bool is_empty{true};
  using Head = not_found_t;
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
  static constexpr bool is_empty{false};
};
} // namespace detail

template <typename... SeqTypes>
  requires((!std::is_same_v<SeqTypes, not_found_t> && ...))
struct Seq : public detail::SeqSpec<SeqTypes...> {
private:
  using Base = detail::SeqSpec<SeqTypes...>;

public:
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

  template <typename Default>
  using head_type_or_t =
      std::conditional_t<Base::is_empty, Default, typename Base::Head>;
};

} // namespace templeano::utils

#endif // TEMPLEANO_UTILS_HPP
