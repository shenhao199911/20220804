#include "head.h"
void do_administrators(int sfd);
void do_user(int sfd);
void lookfor_messg(int sfd);
void look_func1(int sfd,struct info administrators1);
void look_func2(int sfd,struct info administrators1);
void change_messg(int sfd);
void add_newuser(int sfd);
void delete_user(int sfd);
void lookup_history(int sfd);
void lookup_mymessg(int sfd);
void change_mymessg(int sfd);
STAFF message_encapsulation(int type,int cmd,struct info number1);
STAFF user1;
STAFF message;

int main(int argc, char const *argv[])
{
    int choose;
    int sfd = socket(AF_INET,SOCK_STREAM,0);
    if(sfd < 0){
        ERR_MSG("socket");
        return -1;
    }
    //设置超时时间
    struct timeval tm = {5,0};
    if(setsockopt(sfd,SOL_SOCKET,SO_RCVTIMEO,&tm,sizeof(tm))<0){
        ERR_MSG("setsockopt");
        return -1;
    }

    //填充地址信息结构体
    struct sockaddr_in sin;
    sin.sin_family      = AF_INET;
    sin.sin_port        = htons(PORT);
    sin.sin_addr.s_addr = inet_addr(IP);
    //连接服务器
    if(connect(sfd,(struct sockaddr*)&sin,sizeof(sin))<0){
        ERR_MSG("connect");
        return -1;
    }
    printf("连接服务器成功\n");

    while(1){
        system("clear");
        printf("**************员工管理系统**************\n");
        printf("****************************************\n");
        printf("**************1.管理员登录**************\n");
        printf("**************2. 用户登录 **************\n");
        printf("**************3.  退出    **************\n");
        printf("****************************************\n");
        printf("请输入您的操作>>>>");
        scanf("%d",&choose);
        while(getchar()!=10);

        switch(choose){
            case 1:
                do_administrators(sfd);
                break;
            case 2:
                do_user(sfd);
                break;
            case 3:
                exit(0);
            default:
                printf("输入有误，请按照提示输入\n");
        }
        printf("\n请输入任意字符清屏>>>");
        while(getchar()!=10);
    }
    /*if(recv(sfd,&user1,sizeof(user1),0)<0){
            ERR_MSG("recv");
            return 0;
        }*/
    return 0;
}

/*封装发送的信息结构体
loaddr为判断是管理员还是普通用户登录，0为管理员登录，1位普通用户登录
type为0代表管理员，为1代表普通用户；
cmd为0代表管理员查询，为1代表管理员修改，为2代表管理员添加成员，为3代表管理员删除成员，为4代表管理员查询历史记录
   为5代表普通用户查询，为6代表普通用户修改
brr为应答消息，OK为成功，default为不成功
number1为成员的基本信息
*/
STAFF message_encapsulation(int type,int cmd,struct info number1){
    message.usertype = type;
    message.cmdtype = cmd;
    //strcpy(message.buf,brr);
    //bzero(message.buf,sizeof(message.buf));
    strcpy(message.st.account,number1.account);
    message.st.age = number1.age;
    message.st.calling = number1.calling;
    message.st.level = number1.level;
    message.st.money = number1.money;
    strcpy(message.st.name,number1.name);
    strcpy(message.st.password,number1.password);
    strcpy(message.st.place,number1.place);
    strcpy(message.st.sex,number1.sex);
    return message;
}

