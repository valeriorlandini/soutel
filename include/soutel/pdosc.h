/******************************************************************************
Copyright (c) 2023-2024 Valerio Orlandini

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

#ifndef PDOSC_H_
#define PDOSC_H_

#include <algorithm>
#include <cmath>

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
class PDOsc
{
public:
    PDOsc(const TSample &sample_rate = (TSample)44100.0,
              const TSample &frequency = (TSample)0.0,
              const TSample &d = (TSample)0.5);

    void set_sample_rate(const TSample &sample_rate);
    void set_frequency(const TSample &frequency);
    void set_d(const TSample &d);
    void reset();

    TSample get_sample_rate();
    TSample get_frequency();
    TSample get_d();

    inline TSample run();

    inline TSample get_last_out();

private:
    TSample sample_rate_;
    TSample inv_sample_rate_;

    TSample frequency_;

    TSample step_a_;
    TSample step_b_;
    TSample d_;
    TSample inv_d_;
    TSample inv_1d_;
    TSample ramp_;
    TSample out_;

    const TSample double_pi_ = (TSample)(M_PI * 2.0);
};

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
PDOsc<TSample>::PDOsc(const TSample &sample_rate, const TSample &frequency, const TSample &d)
{
    frequency_ = frequency;
    d_ = std::clamp(d, (TSample)0.0, (TSample)1.0);

    set_sample_rate(sample_rate);

    reset();
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
void PDOsc<TSample>::set_sample_rate(const TSample &sample_rate)
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
void PDOsc<TSample>::set_frequency(const TSample &frequency)
{
    frequency_ = frequency;

    step_a_ = (TSample)0.5 * inv_sample_rate_ * frequency_ * inv_d_;
    step_b_ = (TSample)0.5 * inv_sample_rate_ * frequency_ * inv_1d_;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
void PDOsc<TSample>::set_d(const TSample &d)
{
    d_ = std::clamp(d, (TSample)0.0, (TSample)1.0);
    inv_d_ = d_ > (TSample)0.001 ? (TSample)1.0 / d_ : (TSample)1000.0;
    inv_1d_ = d_ < (TSample)0.999 ? (TSample)1.0 / ((TSample)1.0 - d_) : (TSample)1000.0;

    set_frequency(frequency_);
}


template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
TSample PDOsc<TSample>::get_sample_rate()
{
    return sample_rate_;
}


template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
TSample PDOsc<TSample>::get_frequency()
{
    return frequency_;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
TSample PDOsc<TSample>::get_d()
{
    return d_;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
void PDOsc<TSample>::reset()
{
    ramp_ = (TSample)0.0;
    out_ = (TSample)0.0;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
inline TSample PDOsc<TSample>::run()
{
    if (ramp_ < (TSample)0.5)
    {
        ramp_ += step_a_;
    }
    else
    {
        ramp_ += step_b_;    
    }

    while (ramp_ > 1.0)
    {
        ramp_ -= 1.0;
    }
    while (ramp_ < 0.0)
    {
        ramp_ += 1.0;
    }

    out_ = std::sin(ramp_ * double_pi_);

    return out_;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
inline TSample PDOsc<TSample>::get_last_out()
{
    return out_;
}

}

#endif // PDOSC_H_
