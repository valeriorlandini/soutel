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

#ifndef BIQUAD_H_
#define BIQUAD_H_

#include <algorithm>
#include <array>
#include <cmath>
#include <deque>

#if __cplusplus >= 202002L
#include<concepts>
#endif

namespace soutel
{

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_SQRT2
#define M_SQRT2 1.41421356237309504880
#endif

enum class BQFilters
{
    lowpass,
    hipass,
    bandpass,
    bandreject,
    allpass,
    lowshelf,
    hishelf,
    peak
};

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
class Biquad
{
public:
    Biquad(const TSample &sample_rate = (TSample)44100.0,
           const TSample &cutoff = (TSample)11025.0,
           const TSample &q = (TSample)0.707,
           const TSample &gain = (TSample)0.0,
           const BQFilters &type = BQFilters::lowpass);

    void set_sample_rate(const TSample &sample_rate);
    void set_cutoff(const TSample &cutoff);
    void set_q(const TSample &q);
    void set_gain(const TSample &gain);
    void set_type(const BQFilters &type);

    TSample get_sample_rate();
    TSample get_cutoff();
    TSample get_q();
    TSample get_gain();
    BQFilters get_type();
    std::array<TSample, 5> get_coefficients();

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
    TSample gain_;
    TSample v0_;

    BQFilters type_;

    TSample k_;

    TSample w_[3];
    TSample a1_, a2_;
    TSample b0_, b1_, b2_;

    TSample output_;

