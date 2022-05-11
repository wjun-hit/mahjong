#include "sleep.h"

#include <unistd.h>


void sleep(int second){
    for(int i=0; i<second; i++)
        usleep(1000000);
}
