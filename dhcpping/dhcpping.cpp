// dhcpping.cpp : Defines the entry point for the console application.
//

//to do 
// add dhcp release
//add simple output


#include "stdafx.h"

#define MAXPACKETSIZE	1500

#define SIO_RCVALL _WSAIOW(IOC_VENDOR,1)

//global variables
int thread_flag, server_id, req_addr;
char myip[64], reqip[64];
char hostname[64] = "\0";
int xid, server_count=0, server_list[16];

void print_dhcp_tag(DHCP_HDR * tag)
{
	in_addr addr;
	int options_next =0;
	char val, val2[MAXPACKETSIZE];
	int loop;

	printf("\n\n----------------\n");
	printf("DHCP Message Recieved\n");
	printf("----------------\n");
	addr.S_un.S_addr = tag->ipHdr.ip_srcaddr;
	printf("src ip: %s, ", inet_ntoa(addr));
	addr.S_un.S_addr = tag->ipHdr.ip_destaddr;
	printf("dest ip: %s\n", inet_ntoa(addr));
	printf("client hw addr: ");
	for (loop = 0; loop<tag->hlen; loop++)
		printf("%2.2x", tag->chaddr[loop]);
	printf("\n");
	addr.S_un.S_addr = tag->ciaddr;
	printf("client ip: %s, ", inet_ntoa(addr));
	printf("trans id: %d\n",tag->xid);
	printf("DHCP Options Follow\n");
	while(tag->options[options_next] != 255)
	{	
		//dhcp message type
		if (tag->options[options_next] == 53)
		{
			if (tag->options[options_next+2] == 1)
				printf("\tMessage DHCP Discover\n");
			if (tag->options[options_next+2] == 2)
			{
				printf("\tMessage DHCP Offer\n");
				server_list[server_count] = tag->siaddr;
				server_count++;
			}	
			if (tag->options[options_next+2] == 3)
				printf("\tMessage DHCP Request\n");
			if (tag->options[options_next+2] == 4)
				printf("\tMessage DHCP Decline\n");
			if (tag->options[options_next+2] == 5)
				printf("\tMessage DHCP ACK\n");
			if (tag->options[options_next+2] == 6)
				printf("\tMessage DHCP NAK\n");
			if (tag->options[options_next+2] == 7)
				printf("\tMessage DHCP RELEASE\n");
			if (tag->options[options_next+2] == 8)
				printf("\tMessage DHCP INFORM\n");						
	
		}

		//subnet mask
		if (tag->options[options_next] == 1)
		{
			addr.S_un.S_addr = (tag->options[options_next+2] << 24) |
								(tag->options[options_next+3] << 16) |
									(tag->options[options_next+4] << 8)|
									(tag->options[options_next+5]);
			addr.S_un.S_addr = ntohl(addr.S_un.S_addr);
			printf("\tSubnet Mask: %s\n",inet_ntoa(addr));

		}

		//router (default gateway)
		if (tag->options[options_next] == 3)
		{
			val = tag->options[options_next+1];
			for (loop=0;loop < val/4; loop++)
			{
				addr.S_un.S_addr = (tag->options[options_next+2+loop] << 24) |
									(tag->options[options_next+3+loop] << 16) |
										(tag->options[options_next+4+loop] << 8)|
										(tag->options[options_next+5+loop]);
				addr.S_un.S_addr = ntohl(addr.S_un.S_addr);
				printf("\tRouter: %s\n",inet_ntoa(addr));
			}
		}

			//DNS Server)
		if (tag->options[options_next] == 5)
		{
			val = tag->options[options_next+1];
			for (loop=0;loop < val/4; loop++)
			{
				addr.S_un.S_addr = (tag->options[options_next+2+loop] << 24) |
									(tag->options[options_next+3+loop] << 16) |
										(tag->options[options_next+4+loop] << 8)|
										(tag->options[options_next+5+loop]);
				addr.S_un.S_addr = ntohl(addr.S_un.S_addr);
				printf("\tName Server Option: %s\n",inet_ntoa(addr));
			}
		}

			//router (default gateway)
		if (tag->options[options_next] == 6)
		{
			val = tag->options[options_next+1];
			for (loop=0;loop < val/4; loop++)
			{
				addr.S_un.S_addr = (tag->options[options_next+2+loop] << 24) |
									(tag->options[options_next+3+loop] << 16) |
										(tag->options[options_next+4+loop] << 8)|
										(tag->options[options_next+5+loop]);
				addr.S_un.S_addr = ntohl(addr.S_un.S_addr);
				printf("\tDNS Server: %s\n",inet_ntoa(addr));
			}
		}
	
		//domain name
		if (tag->options[options_next] == 15)
		{
			ZeroMemory(val2, MAXPACKETSIZE);
			memcpy(val2, (tag->options + options_next +2), tag->options[options_next+1]);
			printf("\tDomain Name: %s\n",val2);
		}
	


		//get next parameter
		options_next								//next record is equal to 
			= options_next							//this record
			+ (char)tag->options[options_next+1]	//plus the length of the data in this record
			+2;										//plus the code and length octets

	}

	printf("---------------\n");
	printf("end of DHCP Message\n");
	printf("---------------\n");

}

