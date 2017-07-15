#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "effector.h"
#include "sma.h"
      
#define DELAY_NUM 16

static int *sma_buf[2];

void sma_init(int *param)
{
    int i;

    sma_buf[0] = (int *)malloc(DELAY_NUM * sizeof(int));
    sma_buf[1] = (int *)malloc(DELAY_NUM * sizeof(int));

    // バッファを初期化する
    for (i = 0; i < DELAY_NUM; i++) {
        sma_buf[0][i] = 0;
        sma_buf[1][i] = 0;
    }
}

void sma_exit(void)
{
    free(sma_buf[0]);
    free(sma_buf[1]);
}

void sma_main(int **inbuf, int **outbuf)
{
    int *inbuf_l = inbuf[0];
    int *inbuf_r = inbuf[1];
    int *outbuf_l = outbuf[0];
    int *outbuf_r = outbuf[1];
    int sma_sum[2];
    int i, j;

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
}

