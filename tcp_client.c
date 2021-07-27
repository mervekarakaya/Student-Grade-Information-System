#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#define SIZE sizeof(struct sockaddr_in)
int main() {
	int sockfd, nread;
	char buf[128], enter, resp;
    int number;
	fd_set fds;
	//char IP[20];
	struct sockaddr_in server = { AF_INET, 2000 };

	char IP[]="192.---.---.---";
	server.sin_addr.s_addr = inet_addr(IP);
    //opening socket
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		printf("Error creating SOCKET\n");
		return (0);
	}
    //connecting socket which have this address informations
	if (connect(sockfd, (struct sockaddr *) &server, SIZE) == -1) {
		printf("Connect failed\n");
		return (0);
	}
	printf("Connected to the server.\n");
	printf("Enter your ssn id:\n");
    printf("Enter your name and surname:\n");
    printf("Enter your birthday:\n");
	do {
		FD_ZERO(&fds);
		FD_SET(sockfd, &fds);
		FD_SET(0, &fds);
		//Wait for some input.
		select(sockfd + 1, &fds, (fd_set *) 0, (fd_set *) 0, (struct timeval *) 0);

		//If there is a reply from the server
		if (FD_ISSET(sockfd, &fds)) {
			nread = recv(sockfd,&number, sizeof(int), 0);
			//If error or eof, terminate.
			if (nread < 1) {
				//printf("Messsage from server not received correctly!\n");
				close(sockfd);
				exit(0);
			}
			else{
                printf("Congratulations! Your membership id is: %d.\n",number);
                close(sockfd);
				exit(0);
            }
		}
        //If there is an entry from keyboard
		if (FD_ISSET(0, &fds)) {
			nread = read(0, buf, sizeof(buf));
			//If error or eof, terminate.
			if (nread < 1) {
				printf("Messsage from stdin not readed!\n");
				close(sockfd);
				exit(0);
			} 
			else{//send informations
				send(sockfd, buf, nread, 0);
			}
		}
	} while (1);
}
