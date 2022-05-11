#include "people.h"



people::people(int clifd){
    this->fd = clifd;
    is_read = false;
    is_write = false;
}

people::~people(){

}

void people::close_client(int epfd){
    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);  
    close(fd);
    cout << "close--" << fd << endl;
}


int people::recv_data(){

    memset(readBuf, 0, sizeof(readBuf));
    
    int n = read(fd, readBuf, MAXBUFLEN);

    if(n<=0){
        return n;
    }
    cout << "recv:" << fd << "--" << readBuf << "  len:" << n << endl;
    last_active = time(NULL);
    readStatus = readBuf[0];
    readContent = readBuf[1];
    
    status = readStatus;  
    switch(status){
        case CHUPAI:
            chupai = readContent;
            cout << "CHUPAI: " << chupai << endl;
            break;
        case IS_HU:
            is_hu = readContent==YES ? true: false;
            cout << "IS_HU: " << is_hu << endl;
            break;
        case IS_GANG:
            is_gang = readContent==YES ? true: false;
            cout << "IS_GANG: " << is_gang << endl;
            break;
        case IS_PENG:
            is_peng = readContent==YES ? true: false;
            cout << "IS_PENG: " << is_peng << endl;
            break;
        case CREATE:
            fj_name = readContent;
            cout << "CREATE: " << fj_name << endl;
            break;
        case IS_READY:
            cout << "READY" << endl;
            break;
    }
    
    return n;
}


void people::sent_fj_is_full(){
    memset(writeBuf, 0, sizeof(writeBuf));
    strcpy(writeBuf, "recreate ");
    int k=9;
    writeBuf[k] = '\0';
    sent_data();
}

void people::sent_fj_create_ok(){
    memset(writeBuf, 0, sizeof(writeBuf));
    strcpy(writeBuf, "yes");
    sent_data();
}

void people::sent_data(){
    int n = write(fd, writeBuf, strlen(writeBuf));
    cout << "sent:" << fd << "--" << writeBuf << "  len:" << n << endl;
        
}

void people::add_read_event(int epfd){
    epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = fd;
    if(is_write){
        epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &event);  
    }
    else{
        epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);  
    }
    is_read = true;
    is_write = false;
}


void people::add_write_event(int epfd){
    epoll_event event;
    event.events = EPOLLOUT;
    event.data.fd = fd;
    if(is_read)
        epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &event);  
    else
        epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);  
    is_read = false;
    is_write = true;
}

void people::del_write_event(int epfd){
    epoll_event event;
    event.events = EPOLLOUT;
    event.data.fd = fd;
    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &event);  
}

void people::del_read_event(int epfd){
    epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = fd;
    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &event);  
}

int people::get_fd(){
    return fd;
}
int people::get_in_fj_id(){
    return in_fj_id;
}
void people::set_in_fj_id(int id){
    in_fj_id = id;
}
int people::get_status(){
    return status;
}
int people::get_chupai(){
    return chupai;
}
bool people::get_is_hu(){
    return is_hu;
}
bool people::get_is_gang(){
    return is_gang;
}
bool people::get_is_peng(){
    return is_peng;
}
char people::get_fj_name(){
    return fj_name;
}
