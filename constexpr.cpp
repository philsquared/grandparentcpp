#include <sstream>
#include "catch.hpp"

namespace Cpp98 {
    template<unsigned n>
    struct factorial {
        enum {
            value = n * factorial<n - 1>::value
        };
    };

    template<>
    struct factorial<0> {
        enum {
            value = 1
        };
    };
}
namespace Cpp11 {
    [[deprecated("Use for loop version instead")]]
    constexpr unsigned factorial( unsigned n ) {
        return n == 0
            ? 1
            : n * factorial(n-1);
    }
}
namespace Cpp14 {
    constexpr auto factorial(unsigned n) {
        unsigned result = 1;
        for (unsigned i = 2; i <= n; ++i)
            result *= i;
        return result;
    }
}

TEST_CASE( "CT Factorial" ) {

    SECTION( "C++98" ) {
        using namespace Cpp98;

        REQUIRE(factorial<3>::value == 6);

        char array[factorial<3>::value];

        REQUIRE(sizeof(array) == 6);
    }

    SECTION( "C++11" ) {
        using namespace Cpp11;

        REQUIRE(factorial(3) == 6);

        char array[factorial(3)];

        REQUIRE(sizeof(array) == 6);
    }

    SECTION( "C++14" ) {
        using namespace Cpp14;

        REQUIRE(factorial(3) == 6);

        char array[factorial(3)];

        REQUIRE(sizeof(array) == 6);
    }

    SECTION( "C++17" ) {
        auto factorial = []( auto n ) {
            using Type = decltype(n);
            Type result = Type()+1;
            for( Type i = result+1; i <= n; ++i )
                result *= i;
            return result;
        };

        REQUIRE( factorial(3) == 6 );
        REQUIRE( factorial(3.0) == 6.0 );

        char array[factorial(3)];

        REQUIRE(sizeof(array) == 6);
    }
}

namespace Cpp11 {
    std::string join() {
        return {};
    }

    template<typename T, typename... Ts>
    auto join( T&& firstValue, Ts&&... restOfValues ) {
        std::ostringstream oss;
        oss << firstValue << join( restOfValues... );
        return oss.str();
    }
}

namespace Cpp17 {

    template<typename T, typename... Ts>
    auto join( T&& firstValue, Ts&&... restOfValues ) {
        std::ostringstream oss;
        oss << firstValue;
        if constexpr( sizeof...(restOfValues) > 0 ) {
            oss << join(restOfValues...);
        }
        return oss.str();
    }

    template<typename... Ts>
    auto join2( Ts&&... allValues ) {
        std::ostringstream oss;
        ((oss << allValues), ...);
        return oss.str();
    }

    template<typename... Ts>
    auto addAll( Ts&&... allValues ) {
        return ( allValues + ... );
    }
}

TEST_CASE( "Printer" ) {
    SECTION( "C++11/14" ) {
        REQUIRE( Cpp11::join("hello", ", world ", 42) == "hello, world 42" );
    }
    SECTION( "C++17 - if constexpr" ) {
        REQUIRE( Cpp17::join("hello", ", world ", 42) == "hello, world 42" );
    }
    SECTION( "C++17 - fold expression" ) {

        // Version using fold expressions
        REQUIRE( Cpp17::join2("hello", ", world ", 42) == "hello, world 42" );

        // Adding compatible types (better example)
        REQUIRE( Cpp17::addAll( 1, 1, 3, 5 ) == 10 );
        REQUIRE( Cpp17::addAll( 1, 1.1, 3, 5 ) == 10.1 );

        // ... even strings
        using namespace std::string_literals;
        REQUIRE( Cpp17::addAll( "1"s, "1"s, "3"s, "5"s ) == "1135" );

    }
}
