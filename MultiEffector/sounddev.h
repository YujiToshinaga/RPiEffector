#ifndef __SOUNDDEV_H__
#define __SOUNDDEV_H__

extern void sounddev_init(void);
extern void sounddev_exit(void);
extern int sounddev_read(int **inbuf, int frame_num);
extern int sounddev_write(int **outbuf, int frame_num);

#endif /* __SOUNDDEV_H__ */
