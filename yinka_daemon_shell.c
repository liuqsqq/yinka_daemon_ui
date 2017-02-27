#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "yinka_daemon_shell.h"


//static struct Cmd_List cmd_list; //������ʷ��������  

static List cmd_list; //������ʷ��������  
extern char* cmd_set[];


/*��ʼ������*/  
void Initlist(List *list)  
{  
    Node *s = (Node *)malloc(sizeof(Node));  
    assert(s != NULL);  
    s->next = s->pre = s;  
    list->first = list->last = s;  
    list->size = 0;  
}  
/*��ӡ��������*/  
void shell_show()  
{  
    int y,x;
    List *list;
    list = &cmd_list;
    Node *s = list->first->next;  
    while(s != list->first)  
    {  
        getyx(stdscr,y,x);  
        mvprintw(y+1,0,"cmdstr: %s,len: %d",s->data.cmd_p,s->data.len);  
        refresh();  
        s = s->next;  
    }  
    //cout<<"NULL"<<endl;  
}  



/*ͷ�巨*/  
bool Push_front(List *list,ElemType x)  
{  
    Node *s = (Node *)malloc(sizeof(Node));  
    if(s != NULL)  
    {  
        s->data = x;  
        /*�ͺ���Ԫ�أ�ԭ���ĵ�һ��Ԫ�أ�����*/  
        s->next = list->first->next;  
        list->first->next->pre = s;  
        /*��ͷ�������*/  
        list->first->next = s;  
        s->pre = list->first;  
        /*���ǵ�һ���ڵ㣬βָ���ָ����*/  
        if(list->size == 0)  
        {  
            list->last = s;  
        }  
        list->size++;  
        return true;  
    }  
    else  
    {  
        //cout<<"����ռ�ʧ�ܣ�"<<endl;  
        return false;  
    }  
} 

/*β�巨*/  
bool Push_back(List *list,ElemType x)  
{  
    Node *s = (Node *)malloc(sizeof(Node));  
    if(s != NULL)  
    {  
        s->data = x;  
        list->last->next = s;  
        s->pre = list->last;  
  
        list->last = s;  
        list->last->next = list->first;  
        list->first->pre = list->last;  
        //s->next = list->first;  
        //list->last = s;  
        list->size++;  
        return true;  
    }  
    else  
        return false;  
}  

/*�ж������Ƿ�Ϊ��*/  
bool Isempty(List *list)  
{  
    return (list->size == 0);  
}  

bool Pop_front(List *list)  
{  
    if(Isempty(list))  
    {  
        //cout<<"�����ѿգ�"<<endl;  
        return false;  
    }  
    Node *s = list->first->next;//�ҵ���һ���ڵ㣨Ҫ�ͷŵģ�  
    /*����*/  
    list->first->next = s->next;  
    s->next->pre = list->first;  
    /*���Ҫɾ���Ľ���ǵ�һ���ڵ㣬βָ��Ҫ�ı�ָ��*/  
    if(list->size == 1)  
    {  
        list->last = list->first;  
    }  
    free(s);  
    list->size--;  
    return true;  
}  

/*βɾ��*/  
bool Pop_back(List *list)  
{  
    if(Isempty(list))  
    {  
        //cout<<"�����ѿգ�"<<endl;  
        return false;  
    }  
    Node *s = list->last->pre;//�ҵ����һ���ڵ��ǰ��  
    free(list->last);//�ͷ����һ���ڵ�  
    /*����*/  
    s->next = list->first;  
    list->last = s;  
    list->size--;  
    return true;  
} 

/*�������*/  
void clear_list(List *list)  
{  
    Node *s = list->first->next;//s����ָ�������еĵ�һ���ڵ�  
    while(s != list->first)  
    {  
        list->first->next = s->next;//�������еĵ�һ�����ճ���  
        free(s);//�ͷŵ�һ���ڵ�  
        s = list->first->next;//����ָ���µĵ�һ���ڵ�  
    }  
    list->last = list->first;  
    list->size = 0;  
}  
/*��������*/  
void destory_list(List *list)  
{  
    clear_list(list);  
    free(list->first);  
    list->first = list->last = NULL;  
}  


void shell_init()  
{  
    printf("\nshell_init");  
    initscr();  
    cbreak();  
    noecho();  
    scrollok(stdscr,TRUE);  
    //setscrreg(0,1000);  
    //idlok(stdscr,TRUE);  
    keypad(stdscr,TRUE);  
    refresh();  
    Initlist(&cmd_list);
}  
void shell_exit()  
{  
    struct Node* p=cmd_list.first;  
    int listsize = 0;
    nocbreak();  
    echo();  
    keypad(stdscr,FALSE);  
    endwin();  
    listsize = cmd_list.size;
    while(listsize--)  
    {  
        p=p->next;  
        free(p->data.cmd_p);  
        free(p);  
    }  
    printf("shell exit\n");  
}  

