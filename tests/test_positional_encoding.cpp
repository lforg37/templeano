
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

constexpr auto classical_decimal_notation = L"0123456789"_pes;
constexpr auto classical_decimal_236 =
    classical_decimal_notation.decode(L"236"_digits);
constexpr auto classical_decimal_764 =
    classical_decimal_notation.decode(L"764"_digits);
constexpr auto classical_decimal_1000 =
    classical_decimal_notation.decode(L"1000"_digits);
static_assert(is_remcvref_same_v<decltype(classical_decimal_236),
                                 decltype(DecimalEncodingScheme::encode(
                                     helpers::PeanoEncoding<236>{}))>);

constexpr auto add_236_764 = classical_decimal_236 + classical_decimal_764;
static_assert(is_remcvref_same_v<decltype(classical_decimal_1000),
                                 decltype(add_236_764)>);
