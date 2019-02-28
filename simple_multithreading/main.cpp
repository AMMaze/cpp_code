#include "shop.h"
#include "product.h"
#include <string>
#include <iostream>
#include <memory>
#include <thread>
#include <chrono>

class ProductA : public IProduct {
public:
    ProductA(double p): IProduct(p, "A-item") {};
};

class ProductB : public IProduct {
public:
    ProductB(double p): IProduct(p, "B-item") {};
};

class ProductC : public IProduct {
public:
    ProductC(double p): IProduct(p, "C-item") {};
};

int main() {

    std::shared_ptr<IShop> shopPtr1(new IShop("SHOP_1"));
    std::shared_ptr<IShop> shopPtr2(new IShop("SHOP_2"));
    std::shared_ptr<IShop> shopPtr3(new IShop("SHOP_3"));

    std::thread prods ([&]() {
        ProductA prod_a(15);
        prod_a.StartSales();
        prod_a.Attach(shopPtr1);
        prod_a.Attach(shopPtr2);
        ProductB prod_b(13);
        prod_b.StartSales();
        prod_b.Attach(shopPtr3);
        prod_b.Attach(shopPtr1);
        //std::this_thread::sleep_for(std::chrono::milliseconds(10));
        prod_a.Detach(shopPtr2);
        //prod_b.Detach(shopPtr3);
        prod_b.ChangePrice(12.99);
        prod_a.ChangePrice(16);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        ProductC prod_c(45);
        prod_c.Attach(shopPtr1);
    });

    shopPtr1->ListItems();
    //shopPtr1.reset();
    shopPtr2->ListItems();
    shopPtr3->ListItems();
    
    prods.join();
    return 0;
};
