#ifndef USER_H
#define USER_H

typedef struct _UserConfig {
	// put your user data here
	// do not use String, only char arrays, long, boolean, bytes, int
} UserConfig;

extern void userInit();
extern void userLoop();
#endif