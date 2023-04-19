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
    hints.ai_socktype = SOCK_STREAM;

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

    int b = bind(sd, (struct sockaddr *) result->ai_addr, result->ai_addrlen);
    if(b == -1){
        std::cerr << "[bind]: " << strerror(errno);
        return -1;
    }

    int l = listen(sd, 5);
    if(l == -1){
        std::cerr << "[listen]: " << strerror(errno);
        return -1;
    }

    freeaddrinfo(result);

    
    while(1){
        char host[NI_MAXHOST]; //constantes definidas en netdb.h
        char serv[NI_MAXSERV];
        struct sockaddr_storage cliente;
        socklen_t cliente_len = sizeof(struct sockaddr_storage);

        int cliente_sd = accept(sd, (struct sockaddr*) &cliente, &cliente_len);

        int name = getnameinfo((struct sockaddr *) &cliente, cliente_len, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST|NI_NUMERICSERV);
        if(name == -1){
            std::cerr << "[name]: " << strerror(errno);
            return -1;
        }
        std::cout << "Conexion desde " << host <<  " - " << serv << '\n';

        bool conn = true;
        while(conn){
            
            char buffer[1500];

            ssize_t c = recv(cliente_sd, buffer, 1499, 0);

            buffer[c] = '\0';
            send(cliente_sd, buffer, strlen(buffer)+1, 0);

            if(c < 1)
                conn = false;
            
        }
        std::cout << "Conexion terminada\n";
    }

    return 1;
}