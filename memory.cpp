#include "catch.hpp"
#include <memory>

namespace Cpp98 {
    class MyClass {
        std::string m_name;
        std::vector<std::string> m_data;
    public:
        MyClass(std::string const& name, std::vector<std::string> const& data) : m_name(name), m_data(data) {}

        std::string name() const { return m_name; }
        std::string data( int i ) const { return m_data.at(i); }

        size_t size() const { return m_data.size(); }
    };
}
namespace Cpp11 {

    template<typename T>
    std::unique_ptr<T> make_unique() {
        return std::unique_ptr<T>( new T );
    }
//    template<typename T, typename Arg1>
//    std::unique_ptr<T> make_unique( Arg1 const& arg1 ) {
//        return std::unique_ptr<T>( new T( arg1 ) );
//    }
//    template<typename T, typename Arg1, typename Arg2>
//    std::unique_ptr<T> make_unique( Arg1 const& arg1, Arg2 const& arg2 ) {
//        return std::unique_ptr<T>( new T( arg1, arg2 ) );
//    }
//    template<typename T, typename Arg1, typename Arg2, typename Arg3>
//    std::unique_ptr<T> make_unique( Arg1 const& arg1, Arg2 const& arg2, Arg3 const& arg3 ) {
//        return std::unique_ptr<T>( new T( arg1, arg2, arg3 ) );
//    }
    // ...etc

    // use forwarding refs
    // don't use move because these are not necessarily rvalues
    template<typename T, typename Arg1>
    std::unique_ptr<T> make_unique( Arg1&& arg1 ) {
        return std::unique_ptr<T>( new T( std::forward<Arg1>( arg1 ) ) );
    }
    template<typename T, typename Arg1, typename Arg2>
    std::unique_ptr<T> make_unique( Arg1&& arg1, Arg2&& arg2 ) {
        return std::unique_ptr<T>( new T( std::forward<Arg1>( arg1 ), std::forward<Arg2>( arg2 ) ) );
    }
    template<typename T, typename Arg1, typename Arg2, typename Arg3>
    std::unique_ptr<T> make_unique( Arg1&& arg1, Arg2&& arg2, Arg3&& arg3 ) {
        return std::unique_ptr<T>( new T( std::forward<Arg1>( arg1 ), std::forward<Arg2>( arg2 ), std::forward<Arg3>( arg3 ) ) );
    }
    // - perfect forwarding

    // Then use variadic templates

//    template<class T, class... Args>
//    std::unique_ptr<T> make_unique( Args&&... args ) {
//        return std::unique_ptr<T>( new T( std::forward<Args>( args )... ) );
//    }

    class MyClass {
        std::string m_name;
        std::vector<std::string> m_data;
    public:
        MyClass( std::string name, std::vector<std::string> data )
                : m_name( std::move( name ) ),
                  m_data( std::move( data ) ) {}

        MyClass( MyClass &&other ) noexcept
        :   m_name( std::move( other.m_name ) ),
            m_data( std::move( other.m_data ) )
        {}

        std::string name() const { return m_name; }
        std::string data( int i ) const { return m_data.at(i); }

        size_t size() const { return m_data.size(); }
    };
}

TEST_CASE( "Heap memory management" ) {

    SECTION( "C++98" ) {
        using namespace Cpp98;

        std::vector<std::string> data;
        data.push_back("first");

        SECTION( "raw alloc" ) {

            MyClass *obj = new MyClass("Harry", data); // data copied in

            REQUIRE(obj->name() == "Harry");
            REQUIRE(obj->size() == 1);
            REQUIRE(obj->data(0) == "first");
            REQUIRE_THROWS( obj->data(1) );

            delete obj; // leaks if you forget this, or if an exception is thrown
        }
        SECTION( "auto_ptr" ) {
            std::auto_ptr<MyClass> obj( new MyClass("Harry", data) ); // data copied in

            REQUIRE(obj->name() == "Harry");
            REQUIRE(obj->size() == 1);
            REQUIRE(obj->data(0) == "first");

            // auto_ptr deletes obj in destructor - even via exception

            // However: -
            // dangerous to pass around - ownership is _transferred_
            // can't share ownership
            // can't use in a std container (because would be dangerous)
            // - deprecated in C++11, removed in C++17

            // Common alternatives: boost::scoped_ptr, boost::shared_ptr, Loki::SmartPtr
            // unique ownership a thorny problem in C++98
        }
    }
    SECTION( "C++11" ) {
        using namespace Cpp11;

        // Now using init list
        std::vector<std::string> data = { "first", "second" };

        SECTION( "unique_ptr - data copied" ) {
            std::unique_ptr<MyClass> obj(new MyClass("Harry", data)); // data copied in

            REQUIRE(obj->name() == "Harry");
            REQUIRE(obj->size() == 2);
            REQUIRE(obj->data(0) == "first");
        }

        SECTION( "unique_ptr - data moved" ) {
            std::unique_ptr<MyClass> obj(new MyClass("Harry", std::move(data)));
            // data moved in - no longer usable
            // std::move doesn't actually "move" anything
            // it just, effectively, "casts" it's arg to an rvalue reference
            // an rvalue reference can be thought of as an unnamed object - usually a temporary
            // - also variables in a return statement

            REQUIRE(obj->name() == "Harry");
            REQUIRE(obj->size() == 2);
            REQUIRE(obj->data(0) == "first");

            REQUIRE( data.size() == 0 ); // !!! Undefined behaviour - don't do this
        }

        SECTION( "unique_ptr - temp data moved" ) {
            std::unique_ptr<MyClass> obj(new MyClass("Harry", {"first", "second"}));
            REQUIRE(obj->size() == 2);
        }

        SECTION( "unique_ptr - our own moveable object" ) {
            MyClass stackObject("Harry", {"first", "second" } );
            std::unique_ptr<MyClass> obj(new MyClass( std::move( stackObject ) ) );
            REQUIRE(obj->size() == 2);
            REQUIRE( stackObject.size() == 0 ); // UB - don't do this
        }

        SECTION( "shared_ptr" ) {
            std::shared_ptr<MyClass> obj(new MyClass("Harry", {"first", "second"}));
            std::shared_ptr<MyClass> obj2 = obj;

            REQUIRE( obj->name() == "Harry" );
            REQUIRE( obj2->name() == "Harry" );
            REQUIRE( obj.get() == obj2.get() ); // Same underlying object
        }

        // It's possible to leak memory if an exception is thrown between an object being allocated
        // and a unique_ptr or shared_ptr constructed

        SECTION( "make_shared" ) {
            // can use auto
            // (but have to specify vector>
            auto obj = std::make_shared<MyClass>( "Harry", std::vector<std::string>{"first", "second"} );
        }

        SECTION( "make_unique" ) {
            auto obj = Cpp11::make_unique<MyClass>( "Harry", std::vector<std::string>{"first", "second"} );
        }

    }
}
