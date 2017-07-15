#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define ALSA_PCM_NEW_HW_PARAMS_API
#include <alsa/asoundlib.h>

#include "stddef.h"
#include "effdata.h"
#include "sounddev.h"

static snd_pcm_t *inhandle;
static snd_pcm_t *outhandle;

// サウンドデバイスを開く
void sounddev_init(void)
{
    int ret_val;

    // 入力を初期化する
    ret_val = snd_pcm_open(&inhandle, "default", SND_PCM_STREAM_CAPTURE, 0);
    if (ret_val < 0) {
        fprintf(stderr, "capture open error: %s\n", snd_strerror(ret_val));
        exit(1);
    }
    ret_val = snd_pcm_set_params(inhandle, SND_PCM_FORMAT_S32_LE,
            SND_PCM_ACCESS_RW_NONINTERLEAVED, CH_NUM, SAMPLING_RATE, 1, 50000);
    if (ret_val < 0) {
        fprintf(stderr, "capture set error: %s\n", snd_strerror(ret_val));
        exit(1);
    }
           
    // 出力を初期化する
    ret_val = snd_pcm_open(&outhandle, "default", SND_PCM_STREAM_PLAYBACK, 0);
    if (ret_val < 0) {
        fprintf(stderr, "playback open error: %s\n", snd_strerror(ret_val));
        exit(1);
    }
    ret_val = snd_pcm_set_params(outhandle, SND_PCM_FORMAT_S32_LE,
            SND_PCM_ACCESS_RW_NONINTERLEAVED, CH_NUM, SAMPLING_RATE, 1, 50000);
    if (ret_val < 0) {
        fprintf(stderr, "playback set error: %s\n", snd_strerror(ret_val));
        exit(1);
    }
}

// サウンドデバイスを閉じる
void sounddev_exit(void)
{
    // 入力を閉じる
    snd_pcm_drain(inhandle);
    snd_pcm_close(inhandle);

    // 出力を閉じる
    snd_pcm_drain(outhandle);
    snd_pcm_close(outhandle);
}

// サウンドデバイスからオーディオデータを読み出す
int sounddev_read(int **inbuf, int frame_num)
{
    int ret_val;
    int ret;

    // 入力信号を読み込む
    ret_val = snd_pcm_readn(inhandle, (void **)inbuf, frame_num);
    if (ret_val == -EPIPE) {
        fprintf(stderr, "capture underrun");
        ret = 0;
    } else if (ret_val < 0) {
        fprintf(stderr, "capture error: %s\n", snd_strerror(ret_val));
        ret = 0;
        exit(1);
    } else if (ret_val != frame_num) {
        fprintf(stderr, "captrue unexpected num of frames: %d\n", ret_val);
        ret = ret_val;
    } else {
        ret = ret_val;
    }

    return ret;
}

// サウンドデバイスへオーディオデータを書き出す
int sounddev_write(int **outbuf, int frame_num)
{
    int ret_val;
    int ret;

    // 出力信号を書き込む
    ret_val = snd_pcm_writen(outhandle, (void **)outbuf, frame_num);
    if (ret_val == -EPIPE) {
        fprintf(stderr, "playback underrun");
        ret_val = snd_pcm_recover(outhandle, ret_val, 0);
        if (ret_val < 0) {
            fprintf(stderr, "playback recover error: %s\n",
                    snd_strerror(ret_val));
            exit(1);
        }
        ret = 0;
    } else if (ret_val < 0) {
        fprintf(stderr, "playback error: %s\n", snd_strerror(ret_val));
        ret = 0;
        exit(1);
    } else if (ret_val != frame_num) {
        fprintf(stderr, "playback unexpected num of frames: %d\n", ret_val);
        ret = ret_val;
    } else {
        ret = ret_val;
    }

    return ret;
}

