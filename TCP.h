#include <sys/types.h>/*for sys/socker.h functions to work*/
#include <sys/socket.h>/*for socket(), connect(), send(), and recv()*/
#include <unistd.h> /*for socket.h*/
#include <stdio.h> /*for printf(), fprintf(), and perror() */
#include <arpa/inet.h> /*for sockaddr_in and inet_addr()*/
#include <stdlib.h> /*for exit()*/
#include <string.h>

void DieWithError(char *errorMessage);
void HandleTCPClient(int clntSocket);
