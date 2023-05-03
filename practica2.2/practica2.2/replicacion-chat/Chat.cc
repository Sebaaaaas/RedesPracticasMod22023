#include "Chat.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatMessage::to_bin()
{
    alloc_data(MESSAGE_SIZE);

    memset(_data, 0, MESSAGE_SIZE);

    //Serializar los campos type, nick y message en el buffer _data
    char* tmp = _data;
    memcpy(tmp, &type, sizeof(uint8_t));
    tmp+= sizeof(uint8_t);

    memcpy(tmp, nick.c_str(), sizeof(char) * 20);
    tmp+= sizeof(char) * 20;

    memcpy(tmp, message.c_str(), sizeof(char) * 100);
}

int ChatMessage::from_bin(char * bobj)
{
    alloc_data(MESSAGE_SIZE);

    memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);

    //Reconstruir la clase usando el buffer _data

    return 0;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatServer::do_messages()
{
    while (true)
    {
        /*
         * NOTA: los clientes están definidos con "smart pointers", es necesario
         * crear un unique_ptr con el objeto socket recibido y usar std::move
         * para añadirlo al vector
         */

        //Recibir Mensajes en y en función del tipo de mensaje
        // - LOGIN: Añadir al vector clients
        // - LOGOUT: Eliminar del vector clients
        // - MESSAGE: Reenviar el mensaje a todos los clientes (menos el emisor)
        ChatMessage msg;
        Socket *sock = new Socket(socket);
        socket.recv(msg, sock);

        switch(msg.type){
            case ChatMessage::LOGIN:

                clients.push_back(std::move(std::unique_ptr<Socket>(sock)));

            break;
            case ChatMessage::LOGOUT:
                
                for(auto it = clients.begin(); it != clients.end() && !(**it == *sock); ++it){
                    if(it == clients.end()){
                        std::cout <<  "Cliente no conectado" << '\n';
                    }
                    else{
                        clients.erase(it);
                    }
                }

            break;
            case ChatMessage::MESSAGE:

                for(auto it = clients.begin(); it != clients.end(); ++it){
                    if(!(**it == *sock))
                        socket.send(msg, **it);
                }

            break;
        }
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatClient::login()
{
    std::string msg;

    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGIN;

    socket.send(em, socket);
}

void ChatClient::logout()
{
    // Completar
}

void ChatClient::input_thread()
{
    while (true)
    {
        // Leer stdin con std::getline
        // Enviar al servidor usando socket
    }
}

void ChatClient::net_thread()
{
    while(true)
    {
        //Recibir Mensajes de red
        //Mostrar en pantalla el mensaje de la forma "nick: mensaje"
    }
}

