#ifndef TEMPLEANO_UTILS_HPP
#define TEMPLEANO_UTILS_HPP
#include <concepts>
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

template <typename Src, typename Dest> using mapper_t = Dest;

template <typename ReplaceWith, typename... SeqTypes>
constexpr auto replace_types_with(Seq<SeqTypes...>)
    -> Seq<mapper_t<SeqTypes, ReplaceWith>...> {
  return {};
}

// For some operations on sequence only the number of element is important.
// In that case the canonical equivalence class representant will only
// contain Filler.
struct Filler {};

template <typename... Pack> constexpr auto canonicalize(Seq<Pack...> seq) {
  return replace_types_with<Filler>(seq);
}

template <typename PrefixSeq, typename SuffixSeq>
struct SeqParts : std::true_type {
  using Prefix = PrefixSeq;
  using Suffix = SuffixSeq;
  static constexpr Prefix prefix{};
  static constexpr Suffix suffix{};
};

struct InvalidSplit : std::false_type {};
template <typename... OutPack> struct PrefixSplitter {
  template <typename... InnerPack>
  static constexpr auto split(Seq<OutPack..., InnerPack...>)
      -> SeqParts<Seq<OutPack...>, Seq<InnerPack...>> {
    return {};
  }
  template <typename... Pack>
  static constexpr auto split(Seq<Pack...>) -> InvalidSplit {
    return {};
  }
};

template <template <typename...> typename Mapping, typename... Pack>
constexpr auto reuse_pack_in(Seq<Pack...>) -> Mapping<Pack...> {
  return {};
}

constexpr auto get_prefix_splitter(auto seq) {
  return reuse_pack_in<PrefixSplitter>(seq);
}

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

template <typename T> constexpr bool is_seq_v{false};
template <typename... SeqTypes> constexpr bool is_seq_v<Seq<SeqTypes...>>{true};

template <typename T>
concept SeqType = is_seq_v<T>;

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

template <detail::TypePredicate Predicate, SeqType Seq>
using find_type_with_property_t =
    std::remove_cvref_t<decltype(find_type_with_property(Seq{}, Predicate{}))>;

template <typename FillWith, SeqType SourceSeq, SeqType ReferenceSeq>
constexpr auto rfill_to_match(SourceSeq source, ReferenceSeq reference) {
  constexpr auto canonicalized_source = detail::canonicalize(SourceSeq{});
  constexpr auto canonicalized_ref = detail::canonicalize(ReferenceSeq{});
  constexpr auto split =
      get_prefix_splitter(canonicalized_source).split(canonicalized_ref);
  if constexpr (std::is_same_v<std::remove_const_t<decltype(split)>,
                               detail::InvalidSplit>) {
    return source;
  } else {
    return source.concat(detail::replace_types_with<FillWith>(split.suffix));
  }
}

// TODO: add proper constraint on combinator
template <typename WrappedT, SeqType ResT, typename SeqCombinator>
struct AccumulationWrapper {
  using wrapped_t = WrappedT;
  using res_t = ResT;
  using seq_combinator_t = SeqCombinator;
  static constexpr wrapped_t wrapped{};
  static constexpr res_t result{};
  static constexpr seq_combinator_t combinator{};
};

namespace detail {
template <typename T> constexpr bool is_accumulation_wrapper{false};

template <typename WT, SeqType ST, typename SeqCombinator>
constexpr bool
    is_accumulation_wrapper<AccumulationWrapper<WT, ST, SeqCombinator>>{true};
} // namespace detail

template <typename T>
concept AccumulationWrapperType = detail::is_accumulation_wrapper<T>;

template <typename T>
concept ResultHolder = requires {
  typename T::res_t;
  typename T::next_t;
  requires std::same_as<std::remove_const_t<decltype(T::result)>,
                        typename T::res_t>;
  requires std::same_as<std::remove_const_t<decltype(T::next)>,
                        typename T::next_t>;
};

namespace detail {
template <typename Res, typename Next> struct ResHolder {
  using res_t = Res;
  using next_t = Next;
  static constexpr res_t result{};
  static constexpr next_t next{};
};
} // namespace detail

template <std::default_initializable Res, std::default_initializable Next>
constexpr auto wrap_res_next(Res, Next) -> detail::ResHolder<Res, Next> {
  return {};
}

template <typename T, typename Combinator, SeqType ST = Seq<>>
constexpr auto wrap_in_accumulator(T, Combinator, ST = {})
    -> AccumulationWrapper<T, ST, Combinator> {
  return {};
}

template <AccumulationWrapperType Left, typename Right>
  requires ResultHolder<decltype(Left::wrapped + std::declval<Right>())>
constexpr auto operator+(Left, Right) {
  constexpr auto add_res = Left::wrapped + Right{};
  constexpr auto seq_res =
      Left::combinator.combine(Left::result, add_res.result);
  return wrap_in_accumulator(add_res.next, Left::combinator, seq_res);
}

namespace combinator {
struct Append {
  template <SeqType Left, typename Right>
  static constexpr auto combine(Left l, Right) {
    return l.concat(Seq<Right>{});
  }
};

constexpr Append append{};
} // namespace combinator

} // namespace templeano::utils

#endif // TEMPLEANO_UTILS_HPP