void about(int argc, char * argv[])
{
	printf("usage:\n");
	printf("%s REQUEST -int int_ip [-mac mac_address] [-host hostname] [-req req_ip]\n", argv[0]);
	printf("request an ip address from DHCP with:\n");
	printf("int_ip\t\t:listening interface IP Address\n\t\t(use static IP)\n");
	printf("mac_address\t:optional MAC Address\n\t\t(use your own mac address for best results)\n");
	printf("req_ip\t\t:requested ip_address\n\t\t(leave this blank if you wish, it will take the first ip address)\n");
	printf("hostname\t:optional client hostname\n\n") ;
	printf("%s version 0.1(beta) by Ansar Mohammed\n", argv[0]);
	printf("dhcp testing utility\n");
	printf("send all bug reports/comments to ansarm@gmail.com\n");
	printf("this is beta quality, some things may not work as expected\n");
	printf("\n");
	printf("WARNING! WARNING! WARNING! WARNING! WARNING! WARNING! WARNING! WARNING! \n"); 
	printf("# ISC DHCPD will give you a new IP Address every time you run this program\n");
	printf("# even if you use the same MAC address; normal DHCP servers do not do this.\n");
	printf("# This program; if used incorrectly; can cause a Denial of Service on your\n");
	printf("# DHCP Server, and affect the stability of your network\n");
    	
}

