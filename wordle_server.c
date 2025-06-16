// wordle_server.c
// Autores: Javier Pino Herrera, Francisco Muñoz Inostroza, Camilo Campos González
// Descripción: Servidor TCP para juego tipo Wordle. Atiende conexiones de clientes que intentan adivinar palabras de 5 letras, evaluando cada intento y cerrando la conexión cuando se acierta.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <ctype.h>  // Para manejo de mayúsculas

#define MAX_CONN 5
#define MAX_WORD_LEN 6
#define BUFFER_SIZE 1024

int descriptor_socket_servidor, descriptor_socket_cliente;

// Arreglo de palabras válidas (todas de 5 letras y sin tildes)
const char *palabras[] = {
    "ABRIR","BAJAR","BEBER","BRAZO","BROMA","BUENO","CALLE","CALMO","CAMPO","CARTA",
    "CHICA","CHICO","CIELO","CLARO","COMER","CORTO","DECIR","DICHO","DUDAR","DULCE",
    "FAROL","FLACO","FONDO","FRUTO","FUEGO","GANAR","GORRO","HEROE","JOVEN","JUGAR",
    "LARGO","LENTO","LUGAR","MADRE","MATAR","MIRAR","MONTE","MUJER","NARIZ","NEGRO",
    "NIEVE","NUEVO","PADRE","PAGAR","PARED","PARTE","PECHO","PEDIR","PERRO","PLAYA",
    "PLAZA","POBRE","PONER","PUNTO","RELOJ","SABER","SABIO","SACAR","SALIR","SALUD",
    "SELVA","SEÑOR","SOLAS","SOÑAR","SUAVE","SUBIR","SUCIO","TABLA","TENER","TOCAR",
    "TORPE","TRAER","TRAPO","VENIR","VERDE","VIEJO","VIVIR"
};

#define TOTAL_PALABRAS (sizeof(palabras)/sizeof(palabras[0]))

// Esta función evalúa cada letra de la palabra ingresada por el cliente.
// Marca 'G' si la letra está en la posición correcta, 'Y' si está en otra posición, '-' si no existe en la palabra secreta.
void evaluar_palabra(const char *secreta, const char *intento, char *resultado) {
    int len = strlen(secreta);
    for (int i = 0; i < len; i++) {
        if (intento[i] == secreta[i]) {
            resultado[i] = 'G'; // Green
        } else if (strchr(secreta, intento[i])) {
            resultado[i] = 'Y'; // Yellow
        } else {
            resultado[i] = '-'; // Gray
        }
    }
    resultado[len] = '\0';
}

// Convierte la palabra recibida del cliente a mayúsculas.
// Esto permite que el juego no distinga entre mayúsculas y minúsculas.
void convertir_a_mayusculas(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper((unsigned char)str[i]);
    }
}

// Captura la señal de interrupción (Ctrl+C) y cierra adecuadamente los sockets abiertos.
void catch(int sig) {
    printf("\n*** Señal %d capturada. Cerrando servidor...\n", sig);
    close(descriptor_socket_cliente);
    close(descriptor_socket_servidor);
    exit(0);
}

int main(int argc, char *argv[]) {
    // Verifica que se haya pasado el puerto como argumento
    if (argc != 2) {
        printf("Uso: %s <puerto>\n", argv[0]);
        return 1;
    }

    // Declaración de estructuras para sockets y buffers
    struct sockaddr_in socket_servidor, socket_cliente;
    socklen_t cliente_len = sizeof(socket_cliente);
    char buffer[BUFFER_SIZE];
    char resultado[MAX_WORD_LEN + 1];
    srand(time(NULL));  // Inicializa generador aleatorio para elegir palabra

    char secret_word[MAX_WORD_LEN + 1]; // Palabra actual que debe adivinar el cliente

    signal(SIGINT, catch); // Asocia la señal de interrupción al handler

    // Creación del socket del servidor TCP
    descriptor_socket_servidor = socket(AF_INET, SOCK_STREAM, 0);
    if (descriptor_socket_servidor < 0) {
        perror("Socket");
        exit(1);
    }

    // Configuración de dirección del servidor
    socket_servidor.sin_family = AF_INET;
    socket_servidor.sin_addr.s_addr = INADDR_ANY;
    socket_servidor.sin_port = htons(atoi(argv[1]));

    // Enlaza el socket a la IP local y al puerto especificado
    if (bind(descriptor_socket_servidor, (struct sockaddr*)&socket_servidor, sizeof(socket_servidor)) < 0) {
        perror("Bind");
        exit(1);
    }

    // El servidor comienza a escuchar conexiones entrantes
    listen(descriptor_socket_servidor, MAX_CONN);
    printf("Servidor esperando conexiones en puerto %s...\n", argv[1]);

    // Bucle principal que acepta múltiples clientes de forma secuencial
    while (1) {
        // Espera una nueva conexión de cliente
        descriptor_socket_cliente = accept(descriptor_socket_servidor, (struct sockaddr*)&socket_cliente, &cliente_len);
        if (descriptor_socket_cliente < 0) {
            perror("Accept");
            continue;
        }

        // Se elige una palabra secreta nueva para cada cliente
        strcpy(secret_word, palabras[rand() % TOTAL_PALABRAS]);
        printf("\nNuevo cliente conectado desde %s\n", inet_ntoa(socket_cliente.sin_addr));
        printf("🔐 Nueva palabra secreta: %s\n", secret_word);

        // Bucle de interacción con el cliente
        while (1) {
            memset(buffer, 0, sizeof(buffer));
            int valread = recv(descriptor_socket_cliente, buffer, BUFFER_SIZE, 0);
            if (valread <= 0) break;

            convertir_a_mayusculas(buffer);
            printf("Cliente dijo: %s\n", buffer);
            evaluar_palabra(secret_word, buffer, resultado);
            send(descriptor_socket_cliente, resultado, strlen(resultado), 0);

            // Si acierta, se cierra su conexión pero el servidor sigue funcionando
            if (strcmp(resultado, "GGGGG") == 0) {
                printf("✅ Cliente acertó la palabra. Cerrando su conexión.\n");
                break;
            }
        }

        // Cierre de la conexión con el cliente actual
        close(descriptor_socket_cliente);
    }
}
