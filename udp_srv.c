//这个demo用于体会udp通信的编程流程以及接口使用
//1.创建套接字
//2.为套接字绑定地址信息
//3.接收数据
//4.发送数据
//5.关闭套接字
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>//struct sockaddr_in结构体的定义以及协议的宏
#include <arpa/inet.h>//一些字节序转换的接口
#include <sys/socket.h>//套接字接口

int main(int argc,char* argv[])
{
	//通过参数传入服务端要绑定的地址信息（IP地址和端口）
	if(argc!=3){
		printf("em:./udp_srv 192.168.204.129 9000\n");
		return -1;
	}
	const char* ip_addr=argv[1];//通过参数获取ip地址
	uint16_t port_addr=atoi(argv[2]);//通过参数获取端口（这是主机字节序存储的）
	//创建套接字
	int sockfd=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	if(sockfd<0){
		perror("socket error");
		return -1;
	}
	//定义IPV4地址结构
	struct sockaddr_in addr;//定义ipv4的结构体变量
	addr.sin_family=AF_INET;//标识是IPV4的地址结构
	//htons将无符号两个字节的数据转换成网络字节序
	addr.sin_port=htons(port_addr);//网络字节序端口
	//inet_pton 将字符串ip地址转换成网络字节序ip地址
	inet_pton(AF_INET,ip_addr,&addr.sin_addr.s_addr);
	//绑定地址信息
	int ret;
	socklen_t len=sizeof(struct sockaddr_in);
	ret=bind(sockfd,(struct sockaddr*)&addr,len);
	if(ret<0){
		perror("bind error");
		return -1;
	}
	//循环接收/发送数据
	while(1){
		//作为服务端应该先接收数据
		//recvfrom（描述符，缓冲区，长度，参数，对端地址存放缓冲区，对端地址长度缓冲区
		char buf[1024]={0};
		struct sockaddr_in cliaddr;//用于存放对端地址信息
		len=sizeof(struct sockaddr_in);//用于存放对端地址信息长度
		//参数0表示如果socket缓冲区中没有数据则阻塞等待直到有数据读出
		ret=recvfrom(sockfd,buf,1023,0,(struct sockaddr*)&cliaddr,&len);
		if(ret<0){
			perror("recvform error");
			close(sockfd);
			return -1;
		}
		char cli_ip[32]={0};
		//将网络字节序端口转换成主机字节序
		uint16_t cli_port=ntohs(cliaddr.sin_port);
		//将网络字节序ip地址转换为字符串ip地址
		inet_ntop(AF_INET,&cliaddr.sin_addr,cli_ip,32);
		printf("client[%s:%d] say:%s\n",cli_ip,cli_port,buf);
		
		printf("server say: ");
		fflush(stdout);
		memset(buf,0x00,1024);//初始化buf缓冲区为0
		scanf("%s",buf);//从标准输入获取数据到buf发送出去
		//发送数据
		ret=sendto(sockfd,buf,strlen(buf),0,(struct sockaddr*)&cliaddr,len);
		if(ret<0){
			perror("sendto error");
			close(sockfd);//关闭套接字
			return -1;
		}
	}
	close(sockfd);
	return 0;
}
