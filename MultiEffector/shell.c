#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "effector.h"
#include "shell.h"
      
#define INPUT_LEN 128
#define COMMAND_NUM 8

void shell(EFFCMD *effcmd)
{
    char input[INPUT_LEN];
    int cmdc;
    char *cmdv[COMMAND_NUM];
    const char *delim = " \t\n";
    char *p;
    int i;

    printf("command: ");

    if (fgets(input, sizeof(input), stdin) == NULL) {
        effcmd->effstat = DONOTHING;
        effcmd->efftype = THROUGH;
        for (i = 0; i < PARAM_NUM; i++) {
            effcmd->param[i] = 0;
        }
        return;
    }

    p = input;
    for (cmdc = 0; cmdc < COMMAND_NUM; cmdc++) {
        if ((cmdv[cmdc] = strtok(p, delim)) == NULL) {
            break;
        }
        p = NULL;
    }

    if (cmdc >= 1) {
        if (strcmp(cmdv[0], "0") == 0) {
            effcmd->effstat = STOP;
            effcmd->efftype = THROUGH;
            for (i = 0; i < PARAM_NUM; i++) {
                effcmd->param[i] = 0;
            }
        } else if (strcmp(cmdv[0], "1") == 0) {
            effcmd->effstat = PLAY;
            effcmd->efftype = THROUGH;
            for (i = 0; i < PARAM_NUM; i++) {
                effcmd->param[i] = 0;
            }
        } else if (strcmp(cmdv[0], "2") == 0) {
            effcmd->effstat = PLAY;
            effcmd->efftype = SMA;
            for (i = 0; i < PARAM_NUM; i++) {
                effcmd->param[i] = 0;
            }
        } else if (strcmp(cmdv[0], "q") == 0) {
            effcmd->effstat = EXIT;
            effcmd->efftype = THROUGH;
            for (i = 0; i < PARAM_NUM; i++) {
                effcmd->param[i] = 0;
            }
        } else if (strcmp(cmdv[0], "stop") == 0) {
            effcmd->effstat = STOP;
            effcmd->efftype = THROUGH;
            for (i = 0; i < PARAM_NUM; i++) {
                effcmd->param[i] = 0;
            }
        } else if (strcmp(cmdv[0], "exit") == 0) {
            effcmd->effstat = EXIT;
            effcmd->efftype = THROUGH;
            for (i = 0; i < PARAM_NUM; i++) {
                effcmd->param[i] = 0;
            }
        } else if (strcmp(cmdv[0], "help") == 0) {
            effcmd->effstat = HELP;
            effcmd->efftype = THROUGH;
            for (i = 0; i < PARAM_NUM; i++) {
                effcmd->param[i] = 0;
            }
        } else {
            effcmd->effstat = DONOTHING;
            effcmd->efftype = THROUGH;
            for (i = 0; i < PARAM_NUM; i++) {
                effcmd->param[i] = 0;
            }
        }
    } else {
        effcmd->effstat = DONOTHING;
        effcmd->efftype = THROUGH;
        for (i = 0; i < PARAM_NUM; i++) {
            effcmd->param[i] = 0;
        }
    }
}
