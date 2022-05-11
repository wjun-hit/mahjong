#include "fangjian.h"


fangjian::fangjian(){
    majiangStatus = WAIT;
    dapaiPeople = 0;
    
    peopelNum = 0;
    card_ptr = 0;
    readyNum = 0;
    
    pthread_mutex_init(&ready_mutex, NULL);
    
    test = false;
    /*
    for(int i=0; i<4; i++)
        clientFd[i] = i;
    */
    
    for(int i=0; i<4; i++){
        for(int j=1; j<10; j++)
            cards[card_ptr++] = j;
        for(int j=11; j<20; j++)
            cards[card_ptr++] = j;
        for(int j=21; j<30; j++)
            cards[card_ptr++] = j;
    }
    card_ptr = 0;
    card_last = cards.size();
    
    for(int i=0; i<4; i++){
        for(int j=0; j<30; j++){
            oneCards[i][j] = 0;
            oneHupai[i][j] = 0;
        }    
        onePengpai[i].clear();
        oneGangpai[i].clear();
        clientFd[i] = nullptr;
    }
    cout << "create a majiang house!" << endl;
    //test_hupai();
}

fangjian::~fangjian(){
    cout << "shut down a majiang house!" << endl;
}

// ͨ��fd���õ��ڱ������ID��
int fangjian::getID(people *p){
    for(int i=0; i<4; i++){
        if(clientFd[i] == p) return i;
    }
    return -1;
}

int fangjian::getNextPeople(int id){
    //int ID = getID(pp);
    //return clientFd[(ID+1)%4];
    return (id+1)%4;
}

bool fangjian::is_full(){
    if(peopelNum<4) return false;
    return true;
}

bool fangjian::is_empty(){
    if(peopelNum<1) return true;
    return false;
}

bool fangjian::join_house(people *p){
    if(peopelNum>=4) return false;
    clientFd[peopelNum] = p;
    p->set_in_fj_id(peopelNum);
    peopelNum++;
    if(peopelNum==4){
        sendReady();
    }

    //cout << endl;
    //cout << "----" << peopelNum << endl;
    return true;
}

void fangjian::out_house(people *p){
    int id = getID(p);
    if(id>=0){
        for(int i=id; i<3; i++){
            clientFd[i] = clientFd[i+1];
        }
        clientFd[3] = nullptr;
        peopelNum--;
        majiangStatus = WAIT;
    }
}


// ����һ���˺���Щ�ƣ�ÿ�γ����Ƽ��һ�£��д��Ż���
// ��Ҫ��ʽ���Ǳ���ÿһ���ƣ���������û����ж�һ���ܷ����
void fangjian::hupai(int id){
    //int ID=getID(pp);
    int pengGangNum = 3*onePengpai[id].size()+3*oneGangpai[id].size();
    for(int i=1; i<=9; i++){
       // if(oneCards[ID][i]==4) continue;
        oneCards[id][i]++;
        if(hupaiBackTracking(oneCards[id], 1, 14-pengGangNum, false))
            oneHupai[id][i] = 1;
        oneCards[id][i]--;
    }
    for(int i=11; i<=19; i++){
       // if(oneCards[ID][i]==4) continue;
        oneCards[id][i]++;
        if(hupaiBackTracking(oneCards[id], 1, 14-pengGangNum, false))
            oneHupai[id][i] = 1;
        oneCards[id][i]--;
    }
    for(int i=21; i<=29; i++){
       // if(oneCards[ID][i]==4) continue;
        oneCards[id][i]++;
        if(hupaiBackTracking(oneCards[id], 1, 14-pengGangNum, false))
            oneHupai[id][i] = 1;
        oneCards[id][i]--;
    }
    
}

// ���ݷ��������ܷ��
bool fangjian::hupaiBackTracking(vector<int> &nums, int s, int n, bool flag){
    while(s<=29 && nums[s]==0) s++;
    if(n==0) return true;
    if(s==30) return false;
    bool b1=false, b2=false, b3=false;
    if(nums[s]>=3){
        nums[s] -= 3;
        b1 = hupaiBackTracking(nums, s, n-3, flag);
        nums[s] += 3;
    }
    if(b1) return true;
    if((s>0&&s<8) || (s>10&&s<18) || (s>20&&s<28)){
        if(nums[s]>0 && nums[s+1]>0 && nums[s+2]>0){
            nums[s]--; nums[s+1]--; nums[s+2]--;
            b2 = hupaiBackTracking(nums, s, n-3, flag);
            nums[s]++; nums[s+1]++; nums[s+2]++;
        }
    }
    if(b2) return true;
    if(!flag && nums[s]>=2){
        nums[s] -= 2;
        b3 = hupaiBackTracking(nums, s, n-2, true);
        nums[s] += 2;
    }
    return b3;
}

