#pragma once

#include <bit>
#include <cstddef>
#include <cstdint>

namespace jw2::random {

// Credit to KoneLinx https://gist.github.com/KoneLinx/d3601597248bed423daf1a7cf7bd9533
constexpr uint32_t hash(uint32_t in) {
    constexpr uint32_t r[]{0xdf15236c,
                           0x16d16793,
                           0x3a697614,
                           0xe0fe08e4,
                           0xa3a53275,
                           0xccc10ff9,
                           0xb92fae55,
                           0xecf491de,
                           0x36e86773,
                           0x0ed24a6a,
                           0xd7153d80,
                           0x84adf386,
                           0x17110e76,
                           0x6d411a6a,
                           0xcbd41fed,
                           0x4b1d6b30};
    uint32_t out{in ^ r[in & 0xF]};
    out ^= std::rotl(in, 020) ^ r[(in >> 010) & 0xF];
    out ^= std::rotl(in, 010) ^ r[(in >> 020) & 0xF];
    out ^= std::rotr(in, 010) ^ r[(in >> 030) & 0xF];
    return out;
}

template <size_t N>
constexpr uint32_t hash(char const (&str)[N]) {
    uint32_t h{};
    for (uint32_t i{}; i < N; ++i) h ^= uint32_t(str[i]) << (i % 4 * 8);
    return hash(h);
}

template <std::size_t N>
constexpr uint32_t constexpr_rand_impl(char const (&file)[N], uint32_t line, uint32_t column = 0x8dc97987) {
    return hash(hash(__TIME__) ^ hash(file) ^ hash(line) ^ hash(column));
}

#define JW2_CONSTEXPR_RANDOM jw2::random::constexpr_rand_impl(__FILE__, __LINE__)

}  // namespace jw2::random
