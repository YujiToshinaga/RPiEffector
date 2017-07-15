#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "effector.h"
#include "effdata.h"
#include "through.h"
      
void through_init(int *param)
{
}

void through_exit(void)
{
}

void through_main(int **inbuf, int **outbuf)
{
    int *inbuf_l = inbuf[0];
    int *inbuf_r = inbuf[1];
    int *outbuf_l = outbuf[0];
    int *outbuf_r = outbuf[1];

    // 信号処理を行わずバッファコピーのみ行う
    memcpy(outbuf_l, inbuf_l, FRAME_NUM * sizeof(int));
    memcpy(outbuf_r, inbuf_r, FRAME_NUM * sizeof(int));
}
