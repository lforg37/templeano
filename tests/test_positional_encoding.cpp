
#include "templeano/helpers.hpp"
#include "templeano/positional.hpp"
#include "templeano/positional_dsl_helpers.hpp"
#include "test_common/utils.hpp"

using namespace templeano;
using test_common::utils::is_remcvref_same_v;
using namespace helpers::udl;

using BinaryEncodingScheme =
    PositionalEncodingScheme<helpers::PeanoEncoding<2>>;
using DecimalEncodingScheme =
    PositionalEncodingScheme<helpers::PeanoEncoding<10>>;

// Declare a valid encoding
BinaryEncodingScheme::Encoding<Zero, One> test{};

constexpr auto decimal_42 =
    DecimalEncodingScheme::encode(helpers::PeanoEncoding<42>{});
constexpr auto decimal_8 =
    DecimalEncodingScheme::encode(helpers::PeanoEncoding<8>{});
static_assert(is_remcvref_same_v<
              decltype(decimal_42),
              DecimalEncodingScheme::Encoding<helpers::PeanoEncoding<2>,
                                              helpers::PeanoEncoding<4>>>);
static_assert(is_remcvref_same_v<
              decltype(decimal_8),
              DecimalEncodingScheme::Encoding<helpers::PeanoEncoding<8>>>);
constexpr auto decimal_50 = decimal_42 + decimal_8;
static_assert(is_remcvref_same_v<
              decltype(decimal_50),
              DecimalEncodingScheme::Encoding<helpers::PeanoEncoding<0>,
                                              helpers::PeanoEncoding<5>>>);

using TernaryEncoder = SymEncoder<'0', '1', '2'>;
static_assert(TernaryEncoder::symbol_for_value<One> == '1');
static_assert(std::is_same_v<TernaryEncoder::value_for_symbol<'0'>, Zero>);
constexpr auto fancy_binary_notation = L"v^"_pes;
constexpr auto fancy_binary_43 = fancy_binary_notation.decode(L"^v^v^^"_digits);
static_assert(is_remcvref_same_v<decltype(fancy_binary_43),
                                 decltype(fancy_binary_notation.scheme.encode(
                                     helpers::PeanoEncoding<43>{}))>);

void decimal_encoding_tests() {
  constexpr auto decimal = L"0123456789"_pes;
  constexpr auto d_0 = decimal.decode(L"0"_digits);
  constexpr auto d_1 = decimal.decode(L"1"_digits);
  constexpr auto d_236 = decimal.decode(L"236"_digits);
  constexpr auto d_764 = decimal.decode(L"764"_digits);
  constexpr auto d_999 = decimal.decode(L"999"_digits);
  constexpr auto d_1000 = decimal.decode(L"1000"_digits);
  static_assert(is_remcvref_same_v<decltype(d_236),
                                   decltype(DecimalEncodingScheme::encode(
                                       helpers::PeanoEncoding<236>{}))>);

  constexpr auto add_236_764 = d_236 + d_764;
  static_assert(add_236_764 == d_1000);
  static_assert(d_1000 - d_764 == d_236);
  static_assert(d_1000 - d_236 == d_764);
  static_assert(d_764 - d_236 == decimal.decode(L"528"_digits));
  static_assert(d_1000 - d_236 == d_764);
  static_assert(d_1000 - d_0 == d_1000);
  static_assert(d_1000 - d_1 == d_999);
}

void check_ternary_subtraction() {
  constexpr auto ternary = L"012"_pes;
  constexpr auto zero = ternary.decode(L"0"_digits);
  constexpr auto two = ternary.decode(L"2"_digits);
  constexpr auto one = ternary.decode(L"1"_digits);
  constexpr auto three = ternary.decode(L"10"_digits);
  constexpr auto six = ternary.decode(L"20"_digits);
  constexpr auto seven = ternary.decode(L"21"_digits);
  constexpr auto nine = ternary.decode(L"100"_digits);
  static_assert(zero - zero == zero);
  static_assert(one - zero == one);
  static_assert(two - zero == two);
  static_assert(two - one == one);
  static_assert(two - two == zero);
  static_assert(three - zero == three);
  static_assert(three - one == two);
  static_assert(three - two == one);
  static_assert(three - three == zero);
  static_assert(six - three == three);
  static_assert(nine - three == six);
  static_assert(nine - two == seven);
}
