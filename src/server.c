#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <wait.h>
 
enum errors {
    OK,
    ERR_INCORRECT_ARGS,
    ERR_SOCKET,
    ERR_SETSOCKETOPT,
    ERR_BIND,
    ERR_LISTEN
};
 
int init_socket(int port) {
    int server_socket, socket_option = 1;
    struct sockaddr_in server_address;
 
    //open socket, return socket descriptor
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Fail: open socket");
        exit(ERR_SOCKET);
    }
 
    //set socket option
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &socket_option, (socklen_t) sizeof socket_option);
    if (server_socket < 0) {
        perror("Fail: set socket options");
        exit(ERR_SETSOCKETOPT);
    }
 
    //set socket address
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY;
    if (bind(server_socket, (struct sockaddr *) &server_address, (socklen_t) sizeof server_address) < 0) {
        perror("Fail: bind socket address");
        exit(ERR_BIND);
    }
 
    //listen mode start
    if (listen(server_socket, 5) < 0) {
        perror("Fail: bind socket address");
        exit(ERR_LISTEN);
    }
    return server_socket;
}
 
int readStr(int fd, char **string) {
    char *str = NULL;
    char c;
    int i = 0, status;
    while (((status = read(fd, &c, sizeof(char))) > 0) && (c != '\n') && (c != ' ')) {       
        if (i % 16 == 0) {
            str = realloc(str, 16 * ((i / 16) + 1) * sizeof(char));
        }
        str[i] = c;
        i++;
    }
    str = realloc(str, (i + 1) * sizeof(char));
    str[i] = '\0';
    *string = str;
    return status;
}
int run_binary(char *path, int client_socket) {
    if (fork() == 0) {
        dup2(client_socket, 1);
        close(client_socket);
        execl(path, 0);
        exit(0);
    }
    wait(NULL);
    return 0;
}
void sendAnswer(int client_socket, char *type, int len) {
    char *str3 = "HTTP/1.1 200\n";
    char *str1 = "content-type: ";
    char *str2 = "content-length: ";
    char str4[10];
    snprintf(str4, 10, "%d", len);
    write(client_socket, str3, strlen(str3) * sizeof(char));
    write(client_socket, str1, strlen(str1) * sizeof(char));
    write(client_socket, type, strlen(type) * sizeof(char));
    write(client_socket, "\n", sizeof(char));
    write(client_socket, str2, strlen(str2) * sizeof(char));
    write(client_socket, str4, strlen(str4) * sizeof(char));
    write(client_socket, "\n", sizeof(char));
}
int requestAnswer(char *arr[6], int client_socket) {
    struct stat stat;
    char *str1 = "HTTP/1.1 404\n";
    char *str2 = "HTTP/1.1\n";
    char *type = NULL;
    char *string = NULL;
    char *str3 = "html/text\n";
    int fd = 0;
    int len = 0;
    int i = 0;
    char ch;
    if ((type = strchr(arr[1], '.')) == NULL) {
        type = "bin";
    } else {
        type = str3;
    }
    if (strcmp(arr[0], "GET") != 0) {
        write(client_socket, str1, (strlen(str1) + 1) * sizeof(char));
        return -1;
    }
    if (strcmp(arr[2], str2) != 0) {
        write(client_socket, str2, (strlen(str2) + 1) * sizeof(char));
        return -1;
    }
    fd = open(arr[1], O_RDONLY);
    if (fd < 0) {
        perror("open error");
        write(client_socket, str1, (strlen(str1) + 1) * sizeof(char));
        return -1;
    }
    lstat(arr[1], &stat);
    len = stat.st_size;
    if (S_ISDIR(stat.st_mode)) {
        type = "dir";
    }
    sendAnswer(client_socket, type, len);
    if (strcmp(type, "bin") == 0) {
        run_binary(arr[1], client_socket);
        write(client_socket, "\n", sizeof(char));
    } else if (strcmp(type, "bdir") != 0) {
        readStr(fd, &string);
        write(client_socket, string, (strlen(string) + 1) * sizeof(char));
    }
    return 0;
} 
 
int main(int argc, char** argv) {
    int port, server_socket, client_socket;
    struct sockaddr_in client_address;
    socklen_t size = sizeof client_address;
    if (argc != 2) {
        puts("Incorrect args.");
        puts("./server <port>");
        puts("Example:");
        puts("./server 5000");
        return ERR_INCORRECT_ARGS;
    }
    char *arr[6];
    port = atoi(argv[1]);
    int i = 0;
    char ch;
    int j = 0;
    char *str = NULL;
    server_socket = init_socket(port);
        
    int ret = 0;
    while (1) {
        client_socket = accept(server_socket,
                              (struct sockaddr *) &client_address,
                              &size);
        j = 0;
        while (j != 6) {
            ret = readStr(client_socket, &str);
            puts(str);
            arr[j] = str;
            j++;
            str = NULL;
        }
        requestAnswer(arr, client_socket);
        if (ret <= 0) {
            break;
        } 
    }
    close(client_socket);
    return OK;
}