#if 0
void hprintf(char * cmd, ...)
{ 
     getmaxyx(stdscr,y,x);  
     mvprintw(y,0,cmd,...);  
     refresh();  
}
#endif
int  parse_cmd(int *id)
{
    int y,x;
    List *list;
    list = &cmd_list;
    Node *s = NULL;
    char * para[4] = {0};
    char *pargv = NULL;
    char *ptemp = NULL;
    char *key_point = NULL;
    int argc_cnt = 0 ;
    int strlen = 0;
    int ret = -1;
    int program_id = 0;
    if (!Isempty(list))
    {
        s = list->last;
        

        pargv = strdup(s->data.cmd_p);
        #if 0
        getyx(stdscr,y,x);  
        mvprintw(y+1,0,"pargv: %s",pargv);  
        refresh(); 
        #endif
        
        ptemp = pargv;
        while (ptemp)
        {
            while(key_point = strsep(&ptemp," "))
            {
                if (*key_point == 0)
                {
                    continue;
                }
                else
                {
                    break;
                }
            }
            if (key_point != NULL)
            {
                para[argc_cnt] = strdup(key_point);
                argc_cnt++;
                
                #if 0
                getyx(stdscr,y,x);  
                mvprintw(y+1,0,"cmdstr: %s",key_point);  
                refresh(); 
                #endif
            }
        }
        if (argc_cnt == 1)
        {
            if (strcmp(para[0], "exit") == 0)
            {
                #if 0
                getyx(stdscr,y,x);  
                mvprintw(y+1,0,"cmdstr is : exit");  
                refresh(); 
                #endif
                ret = CMD_EXIT;
            } 
            else if (strcmp(para[0], "help") == 0)
            {
                #if 0
                getyx(stdscr,y,x);  
                mvprintw(y+1,0,"cmdstr is : help");  
                refresh();  
                #endif
                ret = CMD_HELP;
            }    
        }
        else if (argc_cnt == 3)
        {
            if ((strcmp(para[0], "show") == 0) &&
                   (strcmp(para[1], "status") == 0) &&
                   (strcmp(para[2], "program") == 0) )
            {
                #if 0
                getyx(stdscr,y,x);  
                mvprintw(y+1,0,"cmdstr is : %s",cmd_set[SHOWSTATUS]);  
                refresh();
                #endif
                ret = CMD_SHOWSTATUS;
            }
            else if ((strcmp(para[0], "start") == 0) &&
                   (strcmp(para[1], "daemon") == 0) &&
                   (strcmp(para[2], "program") == 0) )
            {
                #if 0
                getyx(stdscr,y,x);  
                mvprintw(y+1,0,"cmdstr is : %s",cmd_set[STARTDAEMON]);  
                refresh();
                #endif
                ret = CMD_STARTDAEMON;
            }
            else if((strcmp(para[0], "stop") == 0) &&
                   (strcmp(para[1], "daemon") == 0) &&
                   (strcmp(para[2], "program") == 0) )
            {
                #if 0
                getyx(stdscr,y,x);  
                mvprintw(y+1,0,"cmdstr is : %s",cmd_set[STOPDAEMON]);  
                refresh();
                #endif
                ret = CMD_STOPDAEMON;
            }
            else if((strcmp(para[0], "reset") == 0) &&
                   (strcmp(para[1], "update") == 0) &&
                   (strcmp(para[2], "now") == 0) )
            {
                ret = CMD_RESETUPDATE;
            }
        }
        else if(argc_cnt == 4)
        {
            program_id =  atoi(para[3]);
            if ((strcmp(para[0], "show") == 0) &&
                   (strcmp(para[1], "status") == 0) &&
                   (strcmp(para[2], "program") == 0) )
            { 
                #if 0
                getyx(stdscr,y,x);  
                mvprintw(y+1,0,"cmdstr is : %s %d",cmd_set[SHOWSTATUS], program_id);  
                refresh();
                #endif
                ret = CMD_SHOWSTATUS;
            }
            else if ((strcmp(para[0], "start") == 0) &&
                   (strcmp(para[1], "daemon") == 0) &&
                   (strcmp(para[2], "program") == 0) )
            {
                #if 0
                getyx(stdscr,y,x);  
                mvprintw(y+1,0,"cmdstr is : %s %d",cmd_set[STARTDAEMON], program_id);  
                refresh();
                #endif
                ret = CMD_STARTDAEMON;
            }
            else if ((strcmp(para[0], "stop") == 0) &&
                (strcmp(para[1], "daemon") == 0) &&
                (strcmp(para[2], "program") == 0) )
            {
                #if 0
                getyx(stdscr,y,x);  
                mvprintw(y+1,0,"cmdstr is : %s %d",cmd_set[STOPDAEMON], program_id);  
                refresh(); 
                #endif
                ret = CMD_STOPDAEMON;
            }
        }
        free(pargv);
    }
    *id = program_id;
    return ret;
}

