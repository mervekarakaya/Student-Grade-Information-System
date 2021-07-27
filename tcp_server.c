#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define SIZE sizeof(struct sockaddr_in)
#define MAX 5//max number of clients that can be connected,optional
int client[MAX];
int ActiveClients = 0;//at the begining there are no clients
void findMax(int* maxfd) {
	int i;
	*maxfd = client[0];
	for (i = 1; i < MAX; i++)
		if (client[i] > * maxfd)
			*maxfd = client[i];
}
int main() {
	int sockfd, maxfd, nread, found, i, j;
	char buf[128],buf_ssdid[6],buf_name_surname[128],buf_birthday[128];
	fd_set fds;
	int flag_p=0,flag_u=0;
    int index;
	FILE *fin;
    //At least the number of records in the grade.txt file,the struct must have space available
    char ch[10];//for reading character from file
    int counter=0;
    int s_counter=0;//index counter for struct s
    int input_count=0;//checking the number of password and username informations entered from client
	struct sockaddr_in server = { AF_INET, 2000, INADDR_ANY };
	
    //opening socket
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		printf("Error creating SOCKET\n");
		return (0);
	}
	//binding socket to adresses
	if (bind(sockfd, (struct sockaddr*)&server, SIZE) == -1) {
		printf("bind failed\n");
		return (0);
	}
	//The number of connections that the server can hold in the queue is 5
	//optional
	if (listen(sockfd, 5) == -1) {//5 is equal to MAX
		printf("listen failed\n");
		return (0);
	}
	findMax(&maxfd);
	for (;;) {
		findMax(&maxfd);
		maxfd = (maxfd > sockfd ? maxfd : sockfd) + 1;
		FD_ZERO(&fds);
		FD_SET(sockfd, &fds);
		for (i = 0; i < MAX; i++){
			if (client[i] != 0)
				FD_SET(client[i], &fds);
		}
		//Wait for some input or connection request
		select(maxfd, &fds, (fd_set*)0, (fd_set*)0, (struct timeval*)0);
		//If one of the clients has some input, read and do the necessary actions to grade
		for (i = 0; i < MAX; i++){
			if (FD_ISSET(client[i], &fds)) {
				nread = recv(client[i], buf, sizeof(buf), 0);
				//If error or eof, terminate the connection
				if (nread < 1) {
					//printf("Message from client not received correctly!\n");
					close(client[i]);
					client[i] = 0;
					ActiveClients--;
				}
				else{//the message from client received correctly
                    input_count++;//the number of informations entries made by client
                    if(input_count==1){
                        strcpy(buf_ssdid,buf);
                    }
                    if(input_count==2){
                        strcpy(buf_name_surname,buf);
                    }
                    if(input_count==3){
                        strcpy(buf_birthday,buf);
                    }
                    printf("Received: %s %s %s\n",buf_ssdid,buf_name_surname,buf_birthday);
                    fin=fopen("info.txt","w+");
                    if(fin == NULL)
                    {
                    printf("Error file opening!\n");   
                    exit(1);             
                    }
                    int number=1501;
                    printf("Assigned %d to %s.\n",number,buf_ssdid);
                    fprintf(fin,"%d",number);
                    fprintf(fin,"%c",' ');
                    fputs(buf_ssdid,fin);
                    fprintf(fin,"%c",' ');
                    fputs(buf_name_surname,fin);
                    fprintf(fin,"%c",' ');
                    fputs(buf_birthday,fin);
                    fprintf(fin,"%c",'\n');
                    number++;
                    fclose(fin);
					for (j = 0; j < MAX; j++){
						if (client[j] != 0 && i == j){//send the reply to the same client
                                send(client[j],&number,sizeof(int),0);
                            }	
							
						}	
					}	
				}
			}
		}
		//if there is a request for a new connection
		if (FD_ISSET(sockfd, &fds)) {
			//If no of active clients is less than MAX accept the request
			if (ActiveClients < MAX) {
				found = 0;
				for (i = 0; i < MAX && !found; i++){
					if (client[i] == 0) {
						client[i] = accept(sockfd, NULL, NULL);
						found = 1;
						ActiveClients++;
					}
				}
			}
		}
	}
