#include "TCP.h"

#define RCVBUFSIZE 80 /*Size of receive buffer*/

int main(){
    char *localHost = "127.0.0.1";//Ip for localhost
    int sock; //Client side only needs server socket
    struct sockaddr_in serverAddr;//Server address 
    unsigned short servPort = 12345; //Server port
    char request[RCVBUFSIZE];
    FILE *file = NULL;

    printf("Enter the file name you are looking for: ");
    scanf("%s", request);//To get the file name

    /*Create a socket using TCP*/
    if((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP))<0)
        DieWithError("sock creation failed");

    /*Construct the server address structure*/
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(localHost);
    serverAddr.sin_port = htons(servPort);

    /*Establish connection to echo server*/
    if (connect(sock, (struct sockaddr*) &serverAddr, sizeof (serverAddr))<0)
        DieWithError("sock connection failed");
    
    int requestLen = strlen(request);

    printf("Requesting file %s\n", request);//For debugging

    //Sending request packet
    uint16_t reqLen = htons(requestLen); //Packet header: length of request
    uint16_t seq = htons(0); //Packet header: sequence num: 0
    char requestPacket[RCVBUFSIZE + 4];
    memcpy(requestPacket, &reqLen, 2);
    memcpy(requestPacket + 2, &seq, 2);
    strncpy(requestPacket + 4, request, RCVBUFSIZE); // Copy the file name into the remaining space
    requestPacket[requestLen + 4] = '\0';
    /*send the string*/
    if (send(sock, requestPacket, requestLen + 4, 0) < 0)
        DieWithError("Sending request failed");

    //Opening file so received data can be placed in
    sleep(1);
    char filename[RCVBUFSIZE];
    sprintf(filename, "%s", request);
    

    int bytesReceived;
    char recBuffer[RCVBUFSIZE + 4];

    while((bytesReceived = recv(sock, recBuffer, RCVBUFSIZE + 4, 0)) > 0){
        if(file == NULL){
            file = fopen(filename, "wb");
            if (!file){
                DieWithError("Failed to create new file");
            }
        }
        uint16_t count, seqNum;
        memcpy(&count, recBuffer, 2);  //Extract count
        memcpy(&seqNum, recBuffer + 2, 2);  //Extract sequence number

        count = ntohs(count);  //Convert from network byte order
        seqNum = ntohs(seqNum);

        if (count == 0) {
            printf("End of Transmission Packet with sequence number %d received\n", seqNum);
            break; //EOT received
        }

        fwrite(recBuffer + 4, 1, count, file); //Write data (excluding header)
        printf("Packet %d received with %d data bytes\n", seqNum, count);//For debugging
    }
    fclose(file);
    close(sock);
    printf("File transfer complete. Saved as '%s'.\n", filename);
    return 0;
}