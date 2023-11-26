/******************************************************************************
Copyright (c) 2023 Valerio Orlandini

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

#ifndef DISTORTIONS_H_
#define DISTORTIONS_H_

#include <algorithm>
#include <cmath>

namespace soutel
{

template <class TSample>
inline TSample symmetrical_soft_clip(const TSample &sample,
                                     const TSample &threshold = (TSample)0.333)
{
    TSample abs_sample = std::abs(sample);
    TSample sample_out = (TSample)0.0;

    if (abs_sample < threshold)
    {
        sample_out = sample * (TSample)2.0;
    }
    else if (abs_sample >= threshold && abs_sample < ((TSample)2.0 * threshold))
    {
        sample_out = ((TSample)3.0 - std::pow(((TSample)2.0 -
                                               (TSample)3.0 * abs_sample), (TSample)2.0)) /
                     std::copysign((TSample)3.0, sample);
    }
    else
    {
        sample_out = std::copysign((TSample)1.0, sample);
    }

    return sample_out;
}

template <class TSample>
inline TSample exponential_distortion(const TSample &sample,
                                      const TSample &gain,
                                      const TSample &mix = (TSample)1.0)
{
    TSample q = sample * gain;
    TSample z = std::copysign((TSample)1.0, q) * (1.0 - std::exp((TSample)-1.0 * std::abs(q)));

    return mix * z + ((TSample)1.0 - mix) * sample;
}

template <class TSample>
inline TSample bitcrush(const TSample &sample, const TSample &bit_depth, const TSample &mix = 1.0)
{
    unsigned long in = (unsigned long)std::round(((TSample)1.0 +
                       std::clamp(sample, (TSample)-1.0, (TSample)1.0)) *
                       (TSample)0.5 * (std::pow((TSample)2.0, bit_depth) -
                                       (TSample)1.0));

    return mix * ((((TSample)in / (std::pow((TSample)2.0, bit_depth) -
                                   (TSample)1.0)) * (TSample)2.0) - (TSample)1.0)
           + ((TSample)1.0 - mix) * sample;
}

}

#endif // DISTORTIONS_H_
