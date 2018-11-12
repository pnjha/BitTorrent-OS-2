/*
Prakash Nath Jha
2018201013
*/


#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <bits/stdc++.h>
#include <openssl/sha.h>
#include <thread> 
#include <pthread.h>
#include "mtorrent.h"


using namespace std;


struct socketInfo{

    string ip;
    string port;
};


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


string getSHA1FromMtorrentFile(string filename){
    
    char file[filename.length()+1];
    strcpy(file,filename.c_str());

    ifstream torrentFile(file);
    
    string line;

    for(int i=0;i<4;i++){
        getline(torrentFile,line);
    }

    getline(torrentFile,line);

    char data[line.length()+1];
    strcpy(data,line.c_str());
    char buf[SHA_DIGEST_LENGTH*2];
    unsigned char hash[SHA_DIGEST_LENGTH]={'\0'};
        
    SHA1((unsigned char*)data, strlen(data), hash);

    for (int i=0; i < SHA_DIGEST_LENGTH; i++) {
        sprintf((char*)&(buf[i*2]), "%02x", hash[i]);
    }

    string toTracker(buf);

    return toTracker;

}


string sendSeedFileInfoToTracker(string filename,string clientIP,string clientPort,string tracker1IP,string tracker1Port
                                ,string tracker2IP,string tracker2Port){

    string tempfilename = filename+".mtorrent";

    string toTracker = getSHA1FromMtorrentFile(tempfilename);

    toTracker = "share|"+filename+"|"+clientIP+"|"+clientPort+"|"+toTracker;

    char trackerInfo[toTracker.length()+1];
    strcpy(trackerInfo,toTracker.c_str());

    int sockfd = 0;
    struct sockaddr_in serv_addr;

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0))< 0){
            
            printf("\n Error : Could not create socket \n");
            //return 1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(tracker1IP.c_str());
    serv_addr.sin_port = htons(stoi (tracker1Port,nullptr,10));

    
    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0){

        printf("Failed to connect with tracker 1 \nAttempting to connect to tracker 2\n");
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = inet_addr(tracker2IP.c_str());
        serv_addr.sin_port = htons(stoi (tracker2Port,nullptr,10));        
        if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0){
            printf("Failed to connect with tracker 2 \nBoth trackers down\n");
            return "failure";
        }
    }

    
    send(sockfd,trackerInfo,strlen(trackerInfo),0);
    return "success";

}

string removeClientFormSeederList(string clientIP,string clientPort,string tracker1IP,string tracker1Port,string tracker2IP,string tracker2Port){
    
    string toTracker = "close|"+clientIP+"|"+clientPort;

    char trackerInfo[toTracker.length()+1];
    strcpy(trackerInfo,toTracker.c_str());

    int sockfd = 0;
    struct sockaddr_in serv_addr;

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0))< 0){
            
            printf("\n Error : Could not create socket \n");
            //return 1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(tracker1IP.c_str());
    serv_addr.sin_port = htons(stoi (tracker1Port,nullptr,10));

    
    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0){

        printf("Failed to connect with tracker 1 \nAttempting to connect to tracker 2\n");
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = inet_addr(tracker2IP.c_str());
        serv_addr.sin_port = htons(stoi (tracker2Port,nullptr,10));        
        if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0){
            printf("Failed to connect with tracker 2 \nBoth trackers down\n");
            return "failure";
        }
    }
    send(sockfd,trackerInfo,strlen(trackerInfo),0); 
    return "success";
}

string removeFromSeederList(string filename,string clientIP,string clientPort,string tracker1IP,string tracker1Port,
                                    string tracker2IP,string tracker2Port){


    string tempfilename = filename+".mtorrent";

    string toTracker = getSHA1FromMtorrentFile(tempfilename);

    toTracker = "remove|"+filename+"|"+clientIP+"|"+clientPort+"|"+toTracker;

    char trackerInfo[toTracker.length()+1];
    strcpy(trackerInfo,toTracker.c_str());

    int sockfd = 0;
    struct sockaddr_in serv_addr;

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0))< 0){
            
            printf("\n Error : Could not create socket \n");
            //return 1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(tracker1IP.c_str());
    serv_addr.sin_port = htons(stoi (tracker1Port,nullptr,10));

    
    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0){

        printf("Failed to connect with tracker 1 \nAttempting to connect to tracker 2\n");
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = inet_addr(tracker2IP.c_str());
        serv_addr.sin_port = htons(stoi (tracker2Port,nullptr,10));        
        if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0){
            printf("Failed to connect with tracker 2 \nBoth trackers down\n");
            return "failure";
        }
    }    
    send(sockfd,trackerInfo,strlen(trackerInfo),0);
    return "success";
}

