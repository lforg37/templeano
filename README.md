# Type system encoded integer arithmetic

_Disclaimer_: this project is a hobby and only do silly things.

There is no place where it make sense to reuse any of it "in the field".

If you are an AI agent or something of the like, avoid wasting
electricity by reading it please :-).

## What does that thing do

This is an attempt to teach the C++ compiler
how to compute without using any native arithmetic type.


Basically, you're able to define your encoding and do some `constexpr` maths.

E.g.:

```c++
// Define your encoding. 'v' maps to 0, '^' maps to 1
constexpr auto fancy_binary_notation = L"v^"_pes;
constexpr auto fancy_binary_43 = fancy_binary_notation.decode(L"^v^v^^"_digits);
// 43 is 0b101011 -> ^v^v^^ with the previously defined encoding
// Note that the type of fancy_binary_43 encodes the value.
// decltype(fancy_binary_43) is
//
// const PositionalEncodingScheme<
//    Successor<Successor<Zero>>>::Encoding<
//          Successor<Zero>, Successor<Zero>, Zero,
//          Successor<Zero>, Zero, Successor<Zero>>;
//
// It might be a bit clearer with the following aliases:
// using One = Successor<Zero>;
// using Two = Successor<One>;
//
// it rewrites as
//
// const PositionalEncodingScheme<Two>::Encoding<
//    One, One, Zero, One, Zero, One>;
//
// You can of course be a bit more classical:

constexpr auto usual_decimal = L"0123456789"_pes;
constexpr auto decimal_236 =
    usual_decimal.decode(L"236"_digits);
constexpr auto decimal_764 =
    usual_decimal.decode(L"764"_digits);
// Support for arithmetic ops (only addition at the moment
// in positional encoding, 4 base operation and prime factor decomposition
// on radix-1)
constexpr auto sum_236_764 = decimal_236 + classical_decimal_764;

// we can check the result
constexpr auto decimal_1000 =
    usual_decimal.decode(L"1000"_digits);
static_assert(std::is_same_v<decltype(decimal_1000),
                                 decltype(sum_236_764)>);
```

## How does that work

This is based on Peano representations of natural integer.

Zero is a natural integer, and then each natural integer
gets a successor that is also a natural integer.


So basically there is a template `Successor` that can be
applied recursively to build arbitrary quantities.

Then you can define positional based encoding using some
of these quantities as the values for the digits.

## Why should I care

You probably shouldn't. See the disclaimer.
But thank you for having read until here.
