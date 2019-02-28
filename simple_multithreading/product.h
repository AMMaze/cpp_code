#pragma once

#include <memory>
#include <list>
#include <string>
#include "shop.h"
#include <mutex>
#include <atomic>

class IShop;

using SShop = std::shared_ptr<IShop>;

class IProduct {
    double price;
    std::atomic<bool> sales;
    std::mutex m;

    std::string name;
public:
    void Attach(SShop& shop);
    void Detach(SShop& shop);

    double GetPrice();

    IProduct(double p, const std::string& n);
    virtual ~IProduct();
    
    void StartSales();
    void StopSales();
    bool GetSales();

    void ChangePrice(double new_price);

    const std::string GetName();
    
};
