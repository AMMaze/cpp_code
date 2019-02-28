#pragma once

#include <memory>
#include "generators.h"
#include <string>
#include <vector>

class TFactory {
    struct TParams;
    class TImpl;
    std::unique_ptr<const TImpl> Impl;


    using vect = std::vector<double>;
    using vPtr = std::unique_ptr<vect>;

public:
    TFactory();
    ~TFactory();
    
    std::unique_ptr<TRandomNumberGenerator> CreateObject(
            const std::string& name, double prob) const;
    
    std::unique_ptr<TRandomNumberGenerator> CreateObject(
           const std::string& name, vect& Xvect, vect& Pvect) const;
    
    std::unique_ptr<TRandomNumberGenerator> CreateObject(
           const std::string& name, vect&& Xvect, vect&& Pvect) const;
   
    //хотел сделать шаблон для произвольных параметров конструктора,
    //но пришлось бы перетаскивать всю реализацию TParams и TImpl сюда
    /*template<class... T>
    std::unique_ptr<TRandomNumberGenerator> CreateObject(
           const std::string& name, T&&... args) const {
        return Impl->CreateObject(name, TParams(std::forward<T>(args)...));
    };
    */
    std::vector<std::string> GetAvailableObjects() const;
};
