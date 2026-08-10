#ifndef TEMPLEANO_POSITIONAL_HPP
#define TEMPLEANO_POSITIONAL_HPP

#include "templeano/peano.hpp"
#include "templeano/utils.hpp"
#include <type_traits>
namespace templeano {

namespace detail {
template <typename Number, typename Scheme>
concept DigitFor = Scheme::template is_digit_v<Number>;

template <typename Number, typename Scheme>
concept DigitOrRadixFor = Scheme::template is_digit_v<Number> ||
                          std::is_same_v<Number, typename Scheme::Radix>;

template <PeanoInteger... PIs> constexpr bool check_last_is_not_zero(PIs...) {
  bool res{true};
  ([&res](bool val) { res = val; }(!is_eq_v<PIs, Zero>), ...);
  return res;
}

template <typename EncodingType, typename Scheme>
concept NumberIn = Scheme::template is_encoding_v<EncodingType>;

} // namespace detail

template <typename Candidate>
concept Radixable = is_lower_v<One, Candidate>;

template <typename T>
concept Borrow = std::is_same_v<T, Zero> || std::is_same_v<T, One>;

template <typename T>
concept Carry = std::is_same_v<T, Zero> || std::is_same_v<T, One>;

template <Radixable RadixT> class PositionalEncodingScheme {
public:
  using Radix = RadixT;

private:
  using MaxDigit = sub_t<Radix, One>;
  using PES = PositionalEncodingScheme;
  template <detail::DigitFor<PES>... Digits>
  using DigitSequence = utils::Seq<Digits...>;
  template <typename T> struct GetHeadHelper;

  template <typename... Digits> struct GetHeadHelper<DigitSequence<Digits...>> {
    using type =
        typename DigitSequence<Digits...>::template head_type_or_t<Zero>;
  };

  template <typename T> using lsb_t = typename GetHeadHelper<T>::type;

  static constexpr auto successor(DigitSequence<>) {
    return DigitSequence<One>{};
  }

  template <detail::DigitFor<PES> LSB, detail::DigitFor<PES>... Digits>
  static constexpr auto successor(DigitSequence<LSB, Digits...>) {
    if constexpr (is_eq_v<LSB, MaxDigit>) {
      return successor(DigitSequence<Digits...>{}).prepend(zero);
    } else {
      return DigitSequence<Successor<LSB>, Digits...>{};
    }
  }

  static constexpr auto encode_impl(Zero, auto DigitSequence) {
    return DigitSequence;
  }

  template <PeanoInteger PI>
  static constexpr auto encode_impl(Successor<PI>, auto DigitSequence) {
    return encode_impl(PI{}, successor(DigitSequence));
  }

public:
  template <PeanoInteger PI>
  static constexpr bool is_digit_v = is_lower_v<PI, Radix>;
  template <detail::DigitFor<PES> LSD, detail::DigitFor<PES>... Digits>
  struct Encoding : public DigitSequence<LSD, Digits...> {
    using Scheme = PES;
    static_assert(detail::check_last_is_not_zero(Digits{}...));
    constexpr DigitSequence<LSD, Digits...> sequence() { return {}; }
  };

private:
  template <detail::DigitFor<PES>... Digits>
  static constexpr Encoding<Digits...> encoding_from(DigitSequence<Digits...>) {
    return {};
  }

public:
  template <PeanoInteger PI> static constexpr auto encode(PI pi) {
    return encoding_from(encode_impl(pi, DigitSequence<Zero>{}));
  };

  using zero_t = Encoding<Zero>;

  template <typename T> static constexpr bool is_encoding_v{false};
  template <PeanoInteger... Digits>
  static constexpr bool is_encoding_v<Encoding<Digits...>>{true};

private:
  template <detail::DigitFor<PES> L, detail::DigitFor<PES> R, Carry C = Zero>
  using add_res_unencoded_t =
      std::conditional_t<std::is_same_v<C, Zero>, add_t<L, R>,
                         Successor<add_t<L, R>>>;

  template <detail::DigitFor<PES> LSBT, detail::DigitFor<PES> MSBT>
  struct DigitPair {
    using LSB = LSBT;
    using MSB = MSBT;
    static constexpr LSB lsb{};
    static constexpr MSB msb{};
  };

  template <typename LSB, typename MSB>
  static constexpr DigitPair<LSB, MSB>
  sequence_to_pair(DigitSequence<LSB, MSB>) {
    return {};
  }

  template <typename LSB>
  static constexpr DigitPair<LSB, Zero> sequence_to_pair(DigitSequence<LSB>) {
    return {};
  }

  template <detail::DigitFor<PES> ResultT, Borrow BT> struct SubHelper {
    using Result = ResultT;
    using Borrow = BT;
  };

  // We should only be adding a borrow to non borrow holding result
  template <detail::DigitFor<PES> Digit>
  static constexpr auto add_borrow(SubHelper<Digit, Zero>)
      -> SubHelper<Digit, One> {
    return {};
  }

  // Terminal case, nothing to subtract
  template <detail::DigitFor<PES> L>
  static constexpr auto sub_digit_impl(L, Zero) -> SubHelper<L, Zero> {
    return {};
  }

  // This is where the borrow arrives
  // When subtracting non zero value from zero, borrow is
  // added and subtraction continues cycling back from 9
  template <PeanoInteger R>
    requires detail::DigitOrRadixFor<Successor<R>, PES>
  static constexpr auto sub_digit_impl(Zero, Successor<R>) {
    return add_borrow(sub_digit_impl(MaxDigit{}, R{}));
  }

  // General case, just unwrap the successors
  template <PeanoInteger L, PeanoInteger R>
    requires detail::DigitFor<Successor<L>, PES> &&
             detail::DigitOrRadixFor<Successor<R>, PES>
  static constexpr auto sub_digit_impl(Successor<L>, Successor<R>) {
    return sub_digit_impl(L{}, R{});
  }

  // TODO Add type alias for the sub digit and propagate
  // See if we couldn't be having some kind of helper to iterate because
  // this is going to be very similar to addition

  template <detail::DigitFor<PES> L, detail::DigitFor<PES> R, Carry C = Zero>
  using add_res_t = std::remove_cvref_t<decltype(sequence_to_pair(
      encode(add_res_unencoded_t<L, R, C>{})))>;

  template <detail::DigitFor<PES> LLSD, detail::DigitFor<PES>... LDigits,
            detail::DigitFor<PES> RLSD, detail::DigitFor<PES>... RDigits,
            Carry C = Zero>
  static constexpr auto add_impl(DigitSequence<LLSD, LDigits...>,
                                 DigitSequence<RLSD, RDigits...>, C = {}) {
    using DigitAddRes = add_res_t<LLSD, RLSD, C>;
    return add_impl(DigitSequence<LDigits...>{}, DigitSequence<RDigits...>{},
                    DigitAddRes::msb)
        .prepend(DigitAddRes::lsb);
  }

  template <detail::DigitFor<PES> LLSD, detail::DigitFor<PES>... LDigits,
            Carry C = Zero>
  static constexpr auto add_impl(DigitSequence<LLSD, LDigits...>,
                                 DigitSequence<>, C = {}) {
    using DigitAddRes = add_res_t<LLSD, Zero, C>;
    return add_impl(DigitSequence<LDigits...>{}, DigitSequence<>{},
                    DigitAddRes::msb)
        .prepend(DigitAddRes::lsb);
  }

  template <detail::DigitFor<PES> RLSD, detail::DigitFor<PES>... RDigits,
            Carry C = Zero>
  static constexpr auto add_impl(DigitSequence<>,
                                 DigitSequence<RLSD, RDigits...>, C = {}) {
    using DigitAddRes = add_res_t<Zero, RLSD, C>;
    return add_impl(DigitSequence<>{}, DigitSequence<RDigits...>{},
                    DigitAddRes::msb)
        .prepend(DigitAddRes::lsb);
  }

  template <Carry C = Zero>
  static constexpr auto add_impl(DigitSequence<>, DigitSequence<>, C = {}) {
    return std::conditional_t<std::is_same_v<C, Zero>, DigitSequence<>,
                              DigitSequence<One>>{};
  }

public:
  template <detail::DigitFor<PES>... LDigits, detail::DigitFor<PES>... RDigits>
  static constexpr auto add(Encoding<LDigits...> l, Encoding<RDigits...> r) {
    return encoding_from(add_impl(l.sequence(), r.sequence()));
  }
};

template <Radixable R>
using PositionalEncoding = PositionalEncodingScheme<R>::Encoding;

namespace detail {
template <typename T> constexpr bool is_positional_encoding_scheme_v{false};

template <Radixable R>
constexpr bool is_positional_encoding_scheme_v<PositionalEncodingScheme<R>>{
    true};

template <typename T>
constexpr bool is_positional_encoded_number_v{
    is_positional_encoding_scheme_v<typename T::Scheme> &&
    T::Scheme::template is_encoding_v<T>};
} // namespace detail

template <typename T>
concept PositionalScheme = detail::is_positional_encoding_scheme_v<T>;

template <typename T>
concept PositionalEncodedNumber = requires { typename T::Scheme; } &&
                                  detail::is_positional_encoded_number_v<T>;

template <typename Dependent, typename Scheme>
concept PositionalEncodedNumberRadix =
    detail::is_positional_encoding_scheme_v<Scheme> &&
    detail::is_positional_encoded_number_v<Dependent> &&
    std::is_same_v<Scheme, typename Dependent::Scheme>;

template <PositionalEncodedNumber L,
          PositionalEncodedNumberRadix<typename L::Scheme> R>
constexpr auto operator+(L l, R r) {
  using Scheme = typename L::Scheme;
  return Scheme::add(l, r);
}

namespace detail {
template <wchar_t Sym, PeanoInteger PI> struct DigitSymPair {
  using Digit = PI;
  static constexpr wchar_t symbol = Sym;
};

template <wchar_t... Syms>
using CharSeq = utils::Seq<std::integral_constant<wchar_t, Syms>...>;

template <PeanoInteger PI, typename... Elems>
constexpr auto encode_pairs(PI, utils::Seq<>, utils::Seq<Elems...> elems) {
  return elems;
}

template <PeanoInteger PI, wchar_t Head, wchar_t... Remain,
          typename... SeqElems>
constexpr auto
encode_pairs(PI current,
             utils::Seq<std::integral_constant<wchar_t, Head>,
                        std::integral_constant<wchar_t, Remain>...>
                 digits,
             utils::Seq<SeqElems...> partial) {
  return encode_pairs(successor(current), CharSeq<Remain...>{},
                      utils::Seq<DigitSymPair<Head, PI>, SeqElems...>{});
}

template <PeanoInteger PI> struct GetOnValue {
  template <typename Candidate> struct CheckFor : std::false_type {};

  template <wchar_t Sym>
  struct CheckFor<DigitSymPair<Sym, PI>> : std::true_type {};
};

template <wchar_t Sym> struct GetOnSym {
  template <typename Candidate> struct CheckFor {
    static constexpr bool value = Candidate::symbol == Sym;
  };
};
} // namespace detail

// Two imposed symbols to ensure we are at list in binary
// as radix 0 can't represent anything and radix 1 is just same as
// counting the number of successor
template <wchar_t DigitsSym0, wchar_t DigitsSym1, wchar_t... DigitsSyms>
  requires(utils::all_distinct<DigitsSym0, DigitsSym1, DigitsSyms...>)
struct SymEncoder {
private:
  static constexpr auto mapping_holder = detail::encode_pairs(
      Zero{}, detail::CharSeq<DigitsSym0, DigitsSym1, DigitsSyms...>{},
      utils::Seq<>{});
  using HolderType = std::remove_cvref_t<decltype(mapping_holder)>;

