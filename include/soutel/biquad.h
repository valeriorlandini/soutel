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

#ifndef BIQUAD_H_
#define BIQUAD_H_

#include <algorithm>
#include <cmath>
#include <deque>
#include <iostream>

namespace soutel
{

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

enum BQFilters
{
    LOWPASS,
    HIPASS,
    BANDPASS,
    BANDREJECT,
    ALLPASS
};

template <class TSample>
class Biquad
{
public:
    Biquad(const TSample &sample_rate = (TSample)44100.0,
           const TSample &cutoff = (TSample)11025.0,
           const TSample &q = (TSample)0.707,
           const BQFilters &type = LOWPASS);

    void set_sample_rate(const TSample &sample_rate);
    void set_cutoff(const TSample &cutoff);
    void set_q(const TSample &q);
    void set_type(const BQFilters &type);

    TSample get_sample_rate();
    TSample get_cutoff();
    TSample get_q();
    BQFilters get_type();

    void clear();

    inline TSample run(const TSample &input);
    inline void run(const TSample &input, TSample &output);

    inline TSample get_last_sample();

private:
    TSample sample_rate_;
    TSample half_sample_rate_;
    TSample inv_sample_rate_;
    TSample cutoff_;
    TSample q_;

    BQFilters type_;

    TSample k_;

    TSample w_[3];
    TSample a1_, a2_;
    TSample b0_, b1_, b2_;

    TSample output_;
};

template <class TSample>
Biquad<TSample>::Biquad(const TSample &sample_rate, const TSample &cutoff,
                        const TSample &q, const BQFilters &type)
{

    sample_rate_ = std::max((TSample)1.0, sample_rate);
    inv_sample_rate_ = 1.0 / sample_rate_;
    half_sample_rate_ = sample_rate_ * 0.5;

    q_ = std::max((TSample)0.001, q);

    if (type >= LOWPASS && type <= ALLPASS)
    {
        type_ = type;
    }
    else
    {
        type_ = LOWPASS;
    }

    set_cutoff(cutoff);

    clear();
}

template <class TSample>
void Biquad<TSample>::set_sample_rate(const TSample &sample_rate)
{
    sample_rate_ = std::max((TSample)1.0, sample_rate);
    inv_sample_rate_ = 1.0 / sample_rate_;
    half_sample_rate_ = sample_rate_ * (TSample)0.5;

    if (cutoff_ > half_sample_rate_)
    {
        cutoff_ = half_sample_rate_;
    }

    set_cutoff(cutoff_);
}

template <class TSample>
void Biquad<TSample>::set_cutoff(const TSample &cutoff)
{
    cutoff_ = std::clamp(cutoff, (TSample)0.001, half_sample_rate_);

    k_ = tan((TSample)M_PI * cutoff_ * inv_sample_rate_);

    TSample kkq = k_ * k_ * q_;
    TSample kkm1 = (k_ * k_) - (TSample)1.0;

    a1_ = ((TSample)2.0 * q_ * kkm1) / (kkq + k_ + q_);
    a2_ = (kkq - k_ + q_) / (kkq + k_ + q_);

    switch (type_)
    {
    case LOWPASS:
        b0_ = kkq / (kkq + k_ + q_);
        b1_ = (TSample)2.0 * b0_;
        b2_ = b0_;
        break;
    case HIPASS:
        b0_ = q_ / (kkq + k_ + q_);
        b1_ = (TSample)-2.0 * b0_;
        b2_ = b0_;
        break;
    case BANDPASS:
        b0_ = k_ / (kkq + k_ + q_);
        b1_ = (TSample)0.0;
        b2_ = (TSample)-1.0 * b0_;
        break;
    case BANDREJECT:
        b0_ = (q_ * ((TSample)1.0 + (k_ * k_))) / (kkq + k_ + q_);
        b1_ = ((TSample)2.0 * q_ * kkm1) / (kkq + k_ + q_);
        b2_ = b0_;
        break;
    case ALLPASS:
        b0_ = (kkq - k_ + q_) / (kkq + k_ + q_);
        b1_ = ((TSample)2.0 * q_ * kkm1) / (kkq + k_ + q_);
        b2_ = (TSample)1.0;
        break;
    }
}

template <class TSample>
void Biquad<TSample>::set_q(const TSample &q)
{
    q_ = std::max((TSample)0.001, q);

    set_cutoff(cutoff_);
}

template <class TSample>
void Biquad<TSample>::set_type(const BQFilters &type)
{
    if (type >= LOWPASS && type <= ALLPASS)
    {
        type_ = type;

        set_cutoff(cutoff_);
    }
}

template <class TSample>
TSample Biquad<TSample>::get_sample_rate()
{
    return sample_rate_;
}

template <class TSample>
TSample Biquad<TSample>::get_cutoff()
{
    return cutoff_;
}

template <class TSample>
TSample Biquad<TSample>::get_q()
{
    return q_;
}

template <class TSample>
BQFilters Biquad<TSample>::get_type()
{
    return type_;
}

template <class TSample>
void Biquad<TSample>::clear()
{
    w_[0] = (TSample)0.0;
    w_[1] = (TSample)0.0;
    w_[2] = (TSample)0.0;
}

template <class TSample>
inline TSample Biquad<TSample>::run(const TSample &input)
{
    w_[2] = w_[1];
    w_[1] = w_[0];
    w_[0] = input - a1_ * w_[1] - a2_ * w_[2];

    output_ = b0_ * w_[0] + b1_ * w_[1] + b2_ * w_[2];

    return output_;
}

template <class TSample>
inline void Biquad<TSample>::run(const TSample &input, TSample &output)
{
    output = run(input);
}

template <class TSample>
inline TSample Biquad<TSample>::get_last_sample()
{
    return output_;
}

}

#endif // BIQUAD_H_
