#ifndef __EFFECTOR_H__
#define __EFFECTOR_H__

#define FRAME_NUM 128
#define PARAM_NUM 10

typedef enum error_code {
    OK,
    ERROR,
} ER;

typedef enum effector_status {
    STOP,
    PLAY,
    HELP,
    DONOTHING,
    EXIT,
} EFFSTAT;

typedef enum effector_type {
    THROUGH,
    SMA,
} EFFTYPE;

typedef struct effector_command {
    EFFSTAT     effstat;
    EFFTYPE     efftype;
    int         param[PARAM_NUM];
} EFFCMD;


extern void effector_init(void);
extern void effector_exit(void);
extern void effector_play(EFFCMD effcmd);
extern void effector_stop(void);
extern void effector_help(void);

#endif /* __EFFECTOR_H__ */
