#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <errno.h>

#define PORT 6666
#define MAXLINE 100


#define CHUPAI 1
#define IS_PENG 2
#define IS_GANG 3
#define IS_HU 4
#define IS_READY 5
#define IS_READY 5
#define WAIT 6
#define CREATE 7

#define YES 1
#define NO 2

using namespace std;

string Request;
vector<int> Content;

void analysis_data(char buf[]){
    Request.clear();
    Content.clear();
    int i=0;
    while(buf[i] != ' ' && buf[i] != '\0')
        Request.push_back(buf[i++]);
    i++;
    while(buf[i] != '\0')
        Content.push_back(buf[i++]);
}

void print_cards(int cards[], vector<int> &peng, vector<int> &gang){
    cout << "--muqian pai is: ";
    for(int i=0; i<30; i++){
        if(cards[i]>0){
            for(int j=0; j<cards[i]; j++)
                cout << i << " ";
        }
    }
    if(peng.size()>0){
        cout << "--peng de pai: ";
        for(int i=0; i<peng.size(); i++)
            cout << peng[i]<<peng[i]<<peng[i]<<" ";
    }
    if(gang.size()>0){
        cout << "--gang de pai: ";
        for(int i=0; i<gang.size(); i++)
            cout << gang[i]<<gang[i]<<gang[i]<<gang[i]<<" ";
    }
    cout << endl;
}

ssize_t Write(int fd, const void *ptr, size_t nbytes)
{
	ssize_t n;

again:
	if ( (n = write(fd, ptr, nbytes)) == -1) {
		if (errno == EINTR)
			goto again;
		else
			return -1;
	}
	return n;
}

ssize_t Read(int fd, void *ptr, size_t nbytes)
{
	ssize_t n;

again:
	if ( (n = read(fd, ptr, nbytes)) == -1) {
		if (errno == EINTR)
			goto again;
		else
			return -1;
	}
	return n;
}

int main(){
    char buf[MAXLINE] = {""};
    int cards[30];
    vector<int> peng;
    vector<int> gang;
    memset(cards, 0, sizeof(cards));
    peng.clear();
    
    struct sockaddr_in servaddr;
    
    int clifd = socket(AF_INET, SOCK_STREAM, 0);
    
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    inet_pton(AF_INET, "10.249.40.78", &servaddr.sin_addr);
    
    connect(clifd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    
    char name = 'a';
    buf[0] = CREATE;
    buf[1] = name;
    buf[2] = 0;
    Write(clifd, buf, strlen(buf));
    memset(buf, 0, sizeof(buf));
    
    int n = Read(clifd, buf, 3);
    cout << buf << endl;
    while(buf[0]!='y'){
        cout << buf << endl;
        cout << "please reinput name: " << endl;
        cin >> name;
        buf[0] = CREATE;
        buf[1] = name;
        buf[2] = 0;
        Write(clifd, buf, strlen(buf));
        memset(buf, 0, sizeof(buf));
        
        Read(clifd, buf, 3);
        cout << buf << endl;
    }
    memset(buf, 0, sizeof(buf));
    cout << "zhunbei recv" << endl;
    int c;
    char id;
    string str;
    while(1){

        if(Read(clifd, buf, 20)>0){
            //cout << buf << endl;
            analysis_data(buf);

            cout << endl;
            if(Request=="post"){
                c = Content[0];
                cout << "zhua le yi zhang pai: " << c << endl; 
                
                cards[c]++;
                print_cards(cards, peng, gang);
                
                cout << "please input da de pai: ";
                cin >> c;
                while(c<=0 || c>29 || cards[c]==0){
                    cout << "please reinput youxiao de pai: ";
                    cin >> c;
                }
                cards[c]--;
                print_cards(cards, peng, gang);
                
                memset(buf, 0, sizeof(buf));
                buf[0] = CHUPAI;
                buf[1] = c;
                Write(clifd, buf, strlen(buf));
            }
            else if(Request=="get"){
                cout << "please input da de pai: ";
                cin >> c;
                while(c<=0 || c>29 || cards[c]==0){
                    cout << "please reinput youxiao de pai: ";
                    cin >> c;
                }
                cards[c]--;
                print_cards(cards, peng, gang);
                
                memset(buf, 0, sizeof(buf));
                buf[0] = CHUPAI;
                buf[1] = c;
                Write(clifd, buf, strlen(buf));
            }
            else if(Request=="peng"){
                c = Content[0];
                id = Content[1];
                cout << " user ID: " << id << " chu le pai: " << c << " do you want peng? yes or no" << endl;
                cin >> str;
                
                memset(buf, 0, sizeof(buf));
                if(str=="yes"){
                      buf[1] = YES;
                      cards[c] -= 2;
                      peng.push_back(c);
                }
                else buf[1] = NO;
                buf[0] = IS_PENG;
                Write(clifd, buf, strlen(buf));
            }
            else if(Request=="gang"){
                c = Content[0];
                id = Content[1];
                cout << " user ID: " << id << " chu le pai: " << c << " do you want gang? yes or no" << endl;
                cin >> str;
                
                memset(buf, 0, sizeof(buf));
                if(str=="yes"){
                      buf[1] = YES;
                      cards[c] -= 3;
                      gang.push_back(c);
                }
                else buf[1] = NO;
                buf[0] = IS_GANG;
                Write(clifd, buf, strlen(buf));
            }
            else if(Request=="hupai"){
                c = Content[0];
                id = Content[1];
                cout << " user ID: " << id << " chu le pai: " << c << " do you want hupai? yes or no" << endl;
                cin >> str;
                
                memset(buf, 0, sizeof(buf));
                if(str=="yes"){
                      buf[1] = YES;
                }
                else buf[1] = NO;
                buf[0] = IS_HU;
                Write(clifd, buf, strlen(buf));
            }
            else if(Request=="posts"){
                memset(cards, 0, sizeof(cards));
                peng.clear();
                //cout << buf << endl;
                //cout << Request << endl;
                for(int i=0; i<Content.size(); i++)
                    cards[Content[i]]++;
                    
                print_cards(cards, peng, gang);
            }
            else if(Request=="ready"){

                cout << "qing zhun bei" << endl;
                cin >> str;
                
                memset(buf, 0, sizeof(buf));
                if(str=="yes"){
                      buf[1] = YES;
                }
                else buf[1] = NO;
                buf[0] = IS_READY;
                Write(clifd, buf, strlen(buf));
            }
            memset(buf, 0, sizeof(buf));
        }
    }

        
    close(clifd);


    return 0;
}