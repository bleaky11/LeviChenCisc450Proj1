#include "TCP.h"
#define RCVBUFSIZE 84 /* Size of receive buffer */

void HandleTCPClient(int cliSock){
    char requestBuff[RCVBUFSIZE];
    int requestSize;

    if ((requestSize = recv(cliSock, requestBuff, RCVBUFSIZE - 1, 0)) < 0)
    {
        DieWithError("recv() failed") ;
    }if(requestSize == 0)
        DieWithError("No data received");
    requestBuff[requestSize] = '\0'; //To end the string
    printf("File %s requested\n", requestBuff+4);

    FILE *file = fopen(requestBuff + 4, "rb"); //To open the file
    if (file == NULL)
        DieWithError("File not found");

    char fileChunks[80];//To send data in chunks of 80 bytes
    int bytesRead; //To keep track of bytes being read
    uint16_t sequence = 1; //To keep track of the number of packets sent

    while((bytesRead = fread(fileChunks, 1, 80, file)) > 0){ //To keep the loop going until the file is finished being read
        uint16_t count = htons(bytesRead); //To be put in header of packet as number of bytes
        uint16_t seq = htons(sequence); //To be put in header of packet as the seq-th packet sent

        //To send header of packet
        if(send(cliSock, &count, sizeof(count), 0) != sizeof(count) || send(cliSock, &seq, sizeof(seq), 0) != sizeof(seq))
            DieWithError("Failed to send Header"); //If size of count or seq in header is incorrect
        
        //To send data
        if(send(cliSock, fileChunks, bytesRead, 0) != bytesRead)
            DieWithError("Failed to send data"); //If size of data sent is incorrect

        printf("Packet %d transmitted with %d bytes\n", sequence, bytesRead);//Mainly for debugging, shows when a packet is sent
        sequence++;
    }

    uint16_t eotCount = 0;  // to indicate EOT
    uint16_t eotSeq = htons(sequence);  //Final sequence number
    if(send(cliSock, &eotCount, sizeof(eotCount), 0) != sizeof(eotCount) || send(cliSock, &eotSeq, sizeof(eotSeq), 0) != sizeof(eotSeq))
            DieWithError("Failed to EOT"); //If size of count or seq in header is incorrect
    printf("End of transmission packet sent\n");
    fclose(file);
    close(cliSock);
}