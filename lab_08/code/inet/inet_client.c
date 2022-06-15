#include <time.h>
#include <signal.h>
#include "inet_socket.h"

static int client_socket_fd;

void unlink_socket_addr(int signal_number)
{
    close(client_socket_fd);
    exit(0);
}

int main(void)
{
    struct sockaddr_in client_addr;
    char buffer[BUFFER_SIZE];
    size_t count_bytes;

    signal(SIGINT, unlink_socket_addr);
    client_socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (client_socket_fd == -1){
        perror("Ошибка: не удалось получить дескриптор сокета.");
        return -1;
    }

    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = INADDR_ANY;
    client_addr.sin_port = htons(SOCKET_PORT);

    if (connect(client_socket_fd, (const struct sockaddr *) &client_addr, sizeof(client_addr)) == -1){
        perror("Ошибка: не удалось подключиться к серверу.");
        return -1;
    }

    for (;;)
    {
        snprintf(buffer, BUFFER_SIZE, "сообщение от процесса %d", getpid());
        if (send(client_socket_fd, buffer, strlen(buffer), 0) == -1){
            perror("Ошибка: не удалось отправить сообщение.");
            return -1;
        }
        fprintf(stdout, "Отправлено: %s", buffer);
        count_bytes = recv(client_socket_fd, &buffer, BUFFER_SIZE, 0);

        if (count_bytes){
            printf("Получено сообщение от сервера: %s\n", buffer);
        }
        else{
            printf("Сообщение от сервера не получено.\n");
        }

        sleep(3);
    }
}