//员工处理函数
void do_user(int sfd){
    printf("***************************\n");
    printf("******* 员工登录界面 *******\n");
    printf("请输入员工账号>>>>");
    fgets(user1.st.account,sizeof(user1.st.account),stdin);
    printf("请输入员工密码>>>>>");
    fgets(user1.st.password,sizeof(user1.st.password),stdin);
    user1.cmdtype = userloaddr;
    //发送账户和密码验证是否正确
    if(send(sfd,&user1,sizeof(user1),0)<0){
        ERR_MSG("send");
        return;
    }
    if(recv(sfd,&user1,sizeof(user1),0)<0){
        ERR_MSG("recv");
        return;
    }
    if(!strcmp(user1.buf,"ok")){
        int condition = 1;
        int choose;
        system("clear");
        while(condition){
            printf("*********亲爱的打工人,欢迎您************\n");
            printf("***************************************\n");
            printf("*********  1.查询用户信息  ************\n");
            printf("*********  2.修改用户信息  ************\n");
            printf("*********    3.退出登录    ************\n");
            printf("***************************************\n");
            printf("请输入您的操作>>>>");
            scanf("%d",&choose);
            while(getchar()!=10);

            switch(choose){
                case 1:
                    lookup_mymessg(sfd);
                    break;
                case 2:
                    change_mymessg(sfd);
                    break;
                case 3:
                    condition = 0;
                    break;
                default:
                    printf("输入有误，请按照提示输入\n");
            }
            if(condition){
                printf("\n请输入任意字符清屏>>>\n");
                while(getchar()!=10);
            }
        }
    }
    else if(!strcmp(user1.buf,"default1")){
        printf("请使用普通员工账号登录！！！\n");
    }
    else{
        printf("账号或密码错误，请重试......\n");
    }
    return;
}

//管理员模式处理函数
void do_administrators(int sfd){
    int ret;
    bzero(user1.st.account,sizeof(user1.st.account));
    bzero(user1.st.password,sizeof(user1.st.password));
    printf("***************************************\n");
    printf("*************管理员登录界面*************\n");
    printf("请输入管理员账号>>>>");
    fgets(user1.st.account,sizeof(user1.st.account),stdin);
    printf("请输入账号密码>>>>>");
    fgets(user1.st.password,sizeof(user1.st.password),stdin);
    //user1.usertype = administrators;
    user1.cmdtype =adminatorloaddr;
    //发送账户和密码验证是否正确
    if(send(sfd,&user1,sizeof(user1),0)<0){
        ERR_MSG("send");
        return;
    }
    if(recv(sfd,&user1,sizeof(user1),0)<0){
        ERR_MSG("recv");
        return;
    }
    if(!strcmp(user1.buf,"ok")){
        int condition = 1;
        int choose;
        system("clear");
        while(condition){
            printf("*********尊敬的管理员,欢迎您************\n");
            printf("***************************************\n");
            printf("*********  1.查询用户信息  ************\n");
            printf("*********  2.修改用户信息  ************\n");
            printf("*********  3.添加新的用户  ************\n");
            printf("*********  4.删除用户信息  ************\n");
            printf("*********  5.查询历史记录  ************\n");
            printf("*********    6.退出登录    ************\n");
            printf("***************************************\n");
            printf("请输入您的操作>>>>");
            scanf("%d",&choose);
            while(getchar()!=10);

            switch(choose){
                case 1:
                    lookfor_messg(sfd);
                    break;
                case 2:
                    change_messg(sfd);
                    break;
                case 3:
                    add_newuser(sfd);
                    break;
                case 4:
                    delete_user(sfd);
                    break;
                case 5:
                    lookup_history(sfd);
                    break;
                case 6:
                    condition = 0;
                    break;
                default:
                    printf("输入有误，请按照提示输入\n");
            }
            if(condition){
                printf("\n请输入任意字符清屏>>>\n");
                while(getchar()!=10);
            }
        }
    }
    else if(!strcmp(user1.buf,"default1")){
        printf("请使用管理员账号登录！！！\n");
    }
    else{
        printf("账号或密码错误，请重试......\n");
    }
    return;
}

