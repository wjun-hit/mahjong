#ifndef FANGJIAN_H
#define FANGJIAN_H

#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <iostream>
#include <algorithm>
#include <random>
#include <vector>
#include <stack>
#include <errno.h>

#include "../people/people.h"

#define CHUPAI 1
#define IS_PENG 2
#define IS_GANG 3
#define IS_HU 4
#define IS_READY 5
#define WAIT 6

#define HU 1
#define GANG 2
#define PENG 3
#define FAPAI 4



using namespace std;



class fangjian{
public:
    fangjian();
    ~fangjian();
    int getID(people *p);
    int getNextPeople(int id);
    bool is_full();
    bool is_empty();
    bool join_house(people *p);
    void out_house(people *p);
    void hupai(int id);
    bool hupaiBackTracking(vector<int> &nums, int s, int n, bool flag);
    
    bool have_hupai(int id, int c);
    int getHupaiPeople();
    int have_gang(int id, int c);
    int getGangPeople();
    int have_peng(int id, int c);
    int getPengPeople();
    
    void shuffle_cards();
    void deal_cards();
    int getCard();
    
    void dealHupai();
    void dealGang(int id, int c);
    void dealPeng(int id, int c);
    void print_everyone_cards();
    
    void sendCards();
    void sentHu(int id, int c);
    void sentGang(int id, int c);
    void sentPeng(int id, int c);
    void sentCard();
    void sendLastCard();
    void sendChupai();
    void sendStepError(int id);
    void sendPeopelError(int id);
    void sendReady();
    
    void sendCardsToOne(int id);
    
    void haveOtherDeal(int otherDealStatus);
    void deal(int id);
    
    bool isOneCard(int id, int c);
    void outCard(int id, int c);
    
    

private:
    int dapaiPeople;   //是哪个人操作
    int majiangStatus;   //属于哪个步骤
    
    stack<int> chupaiStack;
    stack<int> statusStack;

    vector<int> cards = vector<int>(108); // 牌 1-9 11-19 21-29 每个4张牌
    int card_ptr;  //指向第几张拍，发牌的时候用，每次开局需要清零
    int card_last;
    
    vector<vector<int> > oneCards = vector<vector<int> >(4, vector<int>(30));
    vector<vector<int> > oneHupai = vector<vector<int> >(4, vector<int>(30));
    vector<vector<int> > onePengpai = vector<vector<int> >(4); 
    vector<vector<int> > oneGangpai = vector<vector<int> >(4); 
    
    int peopelNum;   //加入局的人数
    people* clientFd[4];   //入局人的fd，用该句柄进行交流
    
    int readyNum;
    
    pthread_mutex_t ready_mutex;
    
    int pengPeople;
    int gangPeople;
    vector<int> hupaiPeople;
    
    bool test;
    
};



#endif