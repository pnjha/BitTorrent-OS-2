/*
Prakash Nath Jha
2018201013
*/

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <bits/stdc++.h>
#define PORT_OFFSET 15
using namespace std;

bool isMasterTracker = false;
bool isNotifyTracker = false;

int masterSlaveConnfd;

bool searchFileHash(string,string);

string& ltrim(std::string& s){
    auto it = std::find_if(s.begin(), s.end(),[](char c) {
                                return !std::isspace<char>(c, std::locale::classic());
                            });
    s.erase(s.begin(), it);
    return s;
}

string& rtrim(std::string& s){
    auto it = std::find_if(s.rbegin(), s.rend(),
                        [](char c) {
                            return !std::isspace<char>(c, std::locale::classic());
                        });
    s.erase(it.base(), s.end());
    return s;
}

string& trim(std::string& s){
    return ltrim(rtrim(s));
}

void sendSeederListToSlave(int connfd,string seederFile){


    char file[seederFile.length()+1];

    strcpy(file,(char*)seederFile.c_str());

    FILE *fp = fopen(file,"rb");
    if(fp==NULL){
        printf("File open error");
    }

    while(true){
        
        unsigned char buff[524289]={0};
        int nread = fread(buff,1,524288,fp);
        
        if(nread > 0){

            write(connfd, buff, nread);
        }

        if (nread < 524288)
            break;
    }

    cout<<"seederList sent to slave\n";
    
    fclose(fp);    
    
}

bool receiveSeederListFromMaster(int sockfd,string seederFile){

    bool flag = true;

    char recvBuff[524289];
    int bytesReceived = 0;
    memset(recvBuff, '0', sizeof(recvBuff));


    char file[seederFile.length()+1];

    strcpy(file,(char*)seederFile.c_str());

    while(true){

        if((bytesReceived = read(sockfd, recvBuff, 524288)) > 0){
            
            if(flag){
               FILE *clearfp = fopen(file,"wb");
                fclose(clearfp);    
            }
            
            flag = false;

            FILE *fp = fopen(file,"ab");
            if(fp==NULL){
                printf("File open error");
            }

            fwrite(recvBuff, 1,bytesReceived,fp);
             
            if(bytesReceived<524288){
                fclose(fp);
                break;
            }    
        }else{
            return false;
        }
        
    }

    return true;
}


void waitingForSlaveToConnect(string tracker1IP, string tracker1Port,string tracker2IP,string tracker2Port,string seederFile){
    int listenfd = 0;
    int connfd = 0;
    struct sockaddr_in serv_addr;
    char sendBuff[524289];
    int numrv;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff));

    serv_addr.sin_family = AF_INET;
    int port = stoi (tracker1Port,nullptr,10) + PORT_OFFSET;
    serv_addr.sin_port = htons(port); // port
    serv_addr.sin_addr.s_addr = inet_addr(tracker1IP.c_str());

    bind(listenfd, (struct sockaddr*)&serv_addr,sizeof(serv_addr));

    if(listen(listenfd, 10) == -1){
        cout<<"listen error\n";
    }

    char buffer[524289];


    connfd = accept(listenfd, (struct sockaddr*)NULL ,NULL);
    memset(buffer, 0, 524289);
    read( connfd , buffer, 524289);
    string sock = buffer;
    masterSlaveConnfd = connfd;
    sock = trim(sock);
    string temp = tracker2IP+"|"+tracker2Port;
    if(sock==temp){
        cout<<"slave tracker is up\nHello slave!!!\n";
        isNotifyTracker = true;
        sendSeederListToSlave(connfd,seederFile);
    } 

}


void connectToOtherTracker(string tracker1IP,string tracker1Port,string tracker2IP,string tracker2Port,string seederFile){

    int sockfd = 0;
    int bytesReceived = 0;
    char recvBuff[524289];
    memset(recvBuff, '0', sizeof(recvBuff));
    struct sockaddr_in serv_addr;

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0))< 0){

        printf("\n Error : Could not create socket \n");
        
    }

    serv_addr.sin_family = AF_INET;

    int port = stoi (tracker2Port,nullptr,10) + PORT_OFFSET;

    serv_addr.sin_port = htons(port); // port
    serv_addr.sin_addr.s_addr = inet_addr(tracker2IP.c_str());

    
    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0)
    {
        printf("Could not connect to other tracker\n");
        cout<<"I am the master\n";
        isMasterTracker = true;

        thread waitingForSlave(waitingForSlaveToConnect,tracker1IP,tracker1Port,tracker2IP, tracker2Port,seederFile);
        waitingForSlave.detach();
        
    }else{
        masterSlaveConnfd = sockfd;
        cout<<"I am a slave\n";
        isMasterTracker = false;
        isNotifyTracker = true;
        string msgToMaster = tracker1IP+"|"+tracker1Port;

        char toMaster[msgToMaster.length()+1];
        strcpy(toMaster,(char*)msgToMaster.c_str());

        send(masterSlaveConnfd,toMaster,strlen(toMaster),0);
        
        thread receiveList(receiveSeederListFromMaster ,masterSlaveConnfd,seederFile);;
        
        receiveList.detach();
    }
}


