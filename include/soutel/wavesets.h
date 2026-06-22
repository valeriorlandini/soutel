/******************************************************************************
Copyright (c) 2023-2026 Valerio Orlandini

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

#ifndef WAVESETS_H_
#define WAVESETS_H_

#include <algorithm>
#include <random>
#if __cplusplus >= 202002L
#include<span>
#endif
#include "biquad.h"
#include "interp.h"

namespace soutel
{

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
struct waveset_params
{
    unsigned int start;
    unsigned int half;
    unsigned int end;
    TSample peak;
};

enum Stages
{
    BEGIN,
    HALF
};

enum Shapes
{
    SINE,
    SAW,
    PULSE,
    TRIANGLE
};

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
struct filter_params
{
    unsigned int group_size;
    BQFilters type;
    TSample cutoff;
};



template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
class Wavesets
{
public:
    Wavesets(const TSample &sample_rate = (TSample)44100.0,
             const std::vector<TSample> &buffer = {});

    void set_sample_rate(const TSample &sample_rate);
    void set_buffer(const std::vector<TSample> &buffer);
    void resize_buffer(const int &new_size);
    void normalize_buffer(const TSample &amplitude = (TSample)1.0);

    TSample get_sample_rate();
    std::vector<TSample> get_buffer();
    TSample get_sample(const int &index);

    bool mute(const unsigned int &keep = 1u, const unsigned int &mute = 0u);
    bool shuffle(const unsigned int &group_size = 1u);
    bool reverse(const unsigned int &group_size = 1u);
    bool average(const unsigned int &group_size = 1u);
    bool mirshrink(const unsigned int &group_size = 1u);
    bool multiply(const unsigned int &group_size = 1u);
    bool mix(const unsigned int &group_size = 1u);
    bool power(const unsigned int &group_size = 1u);
    bool stretch(const unsigned int &group_size = 1u, const TSample &stretch_factor = (TSample)1.0);
#if __cplusplus >= 202002L
    bool filter(const std::span<const filter_params<TSample>> &filters);
#endif
    bool same(const TSample &size, const bool &unit_is_ms = true);
    bool reshape(const Shapes &shape);

private:
    TSample sample_rate_;
    TSample inv_sample_rate_;

    std::vector<TSample> buffer_;

    std::vector<waveset_params<TSample>> wavesets_idx_;

    inline void analyse_()
    {
        if (!buffer_.empty())
        {
            wavesets_idx_.clear();

            waveset_params<TSample> curr_waveset;
            bool sign;
            Stages stage = BEGIN;
            curr_waveset.start = 0;
            curr_waveset.half = 0;
            curr_waveset.peak = 0.0;
            for (int s = 0; s < buffer_.size(); s++)
            {
                if (!s)
                {
                    sign = std::signbit(buffer_[s]);
                }
                else if (std::signbit(buffer_[s]) != sign)
                {
                    if (stage == BEGIN)
                    {
                        curr_waveset.half = s;
                        stage = HALF;
                    }
                    else
                    {
                        curr_waveset.end = std::max(0, s - 1);
                        wavesets_idx_.push_back(curr_waveset);
                        curr_waveset.start = s;
                        curr_waveset.half = s;
                        curr_waveset.peak = 0.0;
                        stage = BEGIN;
                    }
                }

                sign = std::signbit(buffer_[s]);

                if (abs(buffer_[s]) > curr_waveset.peak)
                {
                    curr_waveset.peak = abs(buffer_[s]);
                }
            }
            curr_waveset.end = buffer_.size() - 1;
            wavesets_idx_.push_back(curr_waveset);
        }
    }
};

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
Wavesets<TSample>::Wavesets(const TSample &sample_rate, const std::vector<TSample> &buffer)
{
    set_sample_rate(sample_rate);

    set_buffer(buffer);
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
void Wavesets<TSample>::set_sample_rate(const TSample &sample_rate)
{
    sample_rate_ = std::max((TSample)1.0, sample_rate);
    inv_sample_rate_ = (TSample)1.0 / sample_rate_;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
void Wavesets<TSample>::set_buffer(const std::vector<TSample> &buffer)
{
    buffer_ = buffer;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
void Wavesets<TSample>::resize_buffer(const int &new_size)
{
    buffer_ = resize_chunk(buffer_, (unsigned int)std::abs(new_size));
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
void Wavesets<TSample>::normalize_buffer(const TSample &amplitude)
{
    buffer_ = normalize(buffer_, amplitude);
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
std::vector<TSample> Wavesets<TSample>::get_buffer()
{
    return buffer_;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
bool Wavesets<TSample>::mute(const unsigned int &keep, const unsigned int &mute)
{
    analyse_();
    if (!buffer_.empty() && (keep + mute))
    {
        for (int w = 0; w < wavesets_idx_.size(); ++w)
        {
            if (w % (keep + mute) >= keep)
            {
                int start = wavesets_idx_.at(w).start;
                int end = wavesets_idx_.at(w).end;
                for (int s = start; s <= end; s++)
                {
                    buffer_.at(s) = static_cast<TSample>(0.0);
                }
            }
        }
        return true;
    }

    return false;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
bool Wavesets<TSample>::shuffle(const unsigned int &group_size)
{
    analyse_();
    if (!buffer_.empty() && group_size >= 1u)
    {
        int groups = std::max(1, int(ceil((TSample)wavesets_idx_.size() / (TSample)group_size)));
        if (groups == 1)
        {
            return false;
        }

        auto rand_dev = std::random_device{};
        auto rand_eng = std::default_random_engine{rand_dev()};
        std::vector<int> ws(groups);
        std::iota(std::begin(ws), std::end(ws), 0);
        std::shuffle(std::begin(ws), std::end(ws), rand_eng);
        std::vector<TSample> new_buffer;

        for (int g = 0; g < groups; g++)
        {
            int ws_begin = ws.at(g) * group_size;
            int ws_end = static_cast<int>(std::min((unsigned int)(wavesets_idx_.size()), (ws.at(g) + 1u) * group_size) - 1u);

            int start = wavesets_idx_.at(ws_begin).start;
            int end = wavesets_idx_.at(ws_end).end;

            for (int s = start; s <= end; ++s)
            {
                new_buffer.push_back(buffer_.at(s));
            }
        }

        buffer_ = new_buffer;

        return true;
    }

    return false;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
bool Wavesets<TSample>::reverse(const unsigned int &group_size)
{
    analyse_();
    if (!buffer_.empty() && group_size >= 1u)
    {
        for (int w = 0; w < wavesets_idx_.size(); w += group_size)
        {
            int start = wavesets_idx_.at(w).start;
            int end = wavesets_idx_.at(std::min((unsigned int)wavesets_idx_.size() - 1u, w + group_size - 1u)).end;

            std::vector<TSample> curr_waveset;

            for (int s = end; s >= start; --s)
            {
                curr_waveset.push_back(buffer_.at(s));
            }

            int pos = 0;

            for (int s = start; s <= end; ++s)
            {
                buffer_.at(s) = curr_waveset.at(pos++);
            }
        }

        return true;
    }

    return false;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
bool Wavesets<TSample>::average(const unsigned int &group_size)
{
    analyse_();
    if (!buffer_.empty() && group_size >= 1u)
    {
        std::vector<TSample> new_buffer;
        int max_length = 0;

        for (int w = 0; w < wavesets_idx_.size(); w += group_size)
        {
            int start_curr = wavesets_idx_.at(w).start;
            int end_curr = wavesets_idx_.at(std::min((unsigned int)wavesets_idx_.size() - 1u, w + group_size - 1u)).end;

            int start_next = wavesets_idx_.at((w + group_size) % wavesets_idx_.size()).start;
            int end_next = wavesets_idx_.at((w + (group_size * 2) - 1) % wavesets_idx_.size()).end;

            std::vector<TSample> curr_waveset;
            std::vector<TSample> next_waveset;

            if (end_curr < start_curr)
            {
                end_curr += buffer_.size();
            }
            for (int s = start_curr; s <= end_curr; ++s)
            {
                curr_waveset.push_back(buffer_.at(s % buffer_.size()));
            }

            if (end_next < start_next)
            {
                end_next += buffer_.size();
            }
            for (int s = start_next; s <= end_next; ++s)
            {
                next_waveset.push_back(buffer_.at(s % buffer_.size()));
            }

            auto avg_size = (curr_waveset.size() + next_waveset.size()) / 2;
            std::vector<TSample> resized_curr = resize_chunk(curr_waveset, avg_size);
            std::vector<TSample> resized_next = resize_chunk(next_waveset, avg_size);

            for (int s = 0; s < avg_size; s++)
            {
                new_buffer.push_back((resized_curr.at(s) + resized_next.at(s)) * (TSample)0.5);
            }
        }
        buffer_ = new_buffer;

        return true;
    }

    return false;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
bool Wavesets<TSample>::mirshrink(const unsigned int &group_size)
{
    analyse_();
    if (!buffer_.empty() && group_size >= 1u)
    {
        std::vector<TSample> new_buffer;
        int max_length = 0;

        for (int w = 0; w < wavesets_idx_.size(); w += group_size)
        {
            int start = wavesets_idx_.at(w).start;
            int end = wavesets_idx_.at(std::min((unsigned int)wavesets_idx_.size() - 1u, w + group_size - 1u)).end;

            std::vector<TSample> curr_waveset;

            for (int s = start; s <= end; ++s)
            {
                curr_waveset.push_back(buffer_.at(s));
            }
            for (int s = end; s >= start; --s)
            {
                curr_waveset.push_back(buffer_.at(s));
            }

            auto resized_curr = resize_chunk(curr_waveset, curr_waveset.size() / 2);

            for (int s = 0; s < resized_curr.size(); s++)
            {
                new_buffer.push_back(resized_curr.at(s));
            }
        }
        buffer_ = new_buffer;

        return true;
    }

    return false;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
bool Wavesets<TSample>::multiply(const unsigned int &group_size)
{
    analyse_();
    if (!buffer_.empty() && group_size >= 1u)
    {
        std::vector<TSample> new_buffer;

        for (int w = 0; w < wavesets_idx_.size(); w += group_size)
        {
            int start_curr = wavesets_idx_.at(w).start;
            int end_curr = wavesets_idx_.at(std::min((unsigned int)wavesets_idx_.size() - 1u, w + group_size - 1u)).end;

            int start_next = wavesets_idx_.at((w + group_size) % wavesets_idx_.size()).start;
            int end_next = wavesets_idx_.at((w + (group_size * 2) - 1) % wavesets_idx_.size()).end;

            std::vector<TSample> curr_waveset;
            std::vector<TSample> next_waveset;

            if (end_curr < start_curr)
            {
                end_curr += buffer_.size();
            }

            for (int s = start_curr; s <= end_curr; ++s)
            {
                curr_waveset.push_back(buffer_.at(s));
            }

            if (end_next < start_next)
            {
                end_next += buffer_.size();
            }

            for (int s = start_next; s <= end_next; ++s)
            {
                next_waveset.push_back(buffer_.at(s % buffer_.size()));
            }

            std::vector<TSample> resized_next = resize_chunk(next_waveset, curr_waveset.size());

            for (int s = 0; s < curr_waveset.size(); s++)
            {
                new_buffer.push_back(curr_waveset.at(s) * resized_next.at(s));
            }
        }
        buffer_ = new_buffer;

        return true;
    }

    return false;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
bool Wavesets<TSample>::mix(const unsigned int &group_size)
{
    analyse_();
    if (!buffer_.empty() && group_size >= 1u)
    {
        std::vector<TSample> new_buffer;

        for (int w = 0; w < wavesets_idx_.size(); w += group_size)
        {
            int start_curr = wavesets_idx_.at(w).start;
            int end_curr = wavesets_idx_.at(std::min((unsigned int)wavesets_idx_.size() - 1u, w + group_size - 1u)).end;

            int start_next = wavesets_idx_.at((w + group_size) % wavesets_idx_.size()).start;
            int end_next = wavesets_idx_.at((w + (group_size * 2) - 1) % wavesets_idx_.size()).end;

            std::vector<TSample> curr_waveset;
            std::vector<TSample> next_waveset;

            if (end_curr < start_curr)
            {
                end_curr += buffer_.size();
            }

            for (int s = start_curr; s <= end_curr; ++s)
            {
                curr_waveset.push_back(buffer_.at(s % buffer_.size()));
            }

            if (end_next < start_next)
            {
                end_next += buffer_.size();
            }

            for (int s = start_next; s <= end_next; ++s)
            {
                next_waveset.push_back(buffer_.at(s % buffer_.size()));
            }

            std::vector<TSample> resized_next = resize_chunk(next_waveset, curr_waveset.size());

            for (int s = 0; s < curr_waveset.size(); s++)
            {
                new_buffer.push_back((curr_waveset.at(s) + resized_next.at(s)) * (TSample)0.5);
            }
        }
        buffer_ = new_buffer;

        return true;
    }

    return false;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
bool Wavesets<TSample>::power(const unsigned int &group_size)
{
    analyse_();
    if (!buffer_.empty() && group_size >= 1u)
    {
        std::vector<TSample> new_buffer;

        for (int w = 0; w < wavesets_idx_.size(); w += group_size)
        {
            int start_curr = wavesets_idx_.at(w).start;
            int end_curr = wavesets_idx_.at(std::min((unsigned int)wavesets_idx_.size() - 1u, w + group_size - 1u)).end;

            int start_next = wavesets_idx_.at((w + group_size) % wavesets_idx_.size()).start;
            int end_next = wavesets_idx_.at((w + (group_size * 2) - 1) % wavesets_idx_.size()).end;

            std::vector<TSample> curr_waveset;
            std::vector<TSample> next_waveset;

            if (end_curr < start_curr)
            {
                end_curr += buffer_.size();
            }

            for (int s = start_curr; s <= end_curr; ++s)
            {
                curr_waveset.push_back(buffer_.at(s));
            }

            if (end_next < start_next)
            {
                end_next += buffer_.size();
            }

            for (int s = start_next; s <= end_next; ++s)
            {
                next_waveset.push_back(buffer_.at(s % buffer_.size()));
            }

            std::vector<TSample> resized_next = resize_chunk(next_waveset, curr_waveset.size());

            for (int s = 0; s < curr_waveset.size(); s++)
            {
                new_buffer.push_back(pow(curr_waveset.at(s), resized_next.at(s)));
            }
        }
        buffer_ = new_buffer;

        return true;
    }

    return false;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
bool Wavesets<TSample>::stretch(const unsigned int &group_size, const TSample &stretch_factor)
{
    analyse_();
    if (!buffer_.empty() && group_size >= 1u && stretch_factor != (TSample)0.0)
    {
        std::vector<TSample> new_buffer;

        for (int w = 0; w < wavesets_idx_.size(); w += group_size)
        {
            int start = wavesets_idx_.at(w).start;
            int end = wavesets_idx_.at(std::min((unsigned int)wavesets_idx_.size() - 1u, w + group_size - 1u)).end;

            std::vector<TSample> curr_waveset;

            for (int s = start; s <= end; ++s)
            {
                curr_waveset.push_back(buffer_.at(s));
            }

            std::vector<TSample> stretched_waveset = resize_chunk(curr_waveset, (unsigned int)std::ceil(curr_waveset.size() * std::abs(stretch_factor)));

            for (int s = 0; s < stretched_waveset.size(); s++)
            {
                new_buffer.push_back(stretched_waveset.at(s));
            }
        }
        buffer_ = new_buffer;

        if (stretch_factor < (TSample)0.0)
        {
            return reverse(group_size);
        }

        return true;
    }

    return false;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
bool Wavesets<TSample>::same(const TSample &size, const bool &unit_is_ms)
{
    analyse_();
    if (!buffer_.empty() && size > (TSample)0.0)
    {
        std::vector<TSample> new_buffer;

        unsigned int target_size = unit_is_ms ? (unsigned int)std::ceil(size * sample_rate_ * (TSample)0.001) : (unsigned int)std::ceil(size);

        for (int w = 0; w < wavesets_idx_.size(); w++)
        {
            int start = wavesets_idx_.at(w).start;
            int end = wavesets_idx_.at(w).end;

            std::vector<TSample> curr_waveset;

            for (int s = start; s <= end; ++s)
            {
                curr_waveset.push_back(buffer_.at(s));
            }

            std::vector<TSample> resized_waveset = resize_chunk(curr_waveset, target_size);

            for (int s = 0; s < resized_waveset.size(); s++)
            {
                new_buffer.push_back(resized_waveset.at(s));
            }
        }
        buffer_ = new_buffer;

        return true;
    }

    return false;
}

#if __cplusplus >= 202002L
template <typename TSample>
requires std::floating_point<TSample>
bool Wavesets<TSample>::filter(const std::span<const filter_params<TSample>> &filters)
{
    analyse_();
    if (!buffer_.empty() && !filters.empty())
    {
        std::vector<TSample> new_buffer = buffer_;

        for (const auto &filter : filters)
        {
            Biquad<TSample> biquad_filter(sample_rate_, filter.cutoff, static_cast<TSample>(0.707), static_cast<TSample>(0.0), filter.type);

            for (int w = 0; w < wavesets_idx_.size(); w += filter.group_size)
            {
                int start = wavesets_idx_.at(w).start;
                int end = wavesets_idx_.at(std::min((unsigned int)wavesets_idx_.size() - 1u, w + filter.group_size - 1u)).end;

                for (int s = start; s <= end; ++s)
                {
                    buffer_.at(s) = biquad_filter.process(buffer_.at(s));
                }
            }
        }

        return true;
    }

    return false;
}
#endif

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
bool Wavesets<TSample>::reshape(const Shapes &shape)
{
    analyse_();
    if (!buffer_.empty())
    {
        for (int w = 0; w < wavesets_idx_.size(); w++)
        {
            int start = wavesets_idx_.at(w).start;
            int end = wavesets_idx_.at(w).end;

            TSample wave_length = end - start;

            if (wave_length <= (TSample)0.0)
            {
                continue;
            }

            for (int s = start; s <= end; ++s)
            {
                TSample ramp = -1.0 * wrap(((wavesets_idx_.at(w).end - s) / wave_length) * 2.0, -1.0, 1.0);
                if (ramp == -0.0)
                {
                    ramp = 0.0;
                }

                switch (shape)
                {
                case SAW:
                    buffer_.at(s) = ramp;
                    break;
                case SINE:
                    buffer_.at(s) = sin(M_PI * ramp);
                    break;
                case TRIANGLE:
                    if (fabs(ramp) >= 0.0 && fabs(ramp) <= 0.5)
                    {
                        buffer_.at(s) = ramp * 2.0;
                    }
                    else if (ramp > 0.5)
                    {
                        buffer_.at(s) = (1.0 - ramp) * 2.0;
                    }
                    else if (ramp < -0.5)
                    {
                        buffer_.at(s) = (1.0 + ramp) * -2.0;
                    }
                    break;
                case PULSE:
                    if (s < wavesets_idx_.at(w).half)
                    {
                        buffer_.at(s) = -1.0;
                    }
                    else
                    {
                        buffer_.at(s) = 1.0;
                    }
                    break;
                }

                buffer_.at(s) *= wavesets_idx_.at(w).peak;
            }
        }

        return true;
    }

    return false;
}

}

#endif // WAVESETS_H_
