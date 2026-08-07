#include "templeano/helpers.hpp"
#include "templeano/prime_factor.hpp"

using namespace templeano;

namespace {
template <std::size_t val>
using prime_factors_h = prime_factors_t<helpers::PeanoEncoding<val>>;

template <std::size_t factor, std::size_t count>
using factor_holder_h =
    FactorHolder<helpers::PeanoEncoding<factor>, helpers::PeanoEncoding<count>>;

} // namespace

static_assert(std::is_same_v<prime_factors_h<0>, FactorSeq<>>);
static_assert(std::is_same_v<prime_factors_h<1>, FactorSeq<>>);
static_assert(
    std::is_same_v<prime_factors_h<2>, FactorSeq<factor_holder_h<2, 1>>>);
static_assert(
    std::is_same_v<prime_factors_h<8>, FactorSeq<factor_holder_h<2, 3>>>);
static_assert(
    std::is_same_v<prime_factors_h<15>,
                   FactorSeq<factor_holder_h<3, 1>, factor_holder_h<5, 1>>>);
static_assert(
    std::is_same_v<prime_factors_h<45>,
                   FactorSeq<factor_holder_h<3, 2>, factor_holder_h<5, 1>>>);
static_assert(
    std::is_same_v<prime_factors_h<187>,
                   FactorSeq<factor_holder_h<11, 1>, factor_holder_h<17, 1>>>);
