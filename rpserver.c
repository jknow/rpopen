/*
	rpserver: remote popen server
*/

#include <stdlib.h>	/* defines system calls */
#include <stdio.h>	/* needed for printf */
#include <string.h>	/* for memset */
#include <sys/types.h>
#include <sys/socket.h>	/* needed for sockets */
#include <netinet/in.h>	/* needed for sockaddr_in */
#include <errno.h>
#include <unistd.h>

main(int argc, char **argv)
{
	int svc;        /* listening socket providing service */
	int port = 1234;	/* port number */
	int rqst;       /* socket accepting the request */
	socklen_t alen;       /* length of address structure */
	struct sockaddr_in my_addr;    /* address of this service */
	struct sockaddr_in client_addr;  /* client's address */
	int sockoptval = 1;


	/* create a TCP/IP socket */
	if ((svc = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("cannot create socket");
		exit(1);
	}

	/* allow immediate reuse of the port */
	setsockopt(svc, SOL_SOCKET, SO_REUSEADDR, &sockoptval, sizeof(int));

	/* bind the socket to our source address */
	memset((char*)&my_addr, 0, sizeof(my_addr));  /* 0 out the structure */
	my_addr.sin_family = AF_INET;   /* address family */
	my_addr.sin_port = htons(port);
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(svc, (struct sockaddr *)&my_addr, sizeof(my_addr)) < 0) {
		perror("bind failed");
		exit(1);
	}

	/* set the socket for listening (queue backlog of 5) */
	if (listen(svc, 5) < 0) {
		perror("listen failed");
		exit(1);
	}

	/* loop, accepting connection requests */
	for (;;) {
		while ((rqst = accept(svc, (struct sockaddr *)&client_addr, &alen)) < 0) {
			/* we may break out of accept if the system call */
                        /* was interrupted. In this case, loop back and */
                        /* try again */
                        if ((errno != ECHILD) && (errno != ERESTART) && (errno != EINTR)) {
                                perror("accept failed");
                                exit(1);
                        } 
                }
		/* the socket for this accepted connection is rqst */
		printf("connected\n");
		char buffer[32];
		int nbytes = read(rqst, buffer, 16); /* read up to MAXBUF bytes */
		if(nbytes == -1)
		{
			perror("Error:");
		}
		printf("bytes read: %d \nMessage: %s\n", nbytes, buffer);
		shutdown(rqst, 2);    /* close the connection */
	}
}