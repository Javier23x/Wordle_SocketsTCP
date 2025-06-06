#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    char input[100];

    sock = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr); // Cambiar IP si es remoto

    connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    while (1) {
        printf("Ingresa una palabra: ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0; // Quitar newline

        send(sock, input, strlen(input), 0);
        int valread = read(sock, buffer, 1024);
        buffer[valread] = '\0';
        printf("Servidor dice: %s\n", buffer);
    }

    close(sock);
    return 0;
}
