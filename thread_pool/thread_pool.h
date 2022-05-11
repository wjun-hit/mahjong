#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <pthread.h>

#include <ctime>

#include <list>
#include <vector>
#include <unordered_map>

#include "../sleep/sleep.h"

#define MAXTASKNUM 10

using namespace std;
/*
void sleep(int second){
    for(int i=0; i<second; i++)
        usleep(1000000);
}
*/



template<class	T>	
class thread_pool{
public:
    thread_pool();
    thread_pool(int maxTaskNum);
    ~thread_pool();
    
    void thread_create(int num, void* (*fun)(void*));
    void push_task(T a);
    T pop_task(int id);
    
    void print_thread_id();
    int get_thread_id(pthread_t pth);
    bool have_task(int id);
    
    void init_pthread_data(int num);
    
    void finish_noe_task(int id);
    void free_finish_task();
    void malloc_task_num();
    int get_random_pthread_id();

private:
    int threadNum;  //�̳߳���
    int workThreadNum;  //���ڴ���������̸߳���
    
    int maxTaskNum;

    list<pthread_t> threadList;
    unordered_map<pthread_t, int> thread_2_id;
    
    vector<list<T>> task;
    vector<int> taskNum;
    
    vector<list<int>> finishTaskList;  //��¼�߳���ɵ�����������Ҫ����
    int emptyTaskNum;      //�ܹ��ж��ٿ��������������Դ����������
    vector<int> emptyPthreaList;
    vector<int> remainTaskNum;   //ÿ���̻߳��ж��ٿ�������
};


template<class	T>
thread_pool<T>::thread_pool(){
    cout << "thread_pool" << endl;
    threadNum = 0;
    workThreadNum = 0;
    maxTaskNum = 10;
}

template<class	T>
thread_pool<T>::thread_pool(int maxTaskNum){
    thread_pool();

    this->maxTaskNum = maxTaskNum;
}


template<class	T>
thread_pool<T>::~thread_pool(){

    cout << "~thread_pool" << endl;
}



template<class	T>
void thread_pool<T>::finish_noe_task(int id){
    finishTaskList[id].push_back(id);
}

template<class	T>
void thread_pool<T>::free_finish_task(){   //���մ���������������Ž�remainTaskNum���Ա��´�����ʹ��

    for(int i=0; i<threadNum; i++){
        int sz = finishTaskList[i].size();
        if(sz>0){
            remainTaskNum[i] += sz;
            while(sz--) finishTaskList[i].pop_front();
        }
    }
        
}

template<class	T>
void thread_pool<T>::init_pthread_data(int num){
    threadNum = num;
    taskNum = vector<int>(num, 0);
    task.resize(num);
    
    
    emptyTaskNum = 0;
    finishTaskList.resize(num);
    remainTaskNum = vector<int>(num, maxTaskNum);
    malloc_task_num();
}

template<class	T>
void thread_pool<T>::malloc_task_num(){
    for(int i=0; i<threadNum; i++){
        int inc = maxTaskNum/10;  //ÿ�η���������10%
        inc  = min(inc, remainTaskNum[i]);
        
        for(int j=0; j<inc; j++){
            emptyPthreaList.push_back(i);
        }
        emptyTaskNum += inc;
        remainTaskNum[i] -= inc;
        //cout << "inc:" << i << " " << inc << "   ";
    }
    //for(int i: emptyPthreaList) cout << i << " ";
    //cout << emptyTaskNum << endl;
}

template<class	T>
int thread_pool<T>::get_random_pthread_id(){
    if(emptyTaskNum==0){
        malloc_task_num();
    }
    if(emptyTaskNum==0) return -1;
    srand(time(0));
    int num = rand() % emptyTaskNum;
    int id = emptyPthreaList[num];
    emptyTaskNum--;
    swap(emptyPthreaList[num], emptyPthreaList[emptyTaskNum]);
    emptyPthreaList.pop_back();
    return id;
}

template<class	T>	
void thread_pool<T>::push_task(T a){
    
    int id = get_random_pthread_id();

    task[id].push_back(a);
}




/*
template<class	T>	
void thread_pool<T>::push_task(int id, T a){
    
    task[id].push_back(a);
    pthread_mutex_lock(&taskNumMutex[id]);
    taskNum[id]++;
    pthread_mutex_unlock(&taskNumMutex[id]);
}

*/

template<class	T>	
T thread_pool<T>::pop_task(int id){

    T t = task[id].front();
    task[id].pop_front();
    return t;
}


template<class	T>	
void thread_pool<T>::print_thread_id(){
    for(list<pthread_t>::iterator it=this->threadList.begin(); it!=this->threadList.end(); it++){
        cout << "thread id:"  << *it << endl;
    }
}

template<class	T>	
int thread_pool<T>::get_thread_id(pthread_t pth){
    if(thread_2_id.count(pth)>0) return thread_2_id[pth];
    return -1;
}

template<class	T>	
bool thread_pool<T>::have_task(int id){
    if(task[id].size()>0) return true;
    return false;
}

template<class	T>
void thread_pool<T>::thread_create(int num, void* (*fun)(void*)){
    //threadNum = num;
    //taskNum = vector<int>(num, 0);
    //task.resize(num);
    //taskNumMutex_init(num);
    init_pthread_data(num);
    for(int i=0; i<num; i++){
        pthread_t pt;
        pthread_create(&pt, NULL, fun, this);
        threadList.push_back(pt);
        thread_2_id[pt] = i;
    }
}


#endif
