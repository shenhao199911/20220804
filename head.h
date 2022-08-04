#ifndef __HEAD_H__
#define __HEAD_H__

#include <stdio.h>
#include <sqlite3.h> 
#include <stdlib.h>
#include <sys/socket.h> 
#include <sys/types.h> 
#include <string.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <unistd.h> 
#include <time.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/un.h>


struct info
{
	char name[16];
    char account[16];
    char password[16];
    char sex[16];
    int age;
    int calling;
    char place[16];
    int level;
    float money;
}; //员工信息结构体

typedef struct staff
{
	int usertype;  //用户类型
	int cmdtype;  //通信指令类型
    char buf[32]; //通信的消息
	struct info st; //员工信息
    char send_age[16];
    char send_calling[32];
    char send_level[32];
    char send_money[128];
    char send_usertype[32];
}STAFF;//通信结构体
		
enum type{
    administrators,
    user,
};//管理员身份枚举

enum cmd{
    administrators_search,
    administrators_modify,
    administrators_add,
    administrators_delete,
    administrators_history,
    user_serch,
    user_modify,
    adminatorloaddr,
    userloaddr,
};//操作方法枚举

#define ERR_MSG(msg) do{\
    fprintf(stderr,"__%d__:",__LINE__);\
    perror(msg);\
}while(0)
#define PORT 8888
#define IP "192.168.250.100"

#endif