//1.管理员查询用户信息
void lookfor_messg(int sfd){
    struct info administrators1;
    int choose;
    int condition = 1;
    while(condition){
        system("clear");
        printf("*******知道的越多，手中的筹码就越重*********\n");
        printf("******************************************\n");
        printf("*********  1.查询单个用户信息  ************\n");
        printf("*********  2.查询所有用户信息  ************\n");
        printf("*********   3.返回上级目录     ************\n");
        printf("***************************************\n");
        printf("请输入您的操作>>>>");
        scanf("%d",&choose);
        while(getchar()!=10);

        switch(choose){
            case 1:
                printf("请输入您想查看员工的工号>>>>");
                scanf("%s",administrators1.account);
                look_func1(sfd,administrators1);
                break;
            case 2:
                look_func2(sfd,administrators1);
                break;
            case 3:
                condition = 0;
                break;
            default:
                printf("输入有误，请按照提示输入\n");
        }
        if(condition){
            printf("\n请输入任意字符清屏>>>\n");
            while(getchar()!=10);
        }
    }

}

//查看单个成员的函数
void look_func1(int sfd,struct info administrators1){
    user1 = message_encapsulation(administrators,administrators_search,administrators1);
    strcpy(user1.buf,"0");
    if(send(sfd,&user1,sizeof(user1),0)<0){
        ERR_MSG("send");
        return;
    }
    //阻塞等待服务器回消息
    if(recv(sfd,&user1,sizeof(user1),0)<0){
        ERR_MSG("recv");
        return;
    }
    if(!strcmp(user1.buf,"ok")){
        printf("---员工类型---员工姓名---账户---密码---性别---年龄---电话---地址---工作评级---月薪---\n");
        fprintf(stdout,"--- %s ---",user1.send_usertype);
        fprintf(stdout," %s ---",user1.st.name);
        fprintf(stdout," %s ---",user1.st.account);
        fprintf(stdout," %s ---",user1.st.password);
        fprintf(stdout," %s ---",user1.st.sex);
        fprintf(stdout," %s ---",user1.send_age);
        fprintf(stdout," %s ---",user1.send_calling);
        fprintf(stdout," %s ---",user1.st.place);
        fprintf(stdout," %s ---",user1.send_level);
        fprintf(stdout," %s ---",user1.send_money);
        while(getchar()!=10);
        return;
    }
    else{
        printf("该账户不存在，请输入正确的账户\n");
        sleep(1);
        return;
    }
}

//查看所有成员
void look_func2(int sfd,struct info administrators1){
    int column;
    int condition=1;
    user1 = message_encapsulation(administrators,administrators_search,administrators1);
    strcpy(user1.buf,"1");
    if(send(sfd,&user1,sizeof(user1),0)<0){
        ERR_MSG("send");
        return;
    }
    //阻塞等待服务器回消息
    if(recv(sfd,&column,sizeof(column),0)<0){
        ERR_MSG("recv");
        return;
    }
    printf("---员工类型---员工姓名---账户---密码---性别---年龄---电话---地址---工作评级---月薪---\n");
    for(int i=0;i<column;i++){
        while(condition){
            if(recv(sfd,&user1,sizeof(user1),0)<0){
                ERR_MSG("recv");
                return;
            }
            if(strcmp(user1.buf,"ok")){
                fprintf(stdout,"--- %s ---",user1.send_usertype);
                fprintf(stdout," %s ---",user1.st.name);
                fprintf(stdout," %s ---",user1.st.account);
                fprintf(stdout," %s ---",user1.st.password);
                fprintf(stdout," %s ---",user1.st.sex);
                fprintf(stdout," %s ---",user1.send_age);
                fprintf(stdout," %s ---",user1.send_calling);
                fprintf(stdout," %s ---",user1.st.place);
                fprintf(stdout," %s ---",user1.send_level);
                fprintf(stdout," %s ---",user1.send_money);
                printf("\n");
            }
            else{
                printf("员工表打印完成!!!\n");
                condition=0;
            }
        }
    }
    return;
}

