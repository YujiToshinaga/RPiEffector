#include <stdio.h>

#include "stddef.h"
#include "effector.h"
#include "effdata.h"
#include "through.h"
#include "sma.h"
#include "reverb.h"

EFFFUNC efffunc[EFFTYPE_NUM] = {
    "through",  through_init,   through_exit,   through_main,
    "SMA",      sma_init,       sma_exit,       sma_main,
    "reverb",   reverb_init,    reverb_exit,    reverb_main,
};

EFFWORD effword[] = {
    "0",    STOP,   THROUGH, "0    : stop effector",
    "1",    PLAY,   THROUGH, "1    : effector : through",
    "2",    PLAY,   SMA,     "2    : effector : SMA",
    "3",    PLAY,   REVERB,  "3    : effector : REVERB",
    "q",    EXIT,   THROUGH, "q    : exit this program",
    "h",    HELP,   THROUGH, "h    : print help",
    "stop", STOP,   THROUGH, "stop : stop effector",
    "exit", EXIT,   THROUGH, "exit : exit this program",
    "help", HELP,   THROUGH, "help : print help",
};

EFFFUNC *effdata_get_func(void)
{
    return efffunc;
}

EFFWORD *effdata_get_word(void)
{
    return effword;
}

int effdata_get_word_num(void)
{
    return (sizeof(effword) / sizeof(EFFWORD));
}
