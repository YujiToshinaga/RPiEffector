#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
      
#define ALSA_PCM_NEW_HW_PARAMS_API
#include <alsa/asoundlib.h>

#define FRAME_NUM 128
#define DELAY_NUM 16
#define SAMPLING_RATE 96000
#define CH_NUM 2

static pthread_mutex_t mutex;
static int finish_effector = 0;

// サウンドデバイスを開く
void alsa_open(snd_pcm_t **inhandle, snd_pcm_t **outhandle)
{
    int ret;

    // 入力を初期化する
    ret = snd_pcm_open(inhandle, "default", SND_PCM_STREAM_CAPTURE, 0);
    if (ret < 0) {
        fprintf(stderr, "capture open error: %s\n", snd_strerror(ret));
        exit(1);
    }
    ret = snd_pcm_set_params(*inhandle, SND_PCM_FORMAT_S32_LE,
            SND_PCM_ACCESS_RW_NONINTERLEAVED, CH_NUM, SAMPLING_RATE, 1, 50000);
    if (ret < 0) {
        fprintf(stderr, "capture set error: %s\n", snd_strerror(ret));
        exit(1);
    }
           
    // 出力を初期化する
    ret = snd_pcm_open(outhandle, "default", SND_PCM_STREAM_PLAYBACK, 0);
    if (ret < 0) {
        fprintf(stderr, "playback open error: %s\n", snd_strerror(ret));
        exit(1);
    }
    ret = snd_pcm_set_params(*outhandle, SND_PCM_FORMAT_S32_LE,
            SND_PCM_ACCESS_RW_NONINTERLEAVED, CH_NUM, SAMPLING_RATE, 1, 50000);
    if (ret < 0) {
        fprintf(stderr, "playback set error: %s\n", snd_strerror(ret));
        exit(1);
    }
}

// サウンドデバイスを閉じる
void alsa_close(snd_pcm_t *inhandle, snd_pcm_t *outhandle)
{
    // 入力を閉じる
    snd_pcm_drain(inhandle);
    snd_pcm_close(inhandle);

    // 出力を閉じる
    snd_pcm_drain(outhandle);
    snd_pcm_close(outhandle);
}

// サウンドデバイスからオーディオデータを読み出す
void alsa_read(snd_pcm_t *inhandle, int **inbuf)
{
    int ret;

    // 入力信号を読み込む
    ret = snd_pcm_readn(inhandle, (void **)inbuf, FRAME_NUM);
    if (ret == -EPIPE) {
        fprintf(stderr, "capture underrun");
    } else if (ret < 0) {
        fprintf(stderr, "capture error: %s\n", snd_strerror(ret));
        exit(1);
    } else if (ret != FRAME_NUM) {
        fprintf(stderr, "captrue unexpected num of frames: %d\n", ret);
    }
}

// サウンドデバイスへオーディオデータを書き出す
void alsa_write(snd_pcm_t *outhandle, int **outbuf)
{
    int ret;

    // 出力信号を書き込む
    ret = snd_pcm_writen(outhandle, (void **)outbuf, FRAME_NUM);
    if (ret == -EPIPE) {
        fprintf(stderr, "playback underrun");
        ret = snd_pcm_recover(outhandle, ret, 0);
        if (ret < 0) {
            fprintf(stderr, "playback recover error: %s\n", snd_strerror(ret));
            exit(1);
        }
    } else if (ret < 0) {
        fprintf(stderr, "playback error: %s\n", snd_strerror(ret));
        exit(1);
    } else if (ret != FRAME_NUM) {
        fprintf(stderr, "playback unexpected num of frames: %d\n", ret);
    }
}

// ノンエフェクター
void* noneff_thread()
{
    snd_pcm_t *inhandle, *outhandle;
    int *inbuf[2], *outbuf[2];
    int ret;

    // メモリを確保する
    inbuf[0] = (int *)malloc(FRAME_NUM * sizeof(int));
    inbuf[1] = (int *)malloc(FRAME_NUM * sizeof(int));
    outbuf[0] = (int *)malloc(FRAME_NUM * sizeof(int));
    outbuf[1] = (int *)malloc(FRAME_NUM * sizeof(int));

    // サウンドデバイスを開く
    alsa_open(&inhandle, &outhandle);

    // 信号の入力、処理、出力をし続ける
    while (finish_effector == 0) {
        // サウンドデバイスからオーディオデータを読み出す
        alsa_read(inhandle, inbuf);

        // 信号処理を行わずバッファコピーのみ行う
        memcpy(outbuf[0], inbuf[0], FRAME_NUM * sizeof(int));
        memcpy(outbuf[1], inbuf[1], FRAME_NUM * sizeof(int));

        // サウンドデバイスへオーディオデータを書き出す
        alsa_write(outhandle, outbuf);
    }

    // サウンドデバイスを閉じる
    alsa_close(inhandle, outhandle);

    // メモリを解放する
    free(inbuf[0]);
    free(inbuf[1]);
    free(outbuf[0]);
    free(outbuf[1]);
}

