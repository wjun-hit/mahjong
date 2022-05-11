#include "my_socket.h"


int my_socket::epfd;
epoll_event my_socket::event;

void perr_exit(const char *s)
{
	perror(s);
	exit(-1);
}

void my_socket::add_read_event(){
    my_socket::event.events = EPOLLIN;
    my_socket::event.data.fd = fd;
    epoll_ctl(my_socket::epfd, EPOLL_CTL_ADD, fd, &my_socket::event);  
}

int my_socket::getFd(){
    return fd;
}

my_socket::my_socket(){
    
    family = AF_INET;
    type = SOCK_STREAM;
    protocol = 0;
    
    port = htons(PORT);
    ipAddr = htonl(INADDR_ANY);
    
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = family;
    servaddr.sin_port = port;
    servaddr.sin_addr.s_addr = ipAddr;
    
    backlog = 10;
}

void my_socket::sockt_init(){
    
    fd = Socket();
    Bind();
    Listen();
}

int my_socket::Socket()
{
	int n;

	if ((n = socket(family, type, protocol)) < 0)
		perr_exit("socket error");

	return n;
}

int my_socket::Accept()
{
	int n;

again:
	if ((n = accept(fd, (struct sockaddr *)&cliaddr, &cliaddr_len)) < 0) {
		if ((errno == ECONNABORTED) || (errno == EINTR))
			goto again;
		else
			perr_exit("accept error");
	}
	return n;
}

int my_socket::Bind()
{
    int n;

	if ((n = bind(fd, (struct sockaddr *)&servaddr, sizeof(servaddr))) < 0)
		perr_exit("bind error");

    return n;
}

int my_socket::Connect()
{
    int n;

	if ((n = connect(fd, (struct sockaddr *)&cliaddr, sizeof(cliaddr))) < 0)
		perr_exit("connect error");

    return n;
}

int my_socket::Listen()
{
    int n;

	if ((n = listen(fd, backlog)) < 0)
		perr_exit("listen error");

    return n;
}

int my_socket::Close()
{
    int n;
	if ((n = close(fd)) == -1)
		perr_exit("close error");

    return n;
}
