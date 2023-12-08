/* 
    TCPTest.cpp : Defines the entry point for the console application.
*/

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include "constants.h"

int main()
{
	char buff[BUFF_SIZE];
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, SERVER_ADDR, &(serverAddr.sin_addr.s_addr));

	int connSock;
	connSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	int tv = 2000;//Time-out interval: 10000ms
	setsockopt(connSock, SOL_SOCKET, SO_RCVTIMEO, (const char*)(&tv), sizeof(int));

	if (connect(connSock, (sockaddr *)&serverAddr, sizeof(serverAddr))) {
		perror("Error: ");
		close(connSock);
		return 0;
	}

	char sBuff[BUFF_SIZE], rBuff[BUFF_SIZE];
	int ret;

	// ret = recv(connSock, rBuff, BUFF_SIZE, 0);

	/******Test1***/
	strcpy(sBuff, "123");
	strcat(sBuff, ENDING_DELIMITER);
	strcat(sBuff, "a@b");
	strcat(sBuff, ENDING_DELIMITER);
	strcat(sBuff, "456");
	strcat(sBuff, ENDING_DELIMITER);

	std::cout << sBuff << std::endl;
	ret = send(connSock, sBuff, strlen(sBuff), 0);
	ret = recv(connSock, rBuff, BUFF_SIZE, 0);
	if (ret < 0)
		printf("Stream test fail!\n");
	else {
		rBuff[ret] = 0;
		printf("Main: %s-->%s\n", sBuff, rBuff);
	}

	ret = recv(connSock, rBuff, BUFF_SIZE, 0);
	if (ret < 0)
		printf("Stream test fail!\n");
	else {
		rBuff[ret] = 0;
		printf("Main: %s-->%s\n", sBuff, rBuff);
	}

	ret = recv(connSock, rBuff, BUFF_SIZE, 0);
	if (ret < 0)
		printf("Stream test fail!\n");
	else {
		rBuff[ret] = 0;
		printf("Main: %s-->%s\n", sBuff, rBuff);
	}

	/******Test2***/
	strcpy(sBuff, "123");
	ret = send(connSock, sBuff, strlen(sBuff), 0);
	strcpy(sBuff, ENDING_DELIMITER);
	ret = send(connSock, sBuff, strlen(sBuff), 0);
	ret = recv(connSock, rBuff, BUFF_SIZE, 0);
	if (ret < 0)
		printf("Stream test 2 fail!\n");
	else {
		rBuff[ret] = 0;
		printf("Main: %s-->%s\n", sBuff, rBuff);
	}
	close(connSock);
	return 0;
}