//2.管理员修改用户信息
void change_messg(int sfd){
    struct info administrators2;
    memset(user1.buf,0,sizeof(user1.buf));
    int choose,x1;
    int condition = 1;
    char mybrr[32];
    int myflag[8]={0,0,0,0,0,0,0,0};
    printf("请输入您想修改的用户账户>>>>");
    scanf("%s",administrators2.account);
    user1 = message_encapsulation(administrators,administrators_modify,administrators2);
    //发送账户并验证库中是否存在
    if(send(sfd,&user1,sizeof(user1),0)<0){
        ERR_MSG("send");
        return;
    }
    //阻塞等待服务器回消息
    if(recv(sfd,&user1,sizeof(user1),0)<0){
        ERR_MSG("recv");
        return;
    }
    if(!strcmp(user1.buf,"ok")){
        bzero(mybrr,sizeof(mybrr));
        strcpy(mybrr,"9");
        mybrr[strlen(mybrr)]=0;
        while(condition){
            printf("*********（希望是涨工资才修改信息的）************\n");
            printf("温馨提示:成员的账号为唯一信息，一旦注册无法更改(:\n");
            printf("1.修改该成员的名字\n");
            printf("2.修改该成员的密码\n");
            printf("3.修改该成员的性别\n");
            printf("4.修改该成员的年龄\n");
            printf("5.修改该成员的电话\n");
            printf("6.修改该成员的住址\n");
            printf("7.修改该成员的工作评级\n");
            printf("8.修改该成员的工资\n");
            printf("9.退出修改界面\n");
            printf("***************************************\n");
            printf("请输入您想修改的信息的选项:");
            scanf("%d",&choose);
            while(getchar()!=10);
              
            switch(choose){
                case 1:
                    printf("请问尊贵的管理员您想改成啥新名字嘞:");
                    scanf("%s",administrators2.name);
                    if(myflag[0]==0){
                        strcat(mybrr,"1");
                        mybrr[strlen(mybrr)]=0;
                        myflag[0]==1;
                    }
                    system("clear");
                    break;
                case 2:
                    printf("请问尊贵的管理员您想改成啥新密码嘞:");
                    scanf("%s",administrators2.password);
                    if(myflag[1]==0){
                        strcat(mybrr,"2");
                        mybrr[strlen(mybrr)]=0;
                        myflag[1]==1;
                    }
                    system("clear");
                    break;
                case 3:
                    printf("请问尊贵的管理员您修改后的性别:");
                    scanf("%s",administrators2.sex);
                    if(strcmp(administrators2.sex,"男")==0){
                        x1=0;
                    }
                    else if(strcmp(administrators2.sex,"女")==0){
                        x1=1;
                    }
                    switch(x1){
                        case 0:
                            break;
                        case 1:
                            break;
                        default:
                            system("clear");
                            printf("不可以输入非人的种族哦\n");
                        break;
                    }
                    if(myflag[2]==0){
                        strcat(mybrr,"3");
                        mybrr[strlen(mybrr)]=0;
                        myflag[2]==1;
                    }
                    system("clear");
                    break;
                case 4:
                    printf("请问尊贵的管理员您修改的岁数是:");
                    scanf("%d",&administrators2.age);
                    if(administrators2.age>60 | administrators2.age<18){
                        system("clear");
                        printf("想清楚嗷，这个年纪还让上班？\n");
                        break;
                    }
                    if(myflag[3]==0){
                        strcat(mybrr,"4");
                        mybrr[strlen(mybrr)]=0;
                        myflag[3]==1;
                    }
                    system("clear");
                    break;
                case 5:
                    printf("请问尊贵的管理员您想改新号码是多少呢嘞:");
                    scanf("%d",&administrators2.calling);
                    if(myflag[4]==0){
                        strcat(mybrr,"5");
                        mybrr[strlen(mybrr)]=0;
                        myflag[4]==1;
                    }
                    system("clear");
                    break;
                case 6:
                    printf("请问尊贵的管理员您想改的新地址是多少呢嘞:");
                    scanf("%s",administrators2.place);
                    if(myflag[5]==0){
                        strcat(mybrr,"6");
                        mybrr[strlen(mybrr)]=0;
                        myflag[5]==1;
                    }
                    system("clear");
                    break;
                case 7:
                    printf("请问尊贵的管理员您想改的工作评级是:");
                    scanf("%d",&administrators2.level);
                    if(administrators2.level>5 | administrators2.level<1){
                        system("clear");
                        printf("工作评级就一到五，你想咋地？\n");
                        break;
                    }
                    if(myflag[6]==0){
                        strcat(mybrr,"7");
                        mybrr[strlen(mybrr)]=0;
                        myflag[6]==1;
                    }
                    system("clear");
                    break;
                case 8:
                    printf("偷偷问一下涨了还是跌了:");
                    scanf("%f",&administrators2.money);
                    if(myflag[7]==0){
                        strcat(mybrr,"8");
                        mybrr[strlen(mybrr)]=0;
                        myflag[7]==1;
                    }
                    system("clear");
                    break;
                case 9:
                    condition = 0;
                    break;
                default :
                    system("clear");
                    printf("输入有误，请按照提示输入\n");
                    break;
            }
            if(!condition){
                printf("\n请输入任意字符清屏>>>\n");
                while(getchar()!=10);
            }
        }
        printf("信息已修改成功,您今天又麻烦别人了呢\n");
        user1 = message_encapsulation(administrators,administrators_modify,administrators2);
        strcpy(user1.buf,mybrr);
        //向服务器发送成员信息
        if(send(sfd,&user1,sizeof(user1),0)<0){
            ERR_MSG("send");
            return;
        }
        //阻塞等待服务器回消息
        if(recv(sfd,&user1,sizeof(user1),0)<0){
            ERR_MSG("recv");
            return;
        }
        if(!strcmp(user1.buf,"ok")){
            system("clear"); 
            printf("修改用户信息成功\n");
        }
        else{
            system("clear");
            printf("修改信息未成功，请重试....\n");
        }
        return;
    }
    else{
        printf("该账户不存在，请输入正确的账户\n");
        return;
    }
}

