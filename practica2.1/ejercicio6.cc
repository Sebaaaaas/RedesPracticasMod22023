#include <sys/socket.h> 
#include <netdb.h>
#include <sys/types.h>

#include <string.h>
#include <iostream>
#include <time.h>
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

class MessageThread{
public:
    MessageThread(int sd_) : sd(sd_){}

    bool haz_mensaje(){ //devuelve true si se ha pulsado q para escribir todos los mensajes
        
        bool write = false;
        
        char buffer[1500];
        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];


        struct sockaddr_storage cliente;
        socklen_t cliente_len = sizeof(struct sockaddr_storage);
        
        bool loop = true;
        while(loop){
            sleep(3);
            std::cout << std::this_thread::get_id() << '\n';
            
            ssize_t bytes = recvfrom(sd, buffer, 1499, 0, (struct sockaddr *) &cliente, &cliente_len);
            
            buffer[bytes] = '\0';

            getnameinfo((struct sockaddr *) &cliente, cliente_len, 
                        host, NI_MAXHOST, serv, NI_MAXSERV, 
                        NI_NUMERICHOST|NI_NUMERICSERV);

            std::cout << "IP: " << host << "[: " << serv << '\n';
            std::cout << "\tMSG " << buffer;


            char tiempo[13];
            time_t t;
            time(&t);
            tm* lt = localtime(&t);
            
            switch(buffer[0]){
            case 't':
            {
                strftime(tiempo,sizeof(tiempo),"%r", lt);
                sendto(sd, tiempo, strlen(tiempo), 0, (struct sockaddr*) &cliente, cliente_len); 
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
                write = true;
                loop = false;
                std::cout << "Saliendo...";
                break;
            }
            default:
            {
                break;
            }
            }

            return write;
        
        }
    }

private:
    int sd;
};


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

    int b = bind(sd, (struct sockaddr *) result->ai_addr, result->ai_addrlen);
    if(b == -1){
        std::cerr << "[bind]: " << strerror(errno) << '\n';
        return -1;
    }
    
    freeaddrinfo(result);

    std::vector<MessageThread*> messages;
    
    //10 threads
    for(int i = 0; i < 10; ++i){
        MessageThread *message = new MessageThread(sd);
        messages.push_back(message);
    }
    
    for(auto &thr:messages){
        thr->haz_mensaje();  
        // thr.join();
    }

    close(sd);

    return 1;
}