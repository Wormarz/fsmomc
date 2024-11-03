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

static bool init_flag;
static struct working_state* worker1(struct working_state *self)
{
    return self;
}

static void init1(struct working_state* self)
{
    init_flag = true;
}
TEST_CASE("adding new states", "[new_state]")
{
    

    SECTION("add a concrete state successfully") {
        REQUIRE(add_state("state1", worker1, NULL) == 0);
    }

    SECTION("add a concrete state with a initialization function successfully") {
        init_flag = false;
        REQUIRE(add_state("state1", worker1, init1) == 0);
        REQUIRE(init_flag == true);
    }

    SECTION("add a abstract state and its sub-state successfully") {
        init_flag = false;
        REQUIRE(add_state("abs-st", NULL, NULL) == 0);
        REQUIRE(add_substate("abs-st", "state1", worker1, init1) == 0);
        REQUIRE(init_flag == true);
    }
}
