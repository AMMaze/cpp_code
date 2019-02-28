#include "shop.h"
#include <list>
#include <iostream>
#include <mutex>
#include <chrono>

#ifdef USE_QUEUE
std::list<std::tuple<const std::string, ProdEvents, int>> IShop::events;
std::list<std::tuple<const std::string, ProdEvents, int>> IShop::stop;
#else
int IShop::event_id = 0;
std::tuple<std::string, ProdEvents, int> IShop::cur_event;
#endif
std::mutex IShop::out;
std::mutex IShop::qlck;
std::mutex IShop::destruct_lck;
std::condition_variable IShop::qwait; 

int IShop::shops_cnt = 0; 

void IShop::AddEvent(const std::string& name, ProdEvents event) {
    //std::unique_lock<std::mutex> lock(qlck);
    qlck.lock();
#ifdef USE_QUEUE
    events.push_back(std::make_tuple(name, event, 0));
#else
    if (event_id != 0){
        while(true) {
            if (std::get<2>(cur_event) != shops_cnt) {
                qlck.unlock();
                qwait.notify_all();
            } else 
                break;
            qlck.lock();
        }
    }
    cur_event = std::make_tuple(name, event, 0);
    event_id++;
#endif
    qlck.unlock();
    qwait.notify_all();
};

void IShop::AddStop(const std::string& name) {
    //std::unique_lock<std::mutex> lock(qlck);
    qlck.lock();
#ifdef USE_QUEUE
    stop.push_back(std::make_tuple(name, SALES_CHANGE, 0));
#else
    if (event_id != 0){
        while(true) {
            if (std::get<2>(cur_event) != shops_cnt) {
                qlck.unlock();
                qwait.notify_all();
            } else 
                break;
            qlck.lock();
        }
    }
    cur_event = std::make_tuple(name, STOP_SALES, 0);
    event_id++;
#endif
    qlck.unlock();
    qwait.notify_all();
};

IShop::~IShop () {
    std::lock_guard<std::mutex> lock(destruct_lck);
    join_observer.store(true);
    qwait.notify_all();
    obsrv.join();
    shops_cnt--;
    if (shops_cnt == 0)
        Reset();
};

IShop::IShop (const std::string& n): name(n), obsrv(&IShop::Observe, this) {
    join_observer.store(false);
#ifdef USE_QUEUE
    pos = events.end(); 
    spos = stop.end(); 
#else
    last_event_id = 0;
#endif
    std::lock_guard<std::mutex> lock(qlck);
    shops_cnt++;
};

void IShop::AddProduct(IProduct* prod) {
    if (prod == nullptr)
        return;
    std::unique_lock<std::mutex> lock(local_lists);
    products[prod->GetName()] = prod;
    if (prod->GetSales()) {
        active_prods[prod->GetName()] = prod->GetPrice(); 
        lock.unlock();
        std::lock_guard<std::mutex> lck(out);
        std::cout << "In shop \'" << this->name <<"\'\n";
        std::cout << "Product " << prod->GetName() << " is now on sale for " 
            << prod->GetPrice() << " !\n\n";
    }
};  

void IShop::RemoveProduct(IProduct* prod) {
    if (prod == nullptr)
        return;
    std::unique_lock<std::mutex> lock(local_lists);
    auto it = products.find(prod->GetName());
    if (it == products.end())
        return;
    products.erase(it);
    auto active_it = active_prods.find(prod->GetName());
    if (active_it == active_prods.end())
        return;
    active_prods.erase(active_it);
    lock.unlock();
    std::lock_guard<std::mutex> lck(out);
    std::cout << "In shop \'" << this->name <<"\'\n";
    std::cout << "Product " << prod->GetName() << 
        " has been removed from sale\n\n";
};

void IShop::ProcessStops() {
#ifdef USE_QUEUE
    if(stop.empty()) 
        return;
    if(spos == stop.end())
        spos = stop.begin();
    
    bool loop = true;
    while(loop) {
        auto prev = spos;
        auto next = spos;
        next++;
        if(std::get<1>(*spos) == PLACEHOLDER) {
            if (next == stop.end()) 
                break;
        } else {
            auto name = std::get<0>(*spos);
            local_lists.lock();
            auto items = products.erase(name);
            if (items != 0) {
                auto count = active_prods.erase(name);
                if (count != 0) {
                    std::lock_guard<std::mutex> lck(out);
                    std::cout << "In shop \'" << this->name <<"\'\n";
                    std::cout << "Product " << name << 
                        " has been removed from sale\n\n";
                }
            }
            local_lists.unlock();
            if (next == stop.end()) { 
                stop.push_back(std::make_tuple("_", PLACEHOLDER, 0));
                spos++;
                std::get<2>(*prev)++;
                if (std::get<2>(*prev) == shops_cnt)
                    events.erase(prev);
                break;
            }
        }
        spos++;
        std::get<2>(*prev)++;
        if (std::get<2>(*prev) == shops_cnt)
            events.erase(prev);
    }
#endif
};

