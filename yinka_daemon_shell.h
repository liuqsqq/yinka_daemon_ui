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


#define HELP          (1)
#define EXIT          (2)
#define SHOWSTATUS    (3)
#define STARTDAEMON   (4)
#define STOPDAEMON    (5)





int get_cmd();//��ȡ������������  
void shell_exit();//shell��ʼ������  
void shell_init();//shell�˳����� 
void shell_show();
int  parse_cmd(int *id);

#endif
