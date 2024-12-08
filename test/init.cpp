#include "catch.hpp"
#include "fsmomc.h"

TEST_CASE("initlize state machine instance")
{
    ALLOC_SM_INS(buf, 10);
    SECTION("Test Case 1 - Normal Initialization")
    {
        REQUIRE(init_state_machine((struct state_machine *)buf, sizeof(buf),
                                   10) == 0);
        REQUIRE(((struct state_machine *)buf)->stat_nums == 10);
    }

    SECTION("Test Case 2 - NULL State Machine")
    {
        REQUIRE(init_state_machine(NULL, sizeof(buf), 10) != 0);
    }

    SECTION("Test Case 3 - Incorrect buf_zs")
    {
        int buf_zs = 10 * sizeof(struct working_state) + 3;
        REQUIRE(init_state_machine((struct state_machine *)buf, buf_zs, 10) !=
                0);
    }

    SECTION("Test Case 4 - Zero State Numbers")
    {
        REQUIRE(init_state_machine((struct state_machine *)buf, sizeof(buf),
                                   0) != 0);
    }

    SECTION("Test Case 5 - Zero State Numbers and buf_zs")
    {
        REQUIRE(init_state_machine((struct state_machine *)buf, 0, 0) != 0);
    }
}

TEST_CASE("deinitlize state machine instance")
{
    ALLOC_SM_INS(buf, 10);
    struct state_machine *sm = (struct state_machine *)buf;
    REQUIRE(init_state_machine(sm, sizeof(buf), 10) == 0);
    REQUIRE(sm->stat_nums == 10);

    SECTION("Test Case 1 - Normal Deinitialization")
    {
        bool is_all_zero = true;

        for (int i = 0; i < sm->stat_nums; ++i) {
            std::string str = "state";
            str += std::to_string(i);
            REQUIRE(add_state(sm, str.c_str(), NULL, NULL) == 0);
        }

        REQUIRE(deinit_state_machine(sm) == 0);

        for (int i = 0; i < sizeof(buf); i++) {
            if (buf[i] != 0) {
                is_all_zero = false;
                break;
            }
        }

        REQUIRE(is_all_zero);
    }

    SECTION("Test Case 2 - NULL State Machine")
    {
        REQUIRE(deinit_state_machine(NULL) != 0);
    }
}