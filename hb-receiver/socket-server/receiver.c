#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <semaphore.h>

#define LOCALIP "10.0.0.12"
#define MSGSIZE 8
//#define LOCAL
#define REMOTE

static long timeout_intvl_ms = 0;
static bool expired = true;
static int local_client_sock;

static pthread_mutex_t lock;
static sem_t sem;

long now() {   
    struct timespec spec;    
    clock_gettime(CLOCK_REALTIME, &spec);
    return spec.tv_sec * 1000 + spec.tv_nsec/1.0e6;
}

long next_timeout(long now_ms) {   
    return now_ms + timeout_intvl_ms;
}

struct timespec sleep_time(long sleep_t) {
    struct timespec sleep_ts;
    sleep_ts.tv_sec = sleep_t / 1000;
    sleep_ts.tv_nsec = (sleep_t % 1000) * 1.0e6;
    return sleep_ts;
}

// thread for checking expiration
void *expirator(void *arg) {
    int sock = *(int *) arg;
    printf("[expirator] expirator thread created, local client sock = %d\n", sock);
    long timeout_t;
    struct timespec sleep_ts;
    timeout_t = next_timeout(now());
    char *buf = malloc(MSGSIZE);
    
    while(1) {
        while(now() < timeout_t) {
            sleep_ts = sleep_time(timeout_t - now());
            nanosleep(&sleep_ts, NULL);
        }
        timeout_t = next_timeout(now());
       
#ifdef LOCAL
        // send a packet to self
        int ret = send(sock, buf, MSGSIZE, 0);
        if(ret != MSGSIZE) {
            printf("[expirator] send incorrectly, ret = %d\n", ret);
            break;
        }
        // wait for semaphore
        printf("[expirator] wait for sem post by receiver\n");
        sem_wait(&sem); 
#endif
#ifdef REMOTE
        printf("[expirator] send a packet to remote send-self\n");
#endif

        // do expiration check
        //printf("[expirator] it's time to check expiration\n");
        pthread_mutex_lock(&lock);
        if(expired == true) {
            printf("[expirator] expired!\n");
        } else {
            expired = true;
        }
        pthread_mutex_unlock(&lock);
    }
}

// thread for receiving heartbeat
void *receiver(void *arg) {
    char *buf = malloc(MSGSIZE);
    int connfd = *(int *) arg;
    while(1) {
	int ret = recv(connfd, buf, MSGSIZE, 0);
	if(ret <= 0) {
	    close(connfd);
	    break;
	}
	if(ret != MSGSIZE) printf("warning recv ret=%d\n", ret);
        
        if(connfd != local_client_sock) { // packets from remote hosts
            //printf("[receiver] receievd remote heartbeat from sock %d\n", connfd);
            pthread_mutex_lock(&lock);
            expired = false;
            pthread_mutex_unlock(&lock);
        }
#ifdef LOCAL
        else { // packets from localhost
            printf("[receiver] received local heartbeat from sock %d and wake up expirator thread\n", connfd);
            sem_post(&sem);
        }
#endif
#ifdef  REMOTE
        printf("[receiver] received remote send-self\n");
#endif
    } 
}

int main(int argc, char *argv[]) {
    if(argc != 2) {
        printf("./receiver [timeout ms]\n");
	return 1;
    }
    timeout_intvl_ms = atoi(argv[1]);
    printf("timeout = %ld ms\n", timeout_intvl_ms);
    sem_init(&sem, 0, 0); 
    
    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr; 

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    //serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_addr.s_addr = inet_addr(LOCALIP);
    serv_addr.sin_port = htons(5001); 
    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 
    listen(listenfd, 10); 

    pthread_t expirator_tid;
    int sock = 0;
#ifdef LOCAL
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("connect failed. Error");
        return 1;
    } else {
        printf("[main] local send-self sock connected\n");
    }
#endif
#ifdef REMOTE
    printf("[main] remote send-self sock connected\n");
#endif
    // create expirator thread
    pthread_create(&expirator_tid, NULL, expirator, &sock);

    while(1) {
        struct sockaddr_in client;
        unsigned int client_len = sizeof(client);
        connfd = accept(listenfd, (struct sockaddr*) &client, &client_len);
        printf("[main] client connection accepted, sock = %d, ip = %s\n", connfd, inet_ntoa(client.sin_addr));
        // determine which is the local client sockfd
        if(strcmp(inet_ntoa(client.sin_addr), LOCALIP) == 0) {
            local_client_sock = connfd;
        }
	int *tmp = malloc(sizeof(int));
	*tmp = connfd;
	pthread_t receiver_tid;
	pthread_create(&receiver_tid, NULL, receiver, tmp);
    }
}
