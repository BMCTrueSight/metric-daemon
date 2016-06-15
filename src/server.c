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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define MAXBUF 1024*1024

const char *m_count = "COUNT";
const char *m_status = "STATUS";

double process_request(const char *request) {
    double value = 0;
    if (strcmp(request, m_count) == 0) {
        value = 1.0;
    } else if (strcmp(request, m_status) == 0) {
        value = 2.0;
    } else {
        value = -1.0;
    }
    return value;
}

void serve_measurements(int sd) {
    socklen_t len;
    int n;
    char bufin[MAXBUF];
    struct sockaddr_in remote;
    char request[MAXBUF + 1];

    // need to know how big address struct is, len must be set before the call to recvfrom()
    len = sizeof(remote);

    while (1) {
        // read a datagram from the socket (put result in bufin) */
        n = recvfrom(sd, bufin, MAXBUF, 0, (struct sockaddr *) &remote, &len);

        if (n < 0) {
            perror("Error receiving data");
        } else {
            strncpy(request, bufin, n);
            request[n - 1] = '\0';

            double measurement = process_request(request);
            // Output the address of sender and their metric request
            fprintf(stderr, "from: %s port: %d, request: %s, response: %f\n",
                    inet_ntoa(remote.sin_addr), ntohs(remote.sin_port),
		    request, measurement);
            n = sprintf(bufin, "%f", measurement);
            /* Got something, just send it back */
            sendto(sd, bufin, n, 0, (struct sockaddr *) &remote, len);
        }
    }
}

int main(int argc, char *argv[]) {
    int ld;
    struct sockaddr_in skaddr;
    socklen_t length;
    int port = 12345;

    if (argc == 2) {
        port = atoi(argv[2]);
    }

    // create a socket
    // IP protocol family (PF_INET)
    // UDP protocol (SOCK_DGRAM)
    if ((ld = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        fprintf(stderr, "Problem creating socket\n");
        exit(1);
    }

    // establish our address
    // address family is AF_INET
    // our IP address is INADDR_ANY (any of our IP addresses)
    // the port number is assigned by the kernel
    skaddr.sin_family = AF_INET;
    skaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    skaddr.sin_port = htons(port);

    if (bind(ld, (struct sockaddr *) &skaddr, sizeof(skaddr)) < 0) {
        fprintf(stderr, "Problem binding\n");
        exit(0);
    }

    // find out what port we were assigned and print it out

    length = sizeof(skaddr);
    if (getsockname(ld, (struct sockaddr *) &skaddr, &length) < 0) {
        fprintf(stderr, "Error getsockname\n");
        exit(1);
    }

    // port number's are network byte order, we have to convert to
    // host byte order before printing !
    fprintf(stderr, "The server UDP port number is %d\n", ntohs(skaddr.sin_port));

    // Handle requests for measurements
    serve_measurements(ld);
    return 0;
}