//3.管理员添加新的用户
void add_newuser(int sfd){
    char judge;
    int x1;
    struct info administrators3;
    char user_type;
    printf("*********欢迎加入打工人大家庭************\n");
    printf("请问添加的成员是否为管理员?y/n   ");
    judge = getchar();
    while(getchar()!=10);
    switch(judge){
        case 'y':
            user_type = 0;
            break;
        case 'n':
            user_type = 1;
            break;
        default:
            system("clear");
            printf("请勿输入其他字符!\n");
            return ;
    }
    printf("请输入添加的成员的姓名：");
    scanf("%s",administrators3.name);
    printf("请输入添加的成员的账户：");
    scanf("%s",administrators3.account);
    printf("请输入添加的成员的密码：");
    scanf("%s",administrators3.password);
    printf("请输入添加的成员的性别：");
    scanf("%s",administrators3.sex);
    if(strcmp(administrators3.sex,"男")==0){
        x1=0;
    }
    else if(strcmp(administrators3.sex,"女")==0){
        x1=1;
    }
    switch(x1){
        case 0:
            break;
        case 1:
            break;
        default:
            system("clear");
            printf("你告诉我你输入的是个什么玩意儿？\n");
            printf("***************************************\n");
            return ;
    }
    printf("请输入添加的成员的年龄：");
    scanf("%d",&administrators3.age);
    if(administrators3.age>60 | administrators3.age<18){
        system("clear");
        printf("想清楚嗷，这个年纪还让上班？\n");
        printf("***************************************\n");
        return;
    }
    printf("请输入添加的成员的电话：");
    scanf("%d",&administrators3.calling);
    printf("请输入添加的成员的住址：");
    scanf("%s",administrators3.place);
    printf("请输入添加的成员的工作评级(最低1-最高5):");
    scanf("%d",&administrators3.level);
    if(administrators3.level>5 | administrators3.level<1){
        system("clear");
        printf("工作评级就一到五，你想咋地？\n");
        printf("***************************************\n");
        return ;
    }
    printf("请输入添加的成员的工资：");
    scanf("%f",&administrators3.money);
    user1 = message_encapsulation(user_type,administrators_add,administrators3);

    //向服务器发送成员信息
    if(send(sfd,&user1,sizeof(user1),0)<0){
        ERR_MSG("send");
        return;
    }
    //阻塞等待服务器回消息
    if(recv(sfd,&user1,sizeof(user1),0)<0){
        ERR_MSG("recv");
        return;
    }
    if(!strcmp(user1.buf,"ok")){
        system("clear"); 
        printf("用户信息注册成功\n");
    }
    else{
        system("clear");
        printf("用户信息注册未成功,请重试\n");
    }
    return;
}

