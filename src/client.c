#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
enum errors {
    OK,
    ERR_INCORRECT_ARGS,
    ERR_SOCKET,
    ERR_CONNECT
};
 
int init_socket(const char *ip, int port) {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct hostent *host = gethostbyname(ip);
    struct sockaddr_in server_address;
 
    //open socket, result is socket descriptor
    if (server_socket < 0) {
        perror("Fail: open socket");
        exit(ERR_SOCKET);
    }
 
    //prepare server address
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    memcpy(&server_address.sin_addr, host -> h_addr_list[0],
           (socklen_t) sizeof server_address.sin_addr);
 
    //connection
    if (connect(server_socket, (struct sockaddr*) &server_address,
        (socklen_t) sizeof server_address) < 0) {
        perror("Fail: connect");
        exit(ERR_CONNECT);
    }
    return server_socket;
}
 
int isEndLine(char ch) {
    return ch == '\r';
}

int isEmptyChar(char ch) {
    return ch == '\n';
}

int readStr(int fd, char **string) {
    char *str = NULL;
    char ch;
    int i = 0, status;
    while (1) {
        status = read(fd, &ch, sizeof(char)); 
        if(status <= 0) {
            break;
        }
        if(isEndLine(ch)) {
            break;
        }
        if(isEmptyChar(ch)) {
            continue;
        }
        str = realloc(str, (i + 1) * sizeof(char));
        str[i] = ch;
        i++;
    }
    str = realloc(str, (i + 1) * sizeof(char));
    str[i] = '\0';
    *string = str;
    return status;
} 

int main(void) {
    char *errorHeader = "HTTP/1.1 404\r\n\r\n";
    char *strForHTTP = "HTTP/1.1";
    char *correctRequest = "HTTP/1.1 200\r\n";
    char ch;
    int i = 0;
    int server;
    char request[256];
    char *arr[4] = {"GET ", "  ", " HTTP/1.1\r", "\r\n\r\n"};
    char *str[7];
    char  *strtokAnswer = NULL;
    char *string = NULL;
    char *str3 = "http://";
    char *requestArr[3];
    //  http://127.0.0.1:5000/cgi-bin/user?subject=math&name=Alex
    //  http://127.0.0.1:5000/file.html 
    fgets(request, 256, stdin);
    while (strcmp(request, "exit\n") != 0) {
        i = 0;
        while(strstr(request, "http://") == NULL && (strcmp(request, "exit\n") != 0)) {
            puts("Enter correct request");
            fgets(request, 256, stdin);
        }
        strtokAnswer = strtok(request, ":/");
         while((strtokAnswer != NULL) && (i != 4)) {
            requestArr[i] = strtokAnswer;
            if (i == 2) {
                strtokAnswer = strtok(NULL, "\n");
            } else {
                strtokAnswer = strtok(NULL, ":/");
            }
            i++;
        }
        arr[1] = requestArr[3];
        server = init_socket(requestArr[1], atoi(requestArr[2]));
        for(int i = 0; i < 4; i++) {
            write(server, arr[i], strlen(arr[i]) * sizeof(char));
        }
        readStr(server, &str[0]);
        if ((strcmp(str[0], errorHeader) != 0) && (strcmp(str[0], correctRequest) != 0)) {
            readStr(server, &str[1]);
            readStr(server, &str[2]);
            readStr(server, &str[3]);
            readStr(server, &str[4]);
            readStr(server, &str[5]);    
            printf("%s\n%s\n%s\n%s\n%s\n%s\n", str[0], str[1], str[2], str[3], str[4], str[5]);
        } else {
            printf("%s\n", str[0]);
        }
        arr[1] = NULL;
        arr[4] = NULL;
        fgets(request, 256, stdin);
    }
    close(server);
    return OK;
}
