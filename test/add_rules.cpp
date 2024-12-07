#include "catch.hpp"
#include "fsmomc.h"

static DECLARE_STAT_ACTIONS(worker1)
{
    STAT_ACT_START;

    STAT_ACT_END;
}

static DECLARE_STAT_ACTIONS(worker2)
{
    STAT_ACT_START;

    STAT_ACT_END;
}

TEST_CASE("adding rules")
{
    ALLOC_SM_INS(buf, 10);
    REQUIRE(init_state_machine((struct state_machine *)buf, sizeof(buf), 10) ==
            0);
    struct state_machine *sm = (struct state_machine *)buf;
    REQUIRE(add_state(sm, "state1", worker1, NULL) == 0);
    REQUIRE(add_state(sm, "state2", worker2, NULL) == 0);

    SECTION("Test 1 - Add a valid transition rule")
    {
        REQUIRE(add_trans_rule(sm, "state1", "state2") == 0);
    }

    SECTION("Test 2 - Add a transition rule to a non-existent state")
    {
        REQUIRE(add_trans_rule(sm, "state1", "state3") != 0);
    }

    SECTION("Test 3 - Add a transition rule from a non-existent state")
    {
        REQUIRE(add_trans_rule(sm, "state3", "state2") != 0);
    }

    SECTION("Test 4 - Add a transition rule to and from a non-existent state")
    {
        REQUIRE(add_trans_rule(sm, "state3", "state4") != 0);
    }

    SECTION("Test 5 - Add a transition rule to the same state")
    {
        REQUIRE(add_trans_rule(sm, "state1", "state1") == 0);
    }

    SECTION("Test 6 - Add a transition rule with NULL state machine")
    {
        REQUIRE(add_trans_rule(NULL, "state1", "state2") != 0);
    }

    SECTION("Test 7 - Add a transition rule with NULL source state name")
    {
        REQUIRE(add_trans_rule(sm, NULL, "state2") != 0);
    }

    SECTION("Test 8 - Add a transition rule with NULL destination state name")
    {
        REQUIRE(add_trans_rule(sm, "state1", NULL) != 0);
    }

    SECTION("Test 9 - Add a transition rule when the edge array is full")
    {
        for (int i = 0; i < CONFIG_MAX_SM_EDGES_NUMS; i++) {
            REQUIRE(add_trans_rule(sm, "state1", "state1") == 0);
        }
        REQUIRE(add_trans_rule(sm, "state1", "state2") != 0);
    }
}

TEST_CASE("adding rules with pseudo state")
{
    ALLOC_SM_INS(buf, 10);
    REQUIRE(init_state_machine((struct state_machine *)buf, sizeof(buf), 10) ==
            0);
    struct state_machine *sm = (struct state_machine *)buf;
    REQUIRE(add_state(sm, "state1", worker1, NULL) == 0);
    REQUIRE(add_state(sm, "abs-st", NULL, NULL) == 0);

    SECTION("Test 1 - Add a valid transition rule with a pseudo state")
    {
        REQUIRE(add_substate(sm, "abs-st", "state2", worker2, NULL) == 0);
        REQUIRE(add_trans_rule(sm, "state1", "abs-st") == 0);
        REQUIRE(add_trans_rule(sm, "state2", "state1") == 0);
    }

    SECTION("Test 2 - pseudo state can not be a source state")
    {
        REQUIRE(add_substate(sm, "abs-st", "state2", worker2, NULL) == 0);
        REQUIRE(add_trans_rule(sm, "abs-st", "state1") != 0);
    }

    SECTION("Test 3 - pseudo state has no association with concrete sub-state")
    {
        REQUIRE(add_trans_rule(sm, "state1", "abs-st") != 0);
    }
}