void dhcp_discover(in_addr ip, char * hostname, MAC_ADDRESS mac, int xid)
{
	DHCP_HDR req;
	
	struct sockaddr_in remotehost;
	
	SOCKET s;
	BOOL opt= true;
	remotehost.sin_family=AF_INET;
	remotehost.sin_port= htons(1);
	remotehost.sin_addr.s_addr = inet_addr("255.255.255.255");
	char buf[MAXPACKETSIZE];
	int size;
	

	s = WSASocket(AF_INET,SOCK_RAW,IPPROTO_RAW,0,0,0);
	if (setsockopt(s, IPPROTO_IP, IP_HDRINCL, (char*)&opt, sizeof(opt))<0)
		handle_wsaerror(WSAGetLastError());
	if (setsockopt(s, SOL_SOCKET,SO_BROADCAST,(char*)&opt, sizeof(opt))<0)
		handle_wsaerror(WSAGetLastError());
	
	ZeroMemory(&req, sizeof(DHCP_HDR)); //clear memory
	//initalize IP HEader
	req.ipHdr.ip_destaddr = 0xFFFFFFFF;
	req.ipHdr.ip_srcaddr =0;
	req.ipHdr.ip_ttl = 128;
	req.ipHdr.ip_tos = 0;
	req.ipHdr.ip_totallength = htons(sizeof(DHCP_HDR));
	
	req.ipHdr.ip_verlen=0x45; //ip version4 + ip header length of 20
	req.ipHdr.ip_id =0;
	req.ipHdr.ip_offset =0 ;
	req.ipHdr.ip_protocol = 17; //UDP
	req.ipHdr.ip_checksum =0; //OS will calculate it for us;

	req.udpHdr.dst_portno = htons(67);
	req.udpHdr.src_portno = htons(68);
	req.udpHdr.udp_checksum = 0; //OS *should* calculat this for us
	req.udpHdr.udp_length = htons(sizeof(DHCP_HDR) - sizeof(IP_HDR)); // fill out later
	
	//create a DHCP discover message
	req.op = 1; //1 =BOOTREQUEST
	req.htype = 1; // this is the hardware type as defined.. I dont think this make much a difference
	req.hlen = 6; //ethernet address lenght 6 bytes
	req.hops = 0; // client must set # of hops to zero. relay agents may increment this
	req.xid = xid; //random transaction id number
	req.secs = htons(1);
	req.flags = 0; //dont need broadcast on this ip stack
	req.ciaddr = 0; //current IP Address
	req.yiaddr = 0; //my IP Address
	req.siaddr = 0; //Next serevr in bootstrap
	req.giaddr = 0; //DHCP Relay Agent Address
	memcpy(req.chaddr,&mac,req.hlen);  //hardware address
	ZeroMemory(req.sname,64);
	ZeroMemory(req.file,128);
	//magic cookie
	req.magic_cookie =htonl( 0x63825363);

	//DHCP Discover
	req.options[0] = 53;
	req.options[1] = 1;
	req.options[2] = 1;

	//client identifier
	req.options[3]= 61;
	req.options[4]= 7;
	req.options[5]=1;
	memcpy((req.options+5), &mac, sizeof(mac_address));

	//requested address
	req.options[11] = 50;
	req.options[12] = 4;
	memcpy(req.options+13, &req_addr, sizeof(req_addr));
	req.options[18] = 255;

	size = sizeof(req);
	ZeroMemory(buf, size);
	memcpy(buf, &req, sizeof(req));

	if (sendto(s, buf, MAXPACKETSIZE, 0, (SOCKADDR*)&remotehost,sizeof(remotehost))<0)
		handle_wsaerror(WSAGetLastError());
	closesocket(s);  
}

DWORD WINAPI recv_dhcp_offer( LPVOID pxid)
{
	SOCKET r;
	struct sockaddr_in localaddr;
	DWORD 	ioctl_in=1, outbytes;
	char buf[MAXPACKETSIZE];
	DHCP_HDR *offer;
	struct timeval Timeout;
	fd_set readfds;
	int timeout=10000;
	int to=1; // 5s.
	int offers=0;
	

	ZeroMemory(buf, MAXPACKETSIZE);
	r=WSASocket(AF_INET,SOCK_RAW, 0, 0,0,0);
	localaddr.sin_family=AF_INET;
	localaddr.sin_addr.s_addr = inet_addr(myip);
	localaddr.sin_port=68;
	BOOL got_offer=true;
	readfds.fd_count = 1;
	readfds.fd_array[0] = r;
	Timeout.tv_usec = timeout % 1000;
	Timeout.tv_sec = timeout / 1000;
	bind(r,(SOCKADDR*)&localaddr, sizeof(localaddr));
	
	WSAIoctl(r, SIO_RCVALL ,&ioctl_in,sizeof(ioctl_in), NULL, NULL, &outbytes, NULL, NULL);
	ZeroMemory(buf, MAXPACKETSIZE);
	while(got_offer)
	{
		if (select(1, &readfds, NULL, NULL, &Timeout)== 0)
		{
			if(offers ==0)
				printf("request timed out\n");
			got_offer = false;
		}
		else
		{
			if (recv(r, buf, MAXPACKETSIZE, 0)<0)
				handle_wsaerror(WSAGetLastError());
			offer = (DHCP_HDR*)buf;
			if ((offer->xid==xid)&& (offer->udpHdr.dst_portno == ntohs(68)))
			{
				print_dhcp_tag(offer);
				server_id = offer->siaddr;
				if ((req_addr == offer->yiaddr) || (req_addr==0))
				{  
					//if we are not asking for any particular
					//address, then accept the one they are giving us
					if (req_addr==0)
						req_addr = offer->yiaddr; 
					thread_flag =1;
				}
				offers++;
			}
				
		}
	}	
	closesocket(r);  
	return 0;
}


