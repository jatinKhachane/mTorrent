public class Download {
    
    void* downfromclient(void *t)
    {
            //pthread_mutex_lock(&cs); 

        struct node *temp=(struct node *)t;

        string cid=temp->cid;
        string port=temp->port;
        string chunkno=temp->chunkno;
        string filename=temp->filename;

        while(clientFlag[cid] == 1);

        clientFlag[cid]=1;

        //FILE *fp=fopen("/home/jatin/output","w");

        int chunksize=(long int)CHUNKSIZE;
        long int offset=stoi(chunkno)*chunksize;

        //cout<<chunkno<<" "<<offset<<endl;

        //connect to port

        int tempsock=0;
        
        if ((tempsock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
        { 
            printf("\n Socket creation error \n"); 
            pthread_exit(NULL); 
        } 

        struct sockaddr_in serv_addr; 

        serv_addr.sin_family = AF_INET; 
        serv_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
        int x; 
        sscanf(port.c_str(), "%d", &x);
        serv_addr.sin_port = htons(x); 

        if (connect(tempsock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
        { 
            cout<<strerror(errno)<<endl;	
            printf("\nConnection Failed \n"); 
            pthread_exit(NULL); 
        } 

        //send type of function
        //send chunk no
        //send offset
        //send filename


        //cout<<"CID="<<cid<<endl;
        int type=30;
        int chno=stoi(chunkno);

        char *file=stringtocharptr(filename);

        send(tempsock,&type,sizeof(type),0);			//typeoffunc	
        send(tempsock,&chno,sizeof(chno),0);			//chunk number
        send(tempsock,&offset,sizeof(offset),0);		//offset of that chunk
        send(tempsock,file,30,0);						//filename

        char *buffer=new char[(int)CHUNKSIZE];

        int rdata=recv(tempsock,buffer,CHUNKSIZE,0);


        /*int rec=0;
        int ite=CHUNKSIZE/100;
        for(int i=0;i<ite;i++)
        {
            recv(tempsock,buffer,100,0);
            rec++;
        }
    */
        //cout<<"received bytes "<<rec<<endl;
        //cout<<"Bytes read from "<<cid<<" for chunk-"<<chunkno<<" "<<rec<<endl;

        fstream f;
        f.open("ofile",ios::in|ios::out|ios::binary);
        
        if(!f)
        {
            cout<<"File not opened\n";

        }

        f.seekg(offset,f.beg);

        if(rdata<CHUNKSIZE)
        {
            //cout<<chunkno<<" "<<"RECVLESS\n";
            f.write(buffer,rdata);
        }
        else
        {
            //cout<<chunkno<<" "<<"RECVFULL\n";

            f.seekg(offset,f.beg);
            f.write(buffer,CHUNKSIZE);
        }
            //pthread_mutex_unlock(&cs); 
        f.close();

        memset(buffer,'\0',CHUNKSIZE);

        close(tempsock);

        clientFlag[cid]=0;
    }


    void download(int sock)
    {
        //sock1 is tracker port
        char *filename=stringtocharptr(input[1]);
        char *grpid=stringtocharptr(currentgrpid);

        //send tracker request and get the ports if clients
        int whichfunc=7;
        send(sock,&whichfunc,sizeof(whichfunc),0);
        send(sock,filename,100,0);
        send(sock,grpid,10,0);

        char clientports[100];
        char clientids[100];

        recv(sock,clientports,100,0);
        recv(sock,clientids,100,0);

        cout<<clientids<<endl;
        cout<<clientports<<endl;
        

        vector<string> requiredClientPorts;
        vector<string> requiredClientids;

        string cltpts=clientports;

        stringstream ss(cltpts);

        string temp;
        int clientcount=0;
        while(ss>>temp)
        {
            requiredClientPorts.push_back(temp);
            clientcount++;
        }

        string cltids=clientids;
        stringstream ss1(cltids);

        temp="";

        while(ss1>>temp)
        {
            requiredClientids.push_back(temp);
        }


        close(sock);


        ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
        double filesize=in.tellg(); 


        int chunks=ceil(filesize/CHUNKSIZE);

        in.close();

        map<string,string>	BITMAPS;

        //get BITMAPS from clients
        for(int i=0;i<clientcount;i++)
        {
            int tempsock=0;
            
            if ((tempsock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
            { 
                printf("\n Socket creation error \n"); 
                pthread_exit(NULL); 
            } 

            struct sockaddr_in serv_addr; 

            serv_addr.sin_family = AF_INET; 
            serv_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
            int x; 
            sscanf(requiredClientPorts[i].c_str(), "%d", &x);
            serv_addr.sin_port = htons(x); 


            if (connect(tempsock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
            { 
                cout<<strerror(errno)<<endl;	
                printf("\nConnection Failed \n"); 
                pthread_exit(NULL); 
            } 

            char buf[100];
            int whichone=10;
            send(tempsock,&whichone,sizeof(whichone),0);       //sendinf TypeOf funct
            //send(tempsock,requiredClientids[i].c_str(),30,0);       //sending ID of client
            send(tempsock,filename,100,0);


            recv(tempsock,buf,100,0);
            string tempbuf=buf;
            
            //chunks = tempbuf.length();

            BITMAPS[requiredClientids[i]]=tempbuf;
            
            close(tempsock);
        }

        for(auto i=BITMAPS.begin();i!=BITMAPS.end();i++)
        {
            cout<<i->first<<"-"<<i->second<<endl;
        }
        
        //APPLY ALGORITHM

        map<string,string> chunktoclient;
        map<string,string> chunktoport;

        for(int i=0;i<chunks;i++)
        {
            char bit=0;
            int whichClient;
            do
            {
                whichClient = rand()%clientcount;
                string cid = requiredClientids[whichClient];
                string bmap=BITMAPS[cid];
                bit=bmap[i];
            }while(bit == '0');
                
            chunktoclient[to_string(i)]=requiredClientids[whichClient];
            chunktoport[to_string(i)]=requiredClientPorts[whichClient];

            //	cout<<"Choice for chunk "<<i<<"-"<<chunktoclient[i]<<endl;
        }


        for(int i=0;i<chunks;i++)
        {
            clientFlag[chunktoclient[to_string(i)]] = 0;
        }	

        
        struct node structarray[chunks];

        for(int i=0;i<chunks;i++)
        {
            string s=to_string(i);
            struct node temp;
            temp.chunkno=s;
            temp.cid=chunktoclient[s];
            temp.port=chunktoport[s];
            temp.filename=filename;
            structarray[i]=temp;
        }

        for(int i=0;i<chunks;i++)
        {
            struct node t=structarray[i];
            cout<<"chunkno "<<t.chunkno<<" ";
            cout<<"cid "<<t.cid<<" ";
            cout<<"port "<<t.port<<endl;
            cout<<"filename "<<t.filename<<endl;
        }


        pthread_t threadArray[chunks];

        for(int i=0;i<chunks;i++)
        {
            pthread_create(&threadArray[i],NULL,downfromclient,&(structarray[i]));

            void *status;
            pthread_join(threadArray[i],&status);
        }	
        
        cout<<"recdata "<<recdata<<endl;
    }
}