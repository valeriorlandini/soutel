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

#ifndef UTILS_H_
#define UTILS_H_

#include <algorithm>
#include <cmath>
#include <vector>

#if __cplusplus >= 202002L
#include<concepts>
#endif

namespace soutel
{

template <typename TSample>
inline TSample ftom(const TSample &freq, const TSample &a = (TSample)440.0)
{
    return (TSample)69.0 + (TSample)(12.0 * log2(abs((double)freq) / abs((double)a)));
}

template <typename TSample>
inline TSample mtof(const TSample &midi_note, const TSample &a = (TSample)440.0)
{
    return a * (TSample)pow(2.0, ((double)midi_note - 69.0) / 12.0);
}

template <typename TSample>
inline TSample scale(const TSample &in, const TSample &in_min, const TSample &in_max, const TSample &out_min, const TSample &out_max)
{
    if (in_min == in_max)
    {
        return (TSample)0.0;
    }
    
    return out_min + ((in - in_min) * (out_max - out_min) / (in_max - in_min));
}

template <typename TSample>
inline std::vector<TSample> zeropad(const std::vector<TSample> &input, const int &size, const bool &center = true)
{
    std::vector<TSample> output = input;
    zeropad_inplace(output, size, center);

    return output;
}

template <typename TSample>
inline void zeropad_inplace(std::vector<TSample> &input, const int &size, const bool &center = true)
{
    if (size > input.size())
    {
        if (center)
        {
            int zeros = size - input.size();

            if (zeros % 2)
            {
                zeros += 1;
            }

            input.insert(input.begin(), zeros / 2, (TSample)0.0);
        }

        input.resize(size, (TSample)0.0);
    }
}

template <typename TSample>
inline std::vector<TSample> zerophase(const std::vector<TSample> &input)
{
    std::vector<TSample> output = input;

    zerophase_inplace(output);

    return output;
}

template <typename TSample>
inline void zerophase_inplace(std::vector<TSample> &input)
{
    auto middle = input.begin() + input.size() / 2;

    std::rotate(input.begin(), middle, input.end());
}

template <typename TSample>
inline std::vector<TSample> zeropadphase(const std::vector<TSample> &input, const int &size)
{
    std::vector<TSample> output = zeropad(input, size);
    zerophase_inplace(output);

    return output;
}

template <typename TSample>
inline void zeropadphase_inplace(std::vector<TSample> &input, const int &size)
{
    zeropad_inplace(input, size);
    zerophase_inplace(input);
}
}

#endif // UTILS_H_
