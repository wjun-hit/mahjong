#ifndef MY_SOCKET_H
#define MY_SOCKET_H


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <strings.h>
#include <sys/epoll.h>

#define PORT 6666

void perr_exit(const char *s);


class my_socket{

public:
    my_socket();
    
    void sockt_init();
    int Accept();
    int Socket();
    int Bind();
    int Connect();
    int Listen();
    int Close();
    int getFd();
    
    void add_read_event();
    
    static int epfd;
    static epoll_event event;
    
private:

    int fd;
    struct sockaddr_in servaddr;
    struct sockaddr_in cliaddr;
    socklen_t cliaddr_len;
    
    int family;  ///IP4 or IP6
    int type;    ///tcp or udp
    int protocol;  //д╛хо0
    
    unsigned int port;
    unsigned long ipAddr;
    
    int backlog;
};


#endif