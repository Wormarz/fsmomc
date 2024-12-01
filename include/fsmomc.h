#ifndef __FSMOMC_H__
#define __FSMOMC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "autoconf.h"
#include <assert.h>
#include <stdint.h>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define STR1(R) #R
#define STR2(R) STR1(R)

#if (CONFIG_MAX_SM_EDGES_NUMS % 4 != 0) || (CONFIG_MAX_SM_NAME_LEN % 4 != 0)
#error "MAX_SM_EDGES_NUMS and MAX_SM_NAME_LEN must be multiple of 4"
#endif

struct working_state;
typedef struct working_state *(*actions)(struct working_state *self, void *in,
                                         void *out);

struct working_state
{
    char state[CONFIG_MAX_SM_NAME_LEN];
    actions act;
    void (*enter)(struct working_state *, void *, void *);
    void (*exit)(struct working_state *, void *, void *);
    union
    {
        struct working_state *sub;
        struct state_machine *sm;
    };
    uint8_t edges[CONFIG_MAX_SM_EDGES_NUMS];
} __attribute__((aligned(4)));

struct state_machine
{
    uint32_t out_ready : 1;
    uint32_t : 1;
    uint32_t stat_nums : 30;
    struct working_state *prv_stat;
    struct working_state *cur_stat;
    struct working_state states[];
};

#define ALLOC_SM_INS(_b, _n)                                                   \
    uint8_t _b[(sizeof(struct state_machine) +                                 \
                (_n) * sizeof(struct working_state))]

#define CHG_STATE(_ns_)                                                        \
    do {                                                                       \
        self = trans_stat(self->sm, self, (_ns_));                             \
        assert(self != NULL);                                                  \
    } while (0)

#define DECLARE_STAT_ACTIONS(_act_)                                            \
    struct working_state *_act_(struct working_state *self, void *in, void *out)

#define DECLARE_STAT_ENTER(_enter_)                                            \
    void _enter_(struct working_state *self, void *in, void *out)

#define DECLARE_STAT_EXIT(_exit_) DECLARE_STAT_ENTER(_exit_)

#define STAT_ACT_START
#define STAT_ACT_END                                                           \
    do {                                                                       \
        return self;                                                           \
    } while (0)

#define STAT_OUT_READY()                                                       \
    do {                                                                       \
        self->sm->out_ready = 1;                                               \
    } while (0)

int init_state_machine(struct state_machine *sm, uint32_t buf_zs,
                       uint32_t st_nums);
int add_state(struct state_machine *sm, const char *name, actions act,
              void (*init)(struct working_state *));
int del_state(const char *name);
int add_substate(struct state_machine *sm, const char *parent, const char *sub,
                 actions act, void (*init)(struct working_state *));
int del_substate(const char *parent, const char *sub);
int add_trans_rule(struct state_machine *sm, const char *from, const char *to);
struct working_state *trans_stat(struct state_machine *sm,
                                 struct working_state *from, const char *to);
void setup_first_state(struct state_machine *sm, const char *name);
uint32_t fsmomc_version(void);
const char *fsmomc_str_ver(void);

void state_machine_loop(struct state_machine *sm);
int state_machine_out(struct state_machine *sm, void *in, void *out);

#ifdef __cplusplus
}
#endif

#endif /* __FSMOMC_H__ */
