#include "hoonsy.h"

struct Http_format{//请求行
	
	char * type;
	char * http_request_url;
	char * http_version;
};
struct Node{//用链表结构进行存储
	char * name;//请求头对应参数值的key
	char * value;//请求头对应参数的值
	struct Node *next;
};

struct GetNode{
    char * name;
    char * value;
    struct GetNode *next;
};

struct Conf{//用链表结构存储配置
	char * name;//请求头对应参数值的key
	char * value;//请求头对应参数的值
	struct Conf *next;
};
char * request_head;//代表请求行
struct Node *new_link,*head;
struct GetNode * get_link,*get_head;
struct Conf *conf_link,*head_conf;

struct Http_format http_format;
int header_parse(char * buff,int len);
int get_parse(char * buff);
void print_head(struct Node * head);
void print_get(struct GetNode * get_head);
void print_conf(struct Conf * head_conf);
char * get_conf(char * key,struct Conf * head_conf);
void conf_parse();

int main(int argc, char *argv[])
{
	//配置
    conf_link = (struct Conf *)malloc(sizeof(struct Conf *));
	conf_link->next = NULL;
	head_conf = conf_link;
	//读取配置文件
	conf_parse();
	char * port = get_conf("Listen",head_conf);
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
    server_addr.sin_port = htons(atoi(port)); 

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
		new_link = (struct Node *)malloc(sizeof(struct Node *));
		new_link->next = NULL;
		head= new_link;
		get_link = (struct GetNode *)malloc(sizeof(struct GetNode *));
		get_link->next = NULL;
		get_head = get_link;
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

			 printf("\n\n");
			 printf("请求行:\n");
			printf("%s\n",http_format.type);
			printf("%s\n",http_format.http_request_url);
			printf("%s\n",http_format.http_version);
			printf("===================\n\n");
			print_head(head);
            if(!memcmp(http_format.type,"GET",3)) {//get请求
                get_parse(http_format.http_request_url);
                printf("===================\n\n");
                print_get(get_head);
            }

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
				//printf("received:%s\n",buf);
			
			
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

/**
解析请求头
*/
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
			strcat(v3,"HTTP");
			strcat(v3,v1);
			strcat(v3,".");
			strcat(v3,v2);
			strcpy(http_format.http_version,v3);
			//继续递增指针到换行
			while(*(++new_buff)!='\r');//指向第一个回车符
			++new_buff;//指向第一个换行符
			++new_buff;//第二行
			request_head = new_buff;
			parse_header_request(request_head);
		}else{
			return -1;
		}
	}
	
}

/**
 *解析get数据
 */
int get_parse(char * buff){
    char *index,*start,*line = buff;
    struct GetNode * new_node;
    while((index = strstr(line,"?"))==NULL){
        ++index;
    }
    ++index;
    line = index;//指向第一个get字符串的第一个字符
    while((index = strstr(index,"="))!=NULL){
        char * value = (char *)malloc(1500);
        memcpy(value,line,index-line);
        new_node = (struct GetNode *)malloc(sizeof(struct GetNode *));
        new_node->name = value;//得到get参数的第一个字符串（第一个键）
        ++index;
        start = index;//指向第一个get参数的第一个键的第一个字符
        while(*(++index)!='&' && strlen(index)!=0);
        value = (char *)malloc(1500);
        memcpy(value,start,index-start);
        new_node->value = value;//得到get参数的第一个值
        new_node->next = NULL;
        get_link->next = new_node;
        get_link = new_node;
        ++index;
        line = index;
    }
}

/**
解析请求头
*/
int parse_header_request(char * buff){
	char *index,*start,* line = buff;
	struct Node * new_node;
	char * value;
	while((index = strstr(line,":"))!=NULL){
		char * value = (char *)malloc(1500);
		memcpy(value,line,index-line);
		//if(!memcmp(value,'Host',4)){
			new_node = (struct Node *)malloc(sizeof(struct Node *));
			new_node->name = value;//得到第一参数键
			++index;
			++index;
			start = index;//start指向第一个值的第一个字符
			while(*(++index)!='\r');//指向第一个回车符
			value = (char *)malloc(1500);
			memcpy(value,start,index-start);
			new_node->value = value;//得到第一参数值
			new_node->next = NULL;
			new_link->next = new_node;
			new_link = new_node;
			++index;//指向第一个换行符
			++index;//重新一行的第一个字符
			line = index;
	
		//}
	}
	
}

void conf_parse(){
	FILE * fp = NULL;
	struct Conf * new_conf;
	char *value,*index,*line,*start;
	char ftemp[1024],fline[1024];
	fp = fopen("conf/hoonsy.conf","r+");
	if(fp==NULL){
		fopen("conf/hoonsy.conf","w+");
	}
	//获取文件大小
	//fseek(fp,0,SEEK_END);//将文件指针指向末尾
	while(!feof(fp)){
		fgets(fline,1024,fp);
		line = fline;
		while((index = strstr(line,":"))!=NULL){
			value = (char *)malloc(100);
			memcpy(value,line,index-line);
			new_conf = (struct Conf *)malloc(sizeof(struct Conf *));
			new_conf->name = value;//得到第一参数键
			++index;
			start = index;
			while(*(++index)!='\n');//指向第一个回车符
			value = (char *)malloc(100);
			memcpy(value,start,index-start);
			new_conf->value = value;//得到第一参数值
			new_conf->next = NULL;
			conf_link->next = new_conf;
			conf_link = new_conf;
			line = index;
		}
	}
	fclose(fp);
	
}

void print_conf(struct Conf * conf_head){
	struct Conf * temp;
	temp = conf_head->next;
	printf("配置:\n");
	while(temp){
		printf("%s:",temp->name);
		printf("%s",temp->value);
		temp = temp->next;
		printf("\n");
	}
	printf("配置结束\n");
}

char * get_conf(char * key,struct Conf * conf_head){
	struct Conf * temp;
	temp = conf_head->next;
	while(temp){
		if(!memcmp(temp->name,key,strlen(key))){
			return temp->value;
		}
		temp = temp->next;
	}
	return NULL;
}

void print_get(struct GetNode * head){
    struct GetNode * temp;
    temp = head->next;
    printf("GET参数:\n");
    while(temp){
        printf("%s:",temp->name);
        printf("%s",temp->value);
        temp = temp->next;
        printf("\n");
    }
    printf("GET结束\n");
}

void print_head(struct Node * head){
	struct Node * temp;
	temp = head->next;
	printf("请求头:\n");
	while(temp){
		printf("%s:",temp->name);
		printf("%s",temp->value);
		temp = temp->next;
		printf("\n");
	}
	printf("请求头结束\n");
}
/*
获取请求方式
*/
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
    else if(!memcmp(buff,"CONNECT",7)){
        strcpy(http_format.type,"CONNECT");
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
	else if(!memcmp(buff,"MOVE",4)){
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
}















