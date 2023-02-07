/**********************************************************************
  Module: threadwrappers.h
  Author: Daniel Rea and Junseok Lee

  Purpose: error-checking wrappers for pthread functions
	For distribution to students. Not all functions implemented.
	This is just from my solution with parts removed. 
  Treat it as a guide. Feel free to implement,
	change, remove, etc, in your own solution.

**********************************************************************/

#ifndef THREADWRAPPERS_H
#define THREADWRAPPERS_H

#include <pthread.h>

/* locks mutex, returns status*/
int wrappedMutexLock(pthread_mutex_t *mutex);

/* unlocks mutex, returns status*/
int wrappedMutexUnlock(pthread_mutex_t *mutex);

/* initializes mutex, returns status*/
int wrappedMutexInit(pthread_mutex_t  *mutex,  const  pthread_mutexattr_t *mutexattr);

/* creates thread, returns status*/
int wrappedPthreadCreate(pthread_t *thread, const pthread_attr_t *attr,
                          void *(*start_routine) (void *), void *arg);

/* Join thread by waiting for thread to finish, returns status*/                     
int wrappedPthreadJoin(pthread_t __th, void **__thread_return);

/* Signals waiting thread with condition*/
int wrappedCondSignal(pthread_cond_t *__cond);

/* Waits until signalled by another thread*/
int wrappedCondWait(pthread_cond_t *__restrict__ __cond, pthread_mutex_t *__restrict__ __mutex);

/* condition used to signal end of wait*/
pthread_cond_t gameCondition;

/* mutex used as to manage the game*/
pthread_mutex_t gameMutex;


#endif /* THREADWRAPPERS_H*/