//4.管理员删除用户信息
void delete_user(int sfd){
    struct info administrators4;
    printf("***************************************\n");
    printf("请输入您想删除的用户账户>>>>");
    scanf("%s",administrators4.account);
    user1 = message_encapsulation(administrators,administrators_delete,administrators4);
    //向服务器发送成员信息
    if(send(sfd,&user1,sizeof(user1),0)<0){
        ERR_MSG("send");
        return;
    }
    //阻塞等待服务器回消息
    if(recv(sfd,&user1,sizeof(user1),0)<0){
        ERR_MSG("recv");
        return;
    }
    if(!strcmp(user1.buf,"ok")){
        system("clear"); 
        printf("删除用户成功\n");
    }
    else{
        system("clear");
        printf("删除用户未成功，请检查账户是否正确\n");
    }
    return;
}

//5.查看操作历史记录
void lookup_history(int sfd){
    struct info administrators5;
    int column;
    int condition=1;
    user1 = message_encapsulation(administrators,administrators_history,administrators5);
    if(send(sfd,&user1,sizeof(user1),0)<0){
        ERR_MSG("send");
        return;
    }
    //阻塞等待服务器回消息
    if(recv(sfd,&column,sizeof(column),0)<0){
        ERR_MSG("recv");
        return;
    }
    printf("****************************************************\n");
    printf("--------操作日期------操作时间------操作者干了啥-----\n");
    for(int i=0;i<column;i++){
        while(condition){
            if(recv(sfd,&user1,sizeof(user1),0)<0){
                ERR_MSG("recv");
                return;
            }
            if(strcmp(user1.buf,"ok")){
                fprintf(stdout,"-----  %s  -----",user1.send_usertype);
                fprintf(stdout,"  %s  -----",user1.send_calling);
                fprintf(stdout,"  %s",user1.send_level);
                fprintf(stdout,"%s  -----",user1.send_money);
                printf("\n***************************\n");
                printf("\n");
            }
            else{
                printf("操作历史记录打印完成!!!\n");
                printf("\n");
                condition=0;
            }
        }
    }
    return;
}

//6.成员查询函数
void lookup_mymessg(int sfd){
    struct info administrators6;
    user1 = message_encapsulation(user,user_serch,administrators6);
    if(send(sfd,&user1,sizeof(user1),0)<0){
        ERR_MSG("send");
        return;
    }
    //阻塞等待服务器回消息
    if(recv(sfd,&user1,sizeof(user1),0)<0){
        ERR_MSG("recv");
        return;
    }
    printf("*********************************************\n");
    if(!strcmp(user1.buf,"ok")){
        printf("-----员工姓名----账户----性别----年龄----电话----地址-----\n");
        fprintf(stdout," ----- %s ---",user1.st.name);
        fprintf(stdout," %s ---",user1.st.account);
        fprintf(stdout," %s ---",user1.st.sex);
        fprintf(stdout," %s ---",user1.send_age);
        fprintf(stdout," %s ---",user1.send_calling);
        fprintf(stdout," %s ---\n",user1.st.place);
        printf("信息打印完毕，如有想知道其他隐私信息，请找管理员查看\n");
        printf("\n");
        while(getchar()!=10);
        return;
    }
    else{
        printf("信息打印未成功，请重试\n");
        return;
    }
}