void updateSeederList(string fromClient,string seederFile){
   
    cout<<fromClient<<"\n";
    
    bool present = searchFileHash(fromClient,seederFile);
    if(present)
        return;


    char file[seederFile.length()+1];
    strcpy(file,(char *)seederFile.c_str());
    
    FILE *seedFilefp = fopen(file,"a");
    char buffer[fromClient.length()+1];
    strcpy(buffer,(char *)fromClient.c_str());
    
    fputs (buffer,seedFilefp);
    fprintf(seedFilefp, "\n");
    fclose(seedFilefp);
}

bool searchFileHash(string fromClient,string seederFile){

    char file[seederFile.length()+1];
    strcpy(file,(char *)seederFile.c_str());

    ifstream seedFilefp;

    seedFilefp.open (seederFile, ifstream::in);


    string fromFile,temp;

    while(getline(seedFilefp,fromFile)){
        if(fromFile.compare(fromClient)==0){
            return true;
        }
    }

    return false;
}

string searchHash(string fromClient,string seederFile){

    char file[seederFile.length()+1];
    strcpy(file,(char *)seederFile.c_str());

    ifstream seedFilefp;

    seedFilefp.open (seederFile, ifstream::in);


    string fromFile,temp,temp2;

    temp = fromClient.substr(fromClient.find_last_of("|")+1);
    

    while(getline(seedFilefp,fromFile)){

        size_t found = fromFile.find(temp);
        if (found!=std::string::npos){
            temp2 = fromFile.substr(fromFile.find_first_of("|")+1);
            if(temp2!=fromClient){
                return fromFile;
            }
        }
    }

    return "";
}

void removeClient(string fromClient,string seederFile){
    
    char file[seederFile.length()+1];
    strcpy(file,(char *)seederFile.c_str());

    string line;

    ifstream fin;
    fin.open(file);
    ofstream temp;
    temp.open("temp.txt");
    
    while (getline(fin,line)){
  
        size_t found = line.find(fromClient);
  
        if(found==string::npos){ 
            temp << line << endl;
        }
    }

    temp.close();
    fin.close();
    remove(file);
    rename("temp.txt",file);    
}


void removeFromSeederList(string fromClient,string seederFile){
    
    bool present = searchFileHash(fromClient,seederFile);
    if(!present)
        return;

    char file[seederFile.length()+1];
    strcpy(file,(char *)seederFile.c_str());

    string line;

    ifstream fin;
    fin.open(file);
    ofstream temp;
    temp.open("temp.txt");
    
    while (getline(fin,line))
    {
        cout<<line<<"\n";

        if(line!=fromClient)
        temp << line << endl;

    }

    temp.close();
    fin.close();
    remove(file);
    rename("temp.txt",file);
}



int main(int argc, char *argv[]){

    string seperator = ":";

    if(argc<5){
        printf("Invalid number of argument\n");
        exit(0);
    }

    string temp,tracker1IP,tracker1Port,tracker2IP,tracker2Port,seederFile,logFile;

    temp = argv[1];

    size_t found = temp.find(seperator);
    
    if (found!=string::npos){
        tracker1IP = temp.substr(0,found);
        tracker1Port = temp.substr(found+1);
    }

    temp = argv[2];

    found = temp.find(seperator);

    if (found!=string::npos){
        tracker2IP = temp.substr(0,found);
        tracker2Port = temp.substr(found+1);
    }

    seederFile = argv[3];

    logFile = argv[4];

    int listenfd = 0;
    int connfd = 0;
    struct sockaddr_in serv_addr;
    char sendBuff[524289];
    
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(tracker1IP.c_str());

    int port = stoi (tracker1Port,nullptr,10);

    serv_addr.sin_port = htons(port);

    bind(listenfd, (struct sockaddr*)&serv_addr,sizeof(serv_addr));

    if(listen(listenfd, 10) == -1)
    {
        printf("Failed to listen\n");
        return -1;
    }


    connectToOtherTracker(tracker1IP,tracker1Port,tracker2IP,tracker2Port,seederFile);
    if(!isMasterTracker){

        while(receiveSeederListFromMaster(masterSlaveConnfd,seederFile));
    }
    

    char buffer[524288];
    while(1){

        connfd = accept(listenfd, (struct sockaddr*)NULL ,NULL);
        int n;
        memset(buffer, 0, 524288);
        n = read( connfd , buffer, 524288); 
        
        string fromClient = buffer;
        

        found = fromClient.find_first_of("|");

        if (found!=string::npos){
            temp = fromClient.substr(0,found);
            fromClient = fromClient.substr(found+1);
        }

        string clientAddress;
       
        if(temp.compare("fetch")==0){
            clientAddress = searchHash(fromClient,seederFile);    
        }else if(temp.compare("share")==0){
            updateSeederList(fromClient,seederFile);
        }else if(temp.compare("remove")==0){
            removeFromSeederList(fromClient,seederFile);
        }else if(temp.compare("close")==0){
            removeClient(fromClient,seederFile);
        }
        
        if(isNotifyTracker)
                sendSeederListToSlave(masterSlaveConnfd,seederFile);

        if(clientAddress.length()==0)
            cout<<"";
        else{
            strcpy(buffer,(char *)clientAddress.c_str());
            write(connfd, buffer, 524288);    
            memset(buffer, 0, 524288);
        }

        close(connfd);
        
    }


    return 0;
}