#ifndef TEST_COMMON_UTILS_HPP
#define TEST_COMMON_UTILS_HPP
#include <type_traits>
namespace templeano::test_common::utils {
template <typename L, typename R>
constexpr bool is_remcvref_same_v =
    std::is_same_v<std::remove_cvref_t<L>, std::remove_cvref_t<R>>;
}
#endif
