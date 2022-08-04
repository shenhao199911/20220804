#include "head.h"
void message_hadle(STAFF administrator1,sqlite3 *db);
STAFF member_set(STAFF administrator1);
int callback(void *arg,int column,char** column_text,char**column_name);
int callback1(void *arg,int column,char** column_text,char**column_name);
int callback2(void *arg,int column,char** column_text,char**column_name);
int callback3(void *arg,int column,char** column_text,char**column_name);
int callback4(void *arg,int column,char** column_text,char**column_name);
int update_maxfd(int maxfd, fd_set readfds);
STAFF administrator1;
int flag=0;
char operation_user[16]={0};
char loaddr_user[16]={0};
char updater_user[16]={0};
int newfd;

int main(int argc, char const *argv[])
{
    if(!system("rm sys.db")){}
    //创建初始化的数据库
    sqlite3 *db = NULL;
    if(sqlite3_open("./sys.db",&db)!=SQLITE_OK){
        fprintf(stderr, "__%d__ code:%d sqlite3_open:%s\n", __LINE__, \
            sqlite3_errcode(db), sqlite3_errmsg(db)); 
        return -1;
    }
    printf("*******数据库创建成功********\n");

    //在数据库中插入员工信息表
    char sql[256]= "insert into staff values( 0 ,'张三', '1001' , 'zs1001' , '男' , 18 , 1111 , '公安局' , 5 , 3200 )";
    char sql1[256] = "create table if not exists staff (type int,name char,account char primary key,password char,sex char,age int,calling int,place char,level int,money float)";
    char sql2[256] = "create table if not exists history (data char,time char,name char,work char)";
    char *errmsg = NULL; 
    if(sqlite3_exec(db, sql1, NULL, NULL, &errmsg) != SQLITE_OK){
        fprintf(stderr, "__%d__ sqlite3_exec:%s\n", __LINE__, errmsg); 
        return -1; 
    }
    if(sqlite3_exec(db, sql2, NULL, NULL, &errmsg) != SQLITE_OK){
        fprintf(stderr, "__%d__ sqlite3_exec:%s\n", __LINE__, errmsg); 
        return -1; 
    }
    printf("******* 创建表格成功 ********\n");

    //向信息表中插入一个管理员信息
    if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)!=SQLITE_OK){
        fprintf(stderr,"__%d__ sqlite3_exec:%s\n",__LINE__,errmsg);
        return -1;
    }

    //创建流式套接字
    int sfd = socket(AF_INET,SOCK_STREAM,0);
    if(sfd < 0){
        ERR_MSG("socket");
        return -1;
    }

    //允许端口快速重用
    int reuse = 1;
    if(setsockopt(sfd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse))<0){
        ERR_MSG("setsockopt");
        return -1;
    }

    //填充地址信息结构体
    struct sockaddr_in sin;
    sin.sin_family      = AF_INET;
    sin.sin_port        = htons(PORT);
    sin.sin_addr.s_addr = inet_addr(IP);

    //将IP和端口绑定至套接字上
    if(bind(sfd,(struct sockaddr*)&sin,sizeof(sin))<0){
        ERR_MSG("bind");
        return -1;
    }

    //将套接字设置为被动监听状态;
    if(listen(sfd,10)<0){
        ERR_MSG("listen");
        return -1;
    }

    //创建socket函数并将0和sfd添加至被检测的队列中
    int ret,sockfd;
    struct sockaddr_in cin; 
    struct sockaddr_in cinSave[1024-4]; 
    socklen_t addrlen = sizeof(cin);

    fd_set readfds, tempfds; 
    FD_ZERO(&readfds); //清空集合 
    FD_ZERO(&tempfds); //清空集合 
    FD_SET(0, &readfds); //将0号文件描述符添加到集合中 
    FD_SET(sfd, &readfds); //将sfd添加到文件描述符集合中 
    int maxfd = sfd; //集合中最大的文件描述符 
    int select_res = 0; 
    ssize_t res = 0; 
    char buf[128] = "";
    int i=0;

    while(1){
        tempfds = readfds; //调用select函数，让内核检测是否有文件描述符准备就绪 
        select_res = select(maxfd+1, &tempfds, NULL, NULL, NULL); 
        if(select_res < 0){
            ERR_MSG("select"); 
            return -1; 
        }
        else if(0 == select_res){
            fprintf(stderr, "超时了....\n");
            break;
        } 
        for(int i=0; i<= maxfd; i++){
            //如果i所代表写文件描述符不在集合中，则进入下一次循环 
            if(!FD_ISSET(i, &tempfds)){
                continue;
            }//能运行到当前位置，则说明i在tempfds集合中 。 // 
            if(0 == i){ 
                int sendfd; 
                //将数据发送给sendfd对应的客户端 
                res = scanf("%d %s", &sendfd, buf); 
                while(getchar() != 10); 
                if(res != 2){
                    fprintf(stderr, "请输入正确数据： fd string\n");
                    continue;
                }
            if(!FD_ISSET(sendfd, &readfds)){
                fprintf(stderr, "sendfd = %d 不在监测集合中\n", sendfd);
                continue;
            }
            if(send(sendfd, buf, sizeof(buf), 0) < 0){
                //ERR_MSG("send");
                continue;
            }
            printf("发送到 [%s | %d] sendfd=%d 客户端成功\n", \
                inet_ntoa(cinSave[sendfd-4].sin_addr), ntohs(cinSave[sendfd-4].sin_port), sendfd);
            }
            else if(sfd == i){ 
                printf("触发客户端连接事件\n");
                newfd = accept(sfd, (struct sockaddr*)&cin, &addrlen);
                if(newfd < 0){
                    ERR_MSG("accept");
                    return -1;
                }
                printf("[%s:%d] newfd=%d 连接成功\n", inet_ntoa(cin.sin_addr), ntohs(cin.sin_port), newfd);
                cinSave[newfd-4] = cin;
                //将newfd添加到集合中 
                FD_SET(newfd, &readfds); 
                //更新
                maxfd = maxfd>newfd ? maxfd:newfd; 
            }
            else{
                //bzero(buf, sizeof(buf));
                //newfd中读取数据 
                memset(&administrator1,0,sizeof(administrator1));
                res = recv(i, &administrator1, sizeof(administrator1), 0);
                if(res < 0){
                    ERR_MSG("recv");
                    return -1;
                }
                else if(0 == res){
                    fprintf(stderr, "[%s:%d] newfd=%d 断开连接\n", \
                    inet_ntoa(cinSave[i-4].sin_addr), ntohs(cinSave[i- 4].sin_port), i);
                    //将文件描述符关闭 
                    close(i); 
                    //将文件描述符从集合中剔除; 
                    FD_CLR(i, &readfds); 
                    //更新maxfd 
                    maxfd = update_maxfd(maxfd, readfds);
                }
                message_hadle(administrator1,db);
            }
        }
    }
    close(sfd);
    return 0;
}

