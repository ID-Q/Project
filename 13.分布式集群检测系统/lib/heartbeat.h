/*************************************************************************
	> File Name: heartbeat.h
	> Author: 
	> Mail: 
	> Created Time: 二 10/ 6 17:35:43 2020
 ************************************************************************/

#ifndef _HEARTBEAT_H
#define _HEARTBEAT_H

#include "network.h"

#include <iostream>
#include <unistd.h>

class HeartBeat {
public:
	HeartBeat(int port, const char *host) : _port(port), _host(host) {}
	bool heart_test();
private:
	int _port;
	const char *_host;
};

bool HeartBeat::heart_test() {
	TCP t(this->_port, this->_host, 0, 0.5);
	bool ret = true;
    int fd;
    if ((fd = t.socket_connect_tcp()) < 0) {
        ret = false;
    }
    close(fd);
    return ret;
}

#endif
