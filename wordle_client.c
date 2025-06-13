
// wordle_client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

void actualizar_abecedario(char *abecedario, const char *intento, const char *resultado) {
    for (int i = 0; i < 5; i++) {
        if (resultado[i] == '-') {
            for (int j = 0; j < 26; j++) {
                if (abecedario[j] == intento[i]) {
                    abecedario[j] = '.'; 
                    break;
                }
            }
        }
    }
}

void convertir_a_mayusculas(char *str) {
    for (int i = 0; str[i]; i++) {
        if (str[i] >= 'a' && str[i] <= 'z') {
            str[i] -= 32;  
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Uso: %s <IP servidor> <puerto>\n", argv[0]);
        return 1;
    }

    int sock;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    char input[100];

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket");
        return 1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connect");
        return 1;
    }

    printf("Conectado al servidor %s:%s\n", argv[1], argv[2]);
    printf("Categoria: Palabras cualquiera de 5 letras sin tildes.\n");
    char letras_disponibles[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    while (1) {
        do {
            printf("Ingresa una palabra de 5 letras: ");
            fgets(input, sizeof(input), stdin);
            input[strcspn(input, "\n")] = 0;
        
            if (strlen(input) != 5) {
                printf("❌ Error: la palabra debe tener exactamente 5 letras.\n");
            }
        } while (strlen(input) != 5);
        
        convertir_a_mayusculas(input);
        send(sock, input, strlen(input), 0);
        
        int valread = read(sock, buffer, BUFFER_SIZE);
        buffer[valread] = '\0';
        printf("Servidor responde: %s\n", buffer);
        actualizar_abecedario(letras_disponibles, input, buffer);
        printf("Letras disponibles: %s\n", letras_disponibles);

        // Si acierta, mostrar mensaje y cerrar
        if (strcmp(buffer, "GGGGG") == 0) {
            printf("🎉 ¡Acertaste la palabra!\n");
            break;
        }
    }

    close(sock);
    return 0;
}
