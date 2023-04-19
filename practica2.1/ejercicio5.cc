#include <sys/socket.h> 
#include <netdb.h>
#include <sys/types.h>

#include <string.h>
#include <iostream>
#include <time.h>
#include <unistd.h>


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

    bool conn = true;
    while(conn){
        char buffer[1500];
        std::cin >> buffer;

        ssize_t send;

        if(buffer[0] == 'Q'){
            // send = sendto(sd, buffer, 0, 0, result->ai_addr, result->ai_addrlen);
            close(sd);
            conn = false;
        }
        else
            send = sendto(sd, buffer, strlen(buffer), 0, result->ai_addr, result->ai_addrlen);
        
        if(send == -1){
            std::cerr << "[sendto]: " << strerror(errno) << '\n';
            return -1;
        }

        struct sockaddr cliente = *result->ai_addr;
        socklen_t cliente_len = sizeof(struct sockaddr);
        
        ssize_t recv = recvfrom(sd, buffer, 1500, 0, result->ai_addr, &result->ai_addrlen);
        if(recv == -1){
            std::cerr << "[recv]: " << strerror(errno);
            return -1;
        }
        std::cout << buffer << '\n';
    }

}