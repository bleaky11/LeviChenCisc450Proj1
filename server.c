#include "TCP.h"
#define MAXPENDING 5 /* Maximum outstanding connection requests */

int main(){
    char *localHost = "127.0.0.1"; //Hardcoding in local host as ip
    int servSock; //Socket descripter for serverside
    int cliSock; //Socker descripter for clientside
    struct sockaddr_in serverAddr; //Server address
    struct sockaddr_in clientAddr; //Client address
    unsigned short servPort = 12345; //Server port
    unsigned int cliLen; //Length of client address

    /* Create socket for incoming connections */
    if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError( "socket () failed");

    /* Construct local address structure */
    memset(&serverAddr, 0, sizeof(serverAddr)); /* Zero out structure */
    serverAddr.sin_family = AF_INET; /* Internet address family */
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface*/
    serverAddr.sin_port = htons(servPort); /* Local port */

    /* Bind to the local address */
    if (bind(servSock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) <0)
        DieWithError ( "bind () failed");

    /* Mark the socket so it will listen for incoming connections */
    if (listen(servSock, MAXPENDING) < 0)
        DieWithError("listen() failed") ;
    for (;;) /* Run forever */
    {
        /* Set the size of the in-out parameter */
        cliLen = sizeof(clientAddr);
        /* Wait for a client to connect */
        if ((cliSock = accept(servSock, (struct sockaddr *) &clientAddr, &cliLen)) < 0)
            DieWithError("accept() failed");
        /* clntSock is connected to a client! */
        printf("Handling client %s\n", inet_ntoa(clientAddr.sin_addr));
        printf("Starting handling\n");
        HandleTCPClient (cliSock) ;
    }
    return 0;
}