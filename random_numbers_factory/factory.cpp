#include "factory.h"
#include "generators.h"
#include <map>
#include <typeinfo>
#include <type_traits>
#include <utility>
#include <cmath>

    /*
     * Unified parameters for factory methods
     */
struct TFactory::TParams {

    #define TYPE(arg) std::remove_reference_t<decltype(arg)>

    template<typename... T>
	TParams(T&&... t) noexcept{
        for(auto& it : {t...})
            held_.push_back(std::unique_ptr<base_holder>(
                        new holder<TYPE(it)>(std::forward<TYPE(it)>(it))));
    } 

    #undef TYPE

    ~TParams(){ }
	
    template<typename U>
	U get(int idx = 0) const
	{
		if(typeid(U) != held_[idx]->type_info())
			throw std::runtime_error("Bad cast");
		return static_cast<holder<U>*>(held_[idx].get())->t_;
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
        holder(T&& t) : t_(std::forward<T>(t)){}
		const std::type_info& type_info() const
		{
			return typeid(t_);
		}
		T t_;
	};
    
    std::vector<std::unique_ptr<base_holder>> held_;
};

    /*
     * Factory Implementation
     */

#include <iostream>
class TFactory::TImpl {

    /*
     * Base class for actual creators
     */

    class ICreator {
    public:
        virtual ~ICreator(){}            
        virtual std::unique_ptr<TRandomNumberGenerator> Create(TParams&& arg) 
            const = 0;
    };

    using TCreatorPtr = std::shared_ptr<ICreator>;
    using TRegisteredCreators = std::map<std::string, TCreatorPtr>;
    TRegisteredCreators RegisteredCreators;

public:

    /*
     * Template for creator's concrete classes
     */

    template <class TCurrentObject>
    class TCreator : public ICreator {
        std::unique_ptr<TRandomNumberGenerator> Create 
            (TParams&& arg) const override {
            if constexpr (std::is_same_v<TCurrentObject, TFiniteGenerator>) {
                auto pvect = arg.get<vect>(1);
                double sum = 0;
                for (auto& it : pvect)
                    sum += it;
                if (std::abs(sum - 1.0) > 1e-10)
                    return nullptr;

                return std::unique_ptr<TRandomNumberGenerator> ( 
                        new TCurrentObject(
                            std::forward<vect>(arg.get<vect>(0)),
                            std::forward<vect>(arg.get<vect>(1))));
            }
            else {
                auto p = arg.get<double>();
                if constexpr (std::is_same_v<TCurrentObject, 
                        TPoissonGenerator>) {
                    if (p <= 0)
                        return nullptr;
                } else {
                    if ((p < 0) || (p > 1))
                        return nullptr;
                }
                return std::unique_ptr<TRandomNumberGenerator>(
                        new TCurrentObject(p));
            }

            return nullptr;
        }
    };

    
    TImpl() { RegisterAll(); }

    template<class T>
    void RegisterCreater (const std::string& name) {
        RegisteredCreators[name] = 
            std::make_shared<TCreator<T>>();
    }

    void RegisterAll() {
            RegisterCreater<TBernoulliGenerator>("bernoulli");
            RegisterCreater<TPoissonGenerator>("poisson");
            RegisterCreater<TGeometricGenerator>("geometric");
            RegisterCreater<TFiniteGenerator>("finite");
    }

    std::unique_ptr<TRandomNumberGenerator> CreateObject 
        (const std::string& n, TParams&& args) const {
        auto creator = RegisteredCreators.find(n);
        if (creator == RegisteredCreators.end())
            return nullptr;
        return creator->second->Create(std::forward<TParams>(args));
    }

    std::vector<std::string> GetAvailableObjects() const {
        std::vector<std::string> result;
        for(const auto& it : RegisteredCreators)
            result.push_back(it.first);
        return result;
    }
    
};

    /*
     * Factory Interface
     */
    
    std::unique_ptr<TRandomNumberGenerator> TFactory::CreateObject(
            const std::string& name, double prob) const {
        return Impl->CreateObject(name, TParams(prob));
    }
    
    std::unique_ptr<TRandomNumberGenerator> TFactory::CreateObject(
            const std::string& name, vect& Xvect, vect& Pvect) const {
        return Impl->CreateObject(name, TParams(Xvect, Pvect));
    }
    
    std::unique_ptr<TRandomNumberGenerator> TFactory::CreateObject(
            const std::string& name, vect&& Xvect, vect&& Pvect) const {
        return Impl->CreateObject(name, 
                TParams(std::move(Xvect), std::move(Pvect)));
    }
    
    std::vector<std::string> TFactory::GetAvailableObjects() const {
        return Impl->GetAvailableObjects();
    }

TFactory::TFactory() : Impl(std::make_unique<TFactory::TImpl>()) {}

TFactory::~TFactory() {}