// �Ƿ����˺��ƣ���˳���жϣ�������ȡ����ʱ��˳��Ļ������Դӽ�β��ʼȡ
bool fangjian::have_hupai(int id, int c){
    //int ID = getID(pp);
    int x = id==0? 3: id-1;
    while(x!=id){
        if(oneHupai[x][c]>0){
            hupaiPeople.push_back(x);
        }
        x = x==0? 3: x-1;
        //x = (x-1)%4;
    }
    if(hupaiPeople.size()>0) return true;
    return false;
}

// �õ����Ƶ���
int fangjian::getHupaiPeople(){
    int pp = hupaiPeople.back();
    hupaiPeople.pop_back();
    return pp;
}

// �Ƿ����˸�
int fangjian::have_gang(int id, int c){
    for(int i=0; i<4; i++){
        if(i==id) continue;
        if(oneCards[i][c]==3){
            gangPeople = i;
            return true;
        }    
    }
    return false;
}

int fangjian::getGangPeople(){
    return gangPeople;
}

// �Ƿ�������
int fangjian::have_peng(int id, int c){
    for(int i=0; i<4; i++){
        if(i==id) continue;
        if(oneCards[i][c]==2){
            pengPeople = i;
            return true;
        }    
    }
    return false;
}

int fangjian::getPengPeople(){
    return pengPeople;
}

void fangjian::shuffle_cards(){  //ϴ��
    shuffle(cards.begin(), cards.end(), default_random_engine(random_device()()));
    card_ptr = 0;
    card_last = cards.size();
    for(int i=0; i<4; i++){
        for(int j=0; j<30; j++){
            oneCards[i][j] = 0;
            oneHupai[i][j] = 0;
        }
        onePengpai[i].clear();
        oneGangpai[i].clear();
    }
    
}

void fangjian::deal_cards(){   ///����
    card_ptr = 0;
    for(int i=0; i<4; i++){
        for(int j=0; j<13; j++)
            oneCards[i][getCard()]++;
    }
}

int fangjian::getCard(){
    if(card_ptr >= card_last) return -1;  //����û����
    return cards[card_ptr++];
}

void fangjian::dealHupai(){
    //****����׼��״̬
    sendReady();
    //shuffle_cards();  //ϴ��
    //deal_cards();   // ����//*******ϴ�ƣ����·���
    //sentCard(); 
}

void fangjian::dealGang(int id, int c){
    //int id = getID(pp);
    oneCards[id][c] -= 3;
    oneGangpai[id].push_back(c);
}

void fangjian::dealPeng(int id, int c){
    //int id = getID(pp);
    oneCards[id][c] -= 2;
    onePengpai[id].push_back(c);
}

void fangjian::print_everyone_cards(){
    for(int i=0; i<4; i++){
        cout << "user ID: " << i << ": ";
        for(int j=0; j<30; j++){
            if(oneCards[i][j]!=0){
                for(int k=0; k<oneCards[i][j]; k++)
                    cout << j << " ";
            }  
        }
        if(onePengpai[i].size()>0){
            cout << "   peng le pai";
            for(int x: onePengpai[i]) cout << x << x << x << " ";
        }
        if(oneGangpai[i].size()>0){
            cout << "   gang le pai";
            for(int x: oneGangpai[i]) cout << x << x << x << x << " ";
        }
        cout << endl;
    }
}



void fangjian::sendCardsToOne(int id){
    people* p = clientFd[id];
    memset(p->writeBuf, 0, sizeof(p->writeBuf));
    strcpy(p->writeBuf, "posts ");
    int k=6;
    for(int i=1; i<oneCards[id].size(); i++){
        if(oneCards[id][i]>0){
            for(int j=0; j<oneCards[id][i]; j++)
                p->writeBuf[k++] = i;
        }
    }
    p->writeBuf[k] = '\0';
    p->sent_data();
}

void fangjian::sendCards(){
    if(test){
        print_everyone_cards();
        cout << "fa le pai" << endl;
    }
    else{
        deal_cards();
        for(int i=0; i<4; i++)
            sendCardsToOne(i);
    }
}


