/**********************************************************************
  Module: threadwrappers.c
  Author: Daniel Rea and Junseok Lee

  Purpose: error-checking wrappers for pthread functions
	For distribution to students. Not all functions implemented.
	This is just from my solution with parts removed. 
  	Treat it as a guide. Feel free to implement,
	change, remove, etc, in your own solution.

**********************************************************************/
#include <errno.h>
#include <stdio.h>
#include "threadwrappers.h"

/********************Status check function used by functions below***************/

int statusCheck(int s)
{
	if ((errno = s) != 0)
		perror(NULL);
	return s;
}

/********************Mutex functions***************/
int wrappedMutexInit(pthread_mutex_t *__mutex, const pthread_mutexattr_t *__mutexattr)
{
	return statusCheck(pthread_mutex_init(__mutex,__mutexattr));
}

int wrappedMutexLock(pthread_mutex_t *__mutex)
{
	return statusCheck(pthread_mutex_lock(__mutex));
}

int wrappedMutexUnlock(pthread_mutex_t *__mutex)
{
	return statusCheck(pthread_mutex_unlock(__mutex));

}

/********************Thread functions***************/
int wrappedPthreadCreate(pthread_t *thread, const pthread_attr_t *attr,
                          void *(*start_routine) (void *), void *arg)
{
	return statusCheck(pthread_create(thread, attr, start_routine, arg));
}

int wrappedPthreadJoin(pthread_t __th, void **__thread_return)
{
	return statusCheck(pthread_join(__th, __thread_return));
}

/********************Thread conditional functions***************/

int wrappedCondSignal(pthread_cond_t *__cond)
{
	return statusCheck(pthread_cond_signal(__cond));
}

int wrappedCondWait(pthread_cond_t *__restrict__ __cond, pthread_mutex_t *__restrict__ __mutex)
{
	return statusCheck(pthread_cond_wait(__cond, __mutex));
}

int wrappedPthreadBroadcast(pthread_cond_t *__cond)
{
	return statusCheck(pthread_cond_broadcast(__cond));
}

