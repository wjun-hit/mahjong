#ifndef MAHJONG_H
#define MAHJONG_H

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <pthread.h>
#include <memory>

#include <list>
#include <unordered_map>

#include "../fangjian/fangjian.h"
#include "../people/people.h"
#include "../my_socket/my_socket.h"
#include "../thread_pool/thread_pool.h"
#include "../sleep/sleep.h"

#define MAXLINE 100
#define MAXCLIENT 1024

using namespace std;

class mahjong{
public:
    shared_ptr<fangjian> create_fangjian(char name);
    
    static unordered_map<int, shared_ptr<people>> fd2pp;
    static unordered_map<char, shared_ptr<fangjian> > name_fj;
    
    void pthread_work1();
    static void* pthread_pool_work2(void* arg);
    static void* pthread_pool_work3(void* arg);

    static int pthread_num1;
    static int pthread_num2;
};

#endif