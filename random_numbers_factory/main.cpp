#include "generators.h"
#include "factory.h"
#include <memory>
#include <iostream>
#include <initializer_list>
#include <cmath>

void bernoilli_test(double mean = 0.25, int tries = 100) {
    TFactory factory;
    TGenerator gen = TGenerator(factory.CreateObject("bernoulli", mean));
    if (gen.isNull()) {
        std::cout << "Failed to create generator\n";
        return;
    }
    double emp_av = gen.Average(tries);
    std::cout << "Bernoulli:\n" << "Average: " << emp_av << std::endl;
    std::cout << "Mean: " << mean << std::endl;
    std::cout << "Delta: " << mean - emp_av << std::endl;
}

void poisson_test(double mean = 10, int tries = 100) {
    TFactory factory;
    TGenerator gen = TGenerator(factory.CreateObject("poisson", mean));
    if (gen.isNull()) {
        std::cout << "Failed to create generator\n";
        return;
    }
    double av = gen.Average(tries);
    std::cout << "Poisson:\n" << "Average: " << av << std::endl;
    std::cout << "Mean: " << mean << std::endl;
    std::cout << "Delta: " << mean - av << std::endl;
}

void geometric_test(double succ = 0.25, int tries = 100){
    TFactory factory;
    TGenerator gen = TGenerator(factory.CreateObject("geometric", succ));
    if (gen.isNull()) {
        std::cout << "Failed to create generator\n";
        return;
    }
    double av = gen.Average(tries);
    double mean = (1.0 - succ) / succ;
    std::cout << "Geometric:\n" << "Average: " << av << std::endl;
    std::cout << "Mean: " << mean << std::endl;
    std::cout << "Delta: " << mean - av << std::endl;
}

void finite_test(std::initializer_list<double> Xvect = {1, 2, 3, 4}, 
        std::initializer_list<double> Pvect = {0.35, 0.3, 0.2, 0.15}, int tries = 100){
    TFactory factory;
    TGenerator gen = TGenerator(factory.CreateObject("finite", Xvect, Pvect));
    if (gen.isNull()) {
        std::cout << "Failed to create generator\n";
        return;
    }
    double av = gen.Average(tries);
    auto p_it = Pvect.begin();
    auto x_it = Xvect.begin();
    double mean = 0;
    for (;p_it != Pvect.end(); ++p_it) {
        mean += (*x_it) * (*p_it);
        ++x_it;
    }
    std::cout << "Finite:\n" << "Average: " << av << std::endl;
    std::cout << "Mean: " << mean << std::endl;
    std::cout << "Delta: " << mean - av << std::endl;
}

int main() {
    bernoilli_test();
    std::cout << "\n";

    poisson_test();
    std::cout << "\n";

    geometric_test();
    std::cout << "\n";

    finite_test();

    return 0;
}
