#include <sys/types.h> 
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define SERV_SOCKET "my_socket.soc"
#define SOCKET "soccl.soc"
#define MSG_SIZE 256

int main(void)
{
    struct sockaddr serv_addr, addr;
    char msg[MSG_SIZE];
    int namelen;
	struct sockaddr rcvr_name;

	int socket_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (socket_fd < 0){
		perror("Ошибка: не удалось получить дескриптор сокета.\n");
        return -1;
	}
	
    serv_addr.sa_family = AF_UNIX,
	strcpy(serv_addr.sa_data, SERV_SOCKET);

	addr.sa_family = AF_UNIX,
	strcpy(addr.sa_data, SOCKET);
	
	if (bind(socket_fd, (const struct sockaddr *)&addr, sizeof(addr)) < 0){
		perror("Ошибка: не удалось подключиться к серверу.");
        return -1;
	}

    snprintf(msg, MSG_SIZE, "процесс %d; ", getpid());
	
	sendto(socket_fd, &msg, strlen(msg), 0, (struct sockaddr *)&serv_addr, strlen(serv_addr.sa_data) + sizeof(serv_addr.sa_family));
	int count_bytes = recvfrom(socket_fd, msg, MSG_SIZE - 1, 0, (struct sockaddr *)&rcvr_name, &namelen);

    if (count_bytes){
        printf("sa_family = %d, sa_data = %s; ", rcvr_name.sa_family, rcvr_name.sa_data);
        msg[count_bytes] = 0;
        printf("Получено сообщение от сервера: %s\n", msg);
    }
    else{
        printf("Сообщение от сервера не получено.\n");
    }
	
	close(socket_fd);
    unlink(SOCKET);
	
	return 0;
}
