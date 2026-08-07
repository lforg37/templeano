#include "templeano/helpers.hpp"
#include "test_common/utils.hpp"

using namespace templeano;
using test_common::utils::is_remcvref_same_v;

// Addition
constexpr auto five_plus_three =
    helpers::PeanoEncoding<5>{} + helpers::PeanoEncoding<3>{};
static_assert(
    is_remcvref_same_v<decltype(five_plus_three), helpers::PeanoEncoding<8>>);

// Comparison
static_assert(is_leq_v<Zero, helpers::PeanoEncoding<8>>);
static_assert(!is_leq_v<helpers::PeanoEncoding<8>, Zero>);
static_assert(is_leq_v<helpers::PeanoEncoding<7>, helpers::PeanoEncoding<8>>);
static_assert(!is_leq_v<helpers::PeanoEncoding<8>, helpers::PeanoEncoding<7>>);
static_assert(is_leq_v<helpers::PeanoEncoding<17>, helpers::PeanoEncoding<28>>);
static_assert(
    !is_leq_v<helpers::PeanoEncoding<28>, helpers::PeanoEncoding<17>>);
static_assert(is_leq_v<helpers::PeanoEncoding<17>, helpers::PeanoEncoding<17>>);
static_assert(is_lower_v<Zero, helpers::PeanoEncoding<8>>);
static_assert(!is_lower_v<helpers::PeanoEncoding<8>, Zero>);
static_assert(is_lower_v<helpers::PeanoEncoding<7>, helpers::PeanoEncoding<8>>);
static_assert(
    !is_lower_v<helpers::PeanoEncoding<8>, helpers::PeanoEncoding<7>>);
static_assert(
    is_lower_v<helpers::PeanoEncoding<17>, helpers::PeanoEncoding<28>>);
static_assert(
    !is_lower_v<helpers::PeanoEncoding<28>, helpers::PeanoEncoding<17>>);
static_assert(
    !is_lower_v<helpers::PeanoEncoding<17>, helpers::PeanoEncoding<17>>);

// Subtraction
static_assert(zero - zero == zero);
static_assert(one - zero == one);
static_assert(five_plus_three - one == helpers::PeanoEncoding<7>{});

// Product
static_assert(is_remcvref_same_v<Zero, decltype(zero * zero)>);
static_assert(is_remcvref_same_v<Zero, decltype(zero * one)>);
static_assert(is_remcvref_same_v<Zero, decltype(one * zero)>);
static_assert(is_remcvref_same_v<Zero, decltype(zero * five_plus_three)>);
static_assert(is_remcvref_same_v<Zero, decltype(five_plus_three * zero)>);
static_assert(is_remcvref_same_v<One, decltype(one * one)>);
static_assert(is_remcvref_same_v<decltype(five_plus_three),
                                 decltype(five_plus_three * one)>);
static_assert(is_remcvref_same_v<decltype(five_plus_three),
                                 decltype(one * five_plus_three)>);
static_assert(is_remcvref_same_v<helpers::PeanoEncoding<64>,
                                 decltype(five_plus_three * five_plus_three)>);
static_assert(is_remcvref_same_v<helpers::PeanoEncoding<42>,
                                 decltype(helpers::PeanoEncoding<6>{} *
                                          helpers::PeanoEncoding<7>{})>);

// Division
static_assert(one / one == one);
static_assert(zero / one == zero);
static_assert((helpers::PeanoEncoding<15>{} / helpers::PeanoEncoding<5>{}) ==
              helpers::PeanoEncoding<3>{});
static_assert((helpers::PeanoEncoding<17>{} / helpers::PeanoEncoding<5>{}) ==
              helpers::PeanoEncoding<3>{});

// Remainder
static_assert(one % one == zero);
static_assert(zero % one == zero);
static_assert((helpers::PeanoEncoding<15>{} % helpers::PeanoEncoding<5>{}) ==
              zero);
static_assert((helpers::PeanoEncoding<17>{} % helpers::PeanoEncoding<5>{}) ==
              helpers::PeanoEncoding<2>{});