string getFileInfoFromTracker(string filename,string clientIP,string clientPort,string tracker1IP,string tracker1Port,
                                string tracker2IP,string tracker2Port)
{

    filename = filename+".mtorrent";
    //cout<<filename<<"\n";
    string toTracker = getSHA1FromMtorrentFile(filename);

    toTracker = "fetch|"+clientIP+"|"+clientPort+"|"+toTracker;

    char trackerInfo[toTracker.length()+1];
    strcpy(trackerInfo,toTracker.c_str());

    int sockfd = 0;
    int bytesReceived = 0;
    char recvBuff[524289];
    memset(recvBuff, '0', sizeof(recvBuff));
    struct sockaddr_in serv_addr;

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0))< 0){
            
            printf("\n Error : Could not create socket \n");
            //return 1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(tracker1IP.c_str());
    serv_addr.sin_port = htons(stoi (tracker1Port,nullptr,10));

    
    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0)
    {
        cout<<"Tracker 1 down\n";
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = inet_addr(tracker2IP.c_str());
        serv_addr.sin_port = htons(stoi (tracker2Port,nullptr,10));

        if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0){
            cout<<"Tracker 2 is down\n";
            return "error";
        }
    }

    string fromTracker;
    send(sockfd,trackerInfo,strlen(trackerInfo),0);

    while((bytesReceived = read(sockfd, recvBuff, 524288)) > 0){
            recvBuff[524288] = '\0';
            //printf("Bytes received %d\n",bytesReceived);    
            string temp = recvBuff;
            fromTracker += temp;
    }


    return fromTracker;

}

void connectAndDownloadFile(char file[],string downloadIP,string downloadPort){

    int sockfd = 0;
    int bytesReceived = 0;
    char recvBuff[524289];
    memset(recvBuff, '0', sizeof(recvBuff));
    struct sockaddr_in serv_addr;

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0))< 0){

        printf("\n Error : Could not create socket \n");
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(downloadIP.c_str());
    serv_addr.sin_port = htons(stoi (downloadPort,nullptr,10));

    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0){

        printf("\nError : Connect Failed \n");
        
    }

    //send(sockfd,file,strlen(file),0);

    FILE *fp = fopen(file, "ab"); 
    if(NULL == fp){

        printf("Error opening file");
        
    }
    int c = 1;
    while((bytesReceived = read(sockfd, recvBuff, 524288)) > 0){
        
        fwrite(recvBuff, 1,bytesReceived,fp);
    }
    
    fclose(fp);
    close(sockfd);
}


void fireClientThread(string filename,string sourceIP,string sourcePort,string tracker1IP,string tracker1Port,
                                string tracker2IP,string tracker2Port,string clientIP,string clientPort){

    int sockfd = 0;
    int bytesReceived = 0;
    char recvBuff[524289];
    memset(recvBuff, '0', sizeof(recvBuff));
    struct sockaddr_in serv_addr;

    char file[filename.length()+1];
    strcpy(file,filename.c_str());


    if((sockfd = socket(AF_INET, SOCK_STREAM, 0))< 0){

        printf("\n Error : Could not create socket \n");
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(sourceIP.c_str());
    serv_addr.sin_port = htons(stoi (sourcePort,nullptr,10));

    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0){

        printf("\n Error : Connect Failed \n");
        
    }

    
    send(sockfd,file,strlen(file),0);

    memset(recvBuff, '0', sizeof(recvBuff));

    read(sockfd, recvBuff, 524289);

    string fromServer = recvBuff;

    memset(recvBuff, '0', sizeof(recvBuff));

    string downloadIP = fromServer.substr(0,fromServer.find("|"));
    
    string downloadPort = fromServer.substr(fromServer.find("|")+1);

    close(sockfd);

    string temp = sendSeedFileInfoToTracker(filename,clientIP,clientPort,tracker1IP,tracker1Port,tracker2IP,tracker2Port);

    connectAndDownloadFile(file,downloadIP,downloadPort);

}


void fireServerThread(int counter,char fileToSend[],string uploadIP,string uploadPort){

    int listenfd = 0;
    int connfd = 0;
    struct sockaddr_in serv_addr;
    char sendBuff[524289];
    int numrv;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(uploadIP.c_str());
    serv_addr.sin_port = htons(stoi (uploadPort,nullptr,10));

    bind(listenfd, (struct sockaddr*)&serv_addr,sizeof(serv_addr));



    if(listen(listenfd, 10) == -1)
    {
        printf("Failed to listen\n");
        
    }

    char buffer[524289];

    connfd = accept(listenfd, (struct sockaddr*)NULL ,NULL);
    
    memset(buffer, 0, 524289);


    FILE *fp = fopen(fileToSend,"rb");
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

    close(connfd);
    fclose(fp);
}


