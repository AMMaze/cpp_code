#include <iostream>
#include <memory>
#include <vector>

class A {
public:
        int a;
        A () {
            std::cout << "A's constructor\n";
            a = 0;
        };

        A (int x): a(x) {
            std::cout << "A's constructor with arg\n";
        };

        A (const A& obj) {
            std::cout << "A's copy\n";
        };

        static void* operator new (size_t size) {
            std::cout << "operator new!" << std::endl;
            return ::operator new(size);
        }

        static void operator delete (void *p, size_t size) {
            std::cout << "operator delete!" << std::endl;
            return ::operator delete(p);
        }

        static void* operator new (size_t size, void* p) {
            std::cout << "operator new with pointer!\n";
            return ::operator new(size, p);
        };
        
        static void* operator new (size_t size, void* p, int x) {
            std::cout << "operator new with pointer and arg!\n";
            return ::new(p) A(x);
            //return ::operator new(size, p, x);
        };

};

template<typename T>
struct MyAlloc {

    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    template<class U>
    struct rebind {
        using other = MyAlloc<U>;
    };

    MyAlloc() noexcept {}
       
    MyAlloc(const MyAlloc&) noexcept {}
    
    template <class U>
    MyAlloc (const MyAlloc<U>&) noexcept {}
    
    ~MyAlloc() noexcept {}

    pointer allocate (size_type size) {
        std::cout << "allocate\n";
        return static_cast<pointer> (operator new(sizeof(value_type)*size));
    };
    
    void deallocate(pointer p, size_type size) {
        std::cout << "deallocate\n";
        operator delete(p);
    };
    
    template< class U, class... Args >
    void construct (U* p, Args&&... args) {
        std::cout << "Alloc construct\n";
        //new (static_cast<void*>(p)) U(std::forward<Args>(args)...);
        U::operator new (sizeof(U), p);
        *p = U(std::forward<Args>(args)...);
    };

};

int main() {
    MyAlloc<A> alloc;
    auto sp = std::allocate_shared<A>(alloc, 123);
    std::cout << sp -> a << std::endl;

    return 0;
}
