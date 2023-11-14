#include <cstdio>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>

int main() {
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serv_addr{};
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(9800);
    int res = bind(socket_fd, (sockaddr*)&serv_addr, sizeof(serv_addr));
    printf("%d\n", res);
    listen(socket_fd, SOMAXCONN);

    struct sockaddr_in client_addr{};
    socklen_t client_addr_len = sizeof(client_addr);
    bzero(&client_addr, sizeof(client_addr));
    int client_socket_fd = accept(socket_fd, (sockaddr*)&client_addr, &client_addr_len);
    printf("new client fd %d! IP: %s Port: %d\n", client_socket_fd, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    return 0;
}
