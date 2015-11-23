#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <dlfcn.h>

int (*orig_connect)(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
	printf("connect hooked\n");
	return orig_connect(sockfd, addr, addrlen);
}

void _init(void)
{
	printf("Loading %s.\n", __FILE__);
	orig_connect = dlsym(RTLD_NEXT, "connect");
}