// 移動平均フィルタ
void* sma_thread()
{
    snd_pcm_t *inhandle, *outhandle;
    int *inbuf[2], *outbuf[2];
    int *sma_buf[2];
    int sma_sum[2];
    int i, j;
    int ret;

    // メモリを確保する
    inbuf[0] = (int *)malloc(FRAME_NUM * sizeof(int));
    inbuf[1] = (int *)malloc(FRAME_NUM * sizeof(int));
    outbuf[0] = (int *)malloc(FRAME_NUM * sizeof(int));
    outbuf[1] = (int *)malloc(FRAME_NUM * sizeof(int));
    sma_buf[0] = (int *)malloc(DELAY_NUM * sizeof(int));
    sma_buf[1] = (int *)malloc(DELAY_NUM * sizeof(int));

    // サウンドデバイスを開く
    alsa_open(&inhandle, &outhandle);

    // バッファを初期化する
    for (i = 0; i < DELAY_NUM; i++) {
        sma_buf[0][i] = 0;
        sma_buf[1][i] = 0;
    }

    // 信号の入力、処理、出力をし続ける
    while (finish_effector == 0) {
        // サウンドデバイスからオーディオデータを読み出す
        alsa_read(inhandle, inbuf);

        // 信号処理を行う（移動平均フィルタ）
        for (i = 0; i < FRAME_NUM; i++) {
            for (j = (DELAY_NUM - 1); j > 0; j--) {
                sma_buf[0][j] = sma_buf[0][j - 1];
                sma_buf[1][j] = sma_buf[1][j - 1];
            }
            sma_buf[0][0] = inbuf[0][i];
            sma_buf[1][0] = inbuf[1][i];

            sma_sum[0] = 0;
            sma_sum[1] = 0;
            for (j = 0; j < DELAY_NUM; j++) {
                sma_sum[0] += sma_buf[0][j];
                sma_sum[1] += sma_buf[1][j];
            }
            outbuf[0][i] = sma_sum[0] / DELAY_NUM;
            outbuf[1][i] = sma_sum[1] / DELAY_NUM;
        }

        // サウンドデバイスへオーディオデータを書き出す
        alsa_write(outhandle, outbuf);
    }

    // サウンドデバイスを閉じる
    alsa_close(inhandle, outhandle);

    // メモリを解放する
    free(inbuf[0]);
    free(inbuf[1]);
    free(outbuf[0]);
    free(outbuf[1]);
}

void effector() {
    pthread_t th_effector = (pthread_t)NULL;
    char ch = '0';
    int thread_alive = 0;
    int finish = 0;

    // 0 : 停止
    // 1 : エフェクターなし
    // 2 : 移動平均フィルタ
    // q : 終了

    // 初期化する
    finish_effector = 0;

    // キー入力を監視する
    printf("Press q to finish\n");
    while (finish == 0) {
        ch = getchar();

        switch (ch) {
        case '0':
            if (thread_alive == 1) {
                finish_effector = 1;
                pthread_join(th_effector, NULL);
            }
            thread_alive = 0;
            break;
        case '1':
            if (thread_alive == 1) {
                finish_effector = 1;
                pthread_join(th_effector, NULL);
            }
            finish_effector = 0;
            pthread_create(&th_effector, NULL, noneff_thread, NULL);
            thread_alive = 1;
            break;
        case '2':
            if (thread_alive == 1) {
                finish_effector = 1;
                pthread_join(th_effector, NULL);
            }
            finish_effector = 0;
            pthread_create(&th_effector, NULL, sma_thread, NULL);
            thread_alive = 1;
            break;
        case 'q':
            if (thread_alive == 1) {
                finish_effector = 1;
                pthread_join(th_effector, NULL);
            }
            finish = 1;
        default:
            break;
        }
    }
    printf("Finish\n");
}

int main(int argc, char *argv[])
{
    effector();

    return 0;
}