  static constexpr auto decode_impl(detail::CharSeq<>) {
    return utils::Seq<>{};
  }

  template <wchar_t ToDecodeHead, wchar_t... toDecodeRest>
  static constexpr auto
  decode_impl(detail::CharSeq<ToDecodeHead, toDecodeRest...>) {
    return decode_impl(detail::CharSeq<toDecodeRest...>{})
        .concat(utils::Seq<value_for_symbol<ToDecodeHead>>{});
  }

public:
  using Radix = Successor<typename decltype(mapping_holder)::Head::Digit>;
  using Scheme = PositionalEncodingScheme<Radix>;
  static constexpr Scheme scheme{};

private:
  template <PeanoInteger... PI>
  static constexpr auto encoding_from_seq(utils::Seq<PI...>)
      -> Scheme::template Encoding<PI...> {
    return {};
  }

  template <wchar_t Symbol>
  using value_pair_for_symbol =
      HolderType::template get_type_with_property<detail::GetOnSym<Symbol>>;

public:
  template <PeanoInteger PI>
  static constexpr wchar_t symbol_for_value =
      mapping_holder.get_for_predicate(detail::GetOnValue<PI>{}).symbol;

private:
  template <wchar_t symbol, PeanoInteger PI>
  PI static constexpr extract_value_from_type(
      detail::DigitSymPair<symbol, PI>) {
    return {};
  }

  static constexpr utils::not_found_t
  extract_value_from_type(utils::not_found_t) {
    return {};
  }

public:
  template <wchar_t Symbol>
  using value_for_symbol = std::remove_cvref_t<decltype(extract_value_from_type(
      value_pair_for_symbol<Symbol>{}))>;

  template <wchar_t Symbol>
  static constexpr bool is_valid_symbol =
      !std::is_same_v<value_for_symbol<Symbol>, utils::not_found_t>;

  template <wchar_t... Digits>
    requires((is_valid_symbol<Digits> && ...))
  static constexpr auto decode(detail::CharSeq<Digits...> digits) {
    return encoding_from_seq(decode_impl(digits));
  }
};

template <wchar_t... Syms>
constexpr SymEncoder<Syms...> encoder_from_charseq(detail::CharSeq<Syms...>) {
  return {};
}
} // namespace templeano
#endif // TEMPLEANO_POSITIONAL_HPP
