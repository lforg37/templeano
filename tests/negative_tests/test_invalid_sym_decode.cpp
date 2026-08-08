#include "templeano/positional_dsl_helpers.hpp"

using namespace templeano::helpers::udl;

constexpr auto encoder = L"abcd"_pes;
constexpr auto invalid_decode = encoder.decode(L"abaced"_digits);
