#ifndef _YINKA_DAEMON_SHELL_
#define _YINKA_DAEMON_SHELL_

struct line_editor  
{  
    char chars[512];//字符内容  
    int tail;//内容结尾游标  
    int cur;//当前光标前字符的游标  
}; 

typedef struct data
{
    char* cmd_p;//命令内容指针  
    int len;//命令内容的长度   
}ElemType;



typedef struct Node  
{  
    struct Node *pre;  
    struct Node *next;  
    ElemType data;  
}Node;  

typedef struct List  
{  
    Node *first;  
    Node *last;  
    int size;  
}List;


#define HELP          (1)
#define EXIT          (2)
#define SHOWSTATUS    (3)
#define STARTDAEMON   (4)
#define STOPDAEMON    (5)





int get_cmd();//获取命令输入内容  
void shell_exit();//shell初始化函数  
void shell_init();//shell退出函数 
void shell_show();
int  parse_cmd(int *id);

#endif
