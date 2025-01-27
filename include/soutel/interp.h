/******************************************************************************
Copyright (c) 2023-2025 Valerio Orlandini

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/

#ifndef INTERP_H_
#define INTERP_H_

#include <algorithm>
#include <cmath>
#include <vector>

#if __cplusplus >= 202002L
#include<concepts>
#endif

namespace soutel
{

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
inline TSample linip(const TSample &a, const TSample &b, const TSample &t)
{
    return a * ((TSample)1.0 - t) + b * t;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
inline TSample cosip(const TSample &a, const TSample &b, const TSample &t)
{
    TSample interp = ((TSample)1.0 - cos(t * (TSample)M_PI)) * (TSample)0.5;

    return a * ((TSample)1.0 - interp) + b * interp;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
std::vector<TSample> resize_chunk(const std::vector<TSample> &chunk, const unsigned int &new_size)
{
    if (new_size == chunk.size())
    {
        return chunk;
    }
    TSample ratio = (TSample)std::max((int)chunk.size() - 1, 1) / (TSample)new_size;
    std::vector<TSample> output(new_size, 0.0);

    for (auto i = 1; i < new_size; i++)
    {
        TSample in_pos = (TSample)i * ratio;
        int in_a = (int)floor(in_pos);
        int in_b = (int)ceil(in_pos) % chunk.size();
        TSample in_t = in_pos - (TSample)in_a;
        output.at(i) = cosip(chunk.at(in_a), chunk.at(in_b), in_t);
    }

    return output;
}

}

#endif // INTERP_H_
