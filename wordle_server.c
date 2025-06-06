// Paso 1: Includes y configuración
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>

#define PORT 8080
#define MAX_WORD_LEN 6

// Paso 2: Función principal del servidor
int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1, addrlen = sizeof(address);
    char buffer[1024] = {0};
    char secret_word[MAX_WORD_LEN] = "APPLE";

    // Paso 3: Crear socket TCP
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // Escuchar todas las interfaces
    address.sin_port = htons(PORT);

    // Paso 4: Enlazar y escuchar
    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 3);

    printf("Servidor esperando conexiones...\n");
    new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
    printf("Cliente conectado.\n");

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int valread = read(new_socket, buffer, 1024);
        if (valread <= 0) break;
    
        // Mostrar lo que el cliente envió
        printf("Cliente dijo: %s\n", buffer);
    
        // Evaluar la palabra
        char resultado[MAX_WORD_LEN + 1];
        evaluar_palabra(secret_word, buffer, resultado);
    
        // Enviar respuesta al cliente
        send(new_socket, resultado, strlen(resultado), 0);
    }    

    close(new_socket);
    close(server_fd);
    return 0;
}

void evaluar_palabra(const char *secreta, const char *intento, char *resultado) {
    int len = strlen(secreta);
    for (int i = 0; i < len; i++) {
        if (intento[i] == secreta[i]) {
            resultado[i] = 'G'; // Green
        } else if (strchr(secreta, intento[i])) {
            resultado[i] = 'Y'; // Yellow
        } else {
            resultado[i] = '-'; // Gris
        }
    }
    resultado[len] = '\0';
}

