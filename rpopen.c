/*
	rpopen: remote popen function
*/

#include <stdlib.h>	/* defines system calls */
#include <stdio.h>	/* needed for printf */
#include <string.h>	/* needed for memset */
#include <sys/socket.h>	/* needed for sockets */
#include <netinet/in.h>	/* needed for sockaddr_in */
#include <netdb.h>	/* needed for gethostbyname */

/* Don't for get to do error checking:
 * look into getopt
 * port number range */
FILE *
rpopen(char *host, int port, char *cmd) 
{
	int sock = conn_sock(host, port);
	
	write(sock, "hello", 16);
	
	printf("written\n");
	close(sock);
	return 0;
}

/*Function returns a created and bound socket */
int conn_sock(char *host, int port)
{
	struct sockaddr_in myaddr;	/* our address */
	int fd;	/* our socket */
	unsigned int alen;	/* length of address (for getsockname) */ /* Remove during clean up*/

	/* create a tcp/ip socket */
	/* request the Internet address protocol */
	/* and a reliable 2-way byte stream (TCP/IP) */

	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("cannot create socket");
		return 0;
	}

	printf("created socket: descriptor = %d\n", fd); /* Remove during clean up*/

	/* bind to an arbitrary return address */
	/* in this case, assume it's the client side, so we won't
	/* care about the port number as no application will connect here */
	/* INADDR_ANY is the IP address and 0 is the socket */
	/* htonl converts a long integer (e.g. address) to a network */
	/* representation (IP-standard byte ordering) */

	memset((void *)&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(0);

	if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
		perror("bind failed");
		return 0;
	}
	
	/* do not need below functions*/
	alen = sizeof(myaddr);
	if (getsockname(fd, (struct sockaddr *)&myaddr, &alen) < 0) {
		perror("getsockname failed");
		return 0;
	}

	printf("bind complete. Port number = %d\n", ntohs(myaddr.sin_port));
	/*do not need above functions*/
	
	struct hostent *hp;     /* host information */
	struct sockaddr_in servaddr;    /* server address */

	/* fill in the server's address and data */
	memset((char*)&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);

	/* look up the address of the server given its name */
	printf("gethostbyname\n");
	hp = gethostbyname(host);
	if (!hp) {
		fprintf(stderr, "could not obtain address of %s\n", host);
		return 0;
	}

	/* put the host's address into the server address structure */
	printf("memcpy\n");
	memcpy((void *)&servaddr.sin_addr, hp->h_addr_list[0], hp->h_length);

	/* connect to server */
	if (connect(fd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		perror("connect failed");
		return 0;
	}
	
	return fd;
}