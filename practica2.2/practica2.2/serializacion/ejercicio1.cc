#include "Serializable.h"

#include <iostream>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

class Jugador: public Serializable
{
public:
    Jugador(const char * _n, int16_t _x, int16_t _y):pos_x(_x),pos_y(_y)
    {
        strncpy(name, _n, MAX_NAME);
    };

    virtual ~Jugador(){};

    void to_bin()
    {
        alloc_data(bufferSize);

        char* tmp = _data;

        memcpy(tmp, name, sizeof(char)* MAX_NAME);

        tmp += sizeof(char)* MAX_NAME;

        memcpy(tmp, &pos_x, sizeof(int16_t));

        tmp += sizeof(int16_t);

        memcpy(tmp, &pos_y, sizeof(int16_t));

    }

    int from_bin(char * data)
    {
        memcpy(name, data, MAX_NAME);
        data += sizeof(char) * MAX_NAME;

        memcpy(&pos_x, data, sizeof(int16_t));
        data += sizeof(int16_t);

        memcpy(&pos_y, data, sizeof(int16_t));
        
        return 0;
    }


public:
    const int32_t bufferSize = (sizeof(char) * MAX_NAME) + (sizeof(int16_t) * 2);
    static const size_t MAX_NAME = 20;
    char name[MAX_NAME];

    int16_t pos_x;
    int16_t pos_y;

    int fd;
};

int main(int argc, char **argv)
{
    Jugador one_r("", 0, 0);
    Jugador one_w("Player_ONE", 123, 987);
    Jugador one("player one", 12, 345);


    // 1. Serializar el objeto one_w
    one.to_bin();
    // 2. Escribir la serialización en un fichero
    int fd = open("a", O_CREAT | O_RDWR | O_TRUNC, 0644);
    write(fd, one.data(), one.size());
    close(fd);
    // 3. Leer el fichero
    char buff[256];
    fd = open("./a", O_RDONLY);
    read(fd, buff, 256);
    close(fd);
    // 4. "Deserializar" en one_r
    one_r.from_bin(buff);
    // 5. Mostrar el contenido de one_r
    std::cout << one_r.name << ", " << one_r.pos_x << ", " << one_r.pos_y << '\n';

    return 0;
}