
	#include <sys/socket.h>
	#include <netdb.h>

	int getaddrinfo(const char *restrict nodename, /* host 或者IP地址 */
    	const char *restrict servname, /* 十进制端口号 或者常用服务名称如"ftp"、"http"等 */
    	const struct addrinfo *restrict hints, /* 获取信息要求设置 */
    	struct addrinfo **restrict res); /* 获取信息结果 */

	void freeaddrinfo(struct addrinfo *ai); 

# Description
IPv4中使用gethostbyname()函数完成主机名到地址解析，这个函数仅仅支持IPv4，

且不允许调用者指定所需地址类型的任何信息，返回的结构只包含了用于存储IPv4地址的空间。

IPv6中引入了新的API getaddrinfo()，它是协议无关的，既可用于IPv4也可用于IPv6。

getaddrinfo() 函数能够处理名字到地址以及服务到端口这两种转换，返回的是一个 struct addrinfo 的结构体(列表)指针而不是一个地址清单。

这些 struct addrinfo 结构体随后可由套接口函数直接使用。如此以来，getaddrinfo()函数把协议相关性安全隐藏在这个库函数内部。

应用程序只要处理由getaddrinfo()函数填写的套接口地址结构。

# Parameter
## 1) nodename
主机名("www.baidu.com")或者是数字化的地址字符串(IPv4的点分十进制串("192.168.1.100")或者IPv6的16进制串("2000::1:2345:6789:abcd"))，

如果 ai_flags 中设置了AI_NUMERICHOST 标志，那么该参数只能是数字化的地址字符串，不能是域名，

该标志的作用就是阻止进行域名解析。

nodename 和 servname 可以设置为NULL，但是同时只能有一个为NUL。

## 2) servname
服务名可以是十进制的端口号("8080")字符串，也可以是已定义的服务名称，如"ftp"、"http"等,详细请查看/etc/services 文件，

最后翻译成对应服务的端口号。如果此参数设置为NULL，那么返回的socket地址中的端口号不会被设置。

如果 ai_flags 设置了AI_NUMERICSERV 标志并且该参数未设置为NULL，那么该参数必须是一个指向10进制的端口号字符串，

不能设定成服务名，该标志就是用来阻止服务名解析。

## 3) hints
该参数指向用户设定的 struct addrinfo 结构体，只能设定该结构体中 ai_family、ai_socktype、ai_protocol 和 ai_flags 四个域，

其他域必须设置为0 或者 NULL, 通常是申请 结构体变量后使用memset()初始化再设定指定的四个域。

该参数可以设置为NULL，等价于 ai_socktype = 0， ai_protocol = 0，ai_family = AF_UNSPEC， 

ai_flags = 0 （在GNU Linux中ai_flag = AI_V4MAPPED | AI_ADDRCONFIG,可以看作是GNU的改进）。

### ① ai_family
　　指定返回地址的协议簇，取值范围:AF_INET(IPv4)、AF_INET6(IPv6)、AF_UNSPEC(IPv4 and IPv6)

### ② ai_socktype
　　具体类型请查看struct addrinfo 中的 enum __socket_type 类型，用于设定返回地址的socket类型，

　　常用的有SOCK_STREAM、SOCK_DGRAM、SOCK_RAW, 设置为0表示所有类型都可以。

### ③ ai_protocol
　　具体取值范围请查看 Ip Protocol ，常用的有 IPPROTO_TCP、IPPROTO_UDP 等，设置为0表示所有协议。

### ④ ai_flags
　　附加选项,多个选项可以使用或操作进行结合，具体取值范围请查看struct addrinfo , 常用的标志如下：

- AI_PASSIVE

　　　　如果设置了 AI_PASSIVE 标志,并且 nodename 是 NULL, 那么返回的socket地址可以用于的bind()函数，

   返回的地址是通配符地址(wildcard address, IPv4时是INADDR_ANY,IPv6时是IN6ADDR_ANY_INIT)，

   这样应用程序(典型是server)就可以使用这个通配符地址用来接收任何请求主机地址的连接，

   如果 nodename 不是NULL，那么 AI_PASSIVE 标志被忽略；