int update_maxfd(int maxfd, fd_set readfds){
    int i = maxfd;
    for( ; i>0; i--){
        if(FD_ISSET(i, &readfds) == 1) 
            return i; 
        }
    return 0;
}

//信息处理函数
void message_hadle(STAFF administrator1,sqlite3 *db){
    int ret,row, column;
    char **result = NULL;
    int i=0;
    time_t t2;
    t2 = time(NULL);
    char *errmsg = NULL;
    int flag=0;    //判断密码发送的变量
    char sql[256],history_sql[256],data[32],time[32],caozuo[128],mybrr[32];
    char buf[128]="";
    struct tm* info = NULL;      //处理时间字符数组
    info = localtime(&t2);
    sprintf(data,"%4d-%02d-%02d",info->tm_year+1900, info->tm_mon+1, info->tm_mday);
    sprintf(time,"%02d:%02d:%02d",info->tm_hour, info->tm_min, info->tm_sec);

    switch (administrator1.cmdtype){
    case 0:      //0代表管理员查询
        if(!strcmp(administrator1.buf,"0")){
            administrator1 = member_set(administrator1);
            //编写sql语句，使用exec执行
            sprintf(sql,"select * from staff where account ='%s' ",administrator1.st.account);
            if(sqlite3_exec(db, sql, callback3, &flag, &errmsg) != SQLITE_OK){
            fprintf(stderr, "__%d__ sqlite3_exec:%s\n", __LINE__, errmsg); 
            return ;
            }
            if(flag == 0){
                strcpy(administrator1.buf,"default0");
                if(send(newfd,&administrator1,sizeof(administrator1),0)<0){
                    //ERR_MSG("send");
                    return ;
                }   
            }
            //向历史记录表中填充信息
            sprintf(caozuo,"管理员查看了用户的信息,用户账户为%s",administrator1.st.account);
            sprintf(history_sql,"insert into history values( '%s' , '%s' , '%s' , '%s' )",data,time,operation_user,caozuo);
            if(sqlite3_exec(db,history_sql,NULL,NULL,&errmsg)!=SQLITE_OK){
                fprintf(stderr,"__%d__ sqlite3_exec:%s\n",__LINE__,errmsg);
                return ;
            }
        }
        else{
            bzero(buf,sizeof(buf));
            administrator1 = member_set(administrator1);
            //查询表中的所有信息，用的sqlite3_get_table
            sprintf(sql,"select * from staff ");
            if(sqlite3_get_table(db, sql, &result, &row , &column, &errmsg) != SQLITE_OK){
                fprintf(stderr, "__%d__ sqlite3_exec:%s\n", __LINE__, errmsg);
                strcpy(administrator1.buf,"default0");
                if(send(newfd,&administrator1,sizeof(administrator1),0)<0){
                    //ERR_MSG("send");
                    return ;
                } 
                return ;
            }
            if(send(newfd,&row,sizeof(row),0)<0){
                //ERR_MSG("send");
                return ;
            }
            //循环拷贝信息，发送信息
            for(i=0;i<row;i++){
                strcpy(administrator1.send_usertype,result[(i+1)*column+0]);
                strcpy(administrator1.st.name,result[(i+1)*column+1]);
                strcpy(administrator1.st.account,result[(i+1)*column+2]);
                strcpy(administrator1.st.password,result[(i+1)*column+3]);
                strcpy(administrator1.st.sex,result[(i+1)*column+4]);
                strcpy(administrator1.send_age,result[(i+1)*column+5]);
                strcpy(administrator1.send_calling,result[(i+1)*column+6]);
                strcpy(administrator1.st.place,result[(i+1)*column+7]);
                strcpy(administrator1.send_level,result[(i+1)*column+8]);
                strcpy(administrator1.send_money,result[(i+1)*column+9]);
                if(send(newfd,&administrator1,sizeof(administrator1),0)<0){
                    //ERR_MSG("send");
                    return ;
                }
            }
            strcpy(administrator1.buf,"ok");
            if(send(newfd,&administrator1,sizeof(administrator1),0)<0){
                //ERR_MSG("send");
                return ;
            }
            //将记录存到表中
            sprintf(caozuo,"管理员查看了所有用户的信息");
            sprintf(history_sql,"insert into history values( '%s' , '%s' , '%s' , '%s' )",data,time,operation_user,caozuo);
            if(sqlite3_exec(db,history_sql,NULL,NULL,&errmsg)!=SQLITE_OK){
                fprintf(stderr,"__%d__ sqlite3_exec:%s\n",__LINE__,errmsg);
                return ;
            }            
        }
        break;
    case 1:      //1代表管理员修改
        if(!strlen(administrator1.buf)){
            administrator1 = member_set(administrator1);
            //判断账户库中是否存在
            bzero(sql,sizeof(sql));
            sprintf(sql,"select * from staff where account ='%s' ",administrator1.st.account);
            if(sqlite3_exec(db, sql, callback2, &flag, &errmsg) != SQLITE_OK){
                fprintf(stderr, "__%d__ sqlite3_exec:%s\n", __LINE__, errmsg); 
                return ;
            }
            //未执行回调函数时触发发送default0消息
            if (flag == 0)
            {
                strcpy(administrator1.buf,"default0");
                if(send(newfd,&administrator1,sizeof(administrator1),0)<0){
                    //ERR_MSG("send");
                    return ;
                }
            }
        }
        else{
            strcpy(mybrr,administrator1.buf);
            administrator1 = member_set(administrator1);
            for(int i=1;i<strlen(mybrr);i++){
                //循环对比出第一个标识外的所有内容，根据不同的字符进入不同的修改函数
                switch(mybrr[i]){
                    case '1':       //修改名字
                        bzero(sql,sizeof(sql));
                        sprintf(sql,"update staff set name = '%s' where account = '%s' ",administrator1.st.name,updater_user);
                        if(sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK){
                            fprintf(stderr, "__%d__ sqlite3_exec:%s\n", __LINE__, errmsg); 
                            return ;
                        }
                        break;
                    case '2':       //修改密码
                        bzero(sql,sizeof(sql));
                        sprintf(sql,"update staff set password = '%s' where account = '%s' ",administrator1.st.password,updater_user);
                        if(sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK){
                            fprintf(stderr, "__%d__ sqlite3_exec:%s\n", __LINE__, errmsg); 
                            return ;
                        }
                        break;
                    case '3':       //修改性别
                        bzero(sql,sizeof(sql));
                        sprintf(sql,"update staff set sex = '%s' where account = '%s' ",administrator1.st.sex,updater_user);
                        if(sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK){
                            fprintf(stderr, "__%d__ sqlite3_exec:%s\n", __LINE__, errmsg); 
                            return ;
                        }
                        break;
                    case '4':       //修改年龄
                        bzero(sql,sizeof(sql));
                        sprintf(sql,"update staff set age = %d where account = '%s' ",administrator1.st.age,updater_user);
                        if(sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK){
                            fprintf(stderr, "__%d__ sqlite3_exec:%s\n", __LINE__, errmsg); 
                            return ;
                        }
                        break;
                    case '5':       //修改电话
                        bzero(sql,sizeof(sql));
                        sprintf(sql,"update staff set calling = %d where account = '%s' ",administrator1.st.calling,updater_user);
                        if(sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK){
                            fprintf(stderr, "__%d__ sqlite3_exec:%s\n", __LINE__, errmsg); 
                            return ;
                        }
                        break;
                    case '6':       //修改地址
                        bzero(sql,sizeof(sql));
                        sprintf(sql,"update staff set place = '%s' where account = '%s' ",administrator1.st.place,updater_user);
                        if(sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK){
                            fprintf(stderr, "__%d__ sqlite3_exec:%s\n", __LINE__, errmsg); 
                            return ;
                        }
                        break;
                    case '7':       //修改工作评级
                        bzero(sql,sizeof(sql));
                        sprintf(sql,"update staff set level = %d where account = '%s' ",administrator1.st.level,updater_user);
                        if(sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK){
                            fprintf(stderr, "__%d__ sqlite3_exec:%s\n", __LINE__, errmsg); 
                            return ;
                        }
                        break;
                    case '8':       //修改工资
                        bzero(sql,sizeof(sql));
                        sprintf(sql,"update staff set money = %f where account = '%s' ",administrator1.st.money,updater_user);
                        if(sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK){
                            fprintf(stderr, "__%d__ sqlite3_exec:%s\n", __LINE__, errmsg); 
                            return ;
                        }
                        break;
                }

            }
            //回复应答消息
            strcpy(administrator1.buf,"ok");
            administrator1.buf[strlen(administrator1.buf)]=0;
            if(send(newfd,&administrator1,sizeof(administrator1),0)<0){
                //ERR_MSG("send");
                return ;
            }
            //向数据库中的历史记录表填充信息
            sprintf(caozuo,"管理员修改了用户的信息,用户账户为%s",updater_user);
            sprintf(history_sql,"insert into history values( '%s' , '%s' , '%s' , '%s' )",data,time,operation_user,caozuo);
            if(sqlite3_exec(db,history_sql,NULL,NULL,&errmsg)!=SQLITE_OK){
                fprintf(stderr,"__%d__ sqlite3_exec:%s\n",__LINE__,errmsg);
                return ;
            }
        }
        break;
    case 2:      //2代表管理员添加成员
        administrator1 = member_set(administrator1);
        //向数据库中成员表添加新的用户
        bzero(sql,sizeof(sql));
        sprintf(sql,"insert into staff values( %d ,'%s', '%s' , '%s' , '%s' , %d , %d , '%s' , %d , %f )",\
            administrator1.usertype,administrator1.st.name,administrator1.st.account,administrator1.st.password,administrator1.st.sex,\
            administrator1.st.age,administrator1.st.calling,administrator1.st.place,administrator1.st.level,administrator1.st.money);
        if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)!=SQLITE_OK){
            fprintf(stderr,"__%d__ sqlite3_exec:%s\n",__LINE__,errmsg);
            return ;
        }
        //回复应答消息
        strcpy(administrator1.buf,"ok");
        administrator1.buf[strlen(administrator1.buf)]=0;
        if(send(newfd,&administrator1,sizeof(administrator1),0)<0){
            //ERR_MSG("send");
            return ;
        }
        //向数据库中的历史记录表填充信息
        sprintf(caozuo,"管理员添加了新用户%s,账户为%s",administrator1.st.name,administrator1.st.account);
        sprintf(history_sql,"insert into history values( '%s' , '%s' , '%s' , '%s' )",data,time,operation_user,caozuo);
        if(sqlite3_exec(db,history_sql,NULL,NULL,&errmsg)!=SQLITE_OK){
            fprintf(stderr,"__%d__ sqlite3_exec:%s\n",__LINE__,errmsg);
            return ;
        }
        break;
    case 3:      //3代表管理员删除成员
        administrator1 = member_set(administrator1);
        bzero(sql,sizeof(sql));
        sprintf(sql,"delete from staff where account = '%s' ",administrator1.st.account);
        if(sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK){
            fprintf(stderr, "__%d__ sqlite3_exec:%s\n", __LINE__, errmsg); 
            return ;
        }
        //回复应答消息
        strcpy(administrator1.buf,"ok");
        administrator1.buf[strlen(administrator1.buf)]=0;
        if(send(newfd,&administrator1,sizeof(administrator1),0)<0){
            //ERR_MSG("send");
            return ;
        }
        //向数据库中的历史记录表填充信息
        sprintf(caozuo,"管理员删除了用户账户为%s的成员",administrator1.st.account);
        sprintf(history_sql,"insert into history values( '%s' , '%s' , '%s' , '%s' )",data,time,operation_user,caozuo);
        if(sqlite3_exec(db,history_sql,NULL,NULL,&errmsg)!=SQLITE_OK){
            fprintf(stderr,"__%d__ sqlite3_exec:%s\n",__LINE__,errmsg);
            return ;
        }
        break;
    case 4:      //4代表管理员查询历史记录
        bzero(buf,sizeof(buf));
        bzero(administrator1.send_usertype,sizeof(administrator1.send_usertype));
        bzero(administrator1.send_calling,sizeof(administrator1.send_calling));
        bzero(administrator1.send_level,sizeof(administrator1.send_level));
        bzero(administrator1.send_money,sizeof(administrator1.send_money));
        administrator1 = member_set(administrator1);
        //执行查询所有历史记录的sqlite语句
        sprintf(sql,"select * from history ");
        if(sqlite3_get_table(db, sql, &result, &row , &column, &errmsg) != SQLITE_OK){
            fprintf(stderr, "__%d__ sqlite3_exec:%s\n", __LINE__, errmsg);
            strcpy(administrator1.buf,"default0");
            if(send(newfd,&administrator1,sizeof(administrator1),0)<0){
                //ERR_MSG("send");
                return ;
            } 
            return ;
        }
        if(send(newfd,&row,sizeof(row),0)<0){
            //ERR_MSG("send");
            return ;
        }
        //循环拷贝信息，发送信息
        for(i=0;i<row;i++){
            strcpy(administrator1.send_usertype,result[(i+1)*column+0]);
            strcpy(administrator1.send_calling,result[(i+1)*column+1]);
            strcpy(administrator1.send_level,result[(i+1)*column+2]);
            strcpy(administrator1.send_money,result[(i+1)*column+3]);
            if(send(newfd,&administrator1,sizeof(administrator1),0)<0){
                //ERR_MSG("send");
                return ;
            }
        }
        strcpy(administrator1.buf,"ok");
        if(send(newfd,&administrator1,sizeof(administrator1),0)<0){
            //ERR_MSG("send");
            return ;
        }
        //将记录存到表中
        sprintf(caozuo,"管理员查看了历史记录");
        sprintf(history_sql,"insert into history values( '%s' , '%s' , '%s' , '%s' )",data,time,operation_user,caozuo);
        if(sqlite3_exec(db,history_sql,NULL,NULL,&errmsg)!=SQLITE_OK){
            fprintf(stderr,"__%d__ sqlite3_exec:%s\n",__LINE__,errmsg);
            return ;
        }
        break;
    case 5:      //5代表普通用户查询
        administrator1 = member_set(administrator1);
        sprintf(sql,"select * from staff where account ='%s' ",loaddr_user);
        //sql[strlen(sql)-1]='\n';
        if(sqlite3_exec(db, sql, callback4, &flag, &errmsg) != SQLITE_OK){
            fprintf(stderr, "__%d__ sqlite3_exec:%s\n", __LINE__, errmsg); 
            return ;
        }
        if(flag == 0){
            strcpy(administrator1.buf,"default0");
            if(send(newfd,&administrator1,sizeof(administrator1),0)<0){
                //ERR_MSG("send");
                return ;
            }   
        }
        //想历史记录表中填充信息
        sprintf(caozuo,"普通用户查看了自己的信息");
        sprintf(history_sql,"insert into history values( '%s' , '%s' , '%s' , '%s' )",data,time,operation_user,caozuo);
        if(sqlite3_exec(db,history_sql,NULL,NULL,&errmsg)!=SQLITE_OK){
            fprintf(stderr,"__%d__ sqlite3_exec:%s\n",__LINE__,errmsg);
            return ;
        }
        break;
    case 6:      //6代表普通用户修改
        strcpy(mybrr,administrator1.buf);
        administrator1 = member_set(administrator1);
        for(int i=1;i<strlen(mybrr);i++){
            //循环判断除第一个标识符外的字符，进入不同的修改命令
            switch(mybrr[i]){
                case '1':       //用户修改名字
                    bzero(sql,sizeof(sql));
                    sprintf(sql,"update staff set name = '%s' where account = '%s' ",administrator1.st.name,loaddr_user);
                    if(sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK){
                        fprintf(stderr, "__%d__ sqlite3_exec:%s\n", __LINE__, errmsg); 
                        return ;
                    }
                    break;
                case '2':       //用户修改年龄
                    bzero(sql,sizeof(sql));
                    sprintf(sql,"update staff set age = %d where account = '%s' ",administrator1.st.age,loaddr_user);
                    if(sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK){
                        fprintf(stderr, "__%d__ sqlite3_exec:%s\n", __LINE__, errmsg); 
                        return ;
                    }
                    break;
                case '3':       //用户修改电话
                    bzero(sql,sizeof(sql));
                    sprintf(sql,"update staff set calling = %d where account = '%s' ",administrator1.st.calling,loaddr_user);
                    if(sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK){
                        fprintf(stderr, "__%d__ sqlite3_exec:%s\n", __LINE__, errmsg); 
                        return ;
                    }
                    break;
                case '4':       //用户修改地址
                    bzero(sql,sizeof(sql));
                    sprintf(sql,"update staff set place = '%s' where account = '%s' ",administrator1.st.place,loaddr_user);
                    if(sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK){
                        fprintf(stderr, "__%d__ sqlite3_exec:%s\n", __LINE__, errmsg); 
                        return ;
                    }
                    break;
                }
            }
            //回复应答消息
            strcpy(administrator1.buf,"ok");
            administrator1.buf[strlen(administrator1.buf)]=0;
            if(send(newfd,&administrator1,sizeof(administrator1),0)<0){
                //ERR_MSG("send");
                return ;
            }
            //向数据库中的历史记录表填充信息
            sprintf(caozuo,"普通用户修改了自己的信息");
            sprintf(history_sql,"insert into history values( '%s' , '%s' , '%s' , '%s' )",data,time,operation_user,caozuo);
            if(sqlite3_exec(db,history_sql,NULL,NULL,&errmsg)!=SQLITE_OK){
                fprintf(stderr,"__%d__ sqlite3_exec:%s\n",__LINE__,errmsg);
                return ;
            }
        break;
    case 7:      //7代表管理员验证登录函数
        bzero(sql,sizeof(sql));
        administrator1.st.account[strlen(administrator1.st.account)-1]=0;
        administrator1.st.password[strlen(administrator1.st.password)-1]=0;
        sprintf(sql,"select * from staff where account ='%s' and password ='%s' ",administrator1.st.account,administrator1.st.password);
        //sql[strlen(sql)-1]='\n';
        if(sqlite3_exec(db, sql, callback, &flag, &errmsg) != SQLITE_OK){
            fprintf(stderr, "__%d__ sqlite3_exec:%s\n", __LINE__, errmsg); 
            return ;
        }
        //未执行回调函数时触发发送default0消息
        if(flag == 0){
            strcpy(administrator1.buf,"default0");
            if(send(newfd,&administrator1,sizeof(administrator1),0)<0){
                //ERR_MSG("send");
                return ;
            }   
        }
        break;
    case 8:  //用户登录
        bzero(sql,sizeof(sql));
        administrator1.st.account[strlen(administrator1.st.account)-1]=0;
        administrator1.st.password[strlen(administrator1.st.password)-1]=0;
        sprintf(sql,"select * from staff where account ='%s' and password ='%s' ",administrator1.st.account,administrator1.st.password);
        //执行sqlite语句，进入回调函数
        if(sqlite3_exec(db, sql, callback1, &flag, &errmsg) != SQLITE_OK){
            fprintf(stderr, "__%d__ sqlite3_exec:%s\n", __LINE__, errmsg); 
            return ;
        }
        //未执行回调函数时触发发送default0消息
        if(flag == 0){
            strcpy(administrator1.buf,"default0");
            if(send(newfd,&administrator1,sizeof(administrator1),0)<0){
                //ERR_MSG("send");
                return ;
            }   
        }
        break;
    default:
        break;
    }
}
//管理员登录时的回调函数
int callback(void *arg,int column,char** column_text,char**column_name){
    char buf[128]="";
    bzero(buf,sizeof(buf));
    int i=0;
    if(0 == *(int*)arg){
        for(i=0;i<column;i++){
        }
        *(int*)arg=1;
    }
    for(i=0;i<column;i++){
    }
    //登陆时将操作者记录下来
    strcpy(operation_user,column_text[1]);
    operation_user[strlen(operation_user)]=0;
    //对比成员身份，判断登陆时使用的账户密码是否符合
    if(!strcmp(column_text[0],"0")){
        strcpy(administrator1.buf,"ok");
        flag = 1;
        if(send(newfd,&administrator1,sizeof(administrator1),0)<0){
            //ERR_MSG("send");
            return 0;
        }
    }
    //不符合时触发发送default1消息
    else{
        strcpy(administrator1.buf,"default1");
        if(send(newfd,&administrator1,sizeof(administrator1),0)<0){
            //ERR_MSG("send");
            return 0;
        }
    }
    return 0;
}

