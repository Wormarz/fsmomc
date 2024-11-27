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
typedef struct working_state *(*actions)(struct working_state *self);

struct working_state
{
    char state[CONFIG_MAX_SM_NAME_LEN];
    actions act;
    void (*enter)(struct working_state *);
    void (*exit)(struct working_state *);
    union
    {
        void *args;
        struct working_state *sub;
    };
    uint8_t edges[CONFIG_MAX_SM_EDGES_NUMS];
} __attribute__((aligned(4)));

struct state_machine
{
    uint32_t stat_nums;
    struct working_state *prv_stat;
    struct working_state *cur_stat;
    struct working_state states[];
};

#define ALLOC_SM_INS(_b, _n)                                                   \
    uint8_t _b[(sizeof(struct state_machine) +                                 \
                (_n) * sizeof(struct working_state))]

#define CHG_STATE(_c, _ns)                                                     \
    do {                                                                       \
        (_c) = trans_stat(sm, (_c), (_ns));                                    \
        assert((_c) != NULL);                                                  \
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

#ifdef __cplusplus
}
#endif

#endif /* __FSMOMC_H__ */