#ifdef USE_QUEUE
void IShop::ProcessEvents() {
    //std::lock_guard<std::mutex> lock(qlck);
    if(events.empty()) 
        return;
    if(pos == events.end())
        pos = events.begin();
    
    ProcessStops();
    bool loop = true;
    while(loop) {
        auto prev = pos;
        auto next = pos;
        next++;
        if(std::get<1>(*pos) == PLACEHOLDER) {
            if (next == events.end()) 
                break;
        } else {
            auto name = std::get<0>(*pos);
            local_lists.lock();
            auto item = products.find(name);
            if (item != products.end()) {
                if (std::get<1>(*pos) == PRICE_CHANGE) {
                    auto active = active_prods.find(name);
                    if (active != active_prods.end()) {
                        active->second = (item->second)->GetPrice(); 
                        std::lock_guard<std::mutex> lck(out);
                        std::cout << "In shop \'" << this->name <<"\'\n";
                        std::cout << "Price if the product " << name  
                            <<" has been changed. Now it's " << active->second
                            << std::endl << std::endl;
                    }
                } else {
                    if(active_prods.find(name) == active_prods.end()) {
                        active_prods[name] = (item->second)->GetPrice(); 
                        std::lock_guard<std::mutex> lck(out);
                        std::cout << "In shop \'" << this->name <<"\'\n";
                        std::cout << "Product " << name 
                            << " is now on sale for " 
                            << active_prods[name] << " !\n\n";
                    }
                }
            }
            local_lists.unlock();
            if (next == events.end()) { 
                events.push_back(std::make_tuple("_", PLACEHOLDER, 0));
                pos++;
                std::get<2>(*prev)++;
                if (std::get<2>(*prev) == shops_cnt)
                    events.erase(prev);
                break;
            }
        }
        pos++;
        std::get<2>(*prev)++;
        if (std::get<2>(*prev) == shops_cnt)
            events.erase(prev);
    }
};
#else
void IShop::ProcessEvents() {
    if(event_id == 0 || event_id == last_event_id) 
        return;
    auto name = std::get<0>(cur_event);
    local_lists.lock();
    auto item = products.find(name);
    if (item == products.end()) {
        local_lists.unlock();
        last_event_id = event_id;
        std::get<2>(cur_event)++;
        return;
    }

    if (std::get<1>(cur_event) == PRICE_CHANGE) {
        auto active = active_prods.find(name);
        if (active != active_prods.end()) {
            active->second = (item->second)->GetPrice(); 
            std::lock_guard<std::mutex> lck(out);
            std::cout << "In shop \'" << this->name <<"\'\n";
            std::cout << "Price if the product " << name  
                <<" has been changed. Now it's " << active->second
                << std::endl << std::endl;
        }
    } else {
        auto active = active_prods.find(name);
        if(active == active_prods.end()) {
            if (std::get<1>(cur_event) != STOP_SALES) {
                active_prods[name] = (item->second)->GetPrice(); 
                std::lock_guard<std::mutex> lck(out);
                std::cout << "In shop \'" << this->name <<"\'\n";
                std::cout << "Product " << name << " is now on sale for " 
                    << active_prods[name] << " !\n\n";
            } else 
                products.erase(item);
        } else {
            if (std::get<1>(cur_event) == STOP_SALES) {
                products.erase(item);
                active_prods.erase(active);
                std::lock_guard<std::mutex> lck(out);
                std::cout << "In shop \'" << this->name <<"\'\n";
                std::cout << "Product " << name << 
                    " has been removed from sale\n\n";
            }
        }
    }
    local_lists.unlock();
    last_event_id = event_id;
    std::get<2>(cur_event)++;

};
#endif


void IShop::ListItems() {
    qlck.lock();
    ProcessEvents();
    qlck.unlock();
    std::lock_guard<std::mutex> lock(out);
    std::cout << "#In shop \'" << name <<"\'\n";
    std::lock_guard<std::mutex> lck(local_lists);
    if (active_prods.empty()) {
        std::cout << "Nothing on sale\n\n";
        return;
    }
    std::cout << "Items on sale:\n";
    for (const auto& it : active_prods) {
        std::cout << it.first << " for " << it.second << std::endl;
    }
    std::cout<< std::endl;
};

void IShop::Observe() {
    while (!join_observer.load()) {
        std::unique_lock<std::mutex> lock(qlck);
        qwait.wait(lock);
        if (join_observer.load())
            return;
        ProcessEvents();
    }

};

void IShop::Reset() {
#ifdef USE_QUEUE
    events.clear();
    stop.clear();
#else
    event_id = 0;
#endif
};


