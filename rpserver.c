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
	extern char *optarg;
	extern int optind;
	int c, err = 0; 
	int port = 1234;	/* port number */
	static char usage[] = "usage: %s [-d] [-p port]\n";

	while ((c = getopt(argc, argv, "dp:")) != -1)
	{
		switch (c) {
		case 'p':
			port = atoi(optarg);
			/* if (port < 1024 || port > 65535) { */
			if (port < 0 || port > 65535) {
				fprintf(stderr, "invalid port number: %s\n", optarg);
				err = 1;
			}
			break;
		case '?':
			err = 1;
			break;
		}
	}
	if (err || (optind < argc)) {
		fprintf(stderr, usage, argv[0]);
		exit(1);
	}
	set_sock(port);
}
	/* would like to split into different method here*/
	
set_sock(int port)
{
	int svc;				/* listening socket providing service */
	int rqst;				/* socket accepting the request */
	struct sockaddr_in my_addr;		/* address of this service */
	struct sockaddr_in client_addr;		/* client's address */
	int sockoptval = 1;
	socklen_t alen = sizeof(client_addr);	/* length of address structure */


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

	printf("SERVER READY\n");
	
	int pid;
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
		switch (pid = fork()) 
		{
		case 0:		/* a fork returns 0 to the child */
			printf("I am the child process: pid=%d\n", getpid());
			child_socket(rqst);
			break;

		default:	/* a fork returns a pid to the parent */
			printf("I am the parent process: pid=%d, child pid=%d\n", getpid(), pid);
			close(rqst);
			wait(NULL); /* For testing purposes. Delete later*/
			exit(0);
			break;

		case -1:	/* something went wrong */
			perror("fork");
			exit(1);
		}
		
	}
}

child_socket(int rqst)
{
	printf("connected\n");
	char buffer[256];
	int nbytes = read(rqst, buffer, 256); /* read up to MAXBUF bytes */
	if(nbytes == -1)
	{
		perror("Error:");
	}
	shutdown(rqst, SHUT_RD);   /* close the connection */
	printf("bytes read: %d \nMessage: %s\n", nbytes, buffer);
	
	/* Close standard input and standard output*/
	close(0);
	close(1);
	
	/* dup socket onto file descriptor 1 (standard output)*/
	dup2(rqst, 1);
	
	exit(0);
}