void fangjian::sentHu(int id, int c){
    if(test){
        print_everyone_cards();
        cout<< dapaiPeople << "----" << id << "chu le pai: " << c << ". do you want hu?" << endl;
    }
    else{
        people* p = clientFd[dapaiPeople];
        memset(p->writeBuf, 0, sizeof(p->writeBuf));
        strcpy(p->writeBuf, "hupai ");
        p->writeBuf[6] = c;
        p->writeBuf[7] = id+'0';
        p->writeBuf[8] = '\0';
        p->sent_data();
    }
} 


void fangjian::sentGang(int id, int c){
    if(test){
        print_everyone_cards();
        cout<< dapaiPeople << "----" << id << "chu le pai: " << c << ". do you want gang?" << endl;
    }
    else{
        people* p = clientFd[dapaiPeople];
        memset(p->writeBuf, 0, sizeof(p->writeBuf));
        strcpy(p->writeBuf, "gang ");
        p->writeBuf[5] = c;
        p->writeBuf[6] = id+'0';
        p->writeBuf[7] = '\0';
        p->sent_data();
    }
} 

void fangjian::sentPeng(int id, int c){
    if(test){
        print_everyone_cards();
        cout<< dapaiPeople << "----" << id << "chu le pai: " << c << ". do you want peng?" << endl;
    }
    else{
        people* p = clientFd[dapaiPeople];
        memset(p->writeBuf, 0, sizeof(p->writeBuf));
        strcpy(p->writeBuf, "peng ");
        p->writeBuf[5] = c;
        p->writeBuf[6] = id+'0';
        p->writeBuf[7] = '\0';
        p->sent_data();
    }
} 


void fangjian::sentCard(){
    //int id = getID(dapaiPeople);
    int c = getCard();
    oneCards[dapaiPeople][c]++;
    if(test){
        print_everyone_cards();
        cout<< dapaiPeople << "----" << "zhua le pai:" << c << "  qing da pai" << endl;
    }
    else{
        int id = dapaiPeople;
        people* p = clientFd[dapaiPeople];
        memset(p->writeBuf, 0, sizeof(p->writeBuf));
        strcpy(p->writeBuf, "post ");
        p->writeBuf[5] = c;
        p->writeBuf[6] = '\0';
        p->sent_data();
    }
}

//**********************δд��
void fangjian::sendLastCard(){
    //int id = getID(dapaiPeople);
    int c = getCard();
    oneCards[dapaiPeople][c]++;
    if(test){
        print_everyone_cards();
        cout<< dapaiPeople << "----" << "zhua le pai:" << c << "  qing da pai" << endl;
    }
    else{
        int id = dapaiPeople;
        people* p = clientFd[dapaiPeople];
        memset(p->writeBuf, 0, sizeof(p->writeBuf));
        strcpy(p->writeBuf, "post ");
        p->writeBuf[5] = c;
        p->writeBuf[6] = '\0';
        p->sent_data();
    }
}

void fangjian::sendChupai(){
    if(test){
        print_everyone_cards();
        cout<< dapaiPeople << "----" << "qing da pai" << endl;
    }
    else{
        int id = dapaiPeople;
        people* p = clientFd[dapaiPeople];
        memset(p->writeBuf, 0, sizeof(p->writeBuf));
        strcpy(p->writeBuf, "get ");
        p->writeBuf[4] = '\0';
        p->sent_data();
    }
}

void fangjian::sendReady(){
    majiangStatus = IS_READY;
    readyNum = 0;
    for(int i=0; i<4; i++){
        people* p = clientFd[i];
        memset(p->writeBuf, 0, sizeof(p->writeBuf));
        strcpy(p->writeBuf, "ready ");
        p->writeBuf[6] = '\0';
        p->sent_data();
    }
}

void fangjian::sendStepError(int id){
    cout << "bu zhou cuo le" << endl;
}

void fangjian::sendPeopelError(int id){
    cout << "bu shi ni cao zuo de" << endl;
}

bool fangjian::isOneCard(int id, int c){
    if(c<0 || c>=30) return false;
    if(oneCards[id][c]>=1) return true;
    return false;
}

void fangjian::outCard(int id, int c){
    oneCards[id][c]--;
}

