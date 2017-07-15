#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
      
#define ALSA_PCM_NEW_HW_PARAMS_API
#include <alsa/asoundlib.h>

#define FRAME_NUM 128
#define DELAY_NUM 4
#define SAMPLING_RATE 96000
#define CH_NUM 2

static int finish = 0;
static int finish_effector = 0;

static int inbuf_l[FRAME_NUM];
static int inbuf_r[FRAME_NUM];
static int outbuf_l[FRAME_NUM];
static int outbuf_r[FRAME_NUM];
static int sma_buf_l[DELAY_NUM];
static int sma_buf_r[DELAY_NUM];
static int sma_sum_l;
static int sma_sum_r;

// エフェクタースレッド
void* effector_thread()
{
    int *pinbuf[2], *poutbuf[2];
    snd_pcm_t *inhandle, *outhandle;
    int i, j;
    int ret;

    // 入力を初期化する
    ret = snd_pcm_open(&inhandle, "default", SND_PCM_STREAM_CAPTURE, 0);
    if (ret < 0) {
        fprintf(stderr, "capture open error: %s\n", snd_strerror(ret));
        exit(1);
    }
    ret = snd_pcm_set_params(inhandle, SND_PCM_FORMAT_S32_LE,
            SND_PCM_ACCESS_RW_NONINTERLEAVED, CH_NUM, SAMPLING_RATE, 1, 50000);
    if (ret < 0) {
        fprintf(stderr, "capture set error: %s\n", snd_strerror(ret));
        exit(1);
    }
           
    // 出力を初期化する
    ret = snd_pcm_open(&outhandle, "default", SND_PCM_STREAM_PLAYBACK, 0);
    if (ret < 0) {
        fprintf(stderr, "playback open error: %s\n", snd_strerror(ret));
        exit(1);
    }
    ret = snd_pcm_set_params(outhandle, SND_PCM_FORMAT_S32_LE,
            SND_PCM_ACCESS_RW_NONINTERLEAVED, CH_NUM, SAMPLING_RATE, 1, 50000);
    if (ret < 0) {
        fprintf(stderr, "playback set error: %s\n", snd_strerror(ret));
        exit(1);
    }

    // バッファを初期化する
    for (i = 0; i < FRAME_NUM; i++) {
        inbuf_l[i] = 0;
        inbuf_r[i] = 0;
        outbuf_l[i] = 0;
        outbuf_r[i] = 0;
    }
    for (i = 0; i < DELAY_NUM; i++) {
        sma_buf_l[i] = 0;
        sma_buf_r[i] = 0;
    }
    pinbuf[0] = inbuf_l;
    pinbuf[1] = inbuf_r;
    poutbuf[0] = outbuf_l;
    poutbuf[1] = outbuf_r;

    // 信号の入力、処理、出力をし続ける
    while (finish == 0) {
        // 入力信号を読み込む
        ret = snd_pcm_readn(inhandle, (void **)pinbuf, FRAME_NUM);
        if (ret == -EPIPE) {
            fprintf(stderr, "capture underrun");
        } else if (ret < 0) {
            fprintf(stderr, "capture error: %s\n", snd_strerror(ret));
            exit(1);
        } else if (ret != FRAME_NUM) {
            fprintf(stderr, "captrue unexpected num of frames: %d\n", ret);
        }

//        // 信号処理を行わずバッファコピーのみ行う
//        memcpy(outbuf_l, inbuf_l, FRAME_NUM * sizeof(int));
//        memcpy(outbuf_r, inbuf_r, FRAME_NUM * sizeof(int));

        // 信号処理を行う（移動平均フィルタ）
        for (i = 0; i < FRAME_NUM; i++) {
            for (j = (DELAY_NUM - 1); j > 0; j--) {
                sma_buf_l[j] = sma_buf_l[j - 1];
                sma_buf_r[j] = sma_buf_r[j - 1];
            }
            sma_buf_l[0] = inbuf_l[i];
            sma_buf_r[0] = inbuf_r[i];

            sma_sum_l = 0;
            sma_sum_r = 0;
            for (j = 0; j < DELAY_NUM; j++) {
                sma_sum_l += sma_buf_l[j];
                sma_sum_r += sma_buf_r[j];
            }
            outbuf_l[i] = sma_sum_l / DELAY_NUM;
            outbuf_r[i] = sma_sum_r / DELAY_NUM;
        }

        // 出力信号を書き込む
        ret = snd_pcm_writen(outhandle, (void **)poutbuf, FRAME_NUM);
        if (ret == -EPIPE) {
            fprintf(stderr, "playback underrun");
            ret = snd_pcm_recover(outhandle, ret, 0);
            if (ret < 0) {
                fprintf(stderr, "playback recover error: %s\n",
                        snd_strerror(ret));
                exit(1);
            }
        } else if (ret < 0) {
            fprintf(stderr, "playback error: %s\n", snd_strerror(ret));
            exit(1);
        } else if (ret != FRAME_NUM) {
            fprintf(stderr, "playback unexpected num of frames: %d\n", ret);
        }
    }

    // 入力を閉じる
    snd_pcm_drain(inhandle);
    snd_pcm_close(inhandle);

    // 出力を閉じる
    snd_pcm_drain(outhandle);
    snd_pcm_close(outhandle);

    // 終了処理を行う
    finish_effector = 1;
}

int main(int argc, char *argv[])
{
    pthread_t th_effector;

    // エフェクタースレッドを作成する
    pthread_create(&th_effector, NULL, effector_thread, NULL);

    // キー入力を監視する
    printf("Press q to finish\n");
    while (getchar() != 'q') {}
    printf("Finish\n");

    // 終了処理を行う
    finish = 1;
    while (finish_effector == 0) {}

    return 0;
}
