public class Util{
    char* stringtocharptr(string a)
    {
        int length=a.length()+1;
        int i=0;
        char *ptr=(char*)malloc(sizeof(char)*length);
        for(i=0;i<a.length();i++)
        {
            ptr[i]=a[i];
        }ptr[i]='\0';
        return ptr;
    }

    void printVofV(vector<vector<string>> temp)
    {
        for(auto i=temp.begin();i!=temp.end();i++)
        {
            for(string x:(*i))
                cout<<x<<" ";
            cout<<endl;
        }
    }

    //###################################################################################################
    string calculateSHA(char *filename)
    {

        FILE *fp=fopen(filename,"r");


        unsigned char *hash=new unsigned char [20];
        
        unsigned char finalhash[524288];
        size_t readbytes;

        float readsize=512*1024;
        int pos=0;


        ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
        double filesize=in.tellg(); 

        int numberOfChunks=ceil(filesize/readsize);
        
        int temp=ceil(filesize);

        unsigned char buffer[temp];

        unsigned char SHA[20];

        
        SHA_CTX ctx;
        SHA1_Init(&ctx);

        readbytes=fread(buffer,1,filesize,fp);
        
        cout<<readbytes<<endl;
        buffer[readbytes]='\0';

        SHA1((unsigned char*)buffer,readbytes,hash);

        string hsh="";
        int x;
        stringstream stream;
        string result="";
        for(int i=0;i<20;i++)
        {
            x=(int)hash[i];
            stream << std::hex <<x;
        }
        result=stream.str();
            
        return result;


    }

    void createreq(int sock)
    {
        //int sock=*(int *)sck;
        int whichfunc=1;
        char* username=stringtocharptr(input[1]);
        char* password=stringtocharptr(input[2]);
        char* port=stringtocharptr(input[3]);
                
        send(sock,&whichfunc,sizeof(whichfunc),0);		
        send(sock,username,20,0);
        cout<<strerror(errno);
        send(sock,password,20,0);
        cout<<strerror(errno);
        send(sock,port,20,0);
        cout<<strerror(errno);

    }

    void loginreq(int sock)
    {
        int whichfunc=2;
        char* username=stringtocharptr(input[1]);
        char* password=stringtocharptr(input[2]);
                
        send(sock,&whichfunc,sizeof(whichfunc),0);		
        send(sock,username,20,0);
        send(sock,password,20,0);

        char buf[5];
        char prt[5];
        char grpid[5];

        recv(sock,buf,5,0);
        recv(sock,prt,5,0);
        recv(sock,grpid,5,0);

        if(strcmp(buf,"YES") == 0)
        {
            currentUserID=username;
            currentPort=prt;
            currentgrpid=grpid;
            cout<<"Login successful\n";
        }
        else
            cout<<"Wrong ID or Pass\n";	

        currentlyActive=buf;
        cout<<"R u Active Now: "<<currentlyActive<<endl;
    }

    void creategroupreq(int sock)
    {
        int whichfunc=3;
        char* gid=stringtocharptr(input[1]);
        char* uid=stringtocharptr(currentUserID);
        send(sock,&whichfunc,sizeof(whichfunc),0);		
        send(sock,uid,20,0);
        send(sock,gid,10,0);

        char g[10];
        recv(sock,g,10,0);

        currentgrpid=gid;
        cout<<"CGRPID "<<currentgrpid<<endl;

    }

    void joingroupreq(int sock)
    {
        int whichfunc=4;
        char* gid=stringtocharptr(input[1]);
        char* uid=stringtocharptr(currentUserID);
        send(sock,&whichfunc,sizeof(whichfunc),0);		
        send(sock,uid,20,0);
        send(sock,gid,10,0);

        char ownerID[10];
        recv(sock,ownerID,10,0);

        cout<<ownerID<<endl;

        string oid=ownerID;

    }
    void listgroupsre(int sock)
    {
        char buff[1024];

        int whichfunc=5;

        send(sock,&whichfunc,sizeof(whichfunc),0);

        recv(sock,buff,1000,0);

        string grps=buff;

        stringstream ss(grps);

        string temp;

        cout<<"GROUPS\n";
        while(ss>>temp)
        {
            cout<<temp<<endl;
        }

    }

    void logout(int sock)
    {
        const char *userid=currentUserID.c_str();
        int whichfunc=8;
        send(sock,&whichfunc,sizeof(whichfunc),0);
        send(sock,userid,20,0);

        currentUserID="";
        currentPort="";
        currentlyActive="NO";
        currentgrpid="";

        cout<<"Logged out\n";
        int x; 
        sscanf(currentPort.c_str(), "%d", &x);
        close(x);	

    }
}