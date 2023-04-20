#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <time.h>
#include <string>
#include <thread>
#include <vector>
#include <memory>

class MessageThread {
public:
    MessageThread(int sd_) : sd(sd_), close_thread(false) {}

    ~MessageThread() {
        close_thread = true;
    }

    void haz_mensaje() { //devuelve true si se ha pulsado q para escribir todos los mensajes

        char buffer[1500];




        while (!close_thread) {
            sleep(3);

            struct sockaddr_storage cliente;
            socklen_t cliente_len = sizeof(struct sockaddr_storage);
            std::thread::id id = std::this_thread::get_id();


            ssize_t bytes = recvfrom(sd, buffer, 1499, MSG_DONTWAIT, (struct sockaddr*)&cliente, &cliente_len);

            if (bytes == -1) continue;

            buffer[bytes] = '\0';

            char host[NI_MAXHOST];
            char serv[NI_MAXSERV];

            getnameinfo((struct sockaddr*)&cliente, cliente_len,
                host, NI_MAXHOST, serv, NI_MAXSERV,
                NI_NUMERICHOST | NI_NUMERICSERV);

            std::cout << "IP: " << host << "[ " << serv << " ]" << '\n';
            std::cout << "Input: " << buffer << '\n';
            std::cout << "THread ID: " << id << '\n';

            char tiempo[13];
            time_t t = time(NULL);
            tm* lt = localtime(&t);


            if (buffer[0] == 't')
            {
                strftime(tiempo, sizeof(tiempo), "%r", lt);
                sendto(sd, tiempo, strlen(tiempo), 0, (struct sockaddr*)&cliente, cliente_len);
                break;
            }
            else if (buffer[0] == 'd')
            {
                strftime(tiempo, sizeof(tiempo), "%F", lt);
                sendto(sd, tiempo, sizeof(tiempo), 0, (struct sockaddr*)&cliente, cliente_len);
                break;
            }
            else
            {
                sendto(sd, nullptr, 0, 0, (struct sockaddr*)&cliente, cliente_len);
                printf("Comando no soportado: %s\n", buffer);
                break;
            }


        }
    }

private:
    int sd;
public:
    bool close_thread;
};


int main(int arg, char** argv) {

    struct addrinfo hints;
    struct addrinfo* result;
    memset(&hints, 0, sizeof(struct addrinfo));
    std::vector<MessageThread*> messages;
    std::vector<std::thread> threads;


    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    int rc = getaddrinfo(argv[1], argv[2], &hints, &result);
    if (rc != 0) {
        std::cerr << "[addrinfo]: " << gai_strerror(rc) << '\n';
        return -1;
    }

    int sd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (sd == -1) {
        std::cerr << "[socket]: " << strerror(errno);
        return -1;
    }

    int b = bind(sd, (struct sockaddr*)result->ai_addr, result->ai_addrlen);
    if (b == -1) {
        std::cerr << "[bind]: " << strerror(errno) << '\n';
        return -1;
    }

    freeaddrinfo(result);



    //5 threads
    for (int i = 0; i < 5; ++i) {
        MessageThread* message = new MessageThread(sd);
        threads.push_back(std::thread(&MessageThread::haz_mensaje, message));
        messages.push_back(message);
        std::cout << "Hilo creado! | Thread_id: " << threads[i].get_id() << "\n";
    }

    std::string entrada = "";
    while (strcmp(entrada.c_str(), "q")) {
        std::cin >> entrada;
    }

    for (int i = 0; i < 5; i++)
    {
        std::thread::id id = threads[i].get_id();
        messages[i]->close_thread = true;
        threads[i].join();
        std::cout << "Hilo cerrado: | Thread_id: " << id << "\n";
    }
    printf("Hilos cerrados.\n");

    close(sd);

    return 1;
}
