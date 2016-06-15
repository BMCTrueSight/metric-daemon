//
// Copyright 2016 BMC Software, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#define BUFSIZE 1024

/* 
 * error - wrapper for perror
 */
void error(char *msg) {
    perror(msg);
    exit(0);
}

int main(int argc, char **argv) {
    int sockfd, portno, n;
    socklen_t serverlen;
    struct sockaddr_in serveraddr;
    struct hostent *server;
    char *hostname;
    char request[BUFSIZE];
    char response[BUFSIZE];

    /* check command line arguments */
    if (argc > 4) {
        fprintf(stderr, "usage: %s <hostname> <port> [request]\n", basename(argv[0]));
        exit(0);
    }
    hostname = argv[1];
    portno = atoi(argv[2]);

    /* socket: create the socket */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    /* gethostbyname: get the server's DNS entry */
    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host as %s\n", hostname);
        exit(0);
    }

    /* build the server's Internet address */
    memset(&serveraddr, '\0', sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    memcpy(server->h_addr, &serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(portno);

    // Set our request/response buffers to empty values
    memset(request, '\0', BUFSIZE);
    memset(response, '\0', BUFSIZE);

    // Use the passed in request, or if not specified prompt the user for a request
    if (argc == 4) {
        memcpy(request, argv[3], strlen(argv[3]));
    }
    else {
        printf("Please enter request: ");
        char *s = fgets(request, BUFSIZE, stdin);
	// Remove the new line character
	request[strlen(request) - 1] = '\0';
    }

    /* send the message to the server */
    serverlen = sizeof(serveraddr);
    n = sendto(sockfd, request, strlen(request)+1, 0, (struct sockaddr *) &serveraddr, serverlen);
    if (n < 0) {
        error("ERROR in sendto");
    }

    /* print the server's reply */
    n = recvfrom(sockfd, response, BUFSIZE, 0, (struct sockaddr *) &serveraddr, &serverlen);
    if (n < 0) {
        error("ERROR in recvfrom");
    }
    fprintf(stdout, "request: %s, response: %s\n", request, response);
    fflush(stdout);
    return 0;
}
