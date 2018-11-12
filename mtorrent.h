/*
Prakash Nath Jha
2018201013
*/


#ifndef MTORRENT_H
#define MTORRENT_H

#include <openssl/sha.h>
#include <bits/stdc++.h>

using namespace std;


void createMtorrentFile(string filename,string tracker1IP,string tracker1Port,string tracker2IP,string tracker2Port){

    string fileSize;
    string torrentName = filename+".mtorrent";
    char buffer[524289];
    strcpy(buffer,(char*)torrentName.c_str());

    FILE *fpd = fopen(buffer,"w");

    if(fpd==NULL){
        printf("File open error");
    }

    memset(buffer,0,524289);

    string socket = tracker1IP+":"+tracker1Port;

    strcpy(buffer,(char*)socket.c_str());

    buffer[socket.length()+1] = '\n';

   
    fprintf(fpd, "%s\n",buffer);

    memset(buffer,0,524289);

    socket = tracker2IP+":"+tracker2Port;

    strcpy(buffer,(char*)socket.c_str());
    //printf("%s\n", buffer);
    buffer[socket.length()+1] = '\n';

    fputs (buffer,fpd);
    fprintf(fpd, "\n");
    memset(buffer,0,524289);


    strcpy(buffer,(char*)filename.c_str());
    //printf("%s\n", buffer);
    buffer[filename.length()+1] = '\n';

    fputs (buffer,fpd);
    fprintf(fpd, "\n");


    memset(buffer,0,524289);
    

    char file[filename.length()+1];
    strcpy(file,filename.c_str());

    FILE *fp = fopen(file,"rb");
    if(fp==NULL){

        printf("File open error");
    }   

    fseek ( fp ,0, SEEK_END);
    long long int size = ftell(fp);
    fseek(fp,0,SEEK_SET);

    fileSize = to_string(size);
    strcpy(buffer,(char*)fileSize.c_str());
    buffer[fileSize.length()+1] = '\n';

    fputs (buffer,fpd);
    fprintf(fpd, "\n");

    char data[524289]={0};
    int count=1;
    while(fgets (data , 524288 , fp)){
        fseek ( fp , 524288 , SEEK_CUR );   

        char buf[21];
        
        size_t length = 524288;
        
        unsigned char hash[SHA_DIGEST_LENGTH]={'\0'};
        
        SHA1((unsigned char*)data, length, hash);

        for (int i=0; i < 10; i++) {
            sprintf((char*)&(buf[i*2]), "%02x", hash[i]);
        }   

        fputs (buf,fpd);
        memset(data,0,524289);

    }
    fclose(fpd);
    fclose(fp);
}
#endif

