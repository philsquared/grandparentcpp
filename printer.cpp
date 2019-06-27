#include "catch.hpp"

#include <iostream>
#include <vector>

template<typename T>
//template<ranges::InputRange T> // with concepts
void print( T const& container ) {
    std::cout << "{ ";
    bool first = true;
    for( auto const& item : container ) {
        if( first )
            first = false;
        else
            std::cout << ", ";
        std::cout << item;
    }
    std::cout << " }\n";
}

TEST_CASE( "printer" ) {

    std::vector<int> numbers = { 1, 2, 3, 4, 5, 6 };
    print( numbers );
}
TEST_CASE( "printer2" ) {

    int number = 42;
//    print( number ); // compile error - better with concepts
}

TEST_CASE( "algo" ) {

    std::vector numbers = { 1, 2, 3, 4, 5, 6 };
    std::vector<int> result;

    for( int n : numbers ) {
        if( n % 2 == 0 ) {
            result.push_back( n * n );
        }
    }

    print( result );
}
TEST_CASE( "algo2" ) {

    std::vector numbers = { 1, 2, 3, 4, 5, 6 };
    std::vector<int> result;

    for( int n : numbers ) {
        if( n % 2 == 0 ) {
            int squared = n * n;
            if( squared > 10 )
                result.push_back( n * n );
        }
    }

    print( result );
}
TEST_CASE( "algo3" ) {

    std::vector numbers = { 1, 2, 3, 4, 5, 6 };
    std::vector<int> result;
    int total = 0;
    for( int n : numbers ) {
        if( n % 2 == 0 ) {
            int squared = n * n;
            if( squared > 10 ) {
                result.push_back(n * n);
                total += squared;
            }
        }
    }

    print( result );
    std::cout << "total: " << total << "\n";
}

