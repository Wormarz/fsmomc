#include "fsmomc.h"
// this tells catch to provide a main()
// only do this in one cpp file
#define CATCH_CONFIG_MAIN
#include "catch.hpp"


TEST_CASE("initlize state machine instance")
{
    ALLOC_SM_INS(buf, 10);
    SECTION("Test Case 1 - Normal Initialization") {
        REQUIRE(init_state_machine((struct state_machine *)buf, sizeof(buf), 10) == 0);
        REQUIRE(((struct state_machine *)buf)->stat_nums == 10);
    }

    SECTION("Test Case 2 - NULL State Machine") {
        REQUIRE(init_state_machine(NULL, sizeof(buf), 10) != 0);
    }

    SECTION("Test Case 3 - Incorrect buf_zs") {
        int buf_zs = 10 * sizeof(struct working_state) + 3;
        REQUIRE(init_state_machine((struct state_machine *)buf, buf_zs, 10) != 0);
    }

    SECTION("Test Case 4 - Zero State Numbers") {
        REQUIRE(init_state_machine((struct state_machine *)buf, sizeof(buf), 0) != 0);
    }

    SECTION("Test Case 5 - Zero State Numbers and buf_zs") {
        REQUIRE(init_state_machine((struct state_machine *)buf, 0, 0) != 0);
    }
}
