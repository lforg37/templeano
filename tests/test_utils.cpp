#include <type_traits>

#include "templeano/peano.hpp"
#include "templeano/utils.hpp"
using namespace templeano;
using utils::Seq;

void check_sequence_align() {
  using utils::detail::Filler;
  constexpr Seq<int, double> a{};
  constexpr Seq<float> b{};
  constexpr auto c = utils::rfill_to_match<Filler>(b, a);
  static_assert(
      std::is_same_v<std::remove_const_t<decltype(c)>, Seq<float, Filler>>);
  constexpr auto d = utils::rfill_to_match<Filler>(a, b);
  static_assert(std::is_same_v<decltype(a), decltype(d)>);
}

namespace {
template <PeanoInteger PI> struct MyHolder {
  template <PeanoInteger Other> constexpr auto operator+(Other) const {
    return utils::wrap_res_next(PI{}, MyHolder<add_t<PI, Other>>{});
  }
  static constexpr PI held{};
};
} // namespace

void check_accumulator() {
  constexpr auto wrapper = utils::wrap_in_accumulator(MyHolder<Zero>{});
  constexpr auto res = wrapper + Successor<One>{};
  static_assert(
      std::is_same_v<std::remove_const_t<decltype(res.result)>, Seq<Zero>>);
  static_assert(res.wrapped.held == Successor<One>{});
}
