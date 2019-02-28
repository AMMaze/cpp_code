#include <ctime>
#include <vector>
#include <iostream>
#include <assert.h>
#include <algorithm>

struct TItem {
    int value;
    time_t timestamp;

    TItem (int v): value(v), timestamp(std::time(0)) {}

};
using Items = std::vector<TItem>;

bool operator== (const TItem& a, const TItem& b) {
    return a.value == b.value;
}

template <int... Targs>
Items MakeItemsSimple () {
    std::vector<TItem> v = {TItem(Targs)...};
    return v;
}

auto MakePredicate (const Items& vec) {
    return [&vec](int val) -> bool {
        return find(vec.begin(), vec.end(), TItem(val)) != vec.end();    
    };
}

int main () {
    Items items = MakeItemsSimple<0, 1, 4, 5, 6>();
    Items newItems = MakeItemsSimple<7, 15, 1>();
    auto isFound = MakePredicate(items);
    auto isFoundNew = MakePredicate(newItems);
    assert (isFound(0) == true);
    assert (isFound(7) == false);
    assert (isFoundNew(7) == true);
    assert (isFoundNew(6) == false);
    return 0;
}
