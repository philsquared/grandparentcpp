#include <map>
#include <iostream>
#include "catch.hpp"

TEST_CASE( "map insert" ) {

    std::map<int, std::string> numberNames = {
            { 1, "one" },
            { 2, "two" },
            { 3, "three "}
        };

    SECTION( "Full result type" ) {
        std::pair<std::map<int, std::string>::const_iterator, bool> result = numberNames.insert({4, "four"});
        REQUIRE( result.second == true );
    }

    SECTION( "auto result type" ) {
        // using auto is cleaner, but even harder to reason about!
        auto result2 = numberNames.insert({4, "four"});
    }

    SECTION( "structured bindings" ) {
        // structured bindings let us break the result apart
        auto[it, added] = numberNames.insert({4, "four"});
        REQUIRE( added == true );

        SECTION( "in for loops" ) {
            for( auto const& [number, name] : numberNames )
                std::cout << number << ": " << name << "\n";
        }
    }

    // not just pairs - any structs/ classes with accessible members (inc tuples) - and arrays
}
