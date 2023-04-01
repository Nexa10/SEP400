// client1.cpp - A client that communicates with a second client using triple RSA encrpytion/decryption
#include <arpa/inet.h>
#include <iostream>
#include <math.h>
#include <net/if.h>
#include <netinet/in.h>
#include <queue>
#include <signal.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

using namespace std;

const char IP_ADDR[]="127.0.0.1";
const int BUF_LEN=256;
bool is_running = true;
int srcPort=1153;
int destPort=1155;
// Encrpytion/Decryption variables
double n;
double e;
double d;
double phi;

queue<unsigned char*> messageQueue;

pthread_mutex_t lock_x = PTHREAD_MUTEX_INITIALIZER;

void *recv_func(void *arg);

static void shutdownHandler(int sig)
{
    switch(sig) {
        case SIGINT:
            is_running=false;
            break;
    }
}

struct RSA{
    	double d, e,  n;
    	//int fd; //file descriptor
};

char decrypt(double c, struct RSA rsa);
double encrypt(char m, struct RSA rsa);

// Returns a^b mod c
unsigned char PowerMod(int a, int b, int c)
{
    int res = 1;
    for(int i=0; i<b; ++i) {
        res=fmod(res*a, c);
    }
    return (unsigned char)res;
}
  
// Returns gcd of a and b
int gcd(int a, int h)
{
    int temp;
    while (1)
    {
        temp = a%h;
        if (temp == 0)
          return h;
        a = h;
        h = temp;
    }
}
  
// Code to demonstrate RSA algorithm
int main()
{
    // Two random prime numbers
    double p = 11;
    double q = 23;
  
    // First part of public key:
    n = p*q;
  
    // Finding other part of public key.
    // e stands for encrypt
    e = 2;
    phi = (p-1)*(q-1);
    while (e < phi)
    {
        // e must be co-prime to phi and
        // smaller than phi.
        if (gcd((int)e, (int)phi)==1)
            break;
        else
            e++;
  
    }
    // Private key (d stands for decrypt)
    // choosing d such that it satisfies
    // d*e = 1 + k * totient
    int k = 2;  // A constant value
    d = (1 + (k*phi))/e;
    cout<<"p:"<<p<<" q:"<<q<<" n:"<<n<<" phi:"<<phi<<" e:"<<e<<" d:"<<d<<endl;

    const int numMessages=5;
    const unsigned char messages[numMessages][BUF_LEN]={
	    "House? You were lucky to have a house!",
	    "We used to live in one room, all hundred and twenty-six of us, no furniture.",
	    "Half the floor was missing;",
	    "we were all huddled together in one corner for fear of falling.",
            "Quit"};

    signal(SIGINT, shutdownHandler);

   int fd;
    pthread_t tid;
    struct sockaddr_in addr;
    struct sockaddr_in cli_addr;
    socklen_t s_len = sizeof(cli_addr);
    memset(&addr, 0, sizeof(addr));
    memset(&cli_addr, 0, sizeof(cli_addr));


    fd = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, 0);
    if(fd < 0){
    	perror("Socket()");
    	exit(-1);
    }
  
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, IP_ADDR, &addr.sin_addr);
    addr.sin_port = htons(destPort);

 
    
    if ((bind(fd, (struct sockaddr*) &addr, sizeof(addr))) < 0) {
    	perror("Bind()");
    	close(fd);
    	exit(-1);
    }
    
    if((connect(fd, (struct sockaddr *)&addr, sizeof(addr)))< 0){
    		perror("Connect()");
    		exit(-1);
    	}
    
    struct RSA rsa;
    double *sendBuf;
    sendBuf = new double[256];
    rsa.d = d; rsa.e = e; rsa.n = n; 
    pthread_create(&tid, NULL, recv_func, &rsa);
    
    cout << "created" << endl;
    int count = 0;
    sleep(5);
    while(is_running){
    	//encrypt
	
	for(int i = count; i < numMessages; i++){
		int j = 0;
		
		while(j < sizeof(messages[i]) && messages[i][j] != NULL){	
			cout << messages[i][j] << endl;
			sendBuf[j] = encrypt(messages[i][j], rsa);
			cout << sendBuf[j] << endl;
			j++;
		}
		count++;
		if(count == numMessages) count = 0;
		break;
	}
	
    	cout << "insert" << endl;
    	
    	for(int i = 0; i < 256 && sendBuf[i] != NULL; i++){
    		cout << sendBuf[i];
    	}
    	
    	cout << endl;
    	
    	if(sendto(fd, (double *)&sendBuf, sizeof(sendBuf), 0, (struct sockaddr *)&cli_addr, s_len) < 0){
    		perror("Send()");
    		exit(-1);
    	}
    	
    	cout << "sent" << endl;
    	
    	pthread_mutex_lock(&lock_x);
    	while(!messageQueue.empty()){
		cout << messageQueue.front() <<endl;
		messageQueue.pop();
    	}
  
	pthread_mutex_unlock(&lock_x);
	
	sleep(1);
    }
    delete [] sendBuf;
    close(fd);
    pthread_join(tid, NULL);
    return 0;
}

