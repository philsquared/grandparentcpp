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
    constexpr unsigned factorial( unsigned n ) {
        return n == 0
            ? 1
            : n * factorial(n-1);
    }
}
namespace Cpp14 {
    constexpr unsigned factorial( unsigned n ) {
        unsigned result = 1;
        for( unsigned i = 2; i<=n; ++i )
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
}