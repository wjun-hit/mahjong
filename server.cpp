
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <pthread.h>

#include "./mahjong/mahjong.h"


int main(){
    mahjong mj;
    mj.pthread_work1();
    
}