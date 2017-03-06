#include "yinka_daemon_shell.h"
#include <curses.h>
#include <sys/types.h>

#include <sys/socket.h>
#include <sys/un.h>
#include<netinet/in.h>
#include <sys/select.h>
#include <arpa/inet.h>

#define MAX_COLUMN   (30)

char* cmd_set[]={  
    "help - printf help",  
    "exit - exit ",  
    "show status program (program_id) - show program's status info",  
    "start daemon program (program_id) - allow program's daemon",  
    "stop daemon program - close program's daemon",
    "reset update now - force to update once from server",
    "reboot daemon program (program_id)- force to reboot program"
}; 


int toHelp();
int toExit();
int toShow();
int toStartDaemon();
int toStopDaemon();
int toResetUpdate(int para);
int toRebootProgram(int program_id);

int receive_data(int* recv_result, char* msgbuf);
int send_msg_daemon_server(int program_id, int daemon_switch);


static int g_yinka_daemon_client_sock;
static struct sockaddr_in daemon_server_addr;
static struct sockaddr_in update_server_addr;


#define MAX_STR_LEN (64)
#define MSGBUF_SIZE (512)  
#define RECEIVE_TIMEOUT      (-2)
#define RECEIVE_ERROR        (0)
#define RECEIVE_OK           (0)


#define TYPE_CONTROL_CMD         (0)
#define TYPE_KEEPALIVE           (1)
#define TYPE_RES_STATISTIC       (2)
#define TYPE_UPDATE_CONTROL_CMD  (3)


#define TYPE_YINKA_PRINT   (1)
#define TYPE_YINKA_ADS     (2)

#define MAX_DAMEON_PROGRAMS_NUMS    (2)


typedef int (*pfToState)(int program_id);
pfToState g_pFun[] = {toHelp, toExit, toShow, toStartDaemon, toStopDaemon, toResetUpdate, toRebootProgram}; //状态枚举值对应下标


typedef struct {
    unsigned short type;
    unsigned short len;
    char data[0];
}YINKA_DAMEON_TLV_T;

typedef struct {
    int version;
    char prog_name[MAX_STR_LEN];
    int cpurate;
    int memrate;
    long uptime;
    int reboot_times;
    int keepalive_failed_times; 
    int state;
}PROGRAM_STATISTIC_T;



int toHelp(int program_id)
{ 
    int x,y;
  
    for (int i = 0; i < (CMD_MAX-1); i++)
    {
        getyx(stdscr,y,x);  
        mvprintw(y+1,0,cmd_set[i]);  
        refresh();      
    }
    return 0;

}
int toExit(int program_id)
{
    
}
int toShow(int program_id)
{
    int x,y;
    int recv_bytes = 0;
    char msgbuf[MSGBUF_SIZE]; /* Buffer for messages being sent or received */      
    int ret = -1;
    int data_len = 0;
    int type = 0;
    int value_len = 0;
    float memrate = 0.0;
    float cpurate = 0.0;
    char * p_Yinka_Dameon_UI_temp= NULL;
    YINKA_DAMEON_TLV_T *pYinkaDameonUI = NULL;

    PROGRAM_STATISTIC_T *pYinkaDameonUIInfo = NULL;
    int msg_nums = 0;
    char * pDaemonShowtemp = NULL;
    
    ret = send_msg_daemon_server(program_id, 2);
    if (ret < 0){
        getyx(stdscr,y,x);  
        mvprintw(y+1,0,"Send to Yinka Daemon error"); 
        refresh();   
        return -1;
    }
    ret = receive_data(&recv_bytes, msgbuf);
    if (ret == RECEIVE_OK){
        #if 0
        getyx(stdscr,y,x);  
        mvprintw(y+1,0,"\nReceive %d bytes", recv_bytes); 
        refresh();
        #endif 
        p_Yinka_Dameon_UI_temp = msgbuf;
        
        pYinkaDameonUI = (YINKA_DAMEON_TLV_T *)p_Yinka_Dameon_UI_temp;
        type = ntohs(pYinkaDameonUI->type);
        data_len += sizeof(unsigned short);
        data_len += sizeof(unsigned short);
        value_len = ntohs(pYinkaDameonUI->len);
        
        #if 0   
        getyx(stdscr,y,x);  
        mvprintw(y+1,0,"type=%d,len=%d", type, value_len);  
        mvprintw(y+2,0,"value:\n"); 
        refresh();
        
  
        for (int i = 0; i < value_len; i++)
        {
            getyx(stdscr,y,x);  
            mvprintw(y,x,"%2x ", pYinkaDameonUI->data[i]);   
            refresh();
        }
        #endif 
        
        if (type == TYPE_RES_STATISTIC){            
            msg_nums = ntohs(*(unsigned short *)(pYinkaDameonUI->data));
            pYinkaDameonUIInfo = (PROGRAM_STATISTIC_T*)(p_Yinka_Dameon_UI_temp + data_len +sizeof(unsigned short));
            
            #if 0   
            getyx(stdscr,y,x);  
            mvprintw(y+1,0,"msg_num: %d", msg_nums);  
            refresh();  
            #endif
            
            for (int j = 0; j < msg_nums;j++){
                getyx(stdscr,y,x);  
                mvprintw(y+1,0,"Prog_name:%s", pYinkaDameonUIInfo->prog_name); 
                mvprintw(y+2,0,"Version:%d", ntohl(pYinkaDameonUIInfo->version));
                mvprintw(y+3,0,"Uptime:%ld", ntohl(pYinkaDameonUIInfo->uptime)); 
                memrate = (float)ntohl(pYinkaDameonUIInfo->memrate)/1000;
                cpurate = (float)ntohl(pYinkaDameonUIInfo->cpurate)/1000;
                mvprintw(y+4,0,"Memrate:%f%%", memrate);
                mvprintw(y+5,0,"Cpurate:%f%%", cpurate); 
                mvprintw(y+6,0,"Reboot_times:%d\n", ntohl(pYinkaDameonUIInfo->reboot_times));
                refresh(); 
                pYinkaDameonUIInfo++;
            }
        }
                                  
    }
    return 0;
}