int get_cmd()  
{  
    struct line_editor l;  
    memset(l.chars,0,512);  
    l.tail=-1;  
    l.cur=-1;  


    ElemType cmd_data;
    int c=' ';  
    int x,y;  
    struct Node* cmd_cur=NULL;  
        
    while(1)  
    {  
        c=getch();  
        if(c=='\n')  
        {  
            break;    
        }  
        // for normal characters  
        else if(c >= 0x20 && c <= 0x7E)     
        {  
            if(l.cur>=l.tail)  
            {  
                l.cur++;  
                l.chars[l.cur]=c;  
                l.tail=l.cur;  
                l.chars[l.tail+1]='\0';  
            }  
            else  
            {  
                //for data structure  
                l.cur++;  
                memmove(&(l.chars[l.cur])+1,&(l.chars[l.cur]),l.tail-l.cur+1);  
                l.tail++;  
                l.chars[l.cur]=c;  
                l.chars[l.tail+1]='\0';  
  
                //for screen edit  
                getyx(stdscr,y,x);  
                mvaddstr(y,x+1,&(l.chars[l.cur])+1);  
                move(y,x);  
                refresh();  
            }  
            echochar(c);  
            refresh();  
        }  
        //for backspace  
        else if(c==263)   
        {  
            if(l.cur>=0)  
            {  
                if(l.cur<l.tail)  
                {  
                    memmove(&(l.chars[l.cur]),&(l.chars[l.cur])+1,l.tail-l.cur);  
                }  
                l.tail--;  
                l.chars[l.tail+1]='\0';  
                l.cur--;  
  
                getyx(stdscr,y,x);  
                mvaddch(y,strlen("yinkadameonctl# ")+l.tail+1,' ');  
                refresh();  
                mvaddstr(y,x-1,&(l.chars[l.cur])+1);      
                move(y,--x);  
                refresh();  
            }  
        }  
        // for arrow key left  
        else if(c==KEY_LEFT)  
        {  
            if(l.cur>=0)  
            {  
                getyx(stdscr,y,x);  
                move(y,--x);  
                refresh();  
                l.cur--;  
            }  
        }  
        // for arrow key right  
        else if(c==KEY_RIGHT)  
        {  
            getyx(stdscr,y,x);  
            if(l.cur<l.tail)  
            {  
                move(y,++x);  
                refresh();  
                l.cur++;      
            }  
        }  
        // for arrow key up  
        else if(c==KEY_UP)  
        {  
            //for the first time click  
            if(cmd_cur==NULL)     
            {  
                cmd_cur=cmd_list.last;  
            }  
            //if current position has not reached the head  
            else if(cmd_cur!=cmd_list.first)   
            {       
                cmd_cur=cmd_cur->pre;
                if (cmd_cur == cmd_list.first)
                {
                    cmd_cur = cmd_cur->next;
                }
            }

            if (Isempty(&cmd_list))
            {
                cmd_cur = NULL;
            }

            //if the history command cycle bi-direct list is null  
            //and the current position is not the tail  
            if(cmd_cur!=NULL)  
            {  
                getyx(stdscr,y,x);  
                int len=l.tail + 1;  
                int i=0;  
                while(i<=len)  
                {  
                    mvaddch(y,strlen("yinkadameonctl# ")+i++,' ');  
                }  
                refresh();  
                mvaddstr(y,strlen("yinkadameonctl# "),cmd_cur->data.cmd_p);  
                refresh();  
                memcpy(l.chars,cmd_cur->data.cmd_p,cmd_cur->data.len); 
                l.tail=cmd_cur->data.len-1;  
                l.cur=l.tail;  
            }  
        }  
        // for arrow key down  
        else if(c == KEY_DOWN)  
        {  
            //for the first time click  
            if(cmd_cur==NULL)     
            {  
                cmd_cur=cmd_list.last;  
            }  
            //if current position has not reached the tail  
            else if(cmd_cur != cmd_list.last)   
            {  
                cmd_cur=cmd_cur->next;  
            } 
            
            if (Isempty(&cmd_list))
            {
                cmd_cur = NULL;
            }

            //if the history command cycle bi-direct list is null  
            //and the current position is not the tail  
            if(cmd_cur!=NULL)  
            {  
                getyx(stdscr,y,x);  
                int len=l.tail + 1;  
                int i=0;  
                while(i<=len)  
                {  
                    mvaddch(y,strlen("yinkadameonctl# ")+i++,' ');  
                }  
                refresh();  
                mvaddstr(y,strlen("yinkadameonctl# "),cmd_cur->data.cmd_p);  
                refresh();  
                memcpy(l.chars,cmd_cur->data.cmd_p,cmd_cur->data.len); 
                l.tail=cmd_cur->data.len-1;  
                l.cur=l.tail;  
            }  
        }      
    }
    if (strlen(l.chars) != 0)
    {
        cmd_data.cmd_p = strdup(l.chars);
        cmd_data.len = strlen(l.chars);
        Push_back(&cmd_list, cmd_data);
    }
    else
    {
        return 1;
    }
    return 0;
}


