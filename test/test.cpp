#include "fsmomc.h"
// this tells catch to provide a main()
// only do this in one cpp file
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

TEST_CASE("get version of the library", "[version]")
{
    #include "version.h"

    SECTION("hex version check") {
        REQUIRE(fsmomc_version() == (FSMOMC_VERSION_MAJOR << 16 | FSMOMC_VERSION_MINOR << 8 | FSMOMC_VERSION_PATCH));
    }

    SECTION("string version check") {
        using Catch::Matchers::EndsWith;
        REQUIRE_THAT(fsmomc_str_ver() , EndsWith(FSMOMC_VERSION_STR));
    }
}