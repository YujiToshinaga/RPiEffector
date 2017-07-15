#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "effector.h"
#include "effdata.h"
#include "sma.h"
      
#define DELAY_NUM 16

static int *sma_buf_l, *sma_buf_r;

void sma_init(int *param)
{
    int i;

    sma_buf_l = (int *)malloc(DELAY_NUM * sizeof(int));
    sma_buf_r = (int *)malloc(DELAY_NUM * sizeof(int));

    // バッファを初期化する
    for (i = 0; i < DELAY_NUM; i++) {
        sma_buf_l[i] = 0;
        sma_buf_r[i] = 0;
    }
}

void sma_exit(void)
{
    free(sma_buf_l);
    free(sma_buf_r);
}

void sma_main(int **inbuf, int **outbuf)
{
    int *inbuf_l = inbuf[0];
    int *inbuf_r = inbuf[1];
    int *outbuf_l = outbuf[0];
    int *outbuf_r = outbuf[1];
    int sma_sum_l, sma_sum_r;
    int i, j;

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
}

