#include <cstdio>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include "util.h"

int main() {
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    errif(socket_fd == -1, "socket create error");

    struct sockaddr_in serv_addr{};
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(9800);

    errif(bind(socket_fd, (sockaddr*)&serv_addr, sizeof(serv_addr)) == -1, "socket bind error");
    errif(listen(socket_fd, SOMAXCONN) == -1, "socket listen error");

    struct sockaddr_in client_addr{};
    socklen_t client_addr_len = sizeof(client_addr);
    bzero(&client_addr, sizeof(client_addr));

    int client_socket_fd = accept(socket_fd, (sockaddr*)&client_addr, &client_addr_len);
    errif(client_socket_fd == -1, "socket accept error");

    printf("new client fd %d! IP: %s Port: %d\n", client_socket_fd, inet_ntoa(client_addr.sin_addr),
           ntohs(client_addr.sin_port));
    while (true) {
        char buf[1024];
        bzero(&buf, sizeof(buf));
        ssize_t read_bytes = read(client_socket_fd, buf, sizeof(buf));
        if (read_bytes > 0) {
            printf("message from client fd %d: %s\n", client_socket_fd, buf);
            write(client_socket_fd, buf, sizeof(buf));
        } else if (read_bytes == 0) {
            printf("client fd %d disconnected\n", client_socket_fd);
            close(client_socket_fd);
            break;
        } else if (read_bytes == -1) {
            close(client_socket_fd);
            errif(true, "socket read error");
        }
    }
    close(socket_fd);
    return 0;
}
