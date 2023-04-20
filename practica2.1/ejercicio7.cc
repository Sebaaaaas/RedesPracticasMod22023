#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <memory>

class TCPThread {
public:
    TCPThread(int sd_) : sd(sd_), close_thread(false) {}

    ~TCPThread() {
        close_thread = true;
    }

    void haz_mensaje() { //devuelve true si se ha pulsado q para escribir todos los mensajes

        char buffer[1500];
        ssize_t bytes = -1;
        while (!close_thread) {
            sleep(3);
            bytes = recv(sd, buffer, 1499, 0);
            if (bytes == -1) return;

            if (bytes == 0) {
                std::cout << "Fin de conexion.\n";
                return;
            }
            buffer[bytes] = '\0';
            bytes = send(sd, buffer, bytes, 0);
            if (bytes == -1) {
                std::cerr << "[send]: " << strerror(errno);
                return;
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


    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int rc = getaddrinfo(argv[1], argv[2], &hints, &result);
    if (rc != 0) {
        std::cerr << "[addrinfo]: " << gai_strerror(rc) << '\n';
        return -1;
    }

    int sd = socket(result->ai_family, result->ai_socktype, 0);
    if (sd == -1) {
        std::cerr << "[bind]: " << strerror(errno);
        return -1;
    }

    int b = bind(sd, (struct sockaddr*)result->ai_addr, result->ai_addrlen);
    if (b == -1) {
        std::cerr << "[bind]: " << strerror(errno);
        return -1;
    }

    freeaddrinfo(result);

    int l = listen(sd, 16);
    if (l == -1) {
        std::cerr << "[listen]: " << strerror(errno);
        return -1;
    }

    while (true) {

        char host[NI_MAXHOST]; //constantes definidas en netdb.h
        char serv[NI_MAXSERV];
        struct sockaddr_storage cliente;
        socklen_t cliente_len = sizeof(struct sockaddr_storage);


        int cliente_sd = accept(sd, (struct sockaddr*)&cliente, &cliente_len);

        if (cliente_sd == -1)
        {
            std::cerr << "Error en la llamada a la funcion accept." << std::endl;
            return -1;
        }
        getnameinfo((struct sockaddr*)&cliente, cliente_len, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

        printf("Conexion desde %s %s\n", host, serv);

        // -- Apartado de creacion del hilo --

        TCPThread* connection = new TCPThread(cliente_sd);

        std::thread thread(&TCPThread::haz_mensaje, connection);
        thread.detach();
    }

    return 0;
}