#include "mahjong.h"

unordered_map<int, shared_ptr<people>> mahjong::fd2pp;
unordered_map<char, shared_ptr<fangjian> > mahjong::name_fj;
int mahjong::pthread_num1=2;
int mahjong::pthread_num2=2;



void* mahjong::pthread_pool_work3(void* arg){
    thread_pool<shared_ptr<people>> *p = (thread_pool<shared_ptr<people>>*)arg;
    pthread_t pth = pthread_self();
    int id = -1;
    while((id=p->get_thread_id(pth))<0);
    
    while(1){
        while(!p->have_task(id));
               
        shared_ptr<people> t = p->pop_task(id);

        shared_ptr<fangjian> fj = mahjong::name_fj[t->get_fj_name()];
        fj->deal(fj->getID(t.get()));
        
        p->finish_noe_task(id);
    }
}

void* mahjong::pthread_pool_work2(void* arg){
    thread_pool<shared_ptr<people>> *p = (thread_pool<shared_ptr<people>>*)arg;
    pthread_t pth = pthread_self();
    int id = -1;
    while((id=p->get_thread_id(pth))<0);
    
    thread_pool<shared_ptr<people>> pool;
    pool.thread_create(mahjong::pthread_num2, mahjong::pthread_pool_work3); 
    
    int epfd = epoll_create(MAXCLIENT);
    epoll_event cliEvent[MAXCLIENT];
    
    while(1){
        if(p->have_task(id)){

            shared_ptr<people> t = p->pop_task(id);
            t->add_read_event(epfd);

        }
        int connNum = epoll_wait(epfd, cliEvent, MAXCLIENT, 0);
        for(int i=0; i<connNum; i++){
            int connfd = cliEvent[i].data.fd;
            if(cliEvent[i].events & EPOLLIN && mahjong::fd2pp.count(connfd)){
                int n = mahjong::fd2pp[connfd]->recv_data();
                if(n<1){
                    char fj_name = mahjong::fd2pp[connfd]->get_fj_name();
                    
                    if(mahjong::name_fj.count(fj_name)){
                        mahjong::name_fj[fj_name]->out_house(mahjong::fd2pp[connfd].get());
                        if(mahjong::name_fj[fj_name]->is_empty()){
                            mahjong::name_fj.erase(fj_name);
                        }
                    }
                    mahjong::fd2pp[connfd]->del_read_event(epfd);
                    mahjong::fd2pp.erase(connfd);
                }
                else{
                    pool.push_task(mahjong::fd2pp[connfd]);
                }
            }
        }
        pool.free_finish_task();
    }
}

void mahjong::pthread_work1(){
    thread_pool<shared_ptr<people>> pool;
    pool.thread_create(mahjong::pthread_num1, mahjong::pthread_pool_work2); 

    my_socket sock;
    sock.sockt_init();
    int sockfd = sock.getFd();

    cout << "socket fd:" << sockfd << endl;
    
    int epfd = epoll_create(MAXCLIENT);
    my_socket::epfd = epfd;
    
    epoll_event event, cliEvent[MAXCLIENT];
    event.events = EPOLLIN;
    event.data.fd = sockfd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &event);
    
    int connfd, efd;
    while(1){

        int connNum = epoll_wait(epfd, cliEvent, MAXCLIENT, -1);
        for(int i=0; i<connNum; i++){
            efd = cliEvent[i].data.fd;
 
            if(efd==sockfd){
                
                connfd = sock.Accept();
                cout << connfd << "  connect our" << endl;

                shared_ptr<people> pp(new people(connfd));
                mahjong::fd2pp[connfd] = pp;
                pp->add_read_event(epfd);
            }
            else if(cliEvent[i].events & EPOLLIN && mahjong::fd2pp.count(efd)){
                connfd = efd;
                int n = mahjong::fd2pp[connfd]->recv_data();
                if(n<1){
                    char fj_name = mahjong::fd2pp[connfd]->get_fj_name();
                    
                    if(mahjong::name_fj.count(fj_name)){
                        mahjong::name_fj[fj_name]->out_house(mahjong::fd2pp[connfd].get());
                        if(mahjong::name_fj[fj_name]->is_empty()){
                            mahjong::name_fj.erase(fj_name);
                        }
                    }
                    mahjong::fd2pp[connfd]->del_read_event(epfd);
                    mahjong::fd2pp.erase(connfd);
                }
                else{
                    if(mahjong::fd2pp[connfd]->get_status() == CREATE){
                        if(mahjong::name_fj.count(mahjong::fd2pp[connfd]->get_fj_name())){
                            shared_ptr<fangjian> fj = mahjong::name_fj[mahjong::fd2pp[connfd]->get_fj_name()];
                            if(fj->is_full()){  //ÈËÂúÁË
                                mahjong::fd2pp[connfd]->sent_fj_is_full();
                            }
                            else{
                                
                                mahjong::fd2pp[connfd]->del_read_event(epfd);
                                pool.push_task(mahjong::fd2pp[connfd]);
                                mahjong::fd2pp[connfd]->sent_fj_create_ok();
                                fj->join_house(mahjong::fd2pp[connfd].get());
                            }
                        }
                        else{
                           shared_ptr<fangjian> fj = create_fangjian(mahjong::fd2pp[connfd]->get_fj_name());
                           
                           mahjong::fd2pp[connfd]->del_read_event(epfd);
                           pool.push_task(mahjong::fd2pp[connfd]);
                           mahjong::fd2pp[connfd]->sent_fj_create_ok();
                           fj->join_house(mahjong::fd2pp[connfd].get());
                        }
                    }
                }
            }
        }
        
    }
}



shared_ptr<fangjian> mahjong::create_fangjian(char name){

    shared_ptr<fangjian> fj(new fangjian()); 
    mahjong::name_fj[name] = fj;
    cout << "create house: " << name << endl;
    return fj;

}