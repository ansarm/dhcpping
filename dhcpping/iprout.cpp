#include "iprout.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "winsock2.h"
#include "Ws2tcpip.h"

void create_random_ip(char * ip)
{
	char octet[4];
	strcpy(ip, "\0");
	itoa((unsigned char)rand(), octet, 10);
	strcat(ip,octet);
	strcat(ip, ".");
	itoa((unsigned char)rand(), octet, 10);
	strcat(ip,octet);
	strcat(ip, ".");
	itoa((unsigned char)rand(), octet, 10);
	strcat(ip,octet);
	strcat(ip, ".");
	itoa((unsigned char)rand(), octet, 10);
	strcat(ip,octet);
	strcat(ip,"\0");
}





unsigned short checksum(unsigned short * buffer, int size)


{
	unsigned long cksum =0 ;
	while (size >1)
	{
		cksum += *buffer++;
		size -= sizeof(unsigned short);
	}
	if (size)
		cksum += *(unsigned char*) buffer;
	cksum = (cksum >> 16) + (cksum & 0xffff);
	cksum += (cksum >> 16);
	return (unsigned short)(~cksum);
}

void handle_wsaerror (int error)
{

	switch (error)
	{
	case WSANOTINITIALISED:
		printf ("A successful WSAStartup call must occur before using this function.\n");
		break;
	case WSAENETDOWN:
		printf ("The network subsystem has failed.\n");
		break;
	case WSAEACCES:
		printf ("The requested address is a broadcast address, but the appropriate flag was not set. \n");
		break;
	case WSAEINVAL:
		printf ("An unknown flag was specified, or MSG_OOB was specified for a socket with SO_OOBINLINE enabled. \n");
		break;
	case WSAEINTR:
		printf ("A blocking Windows Sockets 1.1 call was canceled through WSACancelBlockingCall. \n");
		break;
	case WSAEINPROGRESS:
		printf ("A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function. \n");
		break;
	case WSAEFAULT:
		printf ("The buf or to parameters are not part of the user address space, or the tolen parameter is too small. \n");
		break;
	case WSAENETRESET:
		printf ("The connection has been broken due to keep-alive activity detecting a failure while the operation was in progress. \n");
		break;
	case WSAENOBUFS:
		printf ("No buffer space is available. \n");
		break;
	case WSAENOTCONN:
		printf ("The socket is not connected (connection-oriented sockets only). \n");
		break;
	case WSAENOTSOCK:
		printf ("The descriptor is not a socket. \n");
		break;
	case WSAEOPNOTSUPP:
		printf ("MSG_OOB was specified, but the socket is not stream-style such as type SOCK_STREAM, OOB data is not supported in the communication domain associated with this socket, or the socket is unidirectional and supports only receive operations. \n");
		break;
	case WSAESHUTDOWN:
		printf ("The socket has been shut down; it is not possible to sendto on a socket after shutdown has been invoked with how set to SD_SEND or SD_BOTH. \n");
		break;
	case WSAEWOULDBLOCK:
		printf ("The socket is marked as nonblocking and the requested operation would block. \n");
		break;
	case WSAEMSGSIZE:
		printf ("Tahe message is larger than the maximum supported by the underlying transport. \n");
		break;
	case WSAEHOSTUNREACH:
		printf ("The remote host cannot be reached from this host at this time. \n");
		break;
	case WSAECONNABORTED:
		printf ("The virtual circuit was terminated due to a time-out or other failure. The application should close the socket as it is no longer usable. \n");
		break;
	case WSAECONNRESET:
		printf ("The virtual circuit was reset by the remote side executing a hard or abortive close. For UPD sockets, the remote host was unable to deliver a previously sent UDP datagram and responded with a \"Port Unreachable\" ICMP packet. The application should close the socket as it is no longer usable. \n");
		break;
	case WSAEADDRNOTAVAIL:
		printf ("The remote address is not a valid address, for example, ADDR_ANY. \n");
		break;
	case WSAEAFNOSUPPORT:
		printf ("Addresses in the specified family cannot be used with this socket. \n");
		break;
	case WSAEDESTADDRREQ:
		printf ("A destination address is required. \n");
		break;
	case WSAENETUNREACH:
		printf ("The network cannot be reached from this host at this time. \n");
		break;
	case WSAETIMEDOUT:
		printf ("Request timed out\n");
		break;
	default:
		printf("Unknown Error %d\n",error);
	}
}