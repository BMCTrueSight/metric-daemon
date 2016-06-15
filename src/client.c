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
#include <ctype.h>
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

char program_name[BUFSIZE];

/* 
 * error - wrapper for perror
 */
void error(char *msg) {
    perror(msg);
    exit(0);
}

void usage() {
    fprintf(stderr, "usage: %s <hostname> <port> <bytecount|duration|number|percent>\n",
            basename(program_name));
    exit(0);
}

char *upper(char *s) {
    for (int i = 0; s[i]; i++) {
        s[i] = toupper(s[i]);
    }
    return s;
}

static const char *requests[] = {
        "bytecount",
        "duration",
        "number",
        "percent",
        NULL
};

int validate_request(const char *request) {
    int result = 1;

    for (int i = 0; requests[i]; i++) {
        if (strcmp(requests[i], request) == 0) {
            result = 0;
            break;
        }
    }
    return result;
}


int main(int argc, char **argv) {
    int sockfd, portno, n;
    socklen_t serverlen;
    struct sockaddr_in serveraddr;
    struct hostent *server;
    char *hostname;
    char request[BUFSIZE];
    char response[BUFSIZE];

    // Set our request/response buffers to empty values
    memset(request, '\0', BUFSIZE);
    memset(response, '\0', BUFSIZE);

    strcpy(program_name, argv[0]);

    // check command line arguments
    if (argc < 4) {
        usage();
    }

    hostname = argv[1];
    portno = atoi(argv[2]);
    if (validate_request(argv[3])) {
        usage();
    }
    else {
        memcpy(request, upper(argv[3]), strlen(argv[3]));
    }

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

    /* send the message to the server */
    serverlen = sizeof(serveraddr);
    n = sendto(sockfd, request, strlen(request) + 1, 0, (struct sockaddr *) &serveraddr, serverlen);
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