char decrypt(double c, struct RSA rsa){
	char decrypt_msg;
	int msg = static_cast<int>(pow(c, rsa.d)) % static_cast<int>(rsa.n); // m = C^d mod(n)
	decrypt_msg = static_cast<char>(msg + '0');
	return decrypt_msg;
}

double encrypt(char m, struct RSA rsa){
	double encrypt_msg;
	int cipher_text = static_cast<int>(pow(m, rsa.e)) % static_cast<int>(rsa.n); // c = M^e mod(n)
	encrypt_msg = cipher_text;
	return encrypt_msg;
}

//TODO: Complete the receive thread
void *recv_func(void *arg){
	cout << "recv thread" << endl;
	struct RSA rsa = *(struct RSA*) arg;
	double *msgBuf = new double[2024];
	unsigned char *buf = new unsigned char[2024];
	int len, fd;
	
	
	struct sockaddr_in addr;
    struct sockaddr_in cli_addr;
    socklen_t s_len = sizeof(cli_addr);
    memset(&addr, 0, sizeof(addr));
    memset(&cli_addr, 0, sizeof(cli_addr));
    	
	fd = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, 0);
	if(fd < 0){
		perror("Socket()");
		exit(-1);
	}

	addr.sin_family = AF_INET;
    inet_pton(AF_INET, IP_ADDR, &addr.sin_addr);
	addr.sin_port = htons(srcPort);

	if ((bind(fd, (struct sockaddr*) &addr, sizeof(addr))) < 0) {
		perror("Bind()");
		close(fd);
		exit(-1);
	}
    
   	cout << "recv loop" << endl;
   	while(is_running){
   		
   		bzero(&buf, sizeof(buf));
   		cout << "before recv from" << endl;
   		int arrSize = sizeof(msgBuf)/sizeof(msgBuf[0]);
   		len = recvfrom(fd, (double *)msgBuf, sizeof(msgBuf), 0, (struct sockaddr *)&cli_addr, &s_len);
   		cout << "recv from" << len <<  msgBuf[0] << endl;
   		if(len < 0){
   		cout << "sleep" << endl;
	    		sleep(1);
    		}
    		
    		//Decrypt
    		if(len > 0){
    		cout << "len: " << len << " data: "<<msgBuf[0] << endl;
    			int j = 0;
			while(j < len && msgBuf[j] != NULL){
				buf[j] = decrypt(msgBuf[j], rsa);
				j++; 
			}
			pthread_mutex_lock(&lock_x);
			messageQueue.push(buf);
			pthread_mutex_unlock(&lock_x);
			delete []msgBuf;
    		}
    			
   	}
   	
   	pthread_exit(NULL);  	
}


