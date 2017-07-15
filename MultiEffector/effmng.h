#ifndef __EFFMNG_H__
#define __EFFMNG_H__

#include "effdata.h"
#include "through.h"
#include "sma.h"

extern void effmng_init(void);
extern void effmng_exit(void);
extern void effmng_play(EFFCMD effcmd);
extern void effmng_stop(void);
extern void effmng_help(void);

#endif /* __EFFMNG_H__ */
