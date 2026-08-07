#ifndef TEMPLEANO_PRIME_FACTORIZATION_HPP
#define TEMPLEANO_PRIME_FACTORIZATION_HPP
#include "templeano/peano.hpp"

namespace templeano {

template <PeanoInteger FactorT, PeanoInteger CountT> struct FactorHolder {
  using Factor = FactorT;
  using Count = CountT;
};

template <typename... Factors> struct FactorSeq {};

template <typename... Factors, PeanoInteger FactorT, PeanoInteger CountT>
constexpr auto operator+(FactorSeq<Factors...> seq,
                         FactorHolder<FactorT, CountT>) {
  if constexpr (std::is_same_v<CountT, Zero>) {
    return seq;
  } else {
    return FactorSeq<Factors..., FactorHolder<FactorT, CountT>>{};
  }
}

namespace detail {
template <PeanoInteger RemainsToFactorT, PeanoInteger CurrentFactorT,
          PeanoInteger CurrentFactorCountT>
struct Iteration {
  using RemainsToFactor = RemainsToFactorT;
  using CurrentFactor = CurrentFactorT;
  using CurrentFactorCount = CurrentFactorCountT;

  constexpr static bool remainer_is_zero_or_one =
      std::is_same_v<RemainsToFactor, Zero> ||
      std::is_same_v<RemainsToFactor, One>;
  constexpr static bool remains_to_factor_is_prime =
      is_lower_v<RemainsToFactor, square_t<CurrentFactor>>;
  constexpr static bool is_final =
      remainer_is_zero_or_one || remains_to_factor_is_prime;
};

using Two = Successor<One>;

template <PeanoInteger T>
using nextFactorCandidate =
    std::conditional_t<std::is_same_v<T, Two>, Successor<T>,
                       Successor<Successor<T>>>;

template <typename... Factors, typename CurIteration>
constexpr auto iterFactorization(FactorSeq<Factors...> seq, CurIteration) {
  using CurrentFactor = typename CurIteration::CurrentFactor;
  using CurrentFactorCount = typename CurIteration::CurrentFactorCount;
  using RemainsToFactor = typename CurIteration::RemainsToFactor;
  // Could be better split in final subsequence compute func...
  if constexpr (CurIteration::is_final) {
    if constexpr (CurIteration::remainer_is_zero_or_one) {
      if constexpr (std::is_same_v<CurrentFactorCount, Zero>) {
        return seq;
      } else {
        return seq + FactorHolder<CurrentFactor, CurrentFactorCount>{};
      }
    } else {
      if constexpr (std::is_same_v<CurrentFactor, RemainsToFactor>) {
        return seq +
               FactorHolder<CurrentFactor, Successor<CurrentFactorCount>>{};
      } else {
        return seq + FactorHolder<CurrentFactor, CurrentFactorCount>{} +
               FactorHolder<RemainsToFactor, One>{};
      }
    }
  } else {
    using DivT = div_t<RemainsToFactor, CurrentFactor>;
    using ModT = mod_t<RemainsToFactor, CurrentFactor>;
    if constexpr (std::is_same_v<ModT, Zero>) {
      return iterFactorization(
          seq, Iteration<DivT, CurrentFactor, Successor<CurrentFactorCount>>{});
    } else {
      return iterFactorization(
          seq + FactorHolder<CurrentFactor, CurrentFactorCount>{},
          Iteration<RemainsToFactor, nextFactorCandidate<CurrentFactor>,
                    Zero>{});
    }
  }
}

} // namespace detail

template <typename T>
auto prime_factors_v = detail::iterFactorization(
    FactorSeq<>{}, detail::Iteration<T, detail::Two, Zero>{});
template <typename T>
using prime_factors_t = std::remove_cvref_t<decltype(prime_factors_v<T>)>;
} // namespace templeano
#endif // TEMPLEANO_PRIME_FACTORIZATION_HPP
