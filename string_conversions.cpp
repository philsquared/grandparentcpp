#include "catch.hpp"

#include <optional>
#include <sstream>
#include <variant>

namespace Cpp17 {

    // Writing a conversion from string to int without exceptions

    // 1. take an out reference
    [[nodiscard]] // must check return value
    auto parse_int1( std::string_view sv, int& result ) -> bool {
        std::stringstream ss;
        ss << sv;
        ss >> result;
        return !ss.fail();
    }

    // 2. returning optional
    auto parse_int2( std::string_view sv ) -> std::optional<int> {
        int i;
        std::stringstream ss;
        ss << sv;
        ss >> i;
        if( !ss.fail() )
            return i;
        else
            return {};
    }

    // 3. returning variant
    auto parse_int3( std::string_view sv ) -> std::variant<int, std::domain_error> {
        int i;
        std::stringstream ss;
        ss << sv;
        ss >> i;
        if( !ss.fail() )
            return i;
        else
            return std::domain_error( "'" + std::string( sv ) + "' is not an integer" );
    }
}

TEST_CASE( "String to int" ) {

    SECTION( "C++ 11 stoul" ) {
        REQUIRE( std::stoul( "7" ) == 7 ); // C++ 11

        REQUIRE_THROWS( std::stoul( "Bakes7" ) ); // C++ 11
    }

    SECTION( "C++ 17" ) {

        using namespace Cpp17;

        SECTION( "out param" ) {
            int i = 0;
            parse_int1( "7", i ); // warning

            REQUIRE( parse_int1( "7", i ) == true );
            REQUIRE( i == 7 );

            REQUIRE( parse_int1( "Blakes7", i ) == false );
            // value of i is not defined
        }

        SECTION( "optional" ) {
            REQUIRE( parse_int2( "7" ) == 7 );
            REQUIRE( parse_int2( "Blakes7" ).has_value() == false );
        }

        SECTION( "variant" ) {
            REQUIRE( std::get<int>( parse_int3( "7" ) ) == 7 );
            REQUIRE( std::get<std::domain_error>( parse_int3( "Blakes7" ) ).what() == std::string( "'Blakes7' is not an integer" ) );
        }

        SECTION( "variant - visit" ) {

            struct ExpectedIntVisitor {
                int expected;

                void operator()( int i ) {
                    REQUIRE( i == expected );
                }
                void operator()( std::domain_error const& err ) {
                    FAIL( "Expected " << expected << " but got error: " << err.what() );
                }
            };
            struct ExpectedErrorVisitor {

                std::string expectedMessage;

                void operator()( int i ) {
                    FAIL( "Expected error but got integer, " << i );
                }
                void operator()( std::domain_error const& err ) {
                    REQUIRE( err.what() == expectedMessage );
                }
            };

            std::visit( ExpectedIntVisitor{ 7 }, parse_int3( "7" ) );
            std::visit( ExpectedErrorVisitor{ "'Blakes7' is not an integer" }, parse_int3( "Blakes7" ) );
        }
    }
}