void dhcp_request(int xid,mac_address mac )

{	
    DHCP_HDR req;
	struct sockaddr_in remotehost;
	SOCKET s;
	BOOL opt= true;
	remotehost.sin_family=AF_INET;
	remotehost.sin_port= htons(1);
	remotehost.sin_addr.s_addr = inet_addr("255.255.255.255");
	char buf[MAXPACKETSIZE];
	int size;

	s = WSASocket(AF_INET,SOCK_RAW,IPPROTO_RAW,0,0,0);
	if (setsockopt(s, IPPROTO_IP, IP_HDRINCL, (char*)&opt, sizeof(opt))<0)
		handle_wsaerror(WSAGetLastError());
	if (setsockopt(s, SOL_SOCKET,SO_BROADCAST,(char*)&opt, sizeof(opt))<0)
		handle_wsaerror(WSAGetLastError());
		
	ZeroMemory(&req, sizeof(DHCP_HDR)); //clear memory
	//initalize IP HEader
	req.ipHdr.ip_destaddr = 0xFFFFFFFF;
	req.ipHdr.ip_srcaddr =0;
	req.ipHdr.ip_ttl = 128;
	req.ipHdr.ip_tos = 0;
	req.ipHdr.ip_totallength = htons(sizeof(DHCP_HDR));
	
	req.ipHdr.ip_verlen=0x45; //ip version4 + ip header length of 20
	req.ipHdr.ip_id =0;
	req.ipHdr.ip_offset =0 ;
	req.ipHdr.ip_protocol = 17; //UDP
	req.ipHdr.ip_checksum =0; //OS will calculate it for us;

	req.udpHdr.dst_portno = htons(67);
	req.udpHdr.src_portno = htons(68);
	req.udpHdr.udp_checksum = 0; //OS *should* calculat this for us
	req.udpHdr.udp_length = htons(sizeof(DHCP_HDR) - sizeof(IP_HDR)); // fill out later
	
	//create a DHCP discover message
	req.op = 1; //1 =BOOTREQUEST
	req.htype = 1; // this is the hardware type as defined.. I dont think this make much a difference
	req.hlen = 6; //ethernet address lenght 6 bytes
	req.hops = 0; // client must set # of hops to zero. relay agents may increment this
	req.xid = xid; //random transaction id number
	req.secs = htons(1);
	req.flags = 0; //dont need broadcast on this ip stack
	req.ciaddr = 0; //current IP Address
	req.yiaddr = 0; //my IP Address
	req.siaddr = 0; //Next serevr in bootstrap
	req.giaddr = 0; //DHCP Relay Agent Address
	memcpy(req.chaddr,&mac,req.hlen);  //hardware address
	ZeroMemory(req.sname,64);
	ZeroMemory(req.file,128);
	//magic cookie
	req.magic_cookie =htonl( 0x63825363);

	//DHCP Request
	req.options[0] = 53;
	req.options[1] = 1;
	req.options[2] = 3;

	//server identifier
	req.options[3]= 54;
	req.options[4] = 4;
	memcpy((req.options+5), &server_id,sizeof(server_id)); 
	
	//client identifier
	req.options[9]= 61;
	req.options[10]= 7;
	req.options[11]=1;
	memcpy((req.options+12), &mac, sizeof(mac_address));

	//requested address
	req.options[18] = 50;
	req.options[19] = 4;
	memcpy(req.options+20, &req_addr, sizeof(req_addr));

	//hostname



	req.options[24] = 255;
	size = sizeof(req);
	ZeroMemory(buf, size);
	memcpy(buf, &req, sizeof(req));

	if (sendto(s, buf, MAXPACKETSIZE, 0, (SOCKADDR*)&remotehost,sizeof(remotehost))<0)
		handle_wsaerror(WSAGetLastError());
	closesocket(s);  
}
void dhcp_release(mac_address mac, int ciaddr,int siaddr)

