#include <string.h>
#include <stdio.h>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#define MAXHOSTNAMELEN 64
#define snprintf _snprintf
#define herror
#define socklen_t int
#pragma comment (lib, "WS2_32.lib")
#else /* #ifdef _WIN32 */
#include <unistd.h>
#include <netdb.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#endif /* #else _WIN32 */

/// deprecated
void gethostbynameDemo()
{
	//struct hostent {
	//	char  *h_name;            /* official name of host */
	//	char **h_aliases;         /* alias list */
	//	int    h_addrtype;        /* host address type */
	//	int    h_length;          /* length of address */
	//	char **h_addr_list;       /* list of addresses */
	//}

	//struct hostent {
	//	char    FAR * h_name;           /* official name of host */
	//	char    FAR * FAR * h_aliases;  /* alias list */
	//	short   h_addrtype;             /* host address type */
	//	short   h_length;               /* length of address */
	//	char    FAR * FAR * h_addr_list; /* list of addresses */
	//#define h_addr  h_addr_list[0]          /* address, for backward compat */
	//};


	char **pptr;
	char str_ip[INET_ADDRSTRLEN];
	struct hostent *hptr;

	if ((hptr = gethostbyname("www.baidu.com")) == NULL)
	{
		fprintf(stderr, "gethostbyname error");
		return;
	}
	printf("official host name: %s\n", hptr->h_name);

	for (pptr = hptr->h_aliases; *pptr != NULL; ++pptr)
	{
		printf("\talials: %s\n", *pptr);
	}

	switch (hptr->h_addrtype)
	{
	case AF_INET:
		pptr = hptr->h_addr_list;
		for (; *pptr != NULL; ++pptr)
			printf("\taddress: %s\n",
				inet_ntop(hptr->h_addrtype, *pptr,
					str_ip, sizeof(str_ip)));
		break;
	default:
		fprintf(stderr, "unknown address type");
		break;
	}
}

//#include <sys/socket.h>
//#include <netdb.h>
//
//int getaddrinfo(const char *restrict nodename, /* host 或者IP地址 */
//	const char *restrict servname, /* 十进制端口号 或者常用服务名称如"ftp"、"http"等 */
//	const struct addrinfo *restrict hints, /* 获取信息要求设置 */
//	struct addrinfo **restrict res); /* 获取信息结果 */
//
//void freeaddrinfo(struct addrinfo *ai);

#ifdef _WIN32
void getaddrinfoDemo()
{
	struct addrinfo hints;
	struct addrinfo *res, *cur;
	int ret;
	struct sockaddr_in *addr;
	char m_ipaddr[16];

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;     /* Allow IPv4 */
	hints.ai_flags = AI_PASSIVE;   /* For wildcard IP address */
	hints.ai_protocol = 0;         /* Any protocol */
	hints.ai_socktype = SOCK_STREAM;

	ret = getaddrinfo("www.baidu.com", NULL, &hints, &res);

	if (ret == -1) {
		perror("getaddrinfo");
		return;
	}

	for (cur = res; cur != NULL; cur = cur->ai_next) {
		addr = (struct sockaddr_in *)cur->ai_addr;
		sprintf(m_ipaddr, "%d.%d.%d.%d",
			(*addr).sin_addr.S_un.S_un_b.s_b1,
			(*addr).sin_addr.S_un.S_un_b.s_b2,
			(*addr).sin_addr.S_un.S_un_b.s_b3,
			(*addr).sin_addr.S_un.S_un_b.s_b4);
		printf("%s\n", m_ipaddr);
	}

	freeaddrinfo(res);
}
#endif // _WIN32

void printAddressInfo(const struct addrinfo *addr)
{
	char str[INET6_ADDRSTRLEN];
	short port;

	printf("ai_family: ");
	switch (addr->ai_family)
	{
	case AF_INET: {
		struct sockaddr_in* in;

		printf("ipv4\n");
		in = (struct sockaddr_in *)(addr->ai_addr);
		port = ntohs(in->sin_port);
		inet_ntop(AF_INET, &in->sin_addr, str, sizeof(str));
		break;
	}
	case AF_INET6: {
		struct sockaddr_in6* in6;

		in6 = (struct sockaddr_in6 *)(addr->ai_addr);
		port = ntohs(in6->sin6_port);
		inet_ntop(AF_INET6, &in6->sin6_addr, str, sizeof(str));
		printf("ipv6\n");
		break;
	}
	default:
		printf("Unknown\n");
		break;
	}

	printf("ai_socktype: ");
	switch (addr->ai_socktype)
	{
	case SOCK_STREAM:
		printf("stream\n");
		break;
	case SOCK_DGRAM:
		printf("dgram\n");
		break;
	case SOCK_SEQPACKET:
		printf("seqpacket\n");
		break;
	case SOCK_RAW:
		printf("raw\n");
		break;
	default:
		printf("others\n");
		break;
	}

	printf("protocol: ");
	switch (addr->ai_protocol)
	{
	case IPPROTO_TCP:
		printf("tcp\n");
		break;
	case IPPROTO_UDP:
		printf("udp\n");
		break;
	case IPPROTO_SCTP:
		printf("sctp\n");
		break;
	default:
		printf("others\n");
		break;
	}

	printf("address: %s\n", str);
	printf("port: %d\n", port);
	printf("canonical name: %s\n\n", addr->ai_canonname);
}

void getaddrinfoDemo2()
{
	int n;
	struct addrinfo *res, *ressave;
	struct addrinfo hint;

	//bzero(&hint, sizeof(hint));
	memset(&hint, 0, sizeof(hint));
	hint.ai_family = AF_UNSPEC;

	if ((n = getaddrinfo("www.baidu.com", /*NULL*/"http", &hint, &res)) != 0)
	{
		perror("getaddrinfo error");
		return;
	}
	ressave = res;

	while (res != NULL)
	{
		printAddressInfo(res);
		res = res->ai_next;
	}

	freeaddrinfo(ressave);
}

int main(int argc, char **argv)
{
#ifdef _WIN32
	WORD wVersion;
	WSADATA WSAData;
	wVersion = MAKEWORD(2, 2);
	WSAStartup(wVersion, &WSAData);
	
	getaddrinfoDemo();
#endif // _WIN32

	gethostbynameDemo();
	getaddrinfoDemo2();

	return 0;
}

