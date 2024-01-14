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

#ifndef LORENZ_H_
#define LORENZ_H_

#include <algorithm>

#if __cplusplus >= 202002L
#include<concepts>
#endif

namespace soutel
{

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
class Lorenz
{
public:
    Lorenz(const TSample &sigma = (TSample)10.0,
           const TSample &rho = (TSample)28.0,
           const TSample &beta = (TSample)(8.0 / 3.0),
           const TSample &x = (TSample)0.01,
           const TSample &y = (TSample)0.0,
           const TSample &z = (TSample)0.0,
           const TSample &t = (TSample)0.099)
    {
        set_sigma(sigma);
        set_rho(rho);
        set_beta(beta);
        set_x(x);
        set_y(y);
        set_z(z);
        set_t(t);
    }

    void set_sigma(const TSample &sigma)
    {
        sigma_ = sigma;
    }

    void set_rho(const TSample &rho)
    {
        rho_ = rho;
    }

    void set_beta(const TSample &beta)
    {
        beta_ = beta;
    }

    void set_x(const TSample &x)
    {
        x_ = x;
    }

    void set_y(const TSample &y)
    {
        y_ = y;
    }

    void set_z(const TSample &z)
    {
        z_ = z;
    }

    void set_t(const TSample &t)
    {
        t_ = std::clamp(t, (TSample)0.0, (TSample)0.025);
    }


    TSample get_sigma()
    {
        return sigma_;
    }

    TSample get_rho()
    {
        return rho_;
    }

    TSample get_beta()
    {
        return beta_;
    }

    TSample get_x()
    {
        return x_;
    }

    TSample get_y()
    {
        return y_;
    }

    TSample get_z()
    {
        return z_;
    }

    inline void step()
    {
        if (t_ > 0.0)
        {
            TSample x_1 = sigma_ * (y_ - x_);
            TSample y_1 = x_ * (rho_ - z_) - y_;
            TSample z_1 = x_ * y_ - beta_ * z_;

            x_ += t_ * x_1;
            y_ += t_ * y_1;
            z_ += t_ * z_1;
        }
    }

    inline void step(TSample &x, TSample &y, TSample &z)
    {
        step();

        x = x_;
        y = y_;
        z = z_;
    }

private:
    TSample sigma_, beta_, rho_;
    TSample x_, y_, z_;
    TSample t_;
};

}

#endif // LORENZ_H_