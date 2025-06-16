// wordle_client.c
// Autores: Javier Pino Herrera, Francisco Muñoz Inostroza, Camilo Campos González
// Descripción: Cliente TCP para juego tipo Wordle. Envia palabras de 5 letras al servidor
// y recibe evaluaciones de coincidencias. Además, mantiene un abecedario actualizado con letras descartadas.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

// Esta función recibe el abecedario disponible, la palabra ingresada y el resultado devuelto por el servidor.
// Si una letra no está en la palabra secreta (marcada con '-'), se reemplaza en el abecedario por un punto '.'.
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

// Convierte una cadena de caracteres a mayúsculas.
// Esto asegura que la comparación con el servidor sea insensible a mayúsculas/minúsculas.
void convertir_a_mayusculas(char *str) {
    for (int i = 0; str[i]; i++) {
        if (str[i] >= 'a' && str[i] <= 'z') {
            str[i] -= 32;  
        }
    }
}

int main(int argc, char *argv[]) {
    // Validación de argumentos: se espera IP del servidor y puerto.
    if (argc != 3) {
        printf("Uso: %s <IP servidor> <puerto>\n", argv[0]);
        return 1;
    }

    // Declaración de variables de conexión y buffers
    int sock;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    char input[100];

    // Creación del socket TCP
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket");
        return 1;
    }

    // Configuración de dirección y puerto del servidor
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &serv_addr.sin_addr);

    // Conexión al servidor
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connect");
        return 1;
    }

    printf("Conectado al servidor %s:%s\n", argv[1], argv[2]);
    printf("Categoria: Palabras cualquiera de 5 letras sin tildes.\n");

    // Abecedario inicial con todas las letras disponibles
    char letras_disponibles[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    // Bucle principal del juego
    while (1) {
        // Solicita al usuario una palabra válida de exactamente 5 letras
        do {
            printf("Ingresa una palabra de 5 letras: ");
            fgets(input, sizeof(input), stdin);
            input[strcspn(input, "\n")] = 0;
        
            if (strlen(input) != 5) {
                printf("❌ Error: la palabra debe tener exactamente 5 letras.\n");
            }
        } while (strlen(input) != 5);
        
        // Convierte la palabra a mayúsculas antes de enviarla
        convertir_a_mayusculas(input);

        // Envía la palabra al servidor
        send(sock, input, strlen(input), 0);
        
        // Espera respuesta del servidor y la imprime
        int valread = read(sock, buffer, BUFFER_SIZE);
        buffer[valread] = '\0';
        printf("Servidor responde: %s\n", buffer);

        // Actualiza letras disponibles eliminando las incorrectas
        actualizar_abecedario(letras_disponibles, input, buffer);
        printf("Letras disponibles: %s\n", letras_disponibles);

        // Si todas las letras son correctas, se finaliza el juego
        if (strcmp(buffer, "GGGGG") == 0) {
            printf("🎉 ¡Acertaste la palabra!\n");
            break;
        }
    }

    // Cierre del socket al finalizar
    close(sock);
    return 0;
}
