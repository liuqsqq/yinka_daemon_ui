#ifndef _YINKA_DAEMON_UI_
#define _YINKA_DAEMON_UI_

#define MAX_COLUMN   (30)




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





int toHelp();
int toExit();
int toShow();
int toStartDaemon();
int toStopDaemon();
int toResetUpdate(int para);
int toRebootProgram(int program_id);

int receive_data(int* recv_result, char* msgbuf);
int send_msg_daemon_server(int program_id, int daemon_switch);
#endif