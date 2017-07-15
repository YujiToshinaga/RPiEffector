#ifndef __EFFDATA_H__
#define __EFFDATA_H__

#define SAMPLING_RATE 96000
#define CH_NUM 2
#define FRAME_NUM 128
#define PARAM_NUM 10

typedef enum effector_status {
    STOP,
    PLAY,
    HELP,
    DONOTHING,
    EXIT,
} EFFSTAT;

//#define EFFTYPE_NUM 3
typedef enum effector_type {
    THROUGH,
    SMA,
    REVERB,
    EFFTYPE_NUM
} EFFTYPE;

typedef struct effector_command {
    EFFSTAT effstat;
    EFFTYPE efftype;
    int     param[PARAM_NUM];
} EFFCMD;

typedef struct effector_function {
    char *effname;
    void (*efffunc_init)(int *);
    void (*efffunc_exit)(void);
    void (*efffunc_main)(int **, int **);
} EFFFUNC;

typedef struct effector_word {
    char*   effcmd;
    EFFSTAT effstat;
    EFFTYPE efftype;
    char*   help;
} EFFWORD;

extern EFFFUNC *effdata_get_func(void);
extern EFFWORD *effdata_get_word(void);
extern int effdata_get_word_num(void);

#endif /* __EFFDATA_H__ */