int receive_data(int* recv_result, char* msgbuf)
{
    int x,y ;
    int server_addr_len = sizeof(daemon_server_addr);
    struct timeval timeout; //Time out setting  
    fd_set fdst;    //File descriptor for select function 


    timeout.tv_sec = 3; 
    timeout.tv_usec = 0;
    FD_ZERO(&fdst);//Clear file descriptor set to enable status checking 
    FD_SET(g_yinka_daemon_client_sock, &fdst);//add fiel descriptor to the set  
    
    int sltRlt = select(g_yinka_daemon_client_sock + 1, &fdst, NULL, NULL, &timeout);
    if (sltRlt < 0) 
    { //Error in the select function  
        return RECEIVE_ERROR;  
    } 
    else if (sltRlt == 0)
    {
        //timeout
        return RECEIVE_TIMEOUT;
    }
    else
    {
        *recv_result = recvfrom(g_yinka_daemon_client_sock, msgbuf,  
                MSGBUF_SIZE, 0,(struct sockaddr*) (&(daemon_server_addr)),&server_addr_len);
        if (*recv_result > 0)
        {
            return RECEIVE_OK;
        }
        else
        {
            return RECEIVE_ERROR;
        }
    }
    
}

int send_msg_daemon_server(int program_id, int daemon_switch)
{
    int x,y;
    char buffer[512];
    int buffer_len = 0;
    int ret = 0;

    char * pstr = NULL;  
    YINKA_DAMEON_TLV_T * pvalue = NULL;
    YINKA_DAMEON_TLV_T * ptemp = (YINKA_DAMEON_TLV_T *)buffer;
    ptemp->type = htons(TYPE_CONTROL_CMD);

    buffer_len = 2 + 2;
    pvalue = (YINKA_DAMEON_TLV_T *)ptemp->data;
    pstr =  ptemp->data;
    if (program_id == 0)
    {
        program_id = 0xffff;
    }
    if (program_id != 0xffff)
    {
        pvalue->type = htons(program_id);
        pvalue->len = htons(1);
        *(char*)pvalue->data = daemon_switch;
        buffer_len += 2 + 2 + 1;
    }
    else
    {
        pvalue->type = htons(program_id);
        pvalue->len = htons(1);
        *(char*)pvalue->data = daemon_switch;
        buffer_len += 2 + 2 + 1;
        #if 0
        for (int i = 1; i < (MAX_DAMEON_PROGRAMS_NUMS + 1);i++)
        {     
            pvalue = (YINKA_DAMEON_TLV_T *)pstr;
            pvalue->type = htons(i);
            pvalue->len = htons(1);
            *(char*)pvalue->data = daemon_switch;  
            buffer_len += 2 + 2 + 1;
            pstr += buffer_len - 4;
        }
        #endif
    }
    ptemp->len = htons(buffer_len - 2 - 2);
    
    #if 0
    for (int i = 0; i < buffer_len; i++)
    {
        getyx(stdscr,y,x);  
        mvprintw(y,x+1,"%2x ", buffer[i]);   
        refresh();
    }
    #endif
           

    ret = sendto(g_yinka_daemon_client_sock, buffer, buffer_len, 0, \
        (struct sockaddr*)&daemon_server_addr, sizeof(daemon_server_addr));
    if (ret < 0)
    {
        perror("send failed");
        return -1;
    }
    return 0;
}