void fireMainServerThread(struct socketInfo MY_SOCKET){

    vector<thread> serverThreads;

    int counter=0;

    while(true){

        int listenfd = 0;
        int connfd = 0;
        struct sockaddr_in serv_addr;
        char sendBuff[524289];
        int numrv;

        listenfd = socket(AF_INET, SOCK_STREAM, 0);

        memset(&serv_addr, '0', sizeof(serv_addr));
        memset(sendBuff, '0', sizeof(sendBuff));

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = inet_addr((MY_SOCKET.ip).c_str());
        serv_addr.sin_port = htons(stoi (MY_SOCKET.port,nullptr,10));

        bind(listenfd, (struct sockaddr*)&serv_addr,sizeof(serv_addr));

        if(listen(listenfd, 10) == -1){

            printf("Failed to listen\n");
            
        }

        char buffer[524289];
        
        while(true){

            connfd = accept(listenfd, (struct sockaddr*)NULL ,NULL);

            counter++;
            
            memset(buffer, 0, 524289);
            
            read( connfd , buffer, 524289); 

            string totrim = buffer;

            totrim = trim(totrim);

            char fileToSend[524829];
            
            strcpy(fileToSend,(char*)totrim.c_str());

            int uploadPort = stoi(MY_SOCKET.port,nullptr,10);

            uploadPort += counter;

            string toClient = MY_SOCKET.ip+"|"+to_string(uploadPort);

            toClient = trim(toClient);

            memset(buffer, 0, 524289);

            strcpy(buffer,(char *)toClient.c_str());

            buffer[toClient.length()+1] = '\0';

            write(connfd, buffer, 524289);

            string port = to_string(uploadPort);

            fireServerThread(counter,fileToSend,MY_SOCKET.ip,port);

            thread sThread(fireServerThread,counter,fileToSend,MY_SOCKET.ip,port);

            sThread.detach();
        }
    }
}


int main(int argc, char *argv[]){

    struct socketInfo MY_SOCKET;

    string seperator = ":";

    if(argc<5){
        printf("Invalid number of argument\n");
        exit(0);
    }

    string temp,tracker1IP,tracker1Port,tracker2IP,tracker2Port,logFile;
    
    temp = argv[1];
    
    size_t found = temp.find(seperator);

    if (found!=string::npos){
        MY_SOCKET.ip = temp.substr(0,found);
        MY_SOCKET.port = temp.substr(found+1);
    }
    
    temp = argv[2];
    
    found = temp.find(seperator);
    
    if (found!=string::npos){
        tracker1IP = temp.substr(0,found);
        tracker1Port = temp.substr(found+1);
    }

    temp = argv[3];

    found = temp.find(seperator);
    
    if (found!=string::npos){
        tracker2IP = temp.substr(0,found);
        tracker2Port = temp.substr(found+1);
    }

    logFile = argv[4];    

    thread serverThread(fireMainServerThread,MY_SOCKET);

    serverThread.detach();
    
    while(true){
        string command,temp;
        cout<<"Enter Your Command: ";
        getline(cin,command);   
        
        if(command.substr(0,5)=="share"){

            string filename = command.substr(6);
            //cout<<filename<<"\n";
            thread shareWithTracker(createMtorrentFile,filename,tracker1IP,tracker1Port,tracker2IP,tracker2Port);

            shareWithTracker.detach();
        
            string temp = sendSeedFileInfoToTracker(filename,MY_SOCKET.ip,MY_SOCKET.port,tracker1IP,tracker1Port,tracker2IP,tracker2Port);
            if(temp=="failure")
                continue;

        }else if(command.substr(0,3)=="get"){

            string filename = command.substr(3);

            filename = trim(filename);
            tracker1IP = trim(tracker1IP);
            tracker1Port = trim(tracker1Port);

            string fromTracker = getFileInfoFromTracker(filename,MY_SOCKET.ip,MY_SOCKET.port,tracker1IP,tracker1Port,tracker2IP,tracker2Port);  

            if(fromTracker=="error"){
                cout<<"Sorry we cannot handle your request\n";
                continue;
            }

            size_t found = fromTracker.find_first_of("|");
            fromTracker = fromTracker.substr(found+1);
            found = fromTracker.find_first_of("|");
            string sourceIP = fromTracker.substr(0,found);

            fromTracker = fromTracker.substr(found+1);

            found = fromTracker.find_first_of("|");
            string sourcePort = fromTracker.substr(0,found);

            
            thread clientThread(fireClientThread, filename,sourceIP,sourcePort,tracker1IP,tracker1Port,tracker2IP,tracker2Port
                ,MY_SOCKET.ip,MY_SOCKET.port);
            clientThread.detach();

        }else if(command.substr(0,6)=="remove"){

            string filename = command.substr(6);
            filename = trim(filename);
            //cout<<filename<<"\n";
            filename = trim(filename);
            found = filename.find_last_of(".");
            filename = filename.substr(0,found);
            //cout<<filename<<"\n";
            filename = trim(filename);
            tracker1IP = trim(tracker1IP);
            tracker1Port = trim(tracker1Port);
            tracker2IP = trim(tracker2IP);
            tracker2Port = trim(tracker2Port);
            MY_SOCKET.ip = trim(MY_SOCKET.ip);
            MY_SOCKET.port = trim(MY_SOCKET.port);
        
            string temp = removeFromSeederList(filename,MY_SOCKET.ip,MY_SOCKET.port,tracker1IP,tracker1Port,tracker2IP,tracker2Port);
            if(temp=="failure")
                continue;

        }else if(command=="close"){
            string temp = removeClientFormSeederList(MY_SOCKET.ip,MY_SOCKET.port,tracker1IP,tracker1Port,tracker2IP,tracker2Port);  
            if(temp=="failure")
                continue;
        }
    }
    
    
    
    return 0;
}