#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "effector.h"
#include "effdata.h"
#include "reverb.h"
      
#define DELAY_TIME      (100)  // delay time in ms
#define DELAY_REPEAT    (0.50) // 0〜0.95
#define DELAY_BUF_SIZE  (SAMPLING_RATE * DELAY_TIME / 1000)

static int *delay_buf_l, *delay_buf_r;
static int delay_buf_idx;

void reverb_init(int *param)
{
    int i;

    delay_buf_l = (int *)malloc(DELAY_BUF_SIZE * sizeof(int));
    delay_buf_r = (int *)malloc(DELAY_BUF_SIZE * sizeof(int));

    // バッファを初期化する
    for (i = 0; i < DELAY_BUF_SIZE; i++) {
        delay_buf_l[i] = 0;
        delay_buf_r[i] = 0;
    }
}

void reverb_exit(void)
{
    free(delay_buf_l);
    free(delay_buf_r);
}

void reverb_main(int **inbuf, int **outbuf)
{
    int *inbuf_l = inbuf[0];
    int *inbuf_r = inbuf[1];
    int *outbuf_l = outbuf[0];
    int *outbuf_r = outbuf[1];
    int delay;
    int i;

    for (i = 0; i < FRAME_NUM; i++) {
        delay = inbuf_l[i] / 2 +
            (int)((double)delay_buf_l[delay_buf_idx] * DELAY_REPEAT);
        delay_buf_l[delay_buf_idx] = delay;
        outbuf_l[i] = delay;

        delay = inbuf_r[i] / 2 +
            (int)((double)delay_buf_r[delay_buf_idx] * DELAY_REPEAT);
        delay_buf_r[delay_buf_idx] = delay;
        outbuf_r[i] = delay;

        delay_buf_idx = (delay_buf_idx + 1) % DELAY_BUF_SIZE;
    }
}
