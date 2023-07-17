#include "iprout.h"


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
