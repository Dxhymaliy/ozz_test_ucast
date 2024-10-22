#pragma once

#include <numeric>
#include <random>

#include "ozz/types.hpp"

namespace OZZ_CORE::utils{

using u32    = uint_least32_t; 
using engine = std::mt19937;

struct unique_values
{
    unique_values() noexcept
        : gen(os_seed())
    {}
    ~unique_values() noexcept = default;

    void rebase() noexcept
    {
        std::iota(arr.begin(), arr.begin() + delimeter, 0);
        idx = delimeter;
    }

    void generate(int64_t* ptr, size_t size)
    {
        engine gen(os_seed());
        size_t n{0}; 

        while(n < size)
        {
            --idx;
            std::uniform_int_distribution< u32 > distribute(0, idx);
            auto v = distribute( gen );
            std::swap(arr[idx], arr[v]);
            ptr[n] += arr[idx];
            ++n;
            if(idx == 0)
                rebase();
        }
    }

    std::random_device  os_seed;
    engine              gen;
    uint                delimeter{1'000};
    std::array<int64_t,
              100'000>  arr;
    int64_t             idx{0};
};

}
