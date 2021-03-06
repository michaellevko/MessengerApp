/*
 * Mthread.cpp
 *
 *  Created on: Feb 13, 2013
 *      Author: Eliav Menachi
 */

#include "MThread.h"
#include <iostream>
#include <stdio.h>
#include <thread>
using namespace std;

void* worker(void* arg){
	MThread* threadObj = (MThread*)arg;
	threadObj->run();
	threadObj->threadId = 0;
	return NULL;
}

void MThread::start(){
	pthread_create(&threadId,NULL,worker,(void*)this);
}

void MThread::waitForThread(){
	pthread_join(threadId,NULL);
	threadId=0;
}

MThread::~MThread(){
	if (threadId>0){
		//kills the thread if exist
		pthread_cancel(threadId);
		cout << "Thread " << std::hash<std::thread::id>()(std::this_thread::get_id()) << " was canceled." << endl;
	}
}

