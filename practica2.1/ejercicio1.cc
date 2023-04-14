#include <sys/socket.h> 
#include <netdb.h>
#include <sys/types.h>

#include <string.h>
#include <iostream>

int main(int arg, char **argv){

    struct addrinfo hints;
    struct addrinfo *result;

    memset(&hints, 0, sizeof(addrinfo));

    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_UNSPEC;

    int errorNo = getaddrinfo(argv[1], argv[2], &hints, &result);

    if(errorNo!=0){
        std::cerr << "[Error]: " << gai_strerror(errorNo) << '\n';
        return -1;
    }
    
    for(struct addrinfo *i = result; i != nullptr; i = i->ai_next){
        char host[NI_MAXHOST]; //constantes definidas en netdb.h
        char serv[NI_MAXSERV];

        getnameinfo(i->ai_addr, i->ai_addrlen, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

        std::cout << host << ' ' << i->ai_family << ' ' << i->ai_socktype << '\n'; 
    }
}