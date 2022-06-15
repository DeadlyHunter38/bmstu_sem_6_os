#include <sys/types.h> 
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET "my_socket.soc"
#define MSG_SIZE 256

char *ans = "OK";

static int sockfd;

void close_socket(int sig) 
{
    close(sockfd);
    unlink(SOCKET);
    exit(0);
}

int main(void)
{
    struct sockaddr addr, rcvr_name;
    char msg[MSG_SIZE];
    int namelen, count_bytes;

    signal(SIGINT, close_socket);

	sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (sockfd < 0){
		perror("Ошибка: не удалось получить дескриптор сокета.\n");
        return -1;
	}
	
	addr.sa_family = AF_UNIX;	
	strcpy(addr.sa_data, SOCKET);

	if (bind(sockfd, &addr, sizeof(addr)) < 0)
	{
		perror("Bind error");
		return -1;
	}
	
	namelen = sizeof(rcvr_name);
	fprintf(stdout, "Сервер находится в режиме прослушивания. Для завершения нажмите Ctrl + C.\n");
	for(;;)
	{
		count_bytes = recvfrom(sockfd, msg, MSG_SIZE - 1, 0, &rcvr_name, &namelen);
        printf("sa_family = %d, sa_data = %s; ", rcvr_name.sa_family, rcvr_name.sa_data);
		sendto(sockfd, ans, strlen(ans), 0, &rcvr_name, sizeof(rcvr_name));
		msg[count_bytes] = '\0';
        printf("Получено сообщение от клиента: %s\n", msg);
	}
	
	
	return 0;
}
