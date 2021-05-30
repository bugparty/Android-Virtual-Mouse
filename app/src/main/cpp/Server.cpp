#include <cstdio>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include <cstdlib>
#include <thread>
#include "packet.h"
#include "virtualMouse.h"
Packet packet;
VirtualMouse virtualMouse;
const int LISTEN_PORT = 18887;
/*
 * This will handle connection for each client
 * */
void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size;
    int ok = 0;
    int failed = 1;
    //Receive a message from client
    while( (read_size = recv(sock , &packet , sizeof(packet) , 0)) > 0 )
    {
        packet.fromNet();
        if (packet.type == MOVE){
            printf("move %i %i\n", packet.move.x, packet.move.y);
            virtualMouse.move(packet.move.x, packet.move.y);
        }else if (packet.type == CLICK){
            printf("click %i %i\n", packet.click.key_code,
                   packet.click.status);
            virtualMouse.click(packet.click.key_code,
                               packet.click.status
                               );
        }else {
            printf("invalid packet, type %i", packet.type);
        }
        //Send the message back to client
        write(sock , &ok, sizeof ok);
    }
    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }

    //Free the socket pointer
    free(socket_desc);

    return 0;
}
static int socket_desc = -1;
void signal_callback_handler(int signum)
{
    printf("Caught signal %d\n",signum);
    // Cleanup and close up stuff here
    close(socket_desc);
    // Terminate program
    exit(signum);
}
int serve_forever() {
    signal(SIGINT, signal_callback_handler);
    printf("pid: %d \n",getpid());
    virtualMouse.setupMouse();
    int new_socket , c , *new_sock;
    struct sockaddr_in server , client;
    char *message;
    socket_desc = socket(AF_INET, SOCK_STREAM, IPPROTO_IP );
    if (socket_desc == -1){
        printf("Count not create socket\n");
        return -1;
    }
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(LISTEN_PORT);
    //Bind
    if (bind(socket_desc, (struct  sockaddr *)& server, sizeof (server)) < 0){
        puts("bind failed");
        return 1;
    }
    //Listen
    listen(socket_desc, 3);
    //Accept and incoming connection
    printf("Listening at %i,Waiting for incoming connection...\n", LISTEN_PORT);
    c = sizeof(struct sockaddr_in);

    while( (new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        puts("Connection accepted");

        pthread_t sniffer_thread;
        new_sock = static_cast<int *>(malloc(1));
        *new_sock = new_socket;

        if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }

        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( sniffer_thread , NULL);
        puts("Handler assigned");
    }
    if (new_socket<0)
    {
        perror("accept failed");
        return 1;
    }

    return 0;
}
