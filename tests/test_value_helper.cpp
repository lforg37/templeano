#include "templeano/helpers.hpp"
#include "templeano/peano.hpp"

using namespace templeano;
using namespace templeano::helpers;

static_assert(helpers::value(successor(successor(successor(successor(
                  successor(successor(successor(successor())))))))) == 8);

static_assert(std::is_same_v<PeanoEncoding<0>, Zero>);
static_assert(std::is_same_v<PeanoEncoding<4>,
                             Successor<Successor<Successor<Successor<Zero>>>>>);