    inline void calc_coeffs_();
};

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
Biquad<TSample>::Biquad(const TSample &sample_rate, const TSample &cutoff,
                        const TSample &q, const TSample &gain, const BQFilters &type)
{

    sample_rate_ = std::max((TSample)1.0, sample_rate);
    inv_sample_rate_ = 1.0 / sample_rate_;
    half_sample_rate_ = sample_rate_ * 0.5;

    q_ = std::max((TSample)0.001, q);
    gain_ = gain;
    v0_ = std::pow((TSample)10.0, gain_/(TSample)20.0);

    if (type >= BQFilters::lowpass && type <= BQFilters::peak)
    {
        type_ = type;
    }
    else
    {
        type_ = BQFilters::lowpass;
    }

    set_cutoff(cutoff);

    clear();
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
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

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
void Biquad<TSample>::set_cutoff(const TSample &cutoff)
{
    cutoff_ = std::clamp(cutoff, (TSample)0.001, half_sample_rate_);
    k_ = std::tan((TSample)M_PI * cutoff_ * inv_sample_rate_);

    calc_coeffs_();
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
void Biquad<TSample>::set_q(const TSample &q)
{
    q_ = std::max((TSample)0.001, q);

    calc_coeffs_();
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
void Biquad<TSample>::set_gain(const TSample &gain)
{
    gain_ = gain;
    v0_ = std::pow((TSample)10.0, gain_/(TSample)20.0);

    if (type_ >= BQFilters::lowshelf && type_ <= BQFilters::peak)
    {
        calc_coeffs_();
    }
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
void Biquad<TSample>::set_type(const BQFilters &type)
{
    if (type >= BQFilters::lowpass && type <= BQFilters::peak)
    {
        type_ = type;

        calc_coeffs_();
    }
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
TSample Biquad<TSample>::get_sample_rate()
{
    return sample_rate_;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
TSample Biquad<TSample>::get_cutoff()
{
    return cutoff_;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
TSample Biquad<TSample>::get_q()
{
    return q_;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
TSample Biquad<TSample>::get_gain()
{
    return gain_;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
std::array<TSample, 5> Biquad<TSample>::get_coefficients()
{
    std::array<TSample, 5> coefficients{a1_, a2_, b0_, b1_, b2_};
    return coefficients;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
BQFilters Biquad<TSample>::get_type()
{
    return type_;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
void Biquad<TSample>::clear()
{
    w_[0] = (TSample)0.0;
    w_[1] = (TSample)0.0;
    w_[2] = (TSample)0.0;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
inline TSample Biquad<TSample>::run(const TSample &input)
{
    w_[2] = w_[1];
    w_[1] = w_[0];
    w_[0] = input - a1_ * w_[1] - a2_ * w_[2];

    output_ = b0_ * w_[0] + b1_ * w_[1] + b2_ * w_[2];

    return output_;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
inline void Biquad<TSample>::run(const TSample &input, TSample &output)
{
    output = run(input);
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
inline TSample Biquad<TSample>::get_last_sample()
{
    return output_;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
inline void Biquad<TSample>::calc_coeffs_()
{
    TSample kkq = k_ * k_ * q_;
    TSample kkm1 = (k_ * k_) - (TSample)1.0;
    TSample s2v0 = std::sqrt((TSample)2.0 * v0_);
    TSample v0kk = v0_ * k_ * k_;
    TSample bood =  (TSample)1.0 + ((TSample)M_SQRT2 * k_) + (k_ * k_);
    TSample lcud =  v0_ + (s2v0 * k_) + (k_ * k_);
    TSample hcud = (TSample)1.0 + (s2v0 * k_) + v0kk;
    TSample kiq = k_ / q_;
    TSample bpkd = ((TSample)1.0 + kiq + (k_ * k_));
    TSample cpkd = ((TSample)1.0 + (kiq / v0_) + (k_ * k_));

    if (type_ >= BQFilters::lowpass && type_ <= BQFilters::allpass)
    {
        a1_ = ((TSample)2.0 * q_ * kkm1) / (kkq + k_ + q_);
        a2_ = (kkq - k_ + q_) / (kkq + k_ + q_);
    }

    switch (type_)
    {
    case BQFilters::lowpass:
        b0_ = kkq / (kkq + k_ + q_);
        b1_ = (TSample)2.0 * b0_;
        b2_ = b0_;
        break;
    case BQFilters::hipass:
        b0_ = q_ / (kkq + k_ + q_);
        b1_ = (TSample)-2.0 * b0_;
        b2_ = b0_;
        break;
    case BQFilters::bandpass:
        b0_ = k_ / (kkq + k_ + q_);
        b1_ = (TSample)0.0;
        b2_ = (TSample)-1.0 * b0_;
        break;
    case BQFilters::bandreject:
        b0_ = (q_ * ((TSample)1.0 + (k_ * k_))) / (kkq + k_ + q_);
        b1_ = ((TSample)2.0 * q_ * kkm1) / (kkq + k_ + q_);
        b2_ = b0_;
        break;
    case BQFilters::allpass:
        b0_ = (kkq - k_ + q_) / (kkq + k_ + q_);
        b1_ = ((TSample)2.0 * q_ * kkm1) / (kkq + k_ + q_);
        b2_ = (TSample)1.0;
        break;
    case BQFilters::lowshelf:
        if (gain_ > (TSample)0.0)
        {
            a1_ = ((TSample)2.0 * ((k_ * k_) - (TSample)1.0)) / bood;
            a2_ = ((TSample)1.0 - ((TSample)M_SQRT2 * k_) + (k_ * k_)) / bood;
            b0_ = ((TSample)1.0 + (s2v0 * k_) + v0kk) / bood;
            b1_ = ((TSample)2.0 * (v0kk - (TSample)1.0)) / bood;
            b2_ = ((TSample)1.0 - (s2v0 * k_) + v0kk) / bood;
        }
        else
        {
            a1_ = ((TSample)2.0 * ((k_ * k_) - v0_)) / lcud;
            a2_ = (v0_ - (s2v0 * k_) + (k_ * k_)) / lcud;
            b0_ = (v0_ * ((TSample)1.0 + ((TSample)M_SQRT2 * k_) + (k_ * k_))) / lcud;
            b1_ = ((TSample)2.0 * v0_ * ((k_ * k_) - (TSample)1.0)) / lcud;
            b2_ = (v0_ * ((TSample)1.0 - ((TSample)M_SQRT2 * k_) + (k_ * k_))) / lcud;
        }
        break;
    case BQFilters::hishelf:
        if (gain_ > (TSample)0.0)
        {
            a1_ = ((TSample)2.0 * ((k_ * k_) - (TSample)1.0)) / bood;
            a2_ = ((TSample)1.0 - ((TSample)M_SQRT2 * k_) + (k_ * k_)) / bood;
            b0_ = ((TSample)v0_ + s2v0 + (k_ * k_)) / bood;
            b1_ = ((TSample)2.0 * ((k_ * k_) - v0_)) / bood;
            b2_ = ((TSample)v0_ - s2v0 + (k_ * k_)) / bood;
        }
        else
        {
            a1_ = ((TSample)2.0 * (v0kk - (TSample)1.0)) / hcud;
            a2_ = ((TSample)1.0 - (s2v0 * k_) + v0kk) / hcud;
            b0_ = (v0_ * ((TSample)1.0 + ((TSample)M_SQRT2 * k_) + (k_ * k_))) / hcud;
            b1_ = ((TSample)2.0 * v0_ * ((k_ * k_) - (TSample)1.0)) / hcud;
            b2_ = (v0_ * ((TSample)1.0 - ((TSample)M_SQRT2 * k_) + (k_ * k_))) / hcud;
        }
        break;
    case BQFilters::peak:
        if (gain_ > (TSample)0.0)
        {
            a1_ = ((TSample)2.0 * ((k_ * k_) - (TSample)1.0)) / bpkd;
            a2_ = ((TSample)1.0 - kiq + (k_ * k_))/ bpkd;
            b0_ = ((TSample)1.0 + (v0_ * kiq) + (k_ * k_)) / bpkd;
            b1_ = a1_;
            b2_ = ((TSample)1.0 - (v0_ * kiq) + (k_ * k_)) / bpkd;
        }
        else
        {
            a1_ = ((TSample)2.0 * ((k_ * k_) - (TSample)1.0)) / cpkd;
            a2_ = ((TSample)1.0 - (kiq / v0_) + (k_ * k_)) / cpkd;
            b0_ = ((TSample)1.0 + kiq + (k_ * k_))/ cpkd;
            b1_ = a1_;
            b2_ = ((TSample)1.0 - kiq + (k_ * k_))/ cpkd;
        }
        break;
    }
}

}

#endif // BIQUAD_H_
