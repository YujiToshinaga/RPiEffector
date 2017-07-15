#ifndef __REVERB_H__
#define __REVERB_H__

extern void reverb_init(int *param);
extern void reverb_exit(void);
extern void reverb_main(int **inbuf, int **outbuf);

#endif /* __REVERB_H__ */
