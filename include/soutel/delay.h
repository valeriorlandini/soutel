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

#ifndef DELAY_H_
#define DELAY_H_

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
class Delay
{
public:
    Delay(const TSample &sample_rate = (TSample)44100.0,
          const TSample &max_delay_time = (TSample)5000.0,
          const TSample &delay_time = (TSample)1000.0,
          const TSample &feedback = (TSample)0.0);

    void set_sample_rate(const TSample &sample_rate);
    void set_time(const TSample &delay_time);
    void set_max_time(const TSample &max_delay_time, bool clear = true);
    void set_feedback(const TSample &feedback);
    void clear();

    TSample get_sample_rate();
    TSample get_time();
    int get_samples();
    TSample get_max_time();
    TSample get_feedback();

    inline TSample run(const TSample &input);
    inline void run(const TSample &input, TSample &output);

    inline TSample get_last_sample();

private:
    TSample max_delay_time_;
    TSample sample_rate_;
    TSample delay_time_;

    int delay_samples_[2];
    TSample delay_interp_;

    TSample feedback_;

    TSample output_;

    int read_pos_[2];
    int write_pos_;

    std::vector<TSample> buffer_;
};

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
Delay<TSample>::Delay(const TSample &sample_rate, const TSample &max_delay_time,
                      const TSample &delay_time, const TSample &feedback)
{

    sample_rate_ = std::max((TSample)1.0, sample_rate);

    max_delay_time_ = std::max((TSample)0.0, max_delay_time);
    buffer_.assign((unsigned int)ceil(max_delay_time_ * sample_rate_ * (TSample)0.001) + 1, (TSample)0.0);

    write_pos_ = 0;

    set_time(delay_time);

    set_feedback(feedback);
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
void Delay<TSample>::set_sample_rate(const TSample &sample_rate)
{
    sample_rate_ = std::max((TSample)1.0, sample_rate);

    set_max_time(max_delay_time_, true);
    set_time(delay_time_);
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
void Delay<TSample>::set_max_time(const TSample &max_delay_time, bool clear)
{
    max_delay_time_ = std::max((TSample)0.0, max_delay_time);

    if (clear)
    {
        buffer_.assign((unsigned int)ceil(max_delay_time_ * sample_rate_ * (TSample)0.001) + 1, (TSample)0.0);
    }
    else
    {
        buffer_.resize((unsigned int)ceil(max_delay_time_ * sample_rate_ * (TSample)0.001) + 1, (TSample)0.0);
    }

    if (delay_time_ > max_delay_time_)
    {
        set_time(max_delay_time_);
    }
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
void Delay<TSample>::set_time(const TSample &delay_time)
{
    delay_time_ = std::clamp(delay_time, (TSample)0.0, max_delay_time_);

    delay_samples_[0] = (int)std::floor(sample_rate_ * delay_time_ * (TSample)0.001);
    delay_samples_[1] = (int)std::ceil(sample_rate_ * delay_time_ * (TSample)0.001) % buffer_.size();
    delay_interp_ = (sample_rate_ * delay_time_ * (TSample)0.001) - std::floor(sample_rate_ * delay_time_ * (TSample)0.001);

    for (int p = 0; p <= 1; p++)
    {
        read_pos_[p] = write_pos_ - delay_samples_[p];
        while (read_pos_[p] < 0)
        {
            read_pos_[p] = (int)buffer_.size() + read_pos_[p];
        }
    }
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
void Delay<TSample>::set_feedback(const TSample &feedback)
{
    feedback_ = feedback;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
TSample Delay<TSample>::get_sample_rate()
{
    return sample_rate_;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
TSample Delay<TSample>::get_time()
{
    return delay_time_;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
int Delay<TSample>::get_samples()
{
    return delay_samples_;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
TSample Delay<TSample>::get_max_time()
{
    return max_delay_time_;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
TSample Delay<TSample>::get_feedback()
{
    return feedback_;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
void Delay<TSample>::clear()
{
    buffer_.assign(buffer_.size(), 0.0);
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
inline TSample Delay<TSample>::run(const TSample &input)
{
    output_ = cosip(buffer_.at(read_pos_[0]), buffer_.at(read_pos_[1]), delay_interp_);

    buffer_.at(write_pos_) = input + (output_ * feedback_);

    if (++write_pos_ >= buffer_.size())
    {
        write_pos_ = 0;
    }

    for (int p = 0; p <= 1; p++)
    {
        if (++read_pos_[p] >= buffer_.size())
        {
            read_pos_[p] = 0;
        }
    }

    return output_;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
inline void Delay<TSample>::run(const TSample &input, TSample &output)
{
    output_ = run(input);
    output = output_;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
inline TSample Delay<TSample>::get_last_sample()
{
    return output_;
}

}

#endif // DELAY_H_