//普通用户登录时的回调函数
int callback1(void *arg,int column,char** column_text,char**column_name){
    char buf[128]="";
    bzero(buf,sizeof(buf));
    int i=0;
    if(0 == *(int*)arg){
        for(i=0;i<column;i++){
        }
        *(int*)arg=1;
    }
    for(i=0;i<column;i++){
    }
    //操作时将操作者和对应操作人记录下来
    strcpy(operation_user,column_text[1]);
    operation_user[strlen(operation_user)]=0;
    strcpy(loaddr_user,column_text[2]);
    loaddr_user[strlen(loaddr_user)]=0;
    //对比成员身份，判断登陆时使用的账户密码是否符合
    if(!strcmp(column_text[0],"1")){
        strcpy(administrator1.buf,"ok");
        flag = 1;
        if(send(newfd,&administrator1,sizeof(administrator1),0)<0){
            //ERR_MSG("send");
            return 0;
        }
    }
    //不符合时触发发送default1消息
    else{
        strcpy(administrator1.buf,"default1");
        if(send(newfd,&administrator1,sizeof(administrator1),0)<0){
            //ERR_MSG("send");
            return 0;
        }
    }
    return 0;
}

//修改数据时验证账户
int callback2(void *arg,int column,char** column_text,char**column_name){
    char buf[128]="";
    bzero(buf,sizeof(buf));
    int i=0;
    if(0 == *(int*)arg){
        for(i=0;i<column;i++){
        }
        *(int*)arg=1;
    }
    for(i=0;i<column;i++){
    }
    //记录普通成员登陆时的名字
    strcpy(updater_user,column_text[2]);
    updater_user[strlen(updater_user)]=0;
    //查询到了回应一个ok包
    strcpy(administrator1.buf,"ok");
    flag = 1;
    if(send(newfd,&administrator1,sizeof(administrator1),0)<0){
        //ERR_MSG("send");
        return 0;
    }
    return 0;
}