void fangjian::haveOtherDeal(int otherDealStatus){
    int c,id;
    switch(otherDealStatus){
        case HU: 
            c = chupaiStack.top();
            id = statusStack.top();
            
            //cout << c <<pp<<endl;
            
            if(have_hupai(id, c)){   //********* �����Ƿ��к�������ѯ���Ƿ��
                dapaiPeople = getHupaiPeople();
                majiangStatus = IS_HU;
                sentHu(id, c);  //��ʵ�ʷ����ȱ��浽buf 
                
                return ;
            }
            
        case GANG:  //�ܾ����ƽ����������������� ��Ҫ�õ���һ����˭����������ʲô��
            c = chupaiStack.top();
            id = statusStack.top();
            if(have_gang(id, c)){
                dapaiPeople = getGangPeople();
                majiangStatus = IS_GANG;
                sentGang(id, c);  
                return ;
            }
        case PENG:
            c = chupaiStack.top();
            id = statusStack.top();
            if(have_peng(id, c)){
                dapaiPeople = getPengPeople();
                majiangStatus = IS_PENG;
                sentPeng(id, c);  
                return ;
            }
        case FAPAI:
            c = chupaiStack.top();
            id = statusStack.top();
            chupaiStack.pop();  //����һ��״̬���ƺ���ȥ��������Ҫ��
            statusStack.pop();
            majiangStatus = CHUPAI;
            dapaiPeople = getNextPeople(id);
            sentCard();  //����
            return ;
    }

}


void fangjian::deal(int id){
    //cout << "a";
    if(id == dapaiPeople || clientFd[id]->get_status()==IS_READY || clientFd[id]->get_status()==WAIT){   //������˲�����
        if(clientFd[id]->get_status() == majiangStatus || clientFd[id]->get_status()==IS_READY || clientFd[id]->get_status()==WAIT){   //����������ȷ
            switch(majiangStatus){
                case CHUPAI:
                    //cout << "a";
                    outCard(id, clientFd[id]->get_chupai());
                    hupai(id);
                    chupaiStack.push(clientFd[id]->get_chupai());  
                    statusStack.push(id);
                    haveOtherDeal(HU);  //�Ӻ��ƿ�ʼ���
                    break;
                case IS_HU:
                    if(clientFd[id]->get_is_hu()){  //*******ϴ�ƣ����·���
                        chupaiStack.pop();  //����һ��״̬���ƺ���ȥ��������Ҫ��
                        statusStack.pop();
                        
                        majiangStatus = CHUPAI;  //*******�齫״̬Ϊ���ƣ�ָ������Ӯ����
                        dealHupai();   //����֮��Ĵ���---ϴ�ƣ����ƣ�������������
                        
                    }
                    else{
                        haveOtherDeal(GANG);  //�����ƣ���Ӹܿ�ʼ���
                    }
                    break;
                case IS_GANG:
                    if(clientFd[id]->get_is_gang()){
                        int c = chupaiStack.top();
                        chupaiStack.pop();  //����һ��״̬���ƺ���ȥ��������Ҫ��
                        statusStack.pop();
                        
                        majiangStatus = CHUPAI;  //*******��������ˣ�״̬תΪCHUPAI
                        dealGang(dapaiPeople, c);  // c�Ƽ�3���ܼ��ϼ�c
                        
                        sendLastCard();  //******* ��ĩβץ�ƣ�ץ��֮����
                    }
                    else{
                        haveOtherDeal(PENG);   //���ܣ��������ʼ���
                    }
                    break;
                case IS_PENG:
                    if(clientFd[id]->get_is_peng()){
                        int c = chupaiStack.top();
                        chupaiStack.pop();  //����һ��״̬���ƺ���ȥ��������Ҫ��
                        statusStack.pop();
                        
                        majiangStatus = CHUPAI;   //********״̬תΪCHUPAI��ָ���˲���
                        dealPeng(dapaiPeople, c);
                        
                        sendChupai();
                    }
                    else{
                        haveOtherDeal(FAPAI);  //��������Ӹ���һ���˷���
                    }
                    break;
                case IS_READY:
                    pthread_mutex_lock(&ready_mutex);
                    readyNum++;
                    if(readyNum==4){
                        shuffle_cards();
                        sendCards();
                        sentCard();
                        majiangStatus = CHUPAI;
                    }
                    pthread_mutex_unlock(&ready_mutex);
                    
                    break;
                case WAIT:
                    break;
            }
        }
        else{  //�����������
            //********* ���Ͳ��������ʾ��Ϣ����
            sendStepError(id);
        }
    }
    else{
        //��������˲�����������˷���ʾ��Ϣ
        //********* ���Ͳ������������ʾ��Ϣ����
        sendPeopelError(id);
    }
}