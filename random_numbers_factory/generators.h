#pragma once

#include <memory>
#include <vector>

class TRandomNumberGenerator {
public:
    virtual ~TRandomNumberGenerator();
    virtual double Generate() const = 0;
};

class TPoissonGenerator : public TRandomNumberGenerator {
    double lambda;

public:
    TPoissonGenerator (double x) noexcept;
    ~TPoissonGenerator();
    virtual double Generate() const noexcept;;
};

class TBernoulliGenerator : public TRandomNumberGenerator {
    double p;
public:
    TBernoulliGenerator (double x) noexcept;
    ~TBernoulliGenerator();
    virtual double Generate() const noexcept;
};

class TGeometricGenerator : public TRandomNumberGenerator {
    double p;
public:
    TGeometricGenerator (double x) noexcept;
    ~TGeometricGenerator();
    virtual double Generate() const noexcept;
};

class TFiniteGenerator : public TRandomNumberGenerator {
    using vect = std::vector<double>;
    using vPtr = std::unique_ptr<vect>;

    vPtr Xvect;
    vPtr Pvect;
public:
    TFiniteGenerator (vect&& x, vect&& p) noexcept;
    ~TFiniteGenerator();
    virtual double Generate() const noexcept;
};

class TGenerator {
    using GenPtr = std::unique_ptr<TRandomNumberGenerator>;
    GenPtr gen;
public:
    TGenerator (GenPtr g) noexcept;
    
    double Generate() const noexcept;

    double Average(int n) const noexcept;

    bool isNull() const noexcept;
};
