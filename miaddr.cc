#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <string.h>
#include <iostream>

int main(int arg, char **argv){

    struct addrinfo hints;
    struct addrinfo *result;

    memset(&hints, 0, sizeof(addrinfo));

    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int rc = getaddrinfo(argv[1], argv[2], &hints, &result);
    if(rc!=0){
        std::cerr << "[addrinfo]: " << gai_strerror(rc) << '\n';
        return -1;
    }

    for(struct addrinfo *i = result; i != nullptr; i = i->ai_next){
        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];

        getnameinfo(i->ai_addr, i->ai_addrlen, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

        std::cout << "IP: " << host << " PORT: " << serv << '\n'; 
    }

    freeaddrinfo(result);
    
    int getaddrinfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res);
    void freeaddrinfo(struct addrinfo *res);
    const char *gai_strerror(int errcode);

    return 0;
};