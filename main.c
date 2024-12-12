#include <stdio.h>
#include <errno.h>     //to capture erros thrown by the functions, we show them with perror fuction
#include <arpa/inet.h> // to use the conversion function to network bytes(little endian / big endian)
// from a int value (port)
#include <sys/socket.h> // to create a socket connectoin
#include <unistd.h>     // to use read operations
#include <string.h>

#define BUFFER_SIZE 1024
// struct sockaddr_in
// {
//     sa_family_t sin_family;  /* address family: AF_INET */
//     in_port_t sin_port;      /* port in network byte order */
//     struct in_addr sin_addr; /* internet address */
// };

// /* Internet address. */
// struct in_addr
// {
//     uint32_t s_addr; /* address in network byte order */
// };

int main()
{
    char buffer[BUFFER_SIZE]; // to use in read function

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1)
    {
        perror("webserver (socket)");
        return 1;
    }
    printf("socket created successfully");
    struct sockaddr_in host_addr;
    int host_addrlen = sizeof(host_addr);

    host_addr.sin_family = AF_INET;
    host_addr.sin_port = htons(8080);
    host_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(socket_fd, (struct sockaddr *)&host_addr, host_addrlen) != 0)
    {
        perror("error in bind function");
        return 1;
    }
    printf("socket bound to address successfully");

    // socket i passive mode, waiting for a active socket to connect
    if (listen(socket_fd, SOMAXCONN) != 0)
    {
        perror("error listening to incoming sockets");
        return 1;
    }
    printf("socket listening for connections");

    for (;;)
    {
        int newSocketfd = accept(socket_fd, (struct sockaddr *)&host_addr, (socklen_t *)&host_addrlen);

        if (newSocketfd == -1)
        {
            perror("Error in accept function");
            continue;
        }
        printf("socket accepted another sucessfully");

        int sockn = getsockname(newSocketfd, (struct sockaddr *)&host_addr,
                                (socklen_t *)&host_addrlen);
        if (sockn < 0)
        {
            perror("webserver (getsockname)");
            continue;
        }

        int valRead = read(newSocketfd, buffer, BUFFER_SIZE);
        if (valRead < 0)
        {
            perror("Error in read function");
            continue;
        }

        char resp[] = "HTTP/1.0 200 OK\r\n"
                      "Server: webserver-c\r\n"
                      "Content-Type: text/html\r\n\r\n"
                      "<html>Hello world</html>\r\n";

        char method[BUFFER_SIZE], uri[BUFFER_SIZE], version[BUFFER_SIZE];

        sscanf(buffer, "%s %s %s", method, uri, version);

        printf("[%s:%u] %s %s %s\n", inet_ntoa(host_addr.sin_addr), ntohs(host_addr.sin_port), method, version, uri);

        int valWrite = write(newSocketfd, resp, strlen(resp));
        if (valWrite < 0)
        {
            perror("Error in write function");
            continue;
        }
        close(newSocketfd);
    }

    return 0;
}