{
DHCP_HDR req;
	struct sockaddr_in remotehost;
	SOCKET s;
	BOOL opt= true;
	remotehost.sin_family=AF_INET;
	remotehost.sin_port= htons(1);
	remotehost.sin_addr.s_addr = inet_addr("255.255.255.255");
	char buf[MAXPACKETSIZE];
	int size;

	s = WSASocket(AF_INET,SOCK_RAW,IPPROTO_RAW,0,0,0);
	if (setsockopt(s, IPPROTO_IP, IP_HDRINCL, (char*)&opt, sizeof(opt))<0)
		handle_wsaerror(WSAGetLastError());
	if (setsockopt(s, SOL_SOCKET,SO_BROADCAST,(char*)&opt, sizeof(opt))<0)
		handle_wsaerror(WSAGetLastError());
		
	ZeroMemory(&req, sizeof(DHCP_HDR)); //clear memory
	//initalize IP HEader
	req.ipHdr.ip_destaddr = 0xFFFFFFFF;
	req.ipHdr.ip_srcaddr =0;
	req.ipHdr.ip_ttl = 128;
	req.ipHdr.ip_tos = 0;
	req.ipHdr.ip_totallength = htons(sizeof(DHCP_HDR));
	
	req.ipHdr.ip_verlen=0x45; //ip version4 + ip header length of 20
	req.ipHdr.ip_id =0;
	req.ipHdr.ip_offset =0 ;
	req.ipHdr.ip_protocol = 17; //UDP
	req.ipHdr.ip_checksum =0; //OS will calculate it for us;

	req.udpHdr.dst_portno = htons(67);
	req.udpHdr.src_portno = htons(68);
	req.udpHdr.udp_checksum = 0; //OS *should* calculat this for us
	req.udpHdr.udp_length = htons(sizeof(DHCP_HDR) - sizeof(IP_HDR)); // fill out later
	
	
	req.op = 1; //1 =BOOTREQUEST
	req.htype = 1; // this is the hardware type as defined.. I dont think this make much a difference
	req.hlen = 6; //ethernet address lenght 6 bytes
	req.hops = 0; // client must set # of hops to zero. relay agents may increment this
	req.xid = xid; //random transaction id number
	req.secs = htons(1);
	req.flags = 0; //dont need broadcast on this ip stack
	req.ciaddr = ciaddr; //current IP Address
	req.yiaddr = 0; //my IP Address
	req.siaddr = 0; //Next serevr in bootstrap
	req.giaddr = 0; //DHCP Relay Agent Address
	memcpy(req.chaddr,&mac,req.hlen);  //hardware address
	ZeroMemory(req.sname,64);
	ZeroMemory(req.file,128);
	//magic cookie
	req.magic_cookie =htonl( 0x63825363);

	//DHCP Release
	req.options[0] = 53;
	req.options[1] = 1;
	req.options[2] = 7;

	//server identifier
	req.options[3]= 54;
	req.options[4] = 4;
	memcpy((req.options+5), &siaddr,sizeof(siaddr)); 
	
	//client identifier
	req.options[9]= 61;
	req.options[10]= 7;
	req.options[11]=1;
	memcpy((req.options+12), &mac, sizeof(mac_address));



	//hostname

	req.options[24] = 255;
	size = sizeof(req);
	ZeroMemory(buf, size);
	memcpy(buf, &req, sizeof(req));

	if (sendto(s, buf, MAXPACKETSIZE, 0, (SOCKADDR*)&remotehost,sizeof(remotehost))<0)
		handle_wsaerror(WSAGetLastError());
	closesocket(s);  
}



