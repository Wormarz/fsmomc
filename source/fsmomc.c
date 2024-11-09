#include "fsmomc.h"
#include "version.h"
#include <string.h>

/* state machine start */
const char version[] = "Ver: "FSMOMC_VERSION_STR; /* version */

static struct working_state states[MAX_SM_NUMS];
static struct working_state *cur_stat = &states[0];

static struct working_state* stat_lookup(const char *name)
{
    for (int i = 0; i < MAX_SM_NUMS; ++i) {
        if (strncmp(states[i].state, name, MAX_SM_NAME_LEN) == 0) {
            return &states[i];
        }
    }

    return NULL;
}

int init_state_machine(struct state_machine *sm, uint32_t buf_zs, uint32_t st_nums)
{
    if (sm == NULL || buf_zs != st_nums * sizeof(struct working_state) + 4) {
        return -1;
    }

    memset(sm, 0, sizeof(struct working_state) * st_nums + 4);
    sm->stat_nums = st_nums;
    return 0;
}

int add_state(struct state_machine *sm, const char *name, worker wkr, void (*init)(struct working_state*))
{
    if (sm == NULL || name == NULL) {
        return -1;
    }

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
    int idx = -1;

    if (sm == NULL || sub == NULL || wkr == NULL || parent == NULL) {
        return -1;
    }

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

int add_trans_rule(const char *from, const char *to)
{
    struct working_state *fstat, *tostat;
    int ret = -1;
    fstat = stat_lookup(from);
    tostat = stat_lookup(to);

    assert(fstat && tostat);

    for (int i = 0; i < MAX_SM_EDGES_NUMS; ++i) {
        if (fstat->edges[i] == 0xff) {
            fstat->edges[i] = (uint8_t) (tostat - states);
            ret = 0;
            break;
        }
    }

    return ret;
}

void state_machine_loop(void)
{
    struct working_state *prv_stat = NULL;

    while (1) {
        if (prv_stat != cur_stat) {
            cur_stat->enter ? cur_stat->enter(cur_stat) : NULL;
            prv_stat = cur_stat;
        }
        cur_stat = cur_stat->runner(cur_stat);
        if (prv_stat != cur_stat) {
            prv_stat->exit ? prv_stat->exit(prv_stat) : NULL;
        }
    }
}

void setup_first_state(const char *name)
{
    CHG_STATE(cur_stat, name);
}

struct working_state* trans_stat(struct working_state *from, const char *to)
{
    for (int i = 0; i < MAX_SM_EDGES_NUMS && from->edges[i] != 0xff; ++i) {
        if (strncmp(states[from->edges[i]].state, to, MAX_SM_NAME_LEN) == 0) {
            return states[from->edges[i]].runner ?
                    &states[from->edges[i]] : states[from->edges[i]].sub;
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
