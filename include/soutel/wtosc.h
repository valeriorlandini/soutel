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

#ifndef WTOSC_H_
#define WTOSC_H_

#include "interp.h"

#if __cplusplus >= 202002L
#include<concepts>
#endif

namespace soutel
{

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
class WTOsc
{
public:
    WTOsc(const TSample &sample_rate = (TSample)44100.0,
          const TSample &frequency = (TSample)0.0,
          const std::vector<TSample> &wavetable = {});

    void set_sample_rate(const TSample &sample_rate);
    void set_frequency(const TSample &frequency);
    void set_wavetable(const std::vector<TSample> &wavetable);
    void set_sample(const TSample &sample, const int &index);
    void reset();
    void resize_wavetable(const int &new_size);
    void normalize(const TSample &amplitude = (TSample)1.0);

    TSample get_sample_rate();
    TSample get_frequency();
    std::vector<TSample> get_wavetable();
    TSample get_sample(const int &index);

    inline TSample run();

    inline TSample get_last_sample();

private:
    TSample sample_rate_;
    TSample inv_sample_rate_;

    TSample frequency_;

    TSample step_;
    TSample read_pos_;
    TSample output_;

    std::vector<TSample> wavetable_;
};

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
WTOsc<TSample>::WTOsc(const TSample &sample_rate, const TSample &frequency, const std::vector<TSample> &wavetable)
{
    frequency_ = frequency;

    set_sample_rate(sample_rate);

    set_wavetable(wavetable);

    reset();
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
void WTOsc<TSample>::set_sample_rate(const TSample &sample_rate)
{
    sample_rate_ = std::max((TSample)1.0, sample_rate);
    inv_sample_rate_ = (TSample)1.0 / sample_rate_;

    set_frequency(frequency_);

    reset();
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
void WTOsc<TSample>::set_frequency(const TSample &frequency)
{
    frequency_ = frequency;

    step_ = frequency_ * inv_sample_rate_;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
void WTOsc<TSample>::set_wavetable(const std::vector<TSample> &wavetable)
{
    wavetable_ = wavetable;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
void WTOsc<TSample>::set_sample(const TSample &sample, const int &index)
{
    if (index >= 0 && index < wavetable_.size())
    {
        wavetable_.at(index) = sample;
    }
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
TSample WTOsc<TSample>::get_sample_rate()
{
    return sample_rate_;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
TSample WTOsc<TSample>::get_frequency()
{
    return frequency_;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
std::vector<TSample> WTOsc<TSample>::get_wavetable()
{
    return wavetable_;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
TSample WTOsc<TSample>::get_sample(const int &index)
{
    if (index >= 0 && index < wavetable_.size())
    {
        return wavetable_.at(index);
    }

    return (TSample)0.0;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
void WTOsc<TSample>::reset()
{
    read_pos_ = (TSample)0.0;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
void WTOsc<TSample>::resize_wavetable(const int &new_size)
{
    wavetable_ = resize_chunk(wavetable_, (unsigned int)std::abs(new_size));
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
void WTOsc<TSample>::normalize(const TSample &amplitude)
{
    TSample max_value = 0.0;

    for (const auto& sample : wavetable_)
    {
        if (std::abs(sample) > max_value)
        {
            max_value = std::abs(sample);
        }
    }

    TSample gain = max_value != (TSample)0.0 ? amplitude / max_value : (TSample)0.0;
    for (auto& sample : wavetable_)
    {
        if (std::abs(sample) > max_value)
        {
            sample *= gain;
        }
    }
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
inline TSample WTOsc<TSample>::run()
{
    if (wavetable_.empty())
    {
        return (TSample)0.0;
    }
    
    read_pos_ += step_;
    while (read_pos_ > (TSample)1.0)
    {
        read_pos_ -= (TSample)1.0;
    }
    while (read_pos_ < (TSample)0.0)
    {
        read_pos_ += (TSample)1.0;
    }

    TSample wt_point = read_pos_ * (TSample)(wavetable_.size() - 1);
    int pos1 = (int)std::floor(wt_point) % wavetable_.size();
    int pos2 = (int)std::ceil(wt_point) % wavetable_.size();

    output_ = cosip(wavetable_.at(pos1), wavetable_.at(pos2), wt_point - std::floor(wt_point));

    return output_;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
inline TSample WTOsc<TSample>::get_last_sample()
{
    return output_;
}

}

#endif // WTOSC_H_
