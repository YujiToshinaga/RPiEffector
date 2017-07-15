#include <stdio.h>

#include "stddef.h"
#include "effector.h"
#include "shell.h"

int main(int argc, char *argv[])
{
    int finish = FALSE;
    EFFCMD effcmd;

    effector_init();

    // キー入力を監視する
    finish = FALSE;
    while (finish == FALSE) {
        shell(&effcmd);
        switch (effcmd.effstat) {
        case PLAY:
            effector_stop();
            effector_play(effcmd);
            break;
        case STOP:
            effector_stop();
            break;
        case HELP:
            effector_help();
            break;
        case DONOTHING:
            break;
        case EXIT:
            effector_stop();
            finish = TRUE;
            break;
        default:
            break;
        }
    }

    effector_exit();

    printf("exit\n");

    return 0;
}

