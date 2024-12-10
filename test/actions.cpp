#include "catch.hpp"
#include "fsmomc.h"

static DECLARE_STAT_ENTER(stat1_enter)
{
    bool *data = (bool *)out;
    (*data) = true;
    STAT_OUT_READY();
}

static DECLARE_STAT_ACTIONS(worker1)
{
    STAT_ACT_START;
    CHG_STATE("state2");
    STAT_ACT_END;
}

void worker1_init(working_state *state)
{
    state->enter = stat1_enter;
}

static DECLARE_STAT_EXIT(stat2_exit)
{
    bool *data = (bool *)out;
    (*data) = true;
    STAT_OUT_READY();
}

static DECLARE_STAT_ACTIONS(worker2)
{
    STAT_ACT_START;
    CHG_STATE("state1");
    STAT_ACT_END;
}

void worker2_init(working_state *state)
{
    state->exit = stat2_exit;
}

TEST_CASE("state action")
{
    ALLOC_SM_INS(buf, 10);
    struct state_machine *sm = (struct state_machine *)buf;
    REQUIRE(init_state_machine(sm, sizeof(buf), 10) == 0);

    SECTION("Test Case 1 - enter action")
    {
        REQUIRE(add_state(sm, "state1", worker1, worker1_init) == 0);
        REQUIRE(add_state(sm, "state2", worker2, NULL) == 0);
        REQUIRE(add_trans_rule(sm, "state1", "state2") == 0);
        REQUIRE(add_trans_rule(sm, "state2", "state1") == 0);

        bool is_entered = false;
        state_machine_out(sm, NULL, &is_entered);
        REQUIRE(is_entered == true);
    }

    SECTION("Test Case 2 - exit action")
    {
        REQUIRE(add_state(sm, "state1", worker1, NULL) == 0);
        REQUIRE(add_state(sm, "state2", worker2, worker2_init) == 0);
        REQUIRE(add_trans_rule(sm, "state1", "state2") == 0);
        REQUIRE(add_trans_rule(sm, "state2", "state1") == 0);

        bool is_exit = false;
        state_machine_out(sm, NULL, &is_exit);
        REQUIRE(is_exit == true);
    }
}