int send_msg_to_update_server()
{
    int x,y;
    char buffer[512];
    int buffer_len = 0;
    int ret = -1;
        
    YINKA_DAMEON_TLV_T * pvalue = (YINKA_DAMEON_TLV_T *)buffer;
    pvalue->type = htons(TYPE_UPDATE_CONTROL_CMD);
    pvalue->len = htons(1);
    pvalue->data[0] = 0x01;
    buffer_len = 2 + 2 + 1;
        
    #if 0
    for (int i = 0; i < buffer_len; i++)
    {
        getyx(stdscr,y,x);  
        mvprintw(y,x+1,"%2x ", buffer[i]);   
        refresh();
    }
    #endif
           
    ret = sendto(g_yinka_daemon_client_sock, buffer, buffer_len, 0, \
        (struct sockaddr*)&update_server_addr, sizeof(update_server_addr));
    if (ret < 0)
    {
        perror("send failed");
        return -1;
    }
    return 0;
}

int toStartDaemon(int program_id)
{
    send_msg_daemon_server(program_id, 1);
}

int toStopDaemon(int program_id)
{
    send_msg_daemon_server(program_id, 0);
}

int toResetUpdate(int para)
{
    send_msg_to_update_server();
}

int toRebootProgram(int program_id)
{
    send_msg_daemon_server(program_id, 4);
}


int yinka_daemon_client_init()
{
    struct sockaddr_in client_addr;


    if ( (g_yinka_daemon_client_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket");
        return -1;
    }
    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(0);
    client_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // 设置套接字选项避免地址使用错误  
    int on=1;  
    if((setsockopt(g_yinka_daemon_client_sock,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)))<0)  
    {  
        perror("setsockopt failed");  
        return -1;  
    }  
    
    if (bind(g_yinka_daemon_client_sock, (struct sockaddr *)&client_addr, sizeof(client_addr)) < 0)
    {
        perror("bind");
        return -1;

    } 

    memset(&daemon_server_addr, 0, sizeof(daemon_server_addr));
    daemon_server_addr.sin_family = AF_INET;
    daemon_server_addr.sin_port = htons(12332);
    daemon_server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    memset(&update_server_addr, 0, sizeof(update_server_addr));
    update_server_addr.sin_family = AF_INET;
    update_server_addr.sin_port = htons(12333);
    update_server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");    
    return 0;
}

void main()
{
    int x = 0;
    int y = 0;
    int originx = 0;
    int originy = 0;
    int ret = -1;
    int program_id = -1;
    yinka_daemon_client_init();
    shell_init();
    
    while(1)
    {         
        getyx(stdscr,y,x); 

        if (MAX_COLUMN < (y - originy)) 
        {
            move(0,0);
            getyx(stdscr,originy, originx); 
            getyx(stdscr,y,x);
            clear();
        }
 
        mvaddstr(y+1,0,"yinkadameonctl# ");    
        refresh();  
        ret  = get_cmd();
        if (ret != 0)
            continue;
        //shell_show();
        ret = parse_cmd(&program_id);

        if (ret < 0)
        {
            getyx(stdscr,y,x);  
            mvprintw(y+1,0,"unknown cmd,Usage:");  
            refresh();   
            toHelp(0);
            continue;
        }
        if (ret == CMD_EXIT)
        {
            break;
        }
        g_pFun[ret - 1](program_id);
    }
    
    shell_exit();
}




