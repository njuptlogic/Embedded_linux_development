#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_IP "192.168.87.130"  // 修改为实际服务器IP
#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    char input[BUFFER_SIZE] = {0};
    char temp[100];

    // 创建socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // 转换IP地址
    if(inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        perror("invalid address");
        exit(EXIT_FAILURE);
    }

    // 连接服务器
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr))){
        perror("connection failed");
        exit(EXIT_FAILURE);
    }

    // 输入数据
    printf("Enter group members (ID Name, empty line to end):\n");
    while(1) {
        fgets(temp, sizeof(temp), stdin);
        if(strcmp(temp, "\n") == 0) break;
        strcat(input, temp);
    }

    // 发送数据
    send(sock, input, strlen(input), 0);
    printf("Data sent:\n%s\n", input);

    // 接收响应
    read(sock, buffer, BUFFER_SIZE);
    printf("Received response:\n%s\n", buffer);

    close(sock);
    return 0;
}
