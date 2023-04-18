#include <sys/socket.h> 
#include <netdb.h>
#include <sys/types.h>

#include <string.h>
#include <iostream>
#include <time.h>

int main(int arg, char **argv){


    struct addrinfo hints;
    struct addrinfo *result;
    memset(&hints, 0, sizeof(struct addrinfo));


    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    int rc = getaddrinfo(argv[1], argv[2], &hints, &result);
    if(rc != 0){
        std::cerr << "[addrinfo]: " << gai_strerror(rc) << '\n';
        return -1;
    }

    int sd = socket(result->ai_family, result->ai_socktype, 0);
    if(sd == -1){
        std::cerr << "[bind]: " << strerror(errno);
        return -1;
    }

    bind(sd, (struct sockaddr *) result->ai_addr, result->ai_addrlen);

    freeaddrinfo(result);

    bool funcionando = true;
    while(funcionando){
        char buffer[1500];
        char host[NI_MAXHOST]; //constantes definidas en netdb.h
        char serv[NI_MAXSERV];

        struct sockaddr_storage cliente;
        socklen_t cliente_len = sizeof(struct sockaddr_storage);

        ssize_t bytes = recvfrom(sd, buffer, 1499, 0, (struct sockaddr *) &cliente, &cliente_len);
        buffer[bytes] = '\0';
        getnameinfo((struct sockaddr *) &cliente, cliente_len, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST|NI_NUMERICSERV);

        std::cout << bytes << " bytes de " << host << ":" << serv << "\n";

        sendto(sd, buffer, bytes, 0, (struct sockaddr *) &cliente, cliente_len);

        //calculamos la hora en funcion del mensaje mandado(t, d, q)

        char tiempo[13];
        time_t t;
        time(&t);
        tm* lt = localtime(&t);

        switch(buffer[0]){
            case 't':
                {
                    strftime(tiempo,sizeof(tiempo),"%r", lt);
                    sendto(sd, tiempo, sizeof(tiempo), 0, (struct sockaddr*) &cliente, cliente_len); 
                    break;
                }
            case 'd':
                {
                    strftime(tiempo,sizeof(tiempo),"%F", lt);
                    sendto(sd, tiempo, sizeof(tiempo), 0, (struct sockaddr*) &cliente, cliente_len);
                    break;
                }
            case 'q':
                {
                    std::cout << "Saliendo...";
                    funcionando = false;
                    break;
                }
            default:
                {
                    break;
                }
        }
    }

    return 1;
}