#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define ALSA_PCM_NEW_HW_PARAMS_API
#include <alsa/asoundlib.h>

#include "stddef.h"
#include "effector.h"
#include "sounddev.h"

#include "through.h"
#include "sma.h"
      
static pthread_t effector_thread = (pthread_t)NULL;
static int effector_fin = FALSE;
static EFFCMD effector_cmd;

// エフェクター関数
static EFFFUNC efffunc[EFFTYPE_NUM] = {
    through_init, through_exit, through_main,
    sma_init, sma_exit, sma_main,
};

static void *effector();

// エフェクターの初期化
void effector_init(void) {
    int i;

    effector_thread = (pthread_t)NULL;
    effector_fin = FALSE;
    effector_cmd.effstat = STOP;
    effector_cmd.efftype = THROUGH;
    for (i = 0; i < PARAM_NUM; i++) {
        effector_cmd.param[i] = 0;
    }
}

// エフェクターの終了
void effector_exit(void) {
    int i;

    effector_thread = (pthread_t)NULL;
    effector_fin = FALSE;
    effector_cmd.effstat = STOP;
    effector_cmd.efftype = THROUGH;
    for (i = 0; i < PARAM_NUM; i++) {
        effector_cmd.param[i] = 0;
    }
}

// エフェクターの実行
void effector_play(EFFCMD effcmd) {
    effector_cmd = effcmd;
    effector_fin = FALSE;

    pthread_create(&effector_thread, NULL, effector, NULL);

    printf("play\n");
}

// エフェクターの停止
void effector_stop() {
    int i;

    if (effector_thread == (pthread_t)NULL) {
        return;
    }

    effector_fin = TRUE;
    pthread_join(effector_thread, NULL);

    effector_thread = (pthread_t)NULL;
    effector_fin = FALSE;
    effector_cmd.effstat = STOP;
    effector_cmd.efftype = THROUGH;
    for (i = 0; i < PARAM_NUM; i++) {
        effector_cmd.param[i] = 0;
    }

    printf("stop\n");
}

// エフェクターのヘルプ
void effector_help() {
    printf("0      : stop\n");
    printf("1      : play through\n");
    printf("2      : play sma\n");
    printf("stop   : stop\n");
    printf("exit   : exit\n");
    printf("help   : hlep\n");
    printf("(else) : do nothing\n");
}

// エフェクタースレッド
void *effector()
{
    void (*efffunc_init)(int *);
    void (*efffunc_exit)(void);
    void (*efffunc_main)(int **, int **);
    int *inbuf[2], *outbuf[2];

    // エフェクター関数を登録する
    if (effector_cmd.efftype < EFFTYPE_NUM) {
        efffunc_init = efffunc[effector_cmd.efftype].efffunc_init;
        efffunc_exit = efffunc[effector_cmd.efftype].efffunc_exit;
        efffunc_main = efffunc[effector_cmd.efftype].efffunc_main;
    } else {
        efffunc_init = through_init;
        efffunc_exit = through_exit;
        efffunc_main = through_main;
    }

    // メモリを確保する
    inbuf[0] = (int *)malloc(FRAME_NUM * sizeof(int));
    inbuf[1] = (int *)malloc(FRAME_NUM * sizeof(int));
    outbuf[0] = (int *)malloc(FRAME_NUM * sizeof(int));
    outbuf[1] = (int *)malloc(FRAME_NUM * sizeof(int));

    // サウンドデバイスを開く
    sounddev_init();

    // エフェクターを初期化する
    efffunc_init(effector_cmd.param);

    // 信号の入力、処理、出力をし続ける
    while (effector_fin == FALSE) {
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
