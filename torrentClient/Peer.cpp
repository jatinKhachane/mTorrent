// Client side C/C++ program to demonstrate Socket programming 
#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 
#include <iostream>
#include <vector>
#include <bits/stdc++.h>
#include<errno.h>
#include<string.h>
#include <openssl/sha.h>
#include "Util.h"
#include "TorrentClient_asServer.h"
#include "Download.h"
//added by other user

#define PORT 9375

//new changes in main branch
//again new changes in main brach
using namespace std;

float CHUNKSIZE=65532;
vector<string> input;
vector<vector<string>> myFilesBitmap;
map<string,int> clientFlag;

//int sentdata;
int recdata;


struct node
{
	string cid;
	string port;
	string chunkno;
	string filename;
};



string currentUserID="";
string currentPort="";
string currentlyActive="";
string currentgrpid="";

pthread_mutex_t cs; 
 
void upload(int sock)
{
	cout<<"in upload\n";
	char *file_name=stringtocharptr(input[1]);
	//string sha=calculateSHA(file_name);
	string sha=" ";
	char *cid=stringtocharptr(currentUserID);
	char *gid=stringtocharptr(currentgrpid);
	char *port=stringtocharptr(currentPort);


	//find BITMAP size


	ifstream in(file_name, std::ifstream::ate | std::ifstream::binary);
    double filesize=in.tellg(); 

    cout<<"FILESIZE "<<filesize<<endl;

	int noc=ceil(filesize/CHUNKSIZE);

	cout<<"NOC "<<noc<<endl;
	//cout<<filesize<<" "<<noc<<endl;	

	in.close();

	string BITMAP="";

	for(int i=0;i<noc;i++)
	{
		BITMAP+="1";
	}

	vector<string> temp;
	temp.push_back(string(file_name));
	temp.push_back(BITMAP);

	myFilesBitmap.push_back(temp);

	printVofV(myFilesBitmap);

	//send request to tracker to get clientports

	int whichfunc=6;

	send(sock,&whichfunc,sizeof(whichfunc),0);

	send(sock,file_name,100,0);
	send(sock,sha.c_str(),20,0);
	send(sock,cid,20,0);
	send(sock,gid,20,0);
	send(sock,port,20,0);

	//char ports[100];				//PORTS to connect
	//recv(sock,ports,100,0);

	close(sock);


}

void *bitmapRequest(void* sock)
{
	int tsock=*((int *)sock);
	char CID[30];
	char FN[100];

	//recv(tsock,CID,30,0);
	recv(tsock,FN,100,0);

	//string cid=CID;
	string fn=FN;
	string bmp="";
	//buff == > client ID
	//return its BITMAP

	for(auto i=myFilesBitmap.begin();i!=myFilesBitmap.end();i++)
	{
		if((*i)[0] == fn)
		{
			bmp=(*i)[1];
		}
	}

	char *btmp=stringtocharptr(bmp);
	send(tsock,btmp,100,0);

	return NULL;
}

void* seeder(void *socket)
{
	int sock=*(int *)socket;

	int p;
	recv(sock,&p,sizeof(p),0);
	cout<<"P "<<p<<endl;

	int t=100;
	send(sock,&t,sizeof(t),0);
}

int main(int argc, char const *argv[]) 
{ 
	pthread_t t;	

	
	while(1)
	{
		int sock=0;
		//CREATING SOCKET
		if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
		{ 
			printf("\n Socket creation error \n"); 
			pthread_exit(NULL); 
		} 

		struct sockaddr_in serv_addr; 

		serv_addr.sin_family = AF_INET; 
		serv_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
		serv_addr.sin_port = htons(PORT); 
		


		if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
		{ 
			cout<<strerror(errno)<<endl;	
			printf("\nConnection Failed \n"); 
			pthread_exit(NULL); 
		} 

		//cout<<"sentdata "<<sentdata<<endl;
		cout<<"again\n";
		
		string command="";

		getline(cin,command);

		stringstream ss(command);

		input.clear();

		string temp="";
		while(ss>>temp)
		{
			input.push_back(temp);
		}
		
		if(input[0] == "c")				// 1
		{
			createreq(sock);
		}	

		else if(input[0] == "l")      	// 2
		{
			if(currentlyActive=="YES")
				cout<<"Already Logged In\n";
			else
			{
				loginreq(sock);
				pthread_create(&t,NULL,asServer,NULL);
			}
		}	
		
		else if(input[0] == "cg")       // 3
		{
			if(currentlyActive!="YES")
				cout<<"Login First\n";
			else
				creategroupreq(sock);
		}
		else if(input[0] == "jg")  		// 4
		{
			if(currentlyActive!="YES")
				cout<<"Login First\n";
			else
				joingroupreq(sock);
		}
		else if(input[0] == "listall")	// 5
		{
			if(currentlyActive!="YES")
				cout<<"Login First\n";
			else
				listgroupsre(sock);
		}
		else if(input[0] == "up")		//6
		{
			if(currentlyActive!="YES")
				cout<<"Login First\n";
			else
				upload(sock);
		}
		else if(input[0] == "down")		//7
		{
			if(currentlyActive!="YES")
				cout<<"Login First\n";
			else
				download(sock);
		}
		else if(input[0] == "logout")		//8
		{
			if(currentlyActive!="YES")
				cout<<"Login First\n";
			else
				logout(sock);
		}
	}



	void *status;
	pthread_join(t,&status);
	
	return 0; 
} 
	