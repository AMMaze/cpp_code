#include "generators.h"
#include <cmath>
#include <cstdlib>
#include <ctime>

TRandomNumberGenerator::~TRandomNumberGenerator(){}

    /*
     * Poisson generator
     */
TPoissonGenerator::TPoissonGenerator (double x) noexcept: lambda(x) {
    std::srand(std::time(0));
}


double TPoissonGenerator::Generate () const noexcept {
    double L = std::exp(-lambda);
    int k = 0;
    double p = 1.0;

    do {
        k++;
        p *= (double) std::rand() / (double) RAND_MAX;
    }
    while (p > L);
    return k - 1;
}

TPoissonGenerator::~TPoissonGenerator() {}

    /*
     * Bernoulli generator
     */
TBernoulliGenerator::TBernoulliGenerator (double x) noexcept : p(x) {
    std::srand(std::time(0));
}

double TBernoulliGenerator::Generate () const noexcept {
    double k = (double) std::rand() / (double) RAND_MAX;
    return (k < p) ? 1 : 0;
}

TBernoulliGenerator::~TBernoulliGenerator() {}

    /*
     * Geometric generator
     */
TGeometricGenerator::TGeometricGenerator (double x) noexcept : p(x) {
    std::srand(std::time(0));
}

double TGeometricGenerator::Generate() const noexcept {
    double r = (double) std::rand() / (double) RAND_MAX;
    return std::floor(std::log(r) / std::log(1 - p));
}

TGeometricGenerator::~TGeometricGenerator () {}

    /*
     * Finite generator
     */

TFiniteGenerator::TFiniteGenerator (vect&& x, vect&& p) noexcept : 
    Xvect(std::make_unique<vect>(std::forward<vect>(x))), 
    Pvect(std::make_unique<vect>(std::forward<vect>(p))) {
    std::srand(std::time(0));
} 

double TFiniteGenerator::Generate () const noexcept {
    double r = (double) std::rand() / (double) RAND_MAX;
    double sum = 0, hit; 
    int i = 0;
    do {
        sum += (*Pvect)[i];
        i++;
        hit = sum - r;
    } while (hit < 0);
    return (*Xvect)[--i];
}

TFiniteGenerator::~TFiniteGenerator() {}

    /*
     * Context
     */
TGenerator::TGenerator (GenPtr g) noexcept : gen(std::move(g)) {}

double TGenerator::Generate() const noexcept {
    return gen->Generate();
}

double TGenerator::Average(int n) const noexcept {
    double sum = 0;
    for (int i = 0; i < n; i++)
        sum += Generate();
    return sum / (double) n;
}

bool TGenerator::isNull() const noexcept {
    return gen.get() == nullptr;
}
