/*
 ============================================================================
 Name        : nonatc.c
 Author      : icehong@gmil.com
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#define MINGW32

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef MINGW32
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#endif

#define MAXLINE 1024

char* response[] = {
		"220 ",
		"331 ",
		"230 ",
		"250 "
};
char* cmd[] = {
		"USER anonymous\r\n",
		"PASS anonymous\r\n",
		"CWD /pub\r\n",
		"PORT 192,168,3,17,100,0\r\n"
};
int res_index = 0;
int cmd_index = 0;

int main(int argc, char **argv) {
	int sockfd, n;
	char receline[MAXLINE + 1];
	struct sockaddr_in serveraddr;

	//输入参数太少，退出
	if (argc != 2) {
		printf("Usage :%s  IP_address\n", argv[0]);
		exit(0);
	}

#ifdef MINGW32
	setbuf(stdout, NULL);
	//Winsows下启用socket
	WSADATA wsadata;
	if (WSAStartup(MAKEWORD(1, 1), &wsadata) == SOCKET_ERROR) {
		printf("WSAStartup() fail\n");
		exit(0);
	}
#endif

	//建立socket
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		printf("socket() fail\n");
		exit(0);
	}

	//设置协议及Port
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(21);

	//设置IP
	serveraddr.sin_addr.s_addr = inet_addr(argv[1]);

	//连接
	if (connect(sockfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr))
			== -1) {
		printf("connect() fail\n");
		exit(0);
	}

	//读取数据并输入到标准输出
#ifdef MINGW32
	while ((n = recv(sockfd, receline, MAXLINE, 0)) > 0)
#else
	while((n=read(sockfd,receline,MAXLINE))>0)
#endif
	{
		receline[n] = 0;
		if (fputs(receline, stdout) == EOF) {
			printf("fputs() error\r\n");
		}

		if (strstr(receline,response[res_index]) > 0 ) {
			res_index++;
#ifdef MINGW32
			send(sockfd, cmd[cmd_index], strlen(cmd[cmd_index]), 0);
#else
			write(sockfd,buff,strlen(buff));
#endif
			fputs(cmd[cmd_index], stdout);
			cmd_index++;
		}
	}

	//没有获取数据
	if (n < 0) {
		printf("read() fail\n");
	}

#ifdef MINGW32
	//Winsows下关闭socket
	closesocket(sockfd);
	WSACleanup();
#else
	close(sockfd);
#endif
	exit(0);
}
