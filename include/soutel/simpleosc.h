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

#ifndef SIMPLEOSC_H_
#define SIMPLEOSC_H_

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
class SimpleOsc
{
public:
    SimpleOsc(const TSample &sample_rate = (TSample)44100.0,
              const TSample &frequency = (TSample)0.0,
              const TSample &pulse_width = (TSample)0.5);

    void set_sample_rate(const TSample &sample_rate);
    void set_frequency(const TSample &frequency);
    void set_pulse_width(const TSample &pulse_width);
    void reset();

    TSample get_sample_rate();
    TSample get_frequency();
    TSample get_pulse_width();

    inline bool run();
    inline bool run(TSample &sine_out, TSample &triangle_out, TSample &saw_out,
                    TSample &pulse_out);

    inline void get_last_sample(TSample &sine_out, TSample &triangle_out,
                                TSample &saw_out, TSample &pulse_out);
    inline TSample get_sine()
    {
        return sine_out_;
    };
    inline TSample get_triangle()
    {
        return triangle_out_;
    };
    inline TSample get_saw()
    {
        return saw_out_;
    };
    inline TSample get_pulse()
    {
        return pulse_out_;
    };

private:
    TSample sample_rate_;
    TSample inv_sample_rate_;
    TSample half_sample_rate_;

    TSample frequency_;

    TSample step_;
    TSample saw_out_;
    TSample sine_out_;
    TSample triangle_out_;
    TSample pulse_out_;
    TSample pulse_width_;
};

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
SimpleOsc<TSample>::SimpleOsc(const TSample &sample_rate, const TSample &frequency, const TSample &pulse_width)
{
    frequency_ = frequency;

    set_sample_rate(sample_rate);

    set_pulse_width(pulse_width);

    reset();
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
void SimpleOsc<TSample>::set_sample_rate(const TSample &sample_rate)
{
    sample_rate_ = std::max((TSample)1.0, sample_rate);
    half_sample_rate_ = sample_rate_ * (TSample)0.5;
    inv_sample_rate_ = (TSample)1.0 / sample_rate_;

    set_frequency(frequency_);

    reset();
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
void SimpleOsc<TSample>::set_frequency(const TSample &frequency)
{
    frequency_ = frequency;

    step_ = (TSample)2.0 * (frequency_ * inv_sample_rate_);
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
void SimpleOsc<TSample>::set_pulse_width(const TSample &pulse_width)
{
    pulse_width_ = std::clamp(pulse_width, (TSample)0.0, (TSample)1.0);
}


template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
TSample SimpleOsc<TSample>::get_sample_rate()
{
    return sample_rate_;
}


template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
TSample SimpleOsc<TSample>::get_frequency()
{
    return frequency_;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
TSample SimpleOsc<TSample>::get_pulse_width()
{
    return pulse_width_;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
void SimpleOsc<TSample>::reset()
{
    sine_out_ = (TSample)0.0;
    saw_out_ = (TSample)0.0;
    triangle_out_ = (TSample)0.0;
    pulse_out_ = (TSample)0.0;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
inline bool SimpleOsc<TSample>::run()
{
    bool new_cycle = false;

    saw_out_ += step_;
    if (std::abs(saw_out_) > (TSample)1.0)
    {
        saw_out_ = std::fmod(saw_out_ + (TSample)1.0, (TSample)2.0) - std::copysign((TSample)1.0, saw_out_);
        new_cycle = true;
    }

    sine_out_ = sin(saw_out_ * (TSample)M_PI);

    triangle_out_ = (copysign((TSample)2.0, -saw_out_) * saw_out_) + (TSample)1.0;

    if ((saw_out_ + (TSample)1.0) * (TSample)0.5 > pulse_width_)
    {
        pulse_out_ = (TSample)1.0;
    }
    else
    {
        pulse_out_ = (TSample)-1.0;
    }

    return new_cycle;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
inline bool SimpleOsc<TSample>::run(TSample &sine_out, TSample &triangle_out,
                                    TSample &saw_out, TSample &pulse_out)
{
    bool new_cycle = run();

    get_last_sample(sine_out, triangle_out, saw_out, pulse_out);

    return new_cycle;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
inline void SimpleOsc<TSample>::get_last_sample(TSample &sine_out,
        TSample &triangle_out,
        TSample &saw_out,
        TSample &pulse_out)
{
    saw_out = saw_out_;
    sine_out = sine_out_;
    triangle_out = triangle_out_;
    pulse_out = pulse_out_;
}

}

#endif // SIMPLEOSC_H_
