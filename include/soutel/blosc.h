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


#ifndef BLOSC_H_
#define BLOSC_H_

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

enum class BLWaveforms
{
    sine,
    triangle,
    saw,
    square
};

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
class BLOsc
{
public:
    BLOsc(const TSample &sample_rate = (TSample)44100.0,
          const TSample &frequency = (TSample)0.0);

    void set_sample_rate(const TSample &sample_rate);
    void set_frequency(const TSample &frequency);
    void reset();

    TSample get_sample_rate();
    TSample get_frequency();

    inline bool run();
    inline bool run(TSample &sine_out, TSample &triangle_out, TSample &saw_out,
                    TSample &square_out);

    inline void get_last_sample(TSample &sine_out, TSample &triangle_out,
                                TSample &saw_out, TSample &square_out);

    inline TSample get_sine()
    {
        return sine_out_;
    }

    inline TSample get_triangle()
    {
        return triangle_out_;
    }

    inline TSample get_saw()
    {
        return saw_out_;
    }

    inline TSample get_square()
    {
        return square_out_;
    }

private:
    TSample sample_rate_;
    TSample inv_sample_rate_;
    TSample half_sample_rate_;

    TSample frequency_;

    TSample step_;
    TSample ramp_;

    TSample harmonics_;

    TSample saw_out_;
    TSample sine_out_;
    TSample triangle_out_;
    TSample square_out_;

    static constexpr TSample double_pi_ = (TSample)(M_PI * 2.0);
};

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
BLOsc<TSample>::BLOsc(const TSample &sample_rate, const TSample &frequency)
{
    frequency_ = frequency;

    set_sample_rate(sample_rate);

    reset();
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
void BLOsc<TSample>::set_sample_rate(const TSample &sample_rate)
{
    sample_rate_ = std::max((TSample)1.0, sample_rate);
    half_sample_rate_ = sample_rate_ * (TSample)0.5;
    inv_sample_rate_ = (TSample)1.0 / sample_rate_;

    set_frequency(frequency_);
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
void BLOsc<TSample>::set_frequency(const TSample &frequency)
{
    frequency_ = std::clamp(frequency, half_sample_rate_ * (TSample)-0.999, half_sample_rate_ * (TSample)0.999);

    step_ = frequency_ * inv_sample_rate_;

    if (frequency != (TSample)0.0)
    {
        harmonics_ = std::min((TSample)30.0, std::floor(half_sample_rate_ / std::abs(frequency_)));
    }
    else
    {
        harmonics_ = (TSample)0.0;
    }
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
void BLOsc<TSample>::reset()
{
    ramp_ = (TSample)0.0;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
TSample BLOsc<TSample>::get_sample_rate()
{
    return sample_rate_;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
TSample BLOsc<TSample>::get_frequency()
{
    return frequency_;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
inline bool BLOsc<TSample>::run()
{
    bool new_cycle = false;

    ramp_ += step_;
    if (ramp_ > (TSample)1.0)
    {
        while (ramp_ > (TSample)1.0)
        {
            ramp_ -=  (TSample)1.0;
        }

        new_cycle = true;
    }

    sine_out_ = std::sin(ramp_ * double_pi_);

    saw_out_ = (TSample)0.0;
    square_out_ = (TSample)0.0;
    triangle_out_ = (TSample)0.0;

    TSample ramp_double_pi = ramp_ * double_pi_;
    for (TSample harmonic = (TSample)1.0; harmonic <= harmonics_; harmonic++)
    {
        saw_out_ += std::sin(-ramp_double_pi * harmonic) / harmonic;
        if (static_cast<unsigned int>(harmonic) % 2)
            if ((unsigned int)harmonic % 2)
            {
                square_out_ += std::sin(ramp_double_pi * harmonic) / harmonic;
                triangle_out_ += std::cos(ramp_double_pi * harmonic) / (harmonic * harmonic);
            }
    }

    saw_out_ *= (TSample)0.55;
    square_out_ *= (TSample)1.07;
    triangle_out_ *= (TSample)0.82;

    return new_cycle;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
inline bool BLOsc<TSample>::run(TSample &sine_out, TSample &triangle_out,
                                TSample &saw_out, TSample &square_out)
{
    bool new_cycle = run();

    get_last_sample(sine_out, triangle_out, saw_out, square_out);

    return new_cycle;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
inline void BLOsc<TSample>::get_last_sample(TSample &sine_out,
        TSample &triangle_out,
        TSample &saw_out,
        TSample &square_out)
{
    saw_out = saw_out_;
    sine_out = sine_out_;
    triangle_out = triangle_out_;
    square_out = square_out_;
}

}

#endif // BLOSC_H_