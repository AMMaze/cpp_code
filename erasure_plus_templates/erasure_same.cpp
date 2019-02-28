#include <iostream>
#include <typeinfo>
#include <memory>
#include <type_traits>

class Any
{
public:
	template<typename T>
	Any(const T& t) : 
        held_(std::unique_ptr<base_holder>(new holder<T>(t))){}
	~Any(){ }
	template<typename U>
	U get() const
	{
		if(typeid(U) != held_->type_info())
			throw std::runtime_error("Bad any cast");
		return static_cast<holder<U>*>(held_.get())->t_;
	}
private:
	struct base_holder
	{
		virtual ~base_holder(){}
		virtual const std::type_info& type_info() const = 0;
	};
	
	template<typename T> struct holder : base_holder
	{
		holder(const T& t) : t_(t){}
		const std::type_info& type_info() const
		{
			return typeid(t_);
		}
		T t_;
	};
private:
    std::unique_ptr<base_holder> held_;
};



template<class... Args>
struct are_same : std::true_type {
};

template<class H1, class H2, class... Tail>
struct are_same<H1, H2, Tail...> : are_same<H2, Tail...> {
    static constexpr bool value = 
        std::is_same<std::decay_t<H1>, std::decay_t<H2>>::value && 
            are_same<H2, Tail...>::value;
};

template<class... TArgs>
constexpr bool are_same_v = are_same<TArgs...>::value;

int main() {

    Any a(5);
    std::cout << a.get<int>() << std::endl;
    try {
        a.get<std::string>();
    }
    catch(std::runtime_error& er){
        std::cout << er.what() << std::endl;
    }


    static_assert(are_same_v<int, int32_t, signed int>, "compile assert");
    
    return 0;
}
