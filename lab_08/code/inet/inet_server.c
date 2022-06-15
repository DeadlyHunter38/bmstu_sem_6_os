#include <sys/select.h>
#include <signal.h>
#include "inet_socket.h"

#define MAX_AMOUNT_CLIENTS 10

static int server_socket_fd;

void unlink_socket_addr(int signal_number)
{
    close(server_socket_fd);
    exit(0);
}

int main(void)
{
    signal(SIGINT, unlink_socket_addr);
    char buffer[BUFFER_SIZE];
    struct sockaddr_in server_addr;
    fd_set read_fd_set;
    int client_fd_sockets[MAX_AMOUNT_CLIENTS] = {0};
    int max_socket_fd, new_socket_fd, flag_insert;
    size_t count_bytes;

    server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket_fd == -1){
        perror("Ошибка: не удалось получить дескриптор сокета.");
        return -1;
    }

    max_socket_fd = server_socket_fd;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SOCKET_PORT);
    memset(server_addr.sin_zero, 0, sizeof(server_addr.sin_zero));

    if (bind(server_socket_fd, (const struct sockaddr *) &server_addr, sizeof(server_addr)) == -1){
        perror("Ошибка: не удалось назначить сокету локальный адрес.\n");
        close(server_socket_fd);
        return -1;
    }   

    if (listen(server_socket_fd, 1) == -1){
        perror("listen failed.");
        return -1;
    }

    printf("Сервер запущен и находится в пассивном ожидании соединения.\n");

    FD_ZERO(&read_fd_set);
    for (;;)
    {
        FD_SET(server_socket_fd, &read_fd_set);

        for (int i = 0; i < MAX_AMOUNT_CLIENTS; i++)
        {
            if (client_fd_sockets[i] > 0){
                FD_SET(client_fd_sockets[i], &read_fd_set);
                max_socket_fd = client_fd_sockets[i] > max_socket_fd ? client_fd_sockets[i] : max_socket_fd;
            }
        }

        if (pselect(max_socket_fd + 1, &read_fd_set, NULL, NULL, NULL, NULL) == -1){
            close(server_socket_fd);
            perror("Ошибка вызова pselect.");
            return -1;
        }

        if (FD_ISSET(server_socket_fd, &read_fd_set))
        {
            new_socket_fd = accept(server_socket_fd, NULL, NULL);
            if (new_socket_fd == -1){
                perror("Ошибка выполнения accept.");
                return -1;
            }

            //добавление нового клиента
            flag_insert = 1;
            for (int i = 0; i < MAX_AMOUNT_CLIENTS && flag_insert; i++){
                if (client_fd_sockets[i] == 0){
                    client_fd_sockets[i] = new_socket_fd;
                    printf("Добавлен клиент (номер %d)\n", i);
                    flag_insert = 0;
                }
            }
            if (flag_insert){
                printf("Заполнено максимальное количество клиентов.\n");
            }
        }

        for (int i = 0; i < MAX_AMOUNT_CLIENTS; i++)
        {
            if (client_fd_sockets[i] && FD_ISSET(client_fd_sockets[i], &read_fd_set))
            {
                count_bytes = recv(client_fd_sockets[i], &buffer, BUFFER_SIZE, 0);
                if (!count_bytes){
                    //соединение разорвано
                    printf("Соединение потеряно с клиентом (файловый дескриптор %d)\n", client_fd_sockets[i]);
                    FD_CLR(client_fd_sockets[i], &read_fd_set);
                    close(client_fd_sockets[i]);
                    client_fd_sockets[i] = 0;
                }
                else{
                    buffer[count_bytes] = 0;
                    printf("Получено сообщение от клиента (файловый дескриптор %d): %s\n", client_fd_sockets[i], buffer);
                    snprintf(buffer, BUFFER_SIZE, "Сообщение сервером получено.\n");
                    send(client_fd_sockets[i], buffer, strlen(buffer), 0);
                }
            }
        }
    }    
}