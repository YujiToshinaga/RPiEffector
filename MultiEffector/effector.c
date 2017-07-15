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

// エフェクタースレッド
void *effector()
{
    snd_pcm_t *inhandle, *outhandle;
    int *inbuf[2], *outbuf[2];
    void (*sigps_init)(void);
    void (*sigps_main)(int **, int **);
    void (*sigps_exit)(void);

    // メモリを確保する
    inbuf[0] = (int *)malloc(FRAME_NUM * sizeof(int));
    inbuf[1] = (int *)malloc(FRAME_NUM * sizeof(int));
    outbuf[0] = (int *)malloc(FRAME_NUM * sizeof(int));
    outbuf[1] = (int *)malloc(FRAME_NUM * sizeof(int));

    // 信号処理を登録する
    switch (effector_cmd.efftype) {
    case THROUGH:
        sigps_init = through_init;
        sigps_main = through_main;
        sigps_exit = through_exit;
        break;
    case SMA:
        sigps_init = sma_init;
        sigps_main = sma_main;
        sigps_exit = sma_exit;
        break;
    default:
        sigps_init = through_init;
        sigps_main = through_main;
        sigps_exit = through_exit;
        break;
    }

    // サウンドデバイスを開く
    sounddev_init();

    // 信号処理を初期化する
    sigps_init();

    // 信号の入力、処理、出力をし続ける
    while (effector_fin == FALSE) {
        // サウンドデバイスからオーディオデータを読み出す
        sounddev_read(inbuf, FRAME_NUM);

        // 信号処理を行う
        sigps_main(inbuf, outbuf);

        // サウンドデバイスへオーディオデータを書き出す
        sounddev_write(outbuf, FRAME_NUM);
    }

    // 信号処理を終了する
    sigps_exit();

    // サウンドデバイスを閉じる
    sounddev_exit();

    // メモリを解放する
    free(inbuf[0]);
    free(inbuf[1]);
    free(outbuf[0]);
    free(outbuf[1]);
}

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

void effector_play(EFFCMD effcmd) {
    effector_cmd = effcmd;
    effector_fin = FALSE;

    pthread_create(&effector_thread, NULL, effector, NULL);

    printf("play\n");
}

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

void effector_help() {
    printf("0      : stop\n");
    printf("1      : play through\n");
    printf("2      : play sma\n");
    printf("stop   : stop\n");
    printf("exit   : exit\n");
    printf("help   : hlep\n");
    printf("(else) : do nothing\n");
}
