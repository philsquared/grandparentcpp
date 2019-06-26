#include "catch.hpp"

#include <vector>
#include <numeric>
#include <sstream>

using namespace Catch::Matchers;


TEST_CASE( "Vector of ints to vector of string" ) {

    SECTION( "C++98" ) {

        std::vector<int> fib;

        fib.reserve( 6 );

        fib.push_back( 1 );
        fib.push_back( 1 );
        fib.push_back( 2 );
        fib.push_back( 3 );
        fib.push_back( 5 );
        fib.push_back( 8 );
        // ...

        std::vector<std::string> stringFib;
        stringFib.reserve( fib.size() );

        std::vector<int>::const_iterator it = fib.begin();
        std::vector<int>::const_iterator itEnd = fib.end();

        for(; it != itEnd; ++it ) {
            // No easy way to convert int to string
            // - could use non-std C, itoa
            // - or C's sprintf
            // Here we'll go the "idiomatic" C++(98) way
            std::stringstream ss;
            ss << *it;
            // In general we should really check ss.bad()
            // and we might need to optimise perf by moving declaration outside the loop
            // and clear each time
            // still performs badly

            stringFib.push_back( ss.str() );
        }

        std::vector<std::string> expected;
        expected.push_back( "1" );
        expected.push_back( "1" );
        expected.push_back( "2" );
        expected.push_back( "3" );
        expected.push_back( "5" );
        expected.push_back( "8" );

        REQUIRE_THAT( stringFib, Equals( expected ) );
    }

    SECTION( "C++11" ) {
        // Initializer list
        std::vector<int> fib = { 1, 1, 2, 3, 5, 8 };

        std::vector<std::string> expected = { "1", "1", "2", "3", "5", "8" };

        // v1
        SECTION("Use to_string") {
            std::vector<std::string> stringFib;
            stringFib.reserve( fib.size() );

            std::vector<int>::const_iterator it = fib.begin();
            std::vector<int>::const_iterator itEnd = fib.end();

            for(; it != itEnd; ++it ) {
                stringFib.push_back( std::to_string( *it ) );
            }

            REQUIRE_THAT( stringFib, Equals(expected) );
        }

        // v2
        SECTION("Use auto") {

            std::vector<std::string> stringFib;
            // reserve?

            auto itEnd = fib.end();
            for( auto it = fib.begin(); it != itEnd; ++it ) {
                stringFib.push_back( std::to_string( *it ) );
            }

            REQUIRE_THAT( stringFib, Equals(expected) );
        }

        // v2
        SECTION( "range-based for" ) {

            std::vector<std::string> stringFib;
            // reserve?

            for ( int i : fib ) {
                stringFib.push_back( std::to_string( i ) );
            }

            REQUIRE_THAT( stringFib, Equals(expected) );
        }

        // v2.5?
        SECTION( "AAA?" )  {
            std::vector<std::string> stringFib;

            for ( auto i : fib ) { // Is auto better?
                stringFib.push_back( std::to_string( i ) );
            }

            REQUIRE_THAT( stringFib, Equals(expected) );
        }

        // v3a
        SECTION( "use algorithm (no lambda)" ) {

            std::vector<std::string> stringFib;

            struct Converter {
                std::string operator()( int i ) const {
                    return std::to_string( i );
                }
            };

            // can't (easily) pass pointer to to_string
            std::transform( fib.begin(), fib.end(), std::back_inserter( stringFib ), Converter() );
            REQUIRE_THAT( stringFib, Equals(expected) );
        }

        // v3b
        SECTION( "use algorithm (lambda)" ) {

            std::vector<std::string> stringFib;

            std::transform(
                    fib.begin(), fib.end(),
                    std::back_inserter( stringFib ),
                    []( int i ) { return std::to_string( i ); } );

            REQUIRE_THAT( stringFib, Equals(expected) );
        }

        // v3c
        SECTION( "use algorithm (lambda with captures)" ) {

            std::vector<std::string> stringFib;

            std::string prefix = ":-) ";
            // not automatically captured
            // can capture specific variables by value, by reference or all by value, by reference (or mix)
            // references may dangle if lifetime of lambda exceeds
            std::transform(
                    fib.begin(), fib.end(),
                    std::back_inserter( stringFib ),
                    [prefix]( int i ) { return prefix + std::to_string( i ); } );

            //REQUIRE_THAT( stringFib, Equals(expected) ); // fails as-is
        }
    }

    SECTION( "C++14" ) {
        std::vector<int> fib = {1, 1, 2, 3, 5, 8};
        std::vector<std::string> expected = {"1", "1", "2", "3", "5", "8"};

        SECTION( "auto lambda" ) {

            std::vector<std::string> stringFib;

            std::transform(
                    fib.begin(), fib.end(),
                    std::back_inserter( stringFib ),
                    []( auto i ) { return std::to_string( i ); } ); // use auto, here

            REQUIRE_THAT( stringFib, Equals(expected) );
        }

        SECTION( "algo with explicit struct, using auto return types" ) {

            std::vector<std::string> stringFib;

            struct Converter {
                auto operator()( int i ) const /* -> std::string */ {
                    return std::to_string( i );
                }
            };

            // can't (easily) pass pointer to to_string
            std::transform( fib.begin(), fib.end(), std::back_inserter( stringFib ), Converter() );
            REQUIRE_THAT( stringFib, Equals(expected) );
        }
    }

    SECTION( "C++17" ) {
        using namespace std::string_literals;

        std::vector fib = {1, 1, 2, 3, 5, 8}; // CTAD
        std::vector expected = {"1"s, "1"s, "2"s, "3"s, "5"s, "8"s}; // Needs string literals for CTAD to work

        SECTION("auto lambda") {

            std::vector<std::string> stringFib;

            std::transform(
                    fib.begin(), fib.end(),
                    std::back_inserter(stringFib),
                    [](auto i) { return std::to_string(i); });

            REQUIRE_THAT(stringFib, Equals(expected));
        }
    }

}
