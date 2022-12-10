public class ClientAsServer {
    
    void* asServer(void *)
    {
        int cserver_fd, new_sock; 
        

        int addrlen = sizeof(struct sockaddr_in);
        
        if ((cserver_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
        { 
            perror("socket failed"); 
            exit(EXIT_FAILURE); 
        } 
        
        struct sockaddr_in address; 

        address.sin_family = AF_INET; 
        address.sin_addr.s_addr = inet_addr("127.0.0.1"); 
        int x; 
        sscanf(currentPort.c_str(), "%d", &x);
        //cout<<"port "<<currentPort<<endl; 
        address.sin_port = htons(x); 
        

        if (bind(cserver_fd, (struct sockaddr *)&address, sizeof(address))<0) 
        { 
            perror("bind failed"); 
            exit(EXIT_FAILURE); 
        } 
        
        if (listen(cserver_fd, 3) < 0) 
        { 
            perror("listen"); 
            exit(EXIT_FAILURE); 
        } 
        
        pthread_t th;

        while(1)	 
        {

            cout<<"waiting\n";

            new_sock = accept(cserver_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
            int whichone;
            recv(new_sock,&whichone,sizeof(whichone),0);

            if(whichone==10)
            {
                if(pthread_create(&th,NULL,bitmapRequest,(void *)&new_sock)!= 0)
                {
                    cout<<"error in pthread_create\n";
                    exit(EXIT_FAILURE);
                }		
            }
            else if(whichone==20)
            {
                if(pthread_create(&th,NULL,seeder,(void *)&new_sock)!= 0)
                {
                    cout<<"error in pthread_create\n";
                    exit(EXIT_FAILURE);
                }	
            }
            else if(whichone==30)
            {
                if(pthread_create(&th,NULL,sendChunk,(void *)&new_sock)!= 0)
                {
                    cout<<"error in pthread_create\n";
                    exit(EXIT_FAILURE);
                }	
            }
        }
    }

    void* sendChunk(void *t)
    {
        int sock=*(int *)t;

        int chunkno;
        long int offset;
        char filename[30];

        recv(sock,&chunkno,sizeof(chunkno),0);
        recv(sock,&offset,sizeof(offset),0);
        recv(sock,filename,30,0);


        cout<<"FILENAME "<<filename<<endl;
        fstream f;
        f.open(filename,ios::in|ios::out);
        
        if(!f)
        {
            cout<<"File not opened\n";

        }

        char *buffer=new char[(int)CHUNKSIZE];
        f.seekg(offset,f.beg);
        f.read(buffer,CHUNKSIZE);
        int sentdata=f.gcount();

        if(sentdata < CHUNKSIZE)
        {
            //cout<<chunkno<<" SENTLESS\n";
            send(sock,buffer,sentdata,0);
        }//cout<<buffer<<endl;
        else
        {
            //cout<<chunkno<<" SENTFULL\n";
            send(sock,buffer,sentdata,0);
        }

        f.close();
        
        /*int sent=0;
        int ite=CHUNKSIZE/100;
        for(int i=0;i<ite;i++)
        {
            send(sock,buffer,100,0);
            sent++;
        }*/
        //cout<<"sentbytes "<<sent<<endl;
        //cout<<"chunk-"<<chunkno<<" sent\n";

        memset(buffer,'\0',CHUNKSIZE);
    }
}