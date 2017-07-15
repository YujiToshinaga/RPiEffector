#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define ALSA_PCM_NEW_HW_PARAMS_API
#include <alsa/asoundlib.h>

#include "stddef.h"
#include "effector.h"
#include "effdata.h"
#include "effmng.h"
#include "sounddev.h"
      
static pthread_t effmng_thread = (pthread_t)NULL;
static int effmng_fin = FALSE;
static EFFCMD effmng_cmd;

static void *thread();

// エフェクターの初期化
void effmng_init(void) {
    int i;

    effmng_thread = (pthread_t)NULL;
    effmng_fin = FALSE;
    effmng_cmd.effstat = STOP;
    effmng_cmd.efftype = THROUGH;
    for (i = 0; i < PARAM_NUM; i++) {
        effmng_cmd.param[i] = 0;
    }
}

// エフェクターの終了
void effmng_exit(void) {
    int i;

    effmng_thread = (pthread_t)NULL;
    effmng_fin = FALSE;
    effmng_cmd.effstat = STOP;
    effmng_cmd.efftype = THROUGH;
    for (i = 0; i < PARAM_NUM; i++) {
        effmng_cmd.param[i] = 0;
    }
}

// エフェクターの再生
void effmng_play(EFFCMD effcmd) {
    EFFFUNC *efffunc;

    effmng_cmd = effcmd;
    effmng_fin = FALSE;

    pthread_create(&effmng_thread, NULL, thread, NULL);

    efffunc = effdata_get_func();
    printf("play : %s\n", efffunc[effcmd.efftype].effname);
}

// エフェクターの停止
void effmng_stop() {
    int i;

    if (effmng_thread == (pthread_t)NULL) {
        return;
    }

    effmng_fin = TRUE;
    pthread_join(effmng_thread, NULL);

    effmng_thread = (pthread_t)NULL;
    effmng_fin = FALSE;
    effmng_cmd.effstat = STOP;
    effmng_cmd.efftype = THROUGH;
    for (i = 0; i < PARAM_NUM; i++) {
        effmng_cmd.param[i] = 0;
    }

    printf("stop\n");
}

// エフェクターのヘルプ
void effmng_help() {
    EFFWORD *effword;
    int effword_num;
    int i;

    effword = effdata_get_word();
    effword_num = effdata_get_word_num();
    for (i = 0; i < effword_num; i++) {
        printf("%s\n", effword[i].help);
    }
}

// エフェクタースレッド
void *thread()
{
    int *inbuf[2], *outbuf[2];
    EFFFUNC *efffunc;
    void (*efffunc_init)(int *);
    void (*efffunc_exit)(void);
    void (*efffunc_main)(int **, int **);

    efffunc = effdata_get_func();

    // エフェクター関数を登録する
    if (effmng_cmd.efftype < EFFTYPE_NUM) {
        efffunc_init = efffunc[effmng_cmd.efftype].efffunc_init;
        efffunc_exit = efffunc[effmng_cmd.efftype].efffunc_exit;
        efffunc_main = efffunc[effmng_cmd.efftype].efffunc_main;
    } else {
        efffunc_init = efffunc[0].efffunc_init;
        efffunc_exit = efffunc[0].efffunc_exit;
        efffunc_main = efffunc[0].efffunc_main;
    }

    // メモリを確保する
    inbuf[0] = (int *)malloc(FRAME_NUM * sizeof(int));
    inbuf[1] = (int *)malloc(FRAME_NUM * sizeof(int));
    outbuf[0] = (int *)malloc(FRAME_NUM * sizeof(int));
    outbuf[1] = (int *)malloc(FRAME_NUM * sizeof(int));

    // サウンドデバイスを開く
    sounddev_init();

    // エフェクターを初期化する
    efffunc_init(effmng_cmd.param);

    // 信号の入力、処理、出力をし続ける
    while (effmng_fin == FALSE) {
        // サウンドデバイスからオーディオデータを読み出す
        sounddev_read(inbuf, FRAME_NUM);

        // エフェクターで信号処理をする
        efffunc_main(inbuf, outbuf);

        // サウンドデバイスへオーディオデータを書き出す
        sounddev_write(outbuf, FRAME_NUM);
    }

    // エフェクターを終了する
    efffunc_exit();

    // サウンドデバイスを閉じる
    sounddev_exit();

    // メモリを解放する
    free(inbuf[0]);
    free(inbuf[1]);
    free(outbuf[0]);
    free(outbuf[1]);
}
