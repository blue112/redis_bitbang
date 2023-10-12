#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include "hashtable.h"
#include "cli.h"

#define MAX_CLIENTS_CONNECTED 20
#define MAX_BUFFER_SIZE 1024

void onMessage(int client_fd, HashTable *ht, int client_n, char *message, int length)
{
    // Remove trailing '\n'
    message[length - 1] = '\0';
    printf("Message from client %d: '%s'\n", client_n, message);

    parse_msg(client_fd, ht, message);
}

int main(int argc, char const *argv[])
{
    HashTable *ht = ht_create(0xF);

    unsigned short port = 6380;
    struct sockaddr_in serveraddr;

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    int optval = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int));

    memset(&serveraddr, 0, sizeof(serveraddr));

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
    {
        printf("FATAL: Error while binding: %s\n", strerror(errno));
        return 1;
    }

    printf("Listening on 0.0.0.0:%d\n", port);

    listen(server_fd, 5);

    int connected_clients[MAX_CLIENTS_CONNECTED] = {0};

    fd_set read_fds;

    while (1)
    {
        FD_ZERO(&read_fds);
        FD_SET(server_fd, &read_fds);

        int max_socket_fd = server_fd;

        // Add child sockets to set
        for (int i = 0; i < MAX_CLIENTS_CONNECTED; i++)
        {
            int client_socket = connected_clients[i];

            if (client_socket > 0)
                FD_SET(client_socket, &read_fds);

            if (client_socket > max_socket_fd)
                max_socket_fd = client_socket;
        }

        pselect(max_socket_fd + 1, &read_fds, NULL, NULL, NULL, NULL);

        // If there is activity on the server socket, it's a new connection
        if (FD_ISSET(server_fd, &read_fds))
        {
            struct sockaddr_in clientaddr;
            socklen_t clientlen;
            int client_fd = accept(server_fd, (struct sockaddr *)&clientaddr, &clientlen);

            int i = 0;
            for (i = 0; i < MAX_CLIENTS_CONNECTED; i++)
            {
                if (connected_clients[i] == 0)
                {
                    connected_clients[i] = client_fd;
                    printf("New client connected (%d, fd=%d)\n", i, client_fd);
                    break;
                }
            }

            /*if (i == MAX_CLIENTS_CONNECTED)
            {
                printf("Max client connected reached, forcefully closing.\n");
                char *reject_msg = "Unable to connect: max connection reached.\n";
                write(client_fd, reject_msg, strlen(reject_msg));
                close(client_fd);
                continue;
            }*/
        }

        for (int i = 0; i < MAX_CLIENTS_CONNECTED; i++)
        {
            int client_socket = connected_clients[i];

            if (client_socket != 0 && FD_ISSET(client_socket, &read_fds))
            {
                char buffer[MAX_BUFFER_SIZE];
                int valread = read(client_socket, buffer, MAX_BUFFER_SIZE);
                if (!valread)
                {
                    // Client disconnected
                    printf("Client %d disconnected\n", i);
                    connected_clients[i] = 0;
                    close(client_socket);
                }
                else
                {
                    onMessage(client_socket, ht, i, buffer, valread);
                }
            }
        }
    }

    return 0;
}
