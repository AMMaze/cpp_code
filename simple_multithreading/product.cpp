#include "product.h"
#include <memory>
#include "shop.h"
#include <iostream>


void IProduct::Attach(SShop& shop) {
    std::lock_guard<std::mutex> lock(IShop::destruct_lck);
    if (shop == nullptr)
        return;
    shop->AddProduct(this);    
};

void IProduct::Detach(SShop& shop) {
    std::lock_guard<std::mutex> lock(IShop::destruct_lck);
    if (shop == nullptr)
        return;
    shop->RemoveProduct(this);
};

double IProduct::GetPrice() {
    std::lock_guard<std::mutex> lock(m);
    return price;
};

IProduct::IProduct(double p, const std::string& n): price(p), name(n) {
    sales.store(false);
};

IProduct::~IProduct() {
    StopSales();
};

void IProduct::StartSales() {
    sales.store(true);
    IShop::AddEvent(GetName(), SALES_CHANGE);
};

void IProduct::StopSales() {
    sales.store(false);
    IShop::AddStop(GetName());
};

bool IProduct::GetSales() {
    return sales.load();
};

void IProduct::ChangePrice(double new_price) {
    m.lock();
    price = new_price;
    m.unlock();
    IShop::AddEvent(GetName(), PRICE_CHANGE);
};

const std::string IProduct::GetName() {
    return name;
};
