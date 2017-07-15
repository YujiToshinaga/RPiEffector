#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "effector.h"
#include "effdata.h"
#include "shell.h"
      
#define INPUT_LEN 128
#define COMMAND_NUM 8

void shell(EFFCMD *effcmd)
{
    char input[INPUT_LEN];
    int cmdc;
    char *cmdv[COMMAND_NUM];
    const char *delim = " \t\n";
    EFFWORD *effword;
    int effword_num;
    char *p;
    int i;

    printf("command: ");

    // 入力から文字列を取得する
    if (fgets(input, sizeof(input), stdin) != NULL) {
        // 文字列を分割する
        p = input;
        for (cmdc = 0; cmdc < COMMAND_NUM; cmdc++) {
            if ((cmdv[cmdc] = strtok(p, delim)) == NULL) {
                break;
            }
            p = NULL;
        }

        // コマンドを解析する
        if (cmdc >= 1) {
            effword = effdata_get_word();
            effword_num = effdata_get_word_num();
            for (i = 0; i < effword_num; i++) {
                if (strcmp(cmdv[0], effword[i].effcmd) == 0) {
                    effcmd->effstat = effword[i].effstat;
                    effcmd->efftype = effword[i].efftype;
                    for (i = 0; i < PARAM_NUM; i++) {
                        effcmd->param[i] = 0;
                    }
                    break;
                }
            }

        // コマンドを解析できないので何もしない
        } else {
            effcmd->effstat = DONOTHING;
            effcmd->efftype = THROUGH;
            for (i = 0; i < PARAM_NUM; i++) {
                effcmd->param[i] = 0;
            }
        }

    // 入力から文字列を取得できないので何もしない
    } else {
        effcmd->effstat = DONOTHING;
        effcmd->efftype = THROUGH;
        for (i = 0; i < PARAM_NUM; i++) {
            effcmd->param[i] = 0;
        }
    }
}
