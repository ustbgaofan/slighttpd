/*************************************************************************
    > File Name: worker.cpp
    > Author: Jiange
    > Mail: jiangezh@qq.com 
    > Created Time: 2016年01月28日 星期四 12时06分22秒
 ************************************************************************/

#include "worker.h"
#include "master.h"
#include "connection.h"

#include <stdlib.h>
#include <iostream>

Worker::Worker(const std::string &ip, unsigned short port)
		:listener(ip, port)
{
	master 			= NULL;
	w_base 			= NULL;
	w_exit_event	= NULL;
}

Worker::~Worker()
{
	//Master不进入run()，故不对w_exit_event初始化
	if (w_exit_event)
		event_free(w_exit_event);

	if (w_base)
	{
		ConnectionMap::iterator con_iter = con_map.begin();
		while (con_iter != con_map.end())
		{
			Connection *con = con_iter->second;
			delete con;
			++con_iter;
		}
		event_base_free(w_base);
	}
	std::cout << "Worker closed" << std::endl;
}

void Worker::Run()
{
	w_base = event_base_new();
	listener.AddListenEvent();
	w_exit_event = evsignal_new(w_base, SIGINT, Worker::WorkerExitSignal, w_base);
	evsignal_add(w_exit_event, NULL);
	
	event_base_dispatch(w_base);
	return;
}

void Worker::WorkerExitSignal(evutil_socket_t signo, short event, void *arg)
{	
	//std::cout << "Worker SIGINT" << std::endl;
	event_base_loopexit((struct event_base*)arg, NULL);
}