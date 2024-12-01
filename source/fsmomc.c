#include "fsmomc.h"
#include "version.h"
#include <string.h>

const char version[] = "Ver: " FSMOMC_VERSION_STR; /* version */

static struct working_state *stat_lookup(struct state_machine *sm,
                                         const char *name)
{
    if (sm == NULL || name == NULL) {
        return NULL;
    }

    for (int i = 0; i < sm->stat_nums; ++i) {
        if (strncmp(sm->states[i].state, name, CONFIG_MAX_SM_NAME_LEN) == 0) {
            return &(sm->states[i]);
        }
    }

    return NULL;
}

/**
 * @brief Initialize the state machine.
 *
 * @param sm Pointer to the state machine structure.
 * @param buf_zs Buffer size of state machine, passed through parameter sm.
 * @param st_nums Number of states.
 * @return Returns 0 on success.
 *
 * @note
 * This function initializes a state machine instance.
 */
int init_state_machine(struct state_machine *sm, uint32_t buf_zs,
                       uint32_t st_nums)
{
    if (sm == NULL || buf_zs != st_nums * sizeof(struct working_state) +
                                    sizeof(struct state_machine)) {
        return -1;
    }

    memset(sm, 0, buf_zs);
    sm->stat_nums = st_nums;
    sm->cur_stat = &(sm->states[0]);

    return 0;
}

int add_state(struct state_machine *sm, const char *name, actions act,
              void (*init)(struct working_state *))
{
    if (sm == NULL || name == NULL) {
        return -1;
    }

    for (int i = 0; i < sm->stat_nums; ++i) {
        if (sm->states[i].state[0] == '\0') {
            strncpy(sm->states[i].state, name, CONFIG_MAX_SM_NAME_LEN);
            if ((sm->states[i].act = act) != NULL) {
                sm->states[i].sm = sm;
            }
            memset(sm->states[i].edges, 0xff,
                   sizeof(uint8_t) * CONFIG_MAX_SM_EDGES_NUMS);
            if (init != NULL) {
                init(&(sm->states[i]));
            }
            return 0;
        }
    }

    return -1;
}

int del_state(const char *name)
{ /* TO-DO */
    return -1;
}

int add_substate(struct state_machine *sm, const char *parent, const char *sub,
                 actions act, void (*init)(struct working_state *))
{
    if (sm == NULL || sub == NULL || act == NULL || parent == NULL) {
        return -1;
    }

    int idx = -1;

    for (int i = 0; i < sm->stat_nums; ++i) {
        idx = strncmp(sm->states[i].state, parent, CONFIG_MAX_SM_NAME_LEN) ? idx
                                                                           : i;
        if (sm->states[i].state[0] == '\0' && idx != -1) {
            strncpy(sm->states[i].state, sub, CONFIG_MAX_SM_NAME_LEN);
            sm->states[i].act = act;
            /* bind sub-state actions to its parent state */
            sm->states[idx].sub = &(sm->states[i]);
            memset(sm->states[i].edges, 0xff,
                   sizeof(uint8_t) * CONFIG_MAX_SM_EDGES_NUMS);
            if (init != NULL) {
                init(&(sm->states[i]));
            }
            return 0;
        }
    }

    return -1;
}

int del_substate(const char *parent, const char *sub)
{ /* TO-DO */
    return -1;
}

int add_trans_rule(struct state_machine *sm, const char *from, const char *to)
{
    if (sm == NULL || from == NULL || to == NULL) {
        return -1;
    }

    struct working_state *fstat, *tostat;
    int ret = -1;
    fstat = stat_lookup(sm, from);
    tostat = stat_lookup(sm, to);

    if (fstat == NULL || tostat == NULL) {
        return -1;
    }

    for (int i = 0; i < CONFIG_MAX_SM_EDGES_NUMS; ++i) {
        if (fstat->edges[i] == 0xff) {
            fstat->edges[i] = (uint8_t)(tostat - sm->states);
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
        sm->cur_stat->enter ? sm->cur_stat->enter(sm->cur_stat, NULL, NULL)
                            : NULL;
        assert((sm->prv_stat = sm->cur_stat) != NULL);
    }
    assert((sm->cur_stat = sm->cur_stat->act(sm->cur_stat, NULL, NULL)) !=
           NULL);
    if (sm->prv_stat != sm->cur_stat) {
        sm->prv_stat->exit ? sm->prv_stat->exit(sm->prv_stat, NULL, NULL)
                           : NULL;
    }
}

int state_machine_out(struct state_machine *sm, void *in, void *out)
{
    assert(sm != NULL);

    if (out == NULL) {
        return -1;
    }

    while (sm->out_ready == 0) {
        if (sm->prv_stat != sm->cur_stat) {
            sm->cur_stat->enter ? sm->cur_stat->enter(sm->cur_stat, in, out)
                                : NULL;
            assert((sm->prv_stat = sm->cur_stat) != NULL);
        }
        assert((sm->cur_stat = sm->cur_stat->act(sm->cur_stat, in, out)) !=
               NULL);
        if (sm->prv_stat != sm->cur_stat) {
            sm->prv_stat->exit ? sm->prv_stat->exit(sm->prv_stat, in, out)
                               : NULL;
        }
    }
    sm->out_ready = 0;

    return 0;
}

void setup_first_state(struct state_machine *sm, const char *name)
{
    assert((sm->cur_stat = stat_lookup(sm, name)) != NULL);
}

struct working_state *trans_stat(struct state_machine *sm,
                                 struct working_state *from, const char *to)
{
    if (from == NULL || to == NULL || sm == NULL) {
        return NULL;
    }

    for (int i = 0; i < CONFIG_MAX_SM_EDGES_NUMS && from->edges[i] != 0xff;
         ++i) {
        if (strncmp(sm->states[from->edges[i]].state, to,
                    CONFIG_MAX_SM_NAME_LEN) == 0) {
            return sm->states[from->edges[i]].act
                       ? &(sm->states[from->edges[i]])
                       : sm->states[from->edges[i]].sub;
        }
    }

    return NULL;
}

uint32_t fsmomc_version(void)
{
    return FSMOMC_VERSION;
}

const char *fsmomc_str_ver(void)
{
    return version;
}
