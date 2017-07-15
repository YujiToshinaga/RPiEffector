#ifndef __EFFECTOR_H__
#define __EFFECTOR_H__

#include "through.h"
#include "sma.h"

#define FRAME_NUM 128
#define PARAM_NUM 10

#define EFFTYPE_NUM 2
typedef enum effector_type {
    THROUGH,
    SMA,
} EFFTYPE;

typedef enum effector_status {
    STOP,
    PLAY,
    HELP,
    DONOTHING,
    EXIT,
} EFFSTAT;

typedef struct effector_command {
    EFFSTAT     effstat;
    EFFTYPE     efftype;
    int         param[PARAM_NUM];
} EFFCMD;

typedef struct effector_function {
    void (*efffunc_init)(int *);
    void (*efffunc_exit)(void);
    void (*efffunc_main)(int **, int **);
} EFFFUNC;

typedef enum error_code {
    OK,
    ERROR,
} ER;

extern void effector_init(void);
extern void effector_exit(void);
extern void effector_play(EFFCMD effcmd);
extern void effector_stop(void);
extern void effector_help(void);

#endif /* __EFFECTOR_H__ */
