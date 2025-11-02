#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE] = {0};
    char response[BUFFER_SIZE] = {0};
    int bytes_read;

    // 接收客户端数据
    bytes_read = read(client_socket, buffer, BUFFER_SIZE);
    if(bytes_read < 0) {
        perror("read failed");
        exit(EXIT_FAILURE);
    }

    printf("Received data:\n%s\n", buffer);

    // 提取学号并构建响应
    char *token = strtok(buffer, "\n");
    while(token != NULL) {
        char id[20];
        if(sscanf(token, "%s", id) == 1) {
            strcat(response, id);
            strcat(response, "\n");
        }
        token = strtok(NULL, "\n");
    }

    // 发送响应
    send(client_socket, response, strlen(response), 0);
    printf("Sent response:\n%s\n", response);
    
    close(client_socket);
    exit(0);
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // 创建TCP socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // 设置socket选项
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // 绑定端口
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // 监听
    if (listen(server_fd, 5) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    // 处理僵尸进程
    signal(SIGCHLD, SIG_IGN);

    while(1) {
        // 接受新连接
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, 
                               (socklen_t*)&addrlen)) < 0) {
            perror("accept failed");
            exit(EXIT_FAILURE);
        }

        // 创建子进程处理客户端
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork failed");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) { // 子进程
            close(server_fd);
            handle_client(new_socket);
        } else { // 父进程
            close(new_socket);
        }
    }

    return 0;
}

