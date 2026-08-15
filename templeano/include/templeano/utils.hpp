#ifndef TEMPLEANO_UTILS_HPP
#define TEMPLEANO_UTILS_HPP
#include <type_traits>

namespace templeano::utils {

struct NotFound {};

using not_found_t = NotFound;
namespace detail {

template <typename... SeqTypes> struct SeqSpec;
template <> struct SeqSpec<> {
  static constexpr bool is_empty{true};
  using Head = not_found_t;
};

template <typename SeqHead, typename... SeqTypes>
struct SeqSpec<SeqHead, SeqTypes...> {
  using Head = SeqHead;
  static constexpr Head head{};
  static constexpr bool is_empty{false};
};

template <typename T>
concept TypePredicate = requires {
  typename T::template CheckFor<int>;
  requires std::is_base_of_v<std::true_type,
                             typename T::template CheckFor<int>> ||
               std::is_base_of_v<std::false_type,
                                 typename T::template CheckFor<int>>;
};

template <typename... SeqTypes> struct SeqIterator;
template <> struct SeqIterator<> {
  template <TypePredicate Predicate> using get_type_with_property = not_found_t;
};

template <typename SeqHead, typename... SeqTypes>
struct SeqIterator<SeqHead, SeqTypes...> {
  template <TypePredicate Predicate>
  using get_type_with_property =
      std::conditional_t<Predicate::template CheckFor<SeqHead>::value, SeqHead,
                         typename SeqIterator<SeqTypes...>::
                             template get_type_with_property<Predicate>>;
};
} // namespace detail

template <typename... SeqTypes>
  requires((!std::is_same_v<SeqTypes, not_found_t> && ...))
struct Seq : public detail::SeqSpec<SeqTypes...> {
private:
  using Base = detail::SeqSpec<SeqTypes...>;

public:
  template <typename T> using prepend_t = Seq<T, SeqTypes...>;
  template <typename T> static constexpr auto prepend(T) -> prepend_t<T> {
    return {};
  }
  template <typename T> using append_t = Seq<SeqTypes..., T>;
  template <typename T> static constexpr auto append(T) -> append_t<T> {
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

namespace detail {
template <auto... Values> struct AllDistinctHelper;

template <auto T> struct AllDistinctHelper<T> {
  static constexpr bool all_unique{true};
};

template <auto HeadVal, auto... Values>
struct AllDistinctHelper<HeadVal, Values...> {
  static constexpr bool is_head_unique = ((HeadVal != Values) && ...);
  static constexpr bool all_unique =
      is_head_unique && AllDistinctHelper<Values...>::all_unique;
};
} // namespace detail

template <auto Head, auto... Values>
constexpr bool all_distinct =
    detail::AllDistinctHelper<Head, Values...>::all_unique;

template <typename HeadT, typename... SeqRest> struct SeqSplitter {
  using Head = HeadT;
  using Rest = Seq<SeqRest...>;
  static constexpr Head head{};
  static constexpr Rest rest{};
};

template <typename Head, typename... SeqRest>
constexpr auto split_seq(Seq<Head, SeqRest...>)
    -> SeqSplitter<Head, SeqRest...> {
  return {};
}

template <detail::TypePredicate Predicate, typename... SeqTypes>
constexpr auto find_type_with_property(Seq<SeqTypes...> seq, Predicate) ->
    typename detail::SeqIterator<SeqTypes...>::template get_type_with_property<
        Predicate> {
  return {};
}

template <detail::TypePredicate Predicate, typename Seq>
using find_type_with_property_t =
    std::remove_cvref_t<decltype(find_type_with_property(Seq{}, Predicate{}))>;
} // namespace templeano::utils

#endif // TEMPLEANO_UTILS_HPP
