#include <stdio.h>
#include <stdlib.h>
#include <string.h>	// for bzero()
#include <unistd.h>	// for close()
#include <sys/types.h>
#include <winsock.h>
#include <windows.h>
#include <errno.h>

int main(int argc, char *argv[])
{
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;
    //WSAStartup
    wVersionRequested = MAKEWORD(2, 2);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        printf("WSAStartup failed with error: %d\n", err);
        return 1;
    }


	struct sockaddr_in serverAddress, clientAddress;
	int server_addr_length = sizeof(serverAddress);
    int client_addr_length = sizeof(clientAddress);
    int serverSocket, clientSocket;
	int ServerPortNumber;

	if(argc == 2){
		ServerPortNumber = atoi(argv[1]);
	}

   	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(serverSocket < 0){
		fprintf(stderr, "Error creating socket : %s\n", strerror(errno));
		exit(0);
	}

 	bzero(&serverAddress, server_addr_length);
	serverAddress.sin_family = PF_INET;
  	serverAddress.sin_port = htons(ServerPortNumber);
	serverAddress.sin_addr.s_addr = INADDR_ANY;

	if(bind(serverSocket,(struct sockaddr *) &serverAddress, server_addr_length) == -1){
		fprintf(stderr, "Error binding : %s\n", strerror(errno));
		close(serverSocket);
		exit(0);
	}

	if(listen(serverSocket, 1) == -1){
		fprintf(stderr, "Error listening : %s\n", strerror(errno));
		close(serverSocket);
		exit(0);
	}

	printf("Waiting...\n");
	if((clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &client_addr_length)) == -1){
		printf("accept failed\n");
		close(serverSocket);
		exit(0);
	}
	printf("Client connect successfully\n");

	int i;
	int bytesRecv, bytesSend;
	int itemID, itemNumber;
	int itemcount[3] = {0};
    char send_buf[500];
	char recv_buf[500];
	char str[100];
	char messege[60][100];
	int messege_idx = 0;
    char *menu = "\
\n---Menu---\n\
1. Read all existing messages.\n\
2. Write a new message.\n\
Please type \"1\" or \"2\" to select an option:\0";

    char *type_a_new_messege = "\
\nType a new message:\0";

    char *new_messege_send = "\
\nNew messege sent.\n\0";
    char *new_line = "\n\0";
    char *messege_are_below = "\nAll messeges:\n\0";


	// Send menu to client
	send_buf[0] = '\0';
	strcat(send_buf, menu);
	bytesSend = send(clientSocket, send_buf, sizeof(send_buf), 0);
	if(bytesSend < 0) printf("Error sending packet\n");


  	while(1){
		bytesRecv = recv(clientSocket, recv_buf, sizeof(recv_buf), 0);
		if(bytesRecv < 0) printf("Error receiving packet\n");

		printf("%s\n", recv_buf);

		if(!strncmp(recv_buf, "1", 1)){
			send_buf[0] = '\0';
			strcat(send_buf, messege_are_below);
			for(int i=0;i<messege_idx;i++){
                strcat(send_buf, messege[i]);
			}
			strcat(send_buf, menu);
			bytesSend = send(clientSocket, send_buf, sizeof(send_buf), 0);
			if(bytesSend < 0) printf("Error sending packet\n");
		}
		else if(!strncmp(recv_buf, "2", 1)){
			send_buf[0] = '\0';
			strcat(send_buf, type_a_new_messege);
			bytesSend = send(clientSocket, send_buf, sizeof(send_buf), 0);
			if(bytesSend < 0) printf("Error sending packet\n");

			bytesRecv = recv(clientSocket, recv_buf, sizeof(recv_buf), 0);
			if(bytesRecv < 0) printf("Error receiving packet\n");
			recv_buf[sizeof(recv_buf)] = '\0';
			strcat(recv_buf, new_line);
			strncpy(messege[messege_idx++], recv_buf, sizeof(recv_buf));

			send_buf[0] = '\0';
			strcat(send_buf, new_messege_send);
			strcat(send_buf, menu);
			bytesSend = send(clientSocket, send_buf, sizeof(send_buf), 0);
			if(bytesSend < 0) printf("Error sending packet\n");
		}

		else{
			bytesSend = send(clientSocket, menu, strlen(menu), 0);
			if(bytesSend < 0) printf("Error sending packet\n");
		}


	}
    WSACleanup();
	return 0;
}


