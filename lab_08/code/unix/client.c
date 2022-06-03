#include "socket.h"

void fill_message_client(char buffer[BUFFER_SIZE], int argc, char *argv[])
{
    snprintf(buffer, BUFFER_SIZE, "Message from process %d; ", getpid());
    if (argc >= 2){
        strncat(buffer, argv[1], BUFFER_SIZE - strlen(buffer));
    }
}

int main(int argc, char *argv[])
{
    int client_socket_fd;
    int error;
    struct sockaddr_un un_addr;
    char buffer[BUFFER_SIZE];

    client_socket_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    printf("client_socket_fd = %d\n", client_socket_fd);
    if (client_socket_fd == -1){
        perror("Ошибка: не удалось получить дескриптор сокета.\n");
        return -1;
    }

    un_addr.sun_family = AF_UNIX;
    strncpy(un_addr.sun_path, SOCKET_NAME, sizeof(un_addr.sun_path) - 1);

    fill_message_client(buffer, argc, argv);

    if (connect(client_socket_fd, (const struct sockaddr *) &un_addr, sizeof(un_addr)) == -1){
        perror("Ошибка: не удалось подключиться к серверу.");
        return -1;
    }

    if (send(client_socket_fd, buffer, strlen(buffer), 0) == -1){
        perror("Ошибка: не удалось отправить сообщение.");
        return -1;
    }

    close(client_socket_fd);
    return 0;
}