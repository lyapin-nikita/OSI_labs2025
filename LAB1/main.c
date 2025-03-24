//КОД ДЛЯ ПЕРВОЙ ЛАБОРАТОРНОЙ РАБОТЫ ДЛЯ КУРСА "ОПЕРАЦИОННЫЕ СИСТЕМЫ" 

// Лабораторная работа №1. Файлы, отображаемый в память.
// Постановка задачи
// -----------------
// Требуется создать программу, выполняющую следующие действия:

// 1. Отображение файла в память (имя файла передается первым аргументом
// командной строки).
// 2. Перемена местами четных и нечетных байт в отображенном блоке памяти (нулевой<->первый, второй<->третий,... Если общее количество байт нечетное, последний байт остается на месте.)
// 3. Отключение отображения файла в память.

// Замечание. Можно выводить на консоль состояние блока памяти в виде
// последовательности значений байт в шестнадцатеричном виде в начале
// и после окончания выполнения операции изменения порядка.


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>




// Функция для вывода содержимого памяти в шестнадцатеричном виде
void print_hex(const unsigned char* data, size_t size) {
    for (size_t i = 0; i < size; i++) {
        printf("%02x ", data[i]);
    }
    printf("\n");
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Использование: %s <имя_файла>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char* filename = argv[1];
    int fd;
    struct stat file_info;
    unsigned char* mapped_memory;
    size_t file_size;

    
    fd = open(filename, O_RDWR); 
    if (fd == -1) {
        perror("Ошибка при открытии файла");
        exit(EXIT_FAILURE);
    }

    
    if (fstat(fd, &file_info) == -1) {
        perror("Ошибка при получении информации о файле");
        close(fd);
        exit(EXIT_FAILURE);
    }
    file_size = file_info.st_size;

    
    mapped_memory = (unsigned char*)mmap(NULL, file_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mapped_memory == MAP_FAILED) {
        perror("Ошибка при отображении файла в память");
        close(fd);
        exit(EXIT_FAILURE);
    }

    printf("Содержимое памяти до изменения:\n");
    print_hex(mapped_memory, file_size);

    
    for (size_t i = 0; i < file_size - 1; i += 2) {
        unsigned char temp = mapped_memory[i];
        mapped_memory[i] = mapped_memory[i + 1];
        mapped_memory[i + 1] = temp;
    }

    printf("Содержимое памяти после изменения:\n");
    print_hex(mapped_memory, file_size);

    

    if (munmap(mapped_memory, file_size) == -1) {
        perror("Ошибка при отключении отображения файла из памяти");
    }

    
    if (close(fd) == -1) {
        perror("Ошибка при закрытии файла");
        exit(EXIT_FAILURE);
    }

    return 0;
}