//7.普通成员修改信息
void change_mymessg(int sfd){
    struct info administrators7;
    memset(user1.buf,0,sizeof(user1.buf));
    int choose,x1;
    int condition = 1;
    char mybrr[32]={0};
    int myflag[4]={0,0,0,0};

    strcpy(mybrr,"9");
    mybrr[strlen(mybrr)]=0; 
    printf("***************************************************\n");  
    printf("*********（让我康康谁填信息的时候填错了）************\n");
    printf("温馨提示:成员的账号为唯一信息，一旦注册无法更改(:\n");
    printf("1.修改自己的名字\n");
    printf("2.修改自己的年龄\n");
    printf("3.修改自己的电话\n");
    printf("4.修改自己的住址\n");
    printf("5.退出修改界面\n");
    printf("***************************************\n");
    printf("请输入您想修改的信息的选项:");
    scanf("%d",&choose);
    while(getchar()!=10);
              
    switch(choose){
        case 1:
            printf("请输入不孝的您想改成啥新名字嘞:");
            scanf("%s",administrators7.name);
            if(myflag[0]==0){
                strcat(mybrr,"1");
                mybrr[strlen(mybrr)]=0;
                myflag[0]==1;
            }
            system("clear");
            break;
        case 2:
            printf("请输入您今年又长成了多少岁了呢:");
            scanf("%d",&administrators7.age);
            if(administrators7.age>60 | administrators7.age<18){
                system("clear");
                printf("想清楚嗷，这个年纪还让上班？\n");
                break;
            }
            if(myflag[3]==0){
                strcat(mybrr,"4");
                mybrr[strlen(mybrr)]=0;
                myflag[3]==1;
            }
            system("clear");
            break;
        case 3:
            printf("请输入您因为躲债而换的新号码是多少呢嘞:");
            scanf("%d",&administrators7.calling);
            if(myflag[4]==0){
                strcat(mybrr,"5");
                mybrr[strlen(mybrr)]=0;
                myflag[4]==1;
            }
            system("clear");
            break;
        case 4:
            printf("请输入您因为破产而换的新地址是多少呢嘞:");
            scanf("%s",administrators7.place);
            if(myflag[5]==0){
                strcat(mybrr,"6");
                mybrr[strlen(mybrr)]=0;
                myflag[5]==1;
            }
            system("clear");
            break;
        case 5:
            condition = 0;
            break;
        default :
            system("clear");
            printf("输入有误，请按照提示输入\n");
            break;
    }
    if(!condition){
        printf("\n请输入任意字符清屏>>>\n");
        while(getchar()!=10);
    }
    printf("***************************************\n");
    printf("信息已修改成功,您今天又麻烦别人了呢\n");
    printf("***************************************\n");
    user1 = message_encapsulation(user,user_modify,administrators7);
    strcpy(user1.buf,mybrr);
    //向服务器发送成员信息
    if(send(sfd,&user1,sizeof(user1),0)<0){
        ERR_MSG("send");
        return;
    }
    //阻塞等待服务器回消息
    if(recv(sfd,&user1,sizeof(user1),0)<0){
        ERR_MSG("recv");
        return;
    }
    if(!strcmp(user1.buf,"ok")){
        system("clear"); 
        printf("修改用户信息成功\n");
    }
    else{
        system("clear");
        printf("修改信息未成功，请重试....\n");
    }
    return;
}
