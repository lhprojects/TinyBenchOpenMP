#include <stddef.h>
#include <stdlib.h>
#include <string>
#include <chrono>
#include <algorithm>
#include <execution>
#include <memory>
#include <cmath>
#include "cpuid.h"
#include "omp.h"

void begin_section(std::string sec)
{
    printf("%*s\n", (int)std::max(0, 20 + (int)sec.length() / 2), sec.c_str());
    printf("vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv\n");
}

void end_section()
{
    printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
    printf("                                                 \n");
}

std::chrono::high_resolution_clock::time_point t0;
void set_t0()
{
    t0 = std::chrono::high_resolution_clock::now();
}

double cal_MOPS(size_t re, size_t len)
{
    auto t1 = std::chrono::high_resolution_clock::now();
    double GOPS = 1. * re * len / (double)std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
    return GOPS;
}

template<class Op>
void timeit(std::string const& title, size_t re, size_t len, Op op)
{
    for (size_t i = 0; i < re; ++i) {
        op();
    }


}

struct op_MulAdd {

    template<class U>
    auto operator()(U v) const
    {
        v = v / 2. + 1;
        return v;
    }
};

struct op_Exp {

    template<class U>
    auto operator()(U v) const
    {
        return std::exp(-v);
    }
};

struct op_SqrtExpSinCos {

    template<class U>
    auto operator()(U v) const
    {
        return sqrt(exp(sin(cos(v))));
    }
};

constexpr size_t cal_n(size_t n)
{
    return 1 << n;
}

template<class Op>
void bench_std_transform(std::string const &title, Op op)
{
    begin_section(title);
    auto* a = new double[cal_n(25)];
    std::fill(a, a + cal_n(25), 1.);
    auto f = std::unique_ptr<double>(a);

    printf("[%10s] %10s %10s %10s\n", "len", "seq[MOPS]", "par/seq", "OpenMP/seq");
    for (int i = 5; i < 25; ++i) {
        size_t n = cal_n(i);
        size_t re = size_t(1E8 / n > 1E7 ? 1E7 : 1E8 / n);

        {
            printf("[%10d]", (int)n);
        }
        auto mops0 = 1.;
        {
            set_t0();
            std::transform(std::execution::seq, a, a + n, a, op);
            mops0 = cal_MOPS(re, n);
            printf(" %10.2E", cal_MOPS(re, n));
        }
        {
            set_t0();
            std::transform(std::execution::par, a, a + n, a, op);
            printf(" %10.2f", cal_MOPS(re, n)/mops0);
        }
        {
            set_t0();
            #pragma omp parallel for
            for (int i = 0; i < (int)n; ++i) {
                a[i] = op(a[i]);
            }
            printf(" %10.2f", cal_MOPS(re, n) / mops0);
        }
        printf("\n");

    }
    end_section();
}

void omp_print()
{
#pragma omp parallel
    {
#pragma omp single
        {
            printf("omp %d threads\n", omp_get_num_threads());
        }
    }

}

int main()
{
    printf("%s\n", cpu_brand().c_str());
    omp_print();
    bench_std_transform("MulAdd", op_MulAdd());
    bench_std_transform("exp", op_Exp());
    bench_std_transform("SqrtExpSinCos", op_SqrtExpSinCos());
}
