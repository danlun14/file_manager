#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#define CREATE 0
#define READ 1
#define MOVE 2
#define COPY 3
#define DELETE 4
#define LINK 5
#define LOOKDIR 6

#define OPERATIONS_COUNT 7
void commands_init(char *commands[])
{
    commands[CREATE] = "create";
    commands[READ] = "read";
    commands[MOVE] = "move";
    commands[COPY] = "copy";
    commands[DELETE] = "delete";
    commands[LINK] = "link";
    commands[LOOKDIR] = "lookdir";
}

int strcmp(char *dest, char *_source)
{
    for (char *source = _source; *source != 0; source++, dest++)
    {
        if (*dest != *source)
        {
            return -1;
        }
    }
    return 0;
}

int get_operation(char *in_command, char *commands[])
{
    for (int i = 0; i < OPERATIONS_COUNT; i++)
    {
        if (!strcmp(in_command, commands[i]))
        {
            return i;
        }
    }
    return -1;
}

void err_quit(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    exit(1);
}

void create_file(const char *pathname)
{
    int fd;
    if ((fd = open(pathname, O_CREAT | O_RDONLY | O_EXCL, S_IRWXU)) == -1)
    {
        err_quit("Файл %s уже существует\n", pathname);
    }
    else
    {
        printf("Файл %s успешно создан\n", pathname);
    }
    close(fd);
}

void read_file(const char *pathname)
{
    char buf;

    int readfd = open(pathname, O_RDONLY);
    if (readfd == -1)
    {
        err_quit("Ошибка открытия файла %s\n", pathname);
    }

    while (read(readfd, &buf, 1) != 0)
    {
        printf("%c", buf);
    }

    close(readfd);
}

void move_file(const char *pathname_from, const char *pathname_where)
{
    char buf;

    int readfd = open(pathname_from, O_RDONLY);
    if (readfd == -1)
    {
        err_quit("Ошибка открытия файла %s\n", pathname_from);
    }

    int writefd = open(pathname_where, O_CREAT | O_WRONLY, S_IRWXU);
    if (writefd == -1)
    {
        close(readfd);
        err_quit("Ошибка открытия файла %s\n", pathname_from);
    }
    while (read(readfd, &buf, 1) != 0)
    {
        write(writefd, &buf, 1);
    }

    close(writefd);
    close(readfd);

    if (unlink(pathname_from) == -1)
    {
        err_quit("Файл %s не может быть удален, ошибка перемещения\n", pathname_from);
        unlink(pathname_where);
    }
}

void copy_file(const char *pathname_from, const char *pathname_where)
{
    char buf;

    int readfd = open(pathname_from, O_RDONLY);
    if (readfd == -1)
    {
        err_quit("Ошибка открытия файла %s\n", pathname_from);
    }

    int writefd = open(pathname_where, O_CREAT | O_WRONLY, S_IRWXU);
    if (writefd == -1)
    {
        close(readfd);
        err_quit("Ошибка открытия файла %s\n", pathname_from);
    }
    while (read(readfd, &buf, 1) != 0)
    {
        write(writefd, &buf, 1);
    }

    close(writefd);
    close(readfd);
}

void delete_file(const char *pathname)
{
    if (unlink(pathname) == -1)
    {
        err_quit("Файл %s не может быть удален\n", pathname);
    }
    else
    {
        printf("Файл %s успешно удален\n", pathname);
    }
}

int main(int argc, char *argv[])
{
    char *commands[OPERATIONS_COUNT];
    commands_init(commands);
    int command = get_operation(argv[1], commands);
    switch (command)
    {
    case CREATE:
        if (argc != 3)
        {
            err_quit("Использование: ./filem create <Имя файла 1>\n");
        }
        create_file(argv[2]);
        break;
    case READ:
        if (argc != 3)
        {
            err_quit("Использование: ./filem read <Имя файла 1>\n");
        }
        read_file(argv[2]);
        break;
    case MOVE:
        if (argc != 4)
        {
            err_quit("Использование: ./filem move <Имя файла 1> <Имя файла 2>\n");
        }
        move_file(argv[2], argv[3]);
        break;
    case COPY:
        if (argc != 4)
        {
            err_quit("Использование: ./filem copy <Имя файла 1> <Имя файла 2>\n");
        }
        copy_file(argv[2], argv[3]);
        break;
    case DELETE:
        if (argc != 3)
        {
            err_quit("Использование: ./filem delete <Имя файла 1>\n");
        }
        delete_file(argv[2]);
        break;
    case LINK:
        if (argc != 3)
        {
            err_quit("Использование: ./filem link <Имя файла 1>\n");
        }
        delete_file(argv[2]);
        break;
    case LOOKDIR:
        if (argc > 2)
        {
            err_quit("Использование: ./filem lookdir [Имя файла 1]\n");
        }
        delete_file(argv[2]);
        break;
    default:
        err_quit("Использование: ./filem <command> <Имя файла 1> [Имя файла 2]\n");
        break;
    }
    return 0;
}