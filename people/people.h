#ifndef PEOPLE_H
#define PEOPLE_H
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <iostream>
#include <sys/epoll.h>
#include <time.h>
#include <pthread.h>

#define MAXBUFLEN 128

#define CHUPAI 1
#define IS_PENG 2
#define IS_GANG 3
#define IS_HU 4
#define IS_READY 5
#define WAIT 6
#define CREATE 7


#define HU 1
#define GANG 2
#define PENG 3
#define FAPAI 4

#define YES 1
#define NO 2

using namespace std;


class people{
public:
    people(int clifd);
    ~people();
    
    int recv_data();
    void sent_data();
    
    void sent_fj_is_full();
    void sent_fj_create_ok();
    
    void close_client(int epfd);
    
    void add_read_event(int epfd);
    void add_write_event(int epfd);
    void del_write_event(int epfd);
    void del_read_event(int epfd);
    
    int get_fd();
    int get_in_fj_id();
    int get_status();
    int get_chupai();
    bool get_is_hu();
    bool get_is_gang();
    bool get_is_peng();
    char get_fj_name();
    
    void set_in_fj_id(int id);
  
    char readBuf[MAXBUFLEN];
    char writeBuf[MAXBUFLEN];

private:  
    int readStatus;
    int readContent;

    int fd;
    int in_fj_id;
    int status;
    int chupai;
    bool is_hu;
    bool is_gang;
    bool is_peng;
    char fj_name;
    
    string name;


    long last_active;
    
    bool is_read;
    bool is_write;
};




#endif