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
#define SERVER_IP "192.168.1.107"
//#define SERVER_IP "127.0.0.1"

int clientSocket = -1;
struct sockaddr_in serverAddr;
struct sockaddr_in clientAddr;
socklen_t serverAddrLength;
socklen_t clientAddrLength;
char sendBuf[MAX_RECV_SIZE];
char recvBuf[MAX_RECV_SIZE];

int main()
{
	int ret = -1;
	clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(clientSocket == -1) {
		printf("Create socket error --> errno is %d !\n", errno);
	}

#ifdef SETNONBLOCK
	ret = fcntl(clientSocket, F_SETFL, O_NONBLOCK);
	if(ret < 0) {
		printf("Set socket nonblock error --> errno is %d !\n", errno);
	}
#endif

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(8888);

	clientAddr.sin_family = AF_INET;
	clientAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	clientAddr.sin_port = htons(0);

	serverAddrLength = sizeof(struct sockaddr_in);
	clientAddrLength = sizeof(struct sockaddr_in);

	ret = bind(clientSocket, (struct sockaddr *)&clientAddr, clientAddrLength);
	if(ret != 0) {
		printf("Bind socket error --> errno is %d !\n", errno);
		return -1;
	}

	ret = inet_aton(SERVER_IP, &serverAddr.sin_addr);
	if(ret == 0) {
		printf("Server socket invalid !\n");
	}

	while(1) {
		printf("Now do connect !\n");
		ret = connect(clientSocket, (struct sockaddr *)&serverAddr, serverAddrLength);
		if(ret == -1) {
			printf("Connect socket error --> errno is %d !\n", errno);
			usleep(1000000);
			continue;
		} else {
			printf("Connect socket success !\n");
		}

		int sendIndex = 0;
		while(1) {
			int sendLength = 0;
			int recvLength = 0;

#ifdef DOSEND
			printf("Now do send !\n");
			sprintf(sendBuf, "Data from Client --> index: %d\n", sendIndex);
			sendLength = send(clientSocket, sendBuf, MAX_SEND_SIZE, MSG_WAITALL); 
			if(sendLength == -1) {
				printf("Send error --> errno is %d !\n", errno);
			} else {
				sendIndex++;
			}
#endif

			recvLength = recv(clientSocket, recvBuf, MAX_RECV_SIZE, MSG_WAITALL);
			if(recvLength == 0) {
				printf("Connection lost !\n");
			} else if (recvLength == -1) {
				printf("Receive error --> errno is %d !\n", errno);
			} else if (recvLength > 0) {
				printf("Receive data: %s\n", recvBuf);
			}

			usleep(1000000);
		}
	}

	close(clientSocket);
	return 0;
}