　　　　如果未设置AI_PASSIVE标志,返回的socket地址可以用于connect(), sendto(), 或者 sendmsg()函数。

　　　　如果 nodename 是NULL，那么网络地址会被设置为lookback接口地址(IPv4时是INADDR_LOOPBACK,IPv6时是IN6ADDR_LOOPBACK_INIT)，

　　　　这种情况下，应用是想与运行在同一个主机上另一个应用通信。

- AI_CANONNAME

 　　　　请求canonical(主机的official name)名字。如果设置了该标志，那么 res 返回的第一个 struct addrinfo 中的 ai_canonname 域会存储official name的指针。

- AI_NUMERICHOST

 　　　　阻止域名解析，具体见 nodename 中的说明。

- AI_NUMERICSERV

　　　　阻止服务名解析，具体见 servname 中的说明。

- AI_V4MAPPED

 　　　　当 ai_family 指定为AF_INT6(IPv6)时，如果没有找到IPv6地址，那么会返回IPv4-mapped IPv6 地址，

　　　　也就是说如果没有找到AAAA record(用来将域名解析到IPv6地址的DNS记录),那么就查询A record(IPv4),

　　　　将找到的IPv4地址映射到IPv6地址, IPv4-mapped IPv6 地址其实是IPv6内嵌IPv4的一种方式，

　　　　地址的形式为"0::FFFF:a.b.c.d"，例如"::ffff:192.168.89.9"(混合格式)这个地址仍然是一个IPv6地址，

   只是"0000:0000:0000:0000:0000:ffff:c0a8:5909"(16机制格式)的另外一种写法罢了。

　　　　当 ai_family 不是AF_INT6(IPv6)时，该标志被忽略。

- AI_ALL

 　　　　查询IPv4和IPv6地址

- AI_ADDRCONFIG

　　　　只有当主机配置了IPv4地址才进行查询IPv4地址；只有当主机配置了IPv6地址才进行查询IPv6地址.

## 4) res
 该参数获取一个指向存储结果的 struct addrinfo 结构体列表，使用完成后调用 freeaddrinfo() 释放存储结果空间。

# Return Value
如果 getaddrinfo() 函数执行成功，返回值为 0 ， 其他情况返回值表示错误种别。使用函数gai_strerror() 可以获取可读性的错误信息,用法用strerror()相同，

错误种别如下：

- EAI_ADDRFAMILY

　　　　指定的主机上没有请求的address family对应的网络地址.

- EAI_AGAIN

　　　　DNS(name server)返回临时性错误. 可以稍后重试.

- EAI_BADFLAGS

　　　　hints.ai_flags 包含了无效的标志; 或者 hints.ai_flags 包含了 AI_CANONNAME 标志但是 name 是 NULL.

- EAI_FAIL

　　　　DNS(name server)返回永久性错误

- EAI_FAMILY

　　　　不支持的 address family(hints.ai_family).

- EAI_MEMORY

　　　　内存耗尽.

- EAI_NODATA

　　　　指定的网络主机存在，但是其未定义任何网络地址.

- EAI_NONAME

　　　　nodename 或者 servname 未知;或者两者都设置为NULL;

　　　　或者设置了 AI_NUMERICSERV 标志但是 servname 不是一个数字化的端口名字符串。

- EAI_SERVICE

　　　　请求的socket类型不支持请求的服务类型.例如服务类型是 "shell" (基于流的socket服务)，

　　　　但是 hints.ai_protocol 是  IPPROTO_UDP 或者hints.ai_socktype 是 SOCK_DGRAM;

　　　　或者 servname 不是NULL 但是 hints.ai_socktype 是 SOCK_RAW (原始套接字不支持服务的概念).

- EAI_SOCKTYPE

　　　　不支持请求的socket类型. 例如, hints.ai_socktype 和 hints.ai_protocol 冲突 (例如分别是SOCK_DGRAM、IPPROTO_TCP).

- EAI_SYSTEM

　　　　系统调用错误，检查 errno.

# Example
man getaddrinfo 最后有客户端和服务器端2个例子。