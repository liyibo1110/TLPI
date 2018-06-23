#ifndef _BINARY_SEMS_H
#define _BINARY_SEMS_H
#include <stdbool.h>

extern bool bsUseSemUndo;
extern bool bsRetryOnEintr;

int initSemAvailable(int semId, int semNum);
int initSemInUse(int semId, int semNum);
int reserveSem(int semId, int semNum);
int releaseSem(int semId, int semNum);

#endif