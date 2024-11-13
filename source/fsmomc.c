#include "fsmomc.h"
#include "version.h"
#include <string.h>

/* state machine start */
const char version[] = "Ver: "FSMOMC_VERSION_STR; /* version */

// static struct working_state states[MAX_SM_NUMS];
// static struct working_state *cur_stat = &states[0];

static struct working_state* stat_lookup(struct state_machine *sm, const char *name)
{
    assert(sm != NULL && name != NULL);

    for (int i = 0; i < sm->stat_nums; ++i) {
        if (strncmp(sm->states[i].state, name, MAX_SM_NAME_LEN) == 0) {
            return &sm->states[i];
        }
    }

    return NULL;
}

int init_state_machine(struct state_machine *sm, uint32_t buf_zs, uint32_t st_nums)
{
    assert(sm != NULL && buf_zs == st_nums * sizeof(struct working_state) + sizeof(struct state_machine));

    memset(sm, 0, sizeof(struct working_state) * st_nums + sizeof(struct state_machine));
    sm->stat_nums = st_nums;
    sm->cur_stat = &sm->states[0];

    return 0;
}

int add_state(struct state_machine *sm, const char *name, worker wkr, void (*init)(struct working_state*))
{
    assert(sm != NULL && name != NULL);

    for (int i = 0; i < sm->stat_nums; ++i) {
        if (sm->states[i].state[0] == '\0') {
            strncpy(sm->states[i].state, name, MAX_SM_NAME_LEN);
            sm->states[i].runner = wkr;
            memset(sm->states[i].edges, 0xff, sizeof(uint8_t) * MAX_SM_EDGES_NUMS);
            if (init != NULL) {
                init(&(sm->states[i]));
            }
            return 0;
        }
    }

    return -1;
}

int del_state(const char *name)
{/* TO-DO */
    return -1;
}

int add_substate(struct state_machine *sm, const char *parent, const char *sub, worker wkr,
                 void (*init)(struct working_state*))
{
    assert(sm != NULL && sub != NULL && wkr != NULL && parent != NULL);
    int idx = -1;

    for (int i = 0; i < sm->stat_nums; ++i) {
        idx = strncmp(sm->states[i].state, parent, MAX_SM_NAME_LEN) ? idx : i;
        if (sm->states[i].state[0] == '\0' && idx != -1) {
            strncpy(sm->states[i].state, sub, MAX_SM_NAME_LEN);
            sm->states[i].runner = wkr;
            sm->states[idx].sub = &(sm->states[i]); /* bind sub-state worker to its parent state */
            memset(sm->states[i].edges, 0xff, sizeof(uint8_t) * MAX_SM_EDGES_NUMS);
            if (init != NULL) {
                init(&(sm->states[i]));
            }
            return 0;
        }
    }

    return -1;
}

int del_substate(const char *parent, const char *sub)
{/* TO-DO */
    return -1;
}

int add_trans_rule(struct state_machine *sm, const char *from, const char *to)
{
    assert(sm != NULL && from != NULL && to != NULL);

    struct working_state *fstat, *tostat;
    int ret = -1;
    fstat = stat_lookup(sm, from);
    tostat = stat_lookup(sm, to);

    assert(fstat != NULL && tostat != NULL);

    for (int i = 0; i < MAX_SM_EDGES_NUMS; ++i) {
        if (fstat->edges[i] == 0xff) {
            fstat->edges[i] = (uint8_t) (tostat - sm->states);
            ret = 0;
            break;
        }
    }

    return ret;
}

void state_machine_loop(struct state_machine *sm)
{
    assert(sm != NULL);

    if (sm->prv_stat != sm->cur_stat) {
        sm->cur_stat->enter ? sm->cur_stat->enter(sm->cur_stat) : NULL;
        sm->prv_stat = sm->cur_stat;
    }
    sm->cur_stat = sm->cur_stat->runner(sm->cur_stat);
    if (sm->prv_stat != sm->cur_stat) {
        sm->prv_stat->exit ? sm->prv_stat->exit(sm->prv_stat) : NULL;
    }
}

void setup_first_state(struct state_machine *sm, const char *name)
{
    assert((sm->cur_stat = stat_lookup(sm, name)) != NULL);
}

struct working_state* trans_stat(struct state_machine *sm, struct working_state *from, const char *to)
{
    assert(from != NULL && to != NULL && sm != NULL);

    for (int i = 0; i < MAX_SM_EDGES_NUMS && from->edges[i] != 0xff; ++i) {
        if (strncmp(sm->states[from->edges[i]].state, to, MAX_SM_NAME_LEN) == 0) {
            return sm->states[from->edges[i]].runner ?
                    &sm->states[from->edges[i]] : sm->states[from->edges[i]].sub;
        }
    }

    return NULL;
}

uint32_t fsmomc_version(void)
{
    return FSMOMC_VERSION;
}

const char * fsmomc_str_ver(void)
{
    return version;
}
/* state machine end */
