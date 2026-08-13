#include "templeano/positional_dsl_helpers.hpp"

using namespace templeano::helpers::udl;

constexpr auto encoder = L"0123456789"_pes;
constexpr auto five = encoder.decode(L"5"_digits);
constexpr auto seven = encoder.decode(L"7"_digits);
constexpr auto res = five - seven;
