#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define HTTP_MAX_LENGTH 1024
struct Http_format{
	
	char * type;
	char * http_request_url;
	char * http_version;
};
struct Http_format http_format;
int header_parse(char * buff,int len);

int main(int argc, char *argv[])
{
    int server_sockfd;
    int client_sockfd;
    int len,pid;
    struct sockaddr_in server_addr;   
    struct sockaddr_in remote_addr; 
    int sin_size;
    char buf[BUFSIZ];  
    memset(&server_addr, 0, sizeof(server_addr)); 
    server_addr.sin_family = AF_INET; 
    server_addr.sin_addr.s_addr = INADDR_ANY; 
    server_addr.sin_port = htons(8080); 

    if((server_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("socket failed\n");
        return 1;
    }
	printf("socket success\n");
    
    if (bind(server_sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0)
    {
        printf("bind failed\n");
        exit(0);
    }
	printf("bind success\n");

 
    listen(server_sockfd, 5);
	printf("beginning listen...\n");
    sin_size = sizeof(struct sockaddr_in);
   
    for(;;)
    {
        client_sockfd = accept(server_sockfd, (struct sockaddr *) &remote_addr, &sin_size);
        if (client_sockfd < 0)
            error("ERROR on accept");
		//创建子进程
        pid = fork();
        if (pid < 0)
            error("ERROR on fork");
        if (pid == 0)
        {
			//由子进程为到来的连接提供服务
            close(server_sockfd);
			char header[] = "HTTP/1.0 200 OK\r\n\r\n";
            char html[] = "<!DOCTYPE html><html><head></head><body><h1>This is server_</h1></body></html>";
            recv(client_sockfd, buf, BUFSIZ, 0);
             header_parse(buf,strlen(buf));
		printf("%s\n",http_format.type);
		printf("%s\n",http_format.http_request_url);
               printf("%s\n", buf);
		
			
			/*************由进程去连接本地apache服务*************/
			int sockfd,numbers;
		
			struct sockaddr_in sockaddress;
			printf("start:\n");
			while((sockfd = socket(AF_INET,SOCK_STREAM,0))==-1);
			printf("get sockfd:\n");
			/*
			*sin_family   AF_INET(ipv4) AF_INET6(ipv6) AF_LOCAL(unix协议) AF_KEY(秘钥)
			*/
			sockaddress.sin_family = AF_INET;
			//绑定端口
			sockaddress.sin_port = htons(80);
			//连接某个ip
			sockaddress.sin_addr.s_addr = inet_addr("127.0.0.1");
			//清零
			bzero(&(sockaddress.sin_zero),8);
			//连接
			while(connect(sockfd,(struct sockaddr*)&sockaddress,sizeof(struct sockaddr))==-1);
				printf("Get the server\n");
				numbers = send(sockfd,buf,sizeof(buf),0);
				//开始阻塞
				numbers = recv(sockfd,buf,BUFSIZ,0);
				buf[numbers]= '\0';
				printf("received:%s\n",buf);
			
			
			close(sockfd);
			
			
			
			
			send(client_sockfd,header,strlen(header),0);
            send(client_sockfd, buf, strlen(buf), 0);
            
            close(client_sockfd);
            exit(0);
        }
		close(client_sockfd);//父进程中一定要关闭连接套接字
    } 
    close(server_sockfd);
    return 0;
}

int header_parse(char * buff,int length){
	char *stop , *stop1;
	char *new_buff = buff;
	get_head_type(buff);
	stop1 = buff;
	stop = buff;
	while((*(++new_buff))!=' '){//解析了请求类型 get、post等
		stop1++;//指针偏移
	}
	stop1++;
	//stop1 也指向了空格位置
	//new_buff 也指向了空格位置
	stop = stop1;//若指向空格
	while((*(++new_buff))!=' '){
		stop1++;
	}
	stop1++;//指向第二个空格处
	//现在stop 指向第一个空格 
	if((stop1-stop)>HTTP_MAX_LENGTH){
		printf("conteng-url is too long");
		return -1;
	}
	http_format.http_request_url = (char*)malloc(100*sizeof(char));
	http_format.http_version = (char*)malloc(100*sizeof(char));
	memcpy(http_format.http_request_url,stop,stop1-stop);
	if(*new_buff == ' '){
		char  v3[100];
		char v1[10],v2[10];
		++new_buff;
		++stop1;
		if(sscanf(new_buff,"HTTP/%[0-9].%[0-9]",v1,v2)==2){
			strcat(v3,v1);
			strcat(v3,v2);
			strcpy(http_format.http_version,v3);
		}
	}
	
}

int get_head_type(char * buff){
	http_format.type = (char*)malloc(100*sizeof(char));
	if(!memcmp(buff,"GET",3)){
		strcpy(http_format.type,"GET");
	}
	else if(!memcmp(buff,"HEAD",4)){
		strcpy(http_format.type,"HEAD");
	}
	else if(!memcmp(buff,"POST",4)){
		strcpy(http_format.type,"POST");
	}
	else if(!memcmp(buff,"PUT",3)){
		strcpy(http_format.type,"PUT");
	}
	else if(!memcmp(buff,"DELETE",6)){
		strcpy(http_format.type,"DELETE");
	}
	else if(!memcmp(buff,"OPTIONS",7)){
		strcpy(http_format.type,"OPTIONS");
	}
	else if(!memcmp(buff,"TRACE",5)){
		strcpy(http_format.type,"TRACE");
	}
	else if(!memcmp(buff,"PATCH",5)){
		strcpy(http_format.type,"PATCH");
	}
	else if(!memcmp(buff,"MOVE",5)){
		strcpy(http_format.type,"MOVE");
	}
	else if(!memcmp(buff,"COPY",4)){
		strcpy(http_format.type,"COPY");
	}
	else if(!memcmp(buff,"LINK",4)){
		strcpy(http_format.type,"LINK");
	}
	else if(!memcmp(buff,"UNLINK",6)){
		strcpy(http_format.type,"UNLINK");
	}
	else if(!memcmp(buff,"WRAPPED",7)){
		strcpy(http_format.type,"WRAPPED");
	}
	else if(!memcmp(buff,"WRAPPED",7)){
		strcpy(http_format.type,"WRAPPED");
	}
}