//普通成员查看单个成员的回调函数
int callback4(void *arg,int column,char** column_text,char**column_name){
    char buf[128]="";
    bzero(buf,sizeof(buf));
    int i=0;
    if(0 == *(int*)arg){
        for(i=0;i<column;i++){
        }
        *(int*)arg=1;
    }
    for(i=0;i<column;i++){
    }
    strcpy(administrator1.buf,"ok");
    flag = 1;
    //发送对应成员的信息且成员只可以查看自己的信息
    strcpy(administrator1.st.name,column_text[1]);
    strcpy(administrator1.st.account,column_text[2]);
    strcpy(administrator1.st.sex,column_text[4]);
    strcpy(administrator1.send_age,column_text[5]);
    strcpy(administrator1.send_calling,column_text[6]);
    strcpy(administrator1.st.place,column_text[7]);
    //回应一个OK包
    if(send(newfd,&administrator1,sizeof(administrator1),0)<0){
        //ERR_MSG("send");
        return 0;
    }
    
    return 0;
}

//管理员查看单个人员时的回调函数
int callback3(void *arg,int column,char** column_text,char**column_name){
    char buf[128]="";
    bzero(buf,sizeof(buf));
    int i=0;
    if(0 == *(int*)arg){
        for(i=0;i<column;i++){
        }
        *(int*)arg=1;
    }
    for(i=0;i<column;i++){
    }
    strcpy(administrator1.buf,"ok");
    flag = 1;
    //拷贝表中的信息，且全部为字符串类型
    strcpy(administrator1.send_usertype,column_text[0]);
    strcpy(administrator1.st.name,column_text[1]);
    strcpy(administrator1.st.account,column_text[2]);
    strcpy(administrator1.st.password,column_text[3]);
    strcpy(administrator1.st.sex,column_text[4]);
    strcpy(administrator1.send_age,column_text[5]);
    strcpy(administrator1.send_calling,column_text[6]);
    strcpy(administrator1.st.place,column_text[7]);
    strcpy(administrator1.send_level,column_text[8]);
    strcpy(administrator1.send_money,column_text[9]);
    //回应一个ok包
    if(send(newfd,&administrator1,sizeof(administrator1),0)<0){
        //ERR_MSG("send");
        return 0;
    }
    
    return 0;
}

//成员处理函数
STAFF member_set(STAFF administrator1){
    bzero(administrator1.buf,sizeof(administrator1.buf));
    administrator1.st.account[strlen(administrator1.st.account)]=0;
    administrator1.st.password[strlen(administrator1.st.password)]=0;
    administrator1.st.name[strlen(administrator1.st.name)]=0;
    administrator1.st.sex[strlen(administrator1.st.sex)]=0;
    administrator1.st.place[strlen(administrator1.st.place)]=0;
    return administrator1;
}
