#ifndef _YINKA_DAEMON_SHELL_
#define _YINKA_DAEMON_SHELL_

struct line_editor  
{  
    char chars[512];//�ַ�����  
    int tail;//���ݽ�β�α�  
    int cur;//��ǰ���ǰ�ַ����α�  
}; 

typedef struct data
{
    char* cmd_p;//��������ָ��  
    int len;//�������ݵĳ���   
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


typedef enum {
    CMD_HELP = 1,
    CMD_EXIT,
    CMD_SHOWSTATUS,
    CMD_STARTDAEMON,
    CMD_STOPDAEMON,
    CMD_RESETUPDATE,
    CMD_MAX
}CMD_TYPE;




int get_cmd();//��ȡ������������  
void shell_exit();//shell��ʼ������  
void shell_init();//shell�˳����� 
void shell_show();
int  parse_cmd(int *id);

#endif
