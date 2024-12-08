#include "catch.hpp"
#include "fsmomc.h"

static DECLARE_STAT_ACTIONS(worker1)
{
    STAT_ACT_START;
    CHG_STATE("state2");
    STAT_ACT_END;
}

static DECLARE_STAT_ACTIONS(worker2)
{
    STAT_ACT_START;
    CHG_STATE("state1");
    STAT_ACT_END;
}

TEST_CASE("states transition")
{
    ALLOC_SM_INS(buf, 10);
    struct state_machine *sm = (struct state_machine *)buf;
    REQUIRE(init_state_machine(sm, sizeof(buf), 10) == 0);
    REQUIRE(add_state(sm, "state1", worker1, NULL) == 0);
    REQUIRE(add_state(sm, "state2", worker2, NULL) == 0);
    REQUIRE(add_trans_rule(sm, "state1", "state2") == 0);
    REQUIRE(add_trans_rule(sm, "state2", "state1") == 0);

    SECTION("Test Case 1 - Normal Transition")
    {
        using Catch::Matchers::Equals;
        state_machine_loop(sm);
        REQUIRE_THAT(sm->cur_stat->state, Equals("state2"));
        state_machine_loop(sm);
        REQUIRE_THAT(sm->cur_stat->state, Equals("state1"));
    }
}