int strtomac(char *strMac, mac_address * mac)
{
	unsigned int loop, loop2=0;
	
	for (loop = 0; loop < strlen(strMac); loop++)
	{
		if ((strMac[loop]-48) >10)
			mac->byte[loop2] = (strMac[loop]-55) << 4;
		else
			mac->byte[loop2] = (strMac[loop]-48) << 4;
		if ((strMac[loop+1]-48) >10)
			mac->byte[loop2] = (strMac[loop+1]-55) | mac->byte[loop2];
		else
			mac->byte[loop2] = (strMac[loop+1]-48) | mac->byte[loop2];
		loop++;
		loop2++;
	}
	if (loop2!=5)
		return 1; //mac address is shorter than expected

return 0;
}


int main(int argc, char* argv[])
{
	mac_address mac;
	in_addr ip, tmp;
	int loop =0;
	HANDLE hThread;
	WSADATA lpWSAData;
	WORD wVersionRequested;
	wVersionRequested = MAKEWORD( 2, 2 );
 	WSAStartup(wVersionRequested,&lpWSAData);
	int timeout=5000;
	ip.S_un.S_addr =0;



	if (argc <2)
	{
		about(argc, argv);
		exit(1);
	}
	if ((stricmp(argv[1], "REQUEST")!=0) && (stricmp(argv[1], "RELEASE")!=0))
	{
		about(argc, argv);
		exit(1);
	}
	else
	{
		for (loop= 2; loop <argc; loop++)
		{
			if (strcmp(argv[loop],"-mac")==0)
			{
				if (!strtomac(argv[loop+1], &mac))
				{
					printf("illegal mac address\n");
					about(argc,argv);
					exit(1);
				}
				loop++;
			}
			else
				if (strcmp(argv[loop],"-int")==0)
				{
					strcpy(myip,argv[loop+1]);
					loop++;
				}
				else
					if (strcmp(argv[loop],"-host")==0)
					{
						strcpy(hostname, argv[loop+1]);
						loop++;
					}
					else
						if(strcmp(argv[loop],"-req")==0)
					{
						strcpy(reqip, argv[loop+1]);
						req_addr = inet_addr(reqip);
						loop++;
					}
		}
	}
	
	printf("Working....\n");
	printf("using hostname: %s\n", hostname );
	printf("listening on interface: %s\n", myip);
	printf("requesting ip address: %s\n",reqip);
	printf("using mac address: ");
	for (loop=0; loop<6; loop++)
		printf("%2.2x", mac.byte[loop]);
		
	printf("\n");

	srand( (unsigned)time( NULL ) );
	xid = rand();


	thread_flag = 0;
	hThread = CreateThread(NULL, 0, recv_dhcp_offer, &xid, 0, NULL);
	if (hThread!= NULL)
	{
		Sleep(500);	
		printf("initalizing listening threads.....\n");
	}
	//create DHCP Discover packet
	dhcp_discover(ip, hostname, mac, xid);
	for (loop=0; loop<8; loop++)
	{
        if (thread_flag ==0)
		{
			Sleep(250); //check four times every second
		}
		else
		{
			dhcp_request( xid, mac ); //we have a valid request to respond to
			thread_flag=0;
		}
	}
	
	Sleep(2500);
	printf("shutting down.....\n");
	Sleep(2500);
	if(server_count !=0)
	{
		printf("\nfound %d DHCP Server(s)\n", server_count);
		while (server_count>0)
		{
			tmp.S_un.S_addr = server_list[server_count-1];
			printf("server: %s\n", inet_ntoa(tmp));
			server_count--;
		}
	}
	else
		printf("no DHCP Servers found on interface %s\n", myip);

	return 0;
}


