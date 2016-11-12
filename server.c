#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <fcntl.h>

//#define SETNONBLOCK
#define DOSEND

#define MAX_SEND_SIZE 1024
#define MAX_RECV_SIZE 1024

int serverSocket = -1;
int clientSocket = -1;
struct sockaddr_in serverAddr;
socklen_t serverAddrLength;
char sendBuf[MAX_RECV_SIZE];
char recvBuf[MAX_RECV_SIZE];

int main()
{
	int ret = -1;
	serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(serverSocket == -1) {
		printf("Create socket error --> errno is %d !\n", errno);
	}

#ifdef SETNONBLOCK
	ret = fcntl(serverSocket, F_SETFL, O_NONBLOCK);
	if(ret < 0) {
		printf("Set socket nonblock error --> errno is %d !\n", errno);
	}
#endif

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(8888);

	serverAddrLength = sizeof(struct sockaddr_in);

	ret = bind(serverSocket, (const struct sockaddr *)&serverAddr, serverAddrLength);
	if(ret != 0) {
		printf("Bind socket error --> errno is %d !\n", errno);
		return -1;
	}

	ret = listen(serverSocket, 3);
	if(ret != 0) {
		printf("Listen socket error --> errno is %d !\n", errno);
		return -1;
	}

	while(1) {
		struct sockaddr_in clientAddr;
		socklen_t clientAddrLength;

		printf("Now do accept !\n");
		clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLength);
		if(clientSocket == -1) {
			printf("Accept error --> errno is %d !\n", errno);
			usleep(1000000);
			continue;
		} else {
			printf("Accept a client: ip:%s port %d !\n", inet_ntoa(clientAddr.sin_addr), clientAddrLength);
		}

		int sendIndex = 0;
		while(1) {
			int sendLength = 0;
			int recvLength = 0;

#ifdef DOSEND
			printf("Now do send !\n");
			sprintf(sendBuf, "Data from Server --> index: %d\n", sendIndex);
			sendLength = send(clientSocket, sendBuf, MAX_SEND_SIZE, MSG_WAITALL); 
			if(sendLength == -1) {
				printf("Send error --> errno is %d !\n", errno);
			} else {
				sendIndex++;
			}
#else
			recvLength = recv(clientSocket, recvBuf, MAX_RECV_SIZE, MSG_WAITALL);
			if(recvLength == 0) {
				printf("Connection lost !\n");
			} else if (recvLength == -1) {
				printf("Receive error --> errno is %d !\n", errno);
			} else if (recvLength > 0) {
				printf("Receive data: %s\n", recvBuf);
			}
#endif

			usleep(1000000);
		}
	}

	close(serverSocket);
	return 0;
}
