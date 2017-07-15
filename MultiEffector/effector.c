#include <stdio.h>

#include "stddef.h"
#include "effector.h"
#include "effmng.h"
#include "shell.h"

void effector(void)
{
    int finish = FALSE;
    EFFCMD effcmd;

    effmng_init();

    // メインの処理を行う
    finish = FALSE;
    while (finish == FALSE) {
        shell(&effcmd);

        switch (effcmd.effstat) {
        case PLAY:
            effmng_stop();
            effmng_play(effcmd);
            break;
        case STOP:
            effmng_stop();
            break;
        case HELP:
            effmng_help();
            break;
        case DONOTHING:
            break;
        case EXIT:
            effmng_stop();
            finish = TRUE;
            break;
        default:
            break;
        }
    }

    effmng_exit();

    printf("exit\n");
}

int main(int argc, char *argv[])
{
    effector();

    return 0;
}
