# 🟨 Wordle_SocketsTCP

Aplicación cliente-servidor implementada en C usando **sockets TCP**, que simula una versión simple del popular juego Wordle. Desarrollado para utilizarse en **máquinas virtuales Ubuntu con VirtualBox**.

---

## 👨‍💻 Autores

- Javier Pino Herrera  
- Francisco Muñoz Inostroza  
- Camilo Campos González

---

## 🎯 Descripción

Este proyecto consiste en un juego interactivo donde un cliente envía palabras al servidor intentando adivinar una palabra secreta de 5 letras seleccionada al azar. El servidor responde con un patrón que indica coincidencias:

- `G` → letra correcta en la posición correcta (green)
- `Y` → letra correcta en posición incorrecta (yellow)
- `-` → letra incorrecta (gray)

El cliente también muestra un **abecedario de letras disponibles**, que se va actualizando tras cada intento, marcando con `.` las letras descartadas.

---

## ⚙️ Requisitos

- Sistema operativo: Ubuntu (recomendado en máquinas virtuales con VirtualBox)
- Compilador: `gcc`
- Comunicación TCP mediante sockets

---

## 🧠 ¿Por qué sockets TCP?

Elegimos **sockets TCP** porque:

- Garantiza entrega ordenada y fiable de los mensajes entre cliente y servidor
- Es adecuado para aplicaciones interactivas como Wordle donde se espera una secuencia de ida y vuelta
- Nos permite mantener una conexión persistente durante el juego


## 🔌 Configuración de red (VirtualBox - NAT)

Para que el cliente (desde el host) pueda comunicarse con el servidor (dentro de la VM Ubuntu), es necesario configurar el **reenvío de puertos (port forwarding)** en VirtualBox.

### 🔧 Pasos para configurar NAT con reenvío:

1. Apagar la máquina virtual si está corriendo.
2. En VirtualBox, abrir **Configuración** de la VM → **Red** → **Adaptador 1**.
3. Asegurarse de que esté en **Modo NAT**.
4. Hacer clic en **"Avanzado"** → **"Reenvío de puertos"**.
5. Agregar una regla con los siguientes valores de ejemplo:

- Nombre: WordleTCP
- IP anfitrión: 192.168.1.8
- Puerto anfitrión: 5647
- IP invitado: 10.0.2.15
- Puerto invitado: 7020
- Protocolo: TCP


> 📌 Importante asegurarse de que la IP invitado (`10.0.2.15`) coincida con la IP real de tu VM. Puede verificarse con el comando `ip a` dentro de la terminal de Ubuntu.

---

## 🏗️ Compilación

Desde la terminal, en cada máquina (cliente y servidor):

```bash
gcc wordle_server.c -o server
gcc wordle_client.c -o client
```


## 🚀 Ejecución

### En la máquina **servidora** :

```bash
./server 7020
```

### En la máquina **cliente** :
```bash
./client 192.168.1.8 5647
```