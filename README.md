# Wordle_SocketsTCP
A wordle using c and TCP sockets with ubuntu virtualbox

# gcc wordle_server.c -o server
# gcc wordle_client.c -o client

Hay que configurar el NAT en la maquina servidora el reenvio de puertos y agregar una regla con los siguientes parametros: ip anfitrión 192.168.1.8 Puerto anfritrión 5647 ip invitado 10.0.2.15 Puerto invitado 7020 Protocolo TCP.

# ./servidor 7020
# ./cliente 192.168.1.8 5647