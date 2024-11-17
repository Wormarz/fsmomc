#include "fsmomc.h"
// this tells catch to provide a main()
// only do this in one cpp file
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

static bool init_flag;
static struct working_state* worker1(struct working_state *self)
{
    return self;
}

static void init1(struct working_state* self)
{
    init_flag = true;
}

TEST_CASE("adding new states")
{
    ALLOC_SM_INS(buf, 10);
    REQUIRE(init_state_machine((struct state_machine *)buf, sizeof(buf), 10) == 0);
    struct state_machine *sm = (struct state_machine *)buf;

    SECTION("Test 1 - Add new state") {
        REQUIRE(add_state(sm, "state1", worker1, NULL) == 0);
    }

    SECTION("Test 2 - Init function is asigned") {
        init_flag = false;
        REQUIRE(add_state(sm, "state1", worker1, init1) == 0);
        REQUIRE(init_flag == true);
    }

    SECTION("Test 3 - State machine full") {
        for (int i = 0; i < sm->stat_nums; ++i) {
            REQUIRE(add_state(sm, "state1", worker1, NULL) == 0);
        }
        REQUIRE(add_state(sm, "state1", worker1, NULL) != 0);
    }

    SECTION("Test 4 - Empty state name") {
        REQUIRE(add_state(sm, NULL, worker1, NULL) != 0);
    }

    SECTION("Test 5 - State machine is NULL") {
        REQUIRE(add_state(NULL, "state1", worker1, NULL) != 0);
    }

}

TEST_CASE("adding sub-states") {
    ALLOC_SM_INS(buf, 10);
    REQUIRE(init_state_machine((struct state_machine *)buf, sizeof(buf), 10) == 0);
    struct state_machine *sm = (struct state_machine *)buf;

    SECTION("Test 1 - Normal case") {
        REQUIRE(add_state(sm, "abs-st", NULL, NULL) == 0);
        REQUIRE(add_substate(sm, "abs-st", "state1", worker1, NULL) == 0);
    }

    SECTION("Test 2 - Init function is asigned") {
        init_flag = false;
        REQUIRE(add_state(sm, "abs-st", NULL, NULL) == 0);
        REQUIRE(add_substate(sm, "abs-st", "state1", worker1, init1) == 0);
        REQUIRE(init_flag == true);
    }

    SECTION("Test 2 - State does not exist") {
        REQUIRE(add_substate(sm, "abs-st", "state1", worker1, NULL) != 0);
    }

    SECTION("Test 3 - Empty state name") {
        REQUIRE(add_state(sm, "abs-st", NULL, NULL) == 0);
        REQUIRE(add_substate(sm, NULL, "state1", worker1, NULL) != 0);
    }

    SECTION("Test 4 - Empty sub-state name") {
        REQUIRE(add_state(sm, "abs-st", NULL, NULL) == 0);
        REQUIRE(add_substate(sm, "abs-st", NULL, worker1, NULL) != 0);
    }

    SECTION("Test 5 - State machine full") {
        for (int i = 0; i < sm->stat_nums; ++i) {
            REQUIRE(add_state(sm, "state1", worker1, NULL) == 0);
        }
        REQUIRE(add_substate(sm, "abs-st", "state1", worker1, NULL) != 0);
    }

    SECTION("Test 6 - State machine is NULL") {
        REQUIRE(add_state(sm, "abs-st", NULL, NULL) == 0);
        REQUIRE(add_substate(NULL, "abs-st", "state1", worker1, NULL) != 0);
    }
}

