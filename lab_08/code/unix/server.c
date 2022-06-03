#include <signal.h>
#include <stdlib.h>
#include "socket.h"

static int server_socket_fd;

void unlink_socket_addr(int signal_number)
{
    close(server_socket_fd);
    unlink(SOCKET_NAME);
    exit(0);
}

int main(void)
{
    signal(SIGINT, unlink_socket_addr);
    struct sockaddr_un server_un_addr;
    char recv_buffer[BUFFER_SIZE];
    int flag_stop = 0;

    server_socket_fd = socket(AF_UNIX, SOCK_DGRAM, 0);

    if (server_socket_fd == -1){
        perror("Ошибка: не удалось получить дескриптор сокета.\n");
        return -1;
    }

    server_un_addr.sun_family = AF_UNIX;
    strncpy(server_un_addr.sun_path, SOCKET_NAME, sizeof(server_un_addr.sun_path) - 1);
    if (bind(server_socket_fd, (const struct sockaddr *) &server_un_addr, sizeof(server_un_addr)) == -1){
        perror("Ошибка: не удалось назначить сокету локальный адрес.\n");
        
        close(server_socket_fd);
        return -1;
    }

    fprintf(stdout, "Сервер находится в режиме прослушивания. Для завершения нажмите Ctrl + C.\n");
    while(flag_stop == 0)
    {
        size_t count_bytes = recv(server_socket_fd, recv_buffer, sizeof(recv_buffer) - 1, 0);
        if (count_bytes == -1){
            perror("Ошибка: не удалось получить сообщение.\n");
            close(server_socket_fd);
            unlink(SOCKET_NAME);
            flag_stop = -1;
        }
        recv_buffer[count_bytes] = 0;
        printf("Server get message: %s\n", recv_buffer);
    }

    return flag_stop;
}