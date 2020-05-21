#ifndef PROTOCOL_IEC101_104_H
#define PROTOCOL_IEC101_104_H

#include <QtNetwork>
#define SEND 1
#define RECEIVE 0
class IEC101_104
{
public:
    IEC101_104();

    ~IEC101_104();
signals:
    void sendData1(QByteArray &);

void  IEC101_104_processPacket(QTcpSocket *p_socket, QByteArray &data);
void  IEC101_104_spont(QTcpSocket *p_socket);
void  IEC101_104_systemTimeTocp56time2a();
void  IEC101_M_SP_DP_TB_1_handle(QTcpSocket *p_socket,unsigned char point_type);
void  IEC101_M_ME_TF_1_handle(QTcpSocket *p_socket,unsigned char point_type);
void  IEC101_C_CS_NA_1_handle(QTcpSocket *p_socket,QByteArray &data);                 //Time Sync;
void  IEC101_M_SP_DP_NA_1_handle(QTcpSocket *p_socket,unsigned long info_object_address, unsigned int points_number,unsigned char point_type,unsigned char cause);
void  IEC101_C_IC_NA_1_handle(QTcpSocket *p_socket,QByteArray &data);                 //Integrity Poll
void  IEC101_Nothing_handle(QTcpSocket *p_socket,QByteArray &data);
/* CP56Time2a timestamp */
struct Cp56time2a {
    unsigned short		msec;
    unsigned char		min	:6;
    unsigned char		res1	:1;
    unsigned char		iv	:1;
    unsigned char		hour	:5;
    unsigned char		res2	:2;
    unsigned char		su	:1;
    unsigned char		mday	:5;
    unsigned char		wday	:3;
    unsigned char		month	:4;
    unsigned char		res3	:4;
    unsigned char		year	:7;
    unsigned char		res4	:1;
}cp56time2a;


union control_field_struct
{
    unsigned char byte;
    /* U format: Control field */
    struct  {
        unsigned char		FORMAT  :2;
        unsigned char		STARTDT :2;
        unsigned char		STOPDT  :2;
        unsigned char		TESTFR  :2;
    } bit;

};
control_field_struct control_field_u;

//typedef struct control_field_struct control_field_struct;
#define MAX_INTERRO_POINT 10
typedef struct
{
    unsigned char  su;
    unsigned int   lru_common_address_size;     /*RTU Address bytes*/

    unsigned int   lru_address;          /*RTU Address*/
    unsigned int   lru_role;                 /*0: Slave; 1: Master*/
    unsigned int   ioBoardNumber;        /* Number of IO Board*/
    unsigned char  trigger_flag;
    unsigned char  point_type_id;
    unsigned int   common_address_config;
    unsigned int   common_address_received;
    unsigned long int emu_info_object_address;
    unsigned long int di_info_object_address;
    unsigned long int do_info_object_address;
    unsigned long int ai_info_object_address;
    int            interrogate_number_di;
    int            interrogate_number_ai;
    unsigned char  interrogate_start;                      //Bit 0:DI; Bit1: DPS; Bit 2:AI
    unsigned char  interro_di_status[MAX_INTERRO_POINT];
    unsigned char  interro_di_type;
    unsigned char  interro_ai_di_status[MAX_INTERRO_POINT];
    unsigned char  emu_di_status;
    int            emu_ai_value_int;
    float          emu_ai_value_float;
    double         ai_value;
    unsigned char  originator;

} IEC101_104_TOOL_CFG;
IEC101_104_TOOL_CFG cfg;
#define MAX_ASDU_LEN 253
#define U_FORMAT 0x03
#define S_FORMAT 0x01
#define I_FORMAT 0x00

/****************************************************************************/
/*          General Definitions                                             */
/****************************************************************************/

typedef char FLAG;

/* application identification definitions */

/* useful C macros                                                     */
#ifndef MAX
    #define MAX(x,y) ( (x) > (y) ) ? (x) : (y);
#endif

#ifndef MIN
    #define MIN(x,y) ( (x) < (y) ) ? (x) : (y);
#endif

#ifndef ABS
    #define ABS(x)  (( (x) > 0 ) ? (x) : -(x));
#endif

#ifndef ABS_LONG
    #define ABS_LONG(x)  (( (x) > 0L ) ? (x) : -(x));
#endif

#ifndef COMPARE     /* return 1 if masked data exists */
    #define COMPARE(data, mask)  ( ((data) & (mask)) ? (1) : (0) );
#endif

#ifndef BIT_TEST     /* return 1 if masked data exists */
    #define BIT_TEST(data, bit) ( ( (data) & (1L << (bit)) ) ? (1) : (0) );
#endif


/*
#ifndef SHIFT_OCTET
#define SHIFT_OCTET(data, num_octet) ( (uchar) ( (data >> (num_octet * 8) ) && 0xff) )
#endif
*/

#define SIGN_EXTEND_12(x) ((x) & 0x800 ) ? ( (x) | 0xf000 ) : (x)

#define ODD(x) ( (x) % 2 )
#define EVEN(x) !(ODD(x))

/**************************************************************************/
/*                Debug Related Defines                                   */
/**************************************************************************/
#define LOG_BUF_SIZE        180
#define LOG_NO_DEBUG          0
#define LOG_ERR_ONLY          1
#define LOG_DEBUG             2
#define LOG_ALL_DEBUG         3


/****************************************************************************/
/*          WIN Related Definitions                                         */
/****************************************************************************/

/* FLAGS/Definitions for config files */
/*#define SOE_ENABLE 1
#define INV_STATUS 2*/

/*#define FRZ_ENABLE      1
#define CLR_ENABLE      2*/

#define NOTUSED      -1     /* indicates spare or future data points      */
#define NUM_WESDAC    6     /* the number of RTU data WESDAC tables       */
#define MAX_WAIT_TIME (300UL * ONE_SECOND)
/* Max time to wait for all DCAs to init (sec)*/
#define LRU_WAIT_TIME 1000  /*                                            */
#define MAX_POINT     32767   /* Largest allowed point number             */
#define QUICK_RETRY_INTERVAL   250L /* quick resend timeout interval */


/**************************************************************************/
/*               WESDAC Interface Node related defines                    */
/**************************************************************************/

#define WIN_EVENT_TYPE_MASK  (WIN_EVENT_MASK & (~WIN_MULTI_PNT_EVENT))


/**************************************************************************/
/*               Ownership Table macro's                                  */
/**************************************************************************/

typedef unsigned char uchar;
typedef unsigned int  uint;
typedef unsigned long ulong;

/* Standard PSOS message overlay to use with SEND_X/REQ_X etc...            */
typedef struct message
{
    long dont_care;          /* psos required filler spaces               */
    long home_ex;            /* psos required must = 0 or xid             */
    long src_xid;            /* type of proc. that sent the message       */
    int type;                /* type of data sent                         */
    int size_buf;            /* number of data bytes                      */
    void * data_ptr;         /* pointer to data to send                   */
    long spare;
} pSOS_MESSAGE;

/* Interprocess communication signal */

/* pSOS naming - support for advanced pSOS process/exchange names */
enum pSOS_NAME_TYPE
{
    pSOS_INTERNAL, pSOS_DISPLAY
};



/**************************************************************************/
/*                  Find Table Type                                       */
/**************************************************************************/
enum TABLE_CLASS
{
    CRITICAL_TABLE = 1,     /* Table missing causes DPA to suspend        */
    NON_CRITICAL_TABLE = 2, /* Table missing does not suspend DPA         */
    MAIN_TABLE = 3          /* Table misssing causes DPA to delete itself
                             * ie. assumed that no DPA config is present  */
};

/* Power UP mode related */
enum RESET_MODE
{
    POWER_UP,
    SOFT_RESET
};


/**************************************************************************/
/*               Control Routine Return Codes                             */
/**************************************************************************/
enum CONTROL_RETURN
{
    CONTROL_SUCCESS,
    CONTROL_NOT_CONFIG,
    CONTROL_NOT_ACKED,
    CONTROL_OFFLINE,
    CONTROL_WRONG_TYPE,
    CONTROL_LOCAL_MODE,
    CONTROL_WIN_FAIL
};

/**************************************************************************/
/*               Control Routine Parameter Flags                          */
/**************************************************************************/

enum PULSE_TYPE
{
    NO_DEFINITION = 0,
    SHORTPULSE,
    LONGPULSE ,
    PERSISTENT
};

enum POINT_STATE { POINT_ON=1,POINT_OFF=0 };

/**************************************************************************/
/*            Local Time Related Defines                                  */
/**************************************************************************/
enum TIME_MODE
{
    TimeLord         = 0,   /* We control the RTU (pSOS) time              */
    TimeSlave        = 1,   /* Don't use or set RTU (pSOS) time            */
    TimeSlaveSOE     = 2    /* Don't set RTU (pSOS) time but report soes
                             * using RTU pSOS) time -ie. ignore time syns  */
};

typedef struct
{
    unsigned long   ulSec;         /* seconds since 1970-1-1 */
    int             iTicks;        /* milliseconds           */
    unsigned long   ulRealTime;    /* reference time         */
    enum TIME_MODE  eMode;         /* time mode of this DPA */
    unsigned char   iec_su;        /* 3.00, 0x80 if it is summer time*/
} LOCAL_TIME;



/****************************************************************************/
/*         Interprocess Message Types                                       */
/****************************************************************************/

enum INTERNAL_MESSAGE
{
    NO_MESSAGE,         /* don't send a message */
    REQ_INIT,           /* request initialisation */
    ACK,                /* internal message ack   */
    RTU_DATA,           /* message contains data for LRU */
    GLOBAL_RTU_DATA,    /* message contains broadcast LRU data */
    SEND_DATA,          /* message contains LRU data to send */
    COM_FAILED,         /* RTU communications have FAILED */
    COM_RESTORED,       /* RTU communications have been RESTORED */
    UNKNOWN_MESSAGE,    /* RTU could not process the message */
    INTERNAL,
    RESEND_MESSAGE,     /* Rx msg with unaltered FCB, thus re-Tx prev msg */
    VALID_MESSAGE,
    DPI_CHANGE,
    DPI_QUALITY,
    ILLEGAL_MESSAGE,
    LINK_STATUS_MSG,    /* link status message*/
    TX_STATUS_MSG,      /* transmission status message*/
    RX_MSG,             /* master request message*/
    IDLE_CHECK,         /* time to check if any spontaneous data to send*/
    RESEND_TX_MSG       /* 710: resend buffered tx messages due to previous send failure */

};


/* PROTOCOL SPECIFIC STUFF ONWARDS */

/****************************************************************************/
/*                      870-5-101 Related Definitions                       */
/****************************************************************************/

/* ASDU, link, & object addresses */
#define MIN_LINK_ADDR       0
#define MAX_LINK_ADDR_OCT1  254
#define MAX_LINK_ADDR_OCT2  65534
#define GLOBAL_LINK_ADDR_OCT1 255
#define GLOBAL_LINK_ADDR_OCT2 65535

#define MIN_OBJ_ADDR        1L
#define MAX_OBJ_ADDR_OCT1   255L
#define MAX_OBJ_ADDR_OCT2   65535L
#define MAX_OBJ_ADDR_OCT3   0xffffffL   /* 16777215 */

#define MIN_ASDU_ADDR       1
#define MAX_ASDU_ADDR_OCT1  254
#define MAX_ASDU_ADDR_OCT2  65534
#define GLOBAL_ASDU_ADDR_OCT1 255
#define GLOBAL_ASDU_ADDR_OCT2 65535

#define BROADCAST_LRU -1

/* general quality descriptor */
#define QUALITY_OV  0x1             /* 1 = overflow */
#define QUALITY_BL  0x10            /* 1 = blocked */
#define QUALITY_SB  0x20            /* 1 = subsituted */
#define QUALITY_NT  0x40            /* 1 = non topical */
#define QUALITY_IV  0x80            /* 1 = invalid */

/* integrated total quality flags */
#define QUALITY_SQ  0x1F        /* sequence number - inc after every freeze */
#define QUALITY_CY  0x20        /* 1 = counter overflow */
#define QUALITY_CA  0x40        /* 1 = counter adjusted */

/* single point state */
#define SPI_MASK            0x1
#define SPI_OFF             0
#define SPI_ON              1

/* double point state */
#define DPI_MASK            0x3
#define DPI_INTERMEDIATE    0
#define DPI_OFF             1
#define DPI_ON              2
#define DPI_INDETERMINATE   3

/* value with transient state indication - M_ST */
#define VTI_TRANSIENT_MASK  0x80
#define VTI_VALUE_MASK      0x7f

/* controls (DO/AO) */
#define SELECT_CONTROL_MASK 0x80

#define SINGLE_COMMAND_STATE        0x01
#define DOUBLE_COMMAND_STATE        0x03
#define REGULATING_COMMAND_STATE    0x03

#define SCS_OFF             0
#define SCS_ON              1
#define DCS_OFF             1
#define DCS_ON              2
#define RCS_OFF             1
#define RCS_ON              2

#define QUALIFIER_CMD_MASK  0x1f /* represents the qualifier of cmd mask
                                  * 0111 1100 shifted down by 2 --> 0001 1111 */

/* fixed test pattern for the C_TS_NA_1 msg */
#define TEST_PATTERN        0x55AA  /* 01010101 10101010 */

/* qualifier of reset process for C_RP_NA_1 & supported reset buffer
 * options for a101_reset_buffer() routine
 */
typedef enum
{
    RESET_ALL = 1,
    RESET_TIME_TAGGED = 2
} RESET_OPERATION;

/* cause of initialisation */
#define CAUSE_OF_INIT_MASK  0x7F
#define LOCAL_POWER_ON      0
#define LOCAL_MANUAL_RESET  1   /* not supported */
#define REMOTE_RESET        2
#define LOCAL_PARAM_CHANGE  0x80

/* qualifier of counter interrogation */
#define REQUEST_GROUP_MASK  0x3F
#define FREEZE_MASK         0x03    /* 1100 0000 shifted down by 6 */

#define GENERAL_REQUEST_COUNTER 5

#define NO_FREEZE_NO_RESET      0
#define FREEZE_NO_RESET         1
#define FREEZE_WITH_RESET       2
#define NO_FREEZE_WITH_RESET    3

typedef enum
{
    GENERAL_COUNTER = 0,
    GROUP_1_COUNTER,
    GROUP_2_COUNTER,
    GROUP_3_COUNTER,
    GROUP_4_COUNTER,
    NUM_COUNTER_GROUP
} GROUP_COUNTER_TYPE;

/* qualifier of parameter of measured values */
#define NOT_OPERATE_PARAM   0x80
#define CHANGE_PARAM        0x40
#define KIND_OF_PARAM_MASK  0x3F
#define THRESHOLD_VALUE     1
#define SMOOTHING_FACTOR    2   /* not supported */
#define LOW_LIMIT           3
#define HIGH_LIMIT          4

/* qualifier of parameter activation */
#define PERIODIC_CYCLIC_ACT 3


/* reply types */
typedef enum
{
    CONFIRM,
    NOREPLY
} REPLY_TYPES;

/* available class types */
typedef enum
{
    CLASS1 = 0,
    CLASS2,
    NUM_CLASS
} CLASS_TYPES;

/* non-time tagged & time tagged event types */
typedef enum
{
    SP_NA_1=0,      /* single point */
    SP_TA_1,        /* single point with time tag */
    DP_NA_1,        /* double point */
    DP_TA_1,        /* double point with time tag */
    BO_NA_1,        /* bitstring */
    BO_TA_1,        /* bitstring with time tag */
    IT_NA_1,        /* couter without time tag*/
    IT_TA_1,        /* counter with time tag */
    /*v600*/
    SP_TB_1,        /* single point with CP56Time time tag */
    DP_TB_1,        /* double point with CP56Time time tag */
    BO_TB_1,        /* bitstring with CP56Time time tag */
    IT_TB_1,        /* counter with CP56Time time tag */
    ST_NA_1,        /* buffered analogs*/
    ST_TA_1,
    ST_TB_1,
    ME_NA_1,
    ME_NB_1,
    ME_ND_1,
    ME_TA_1,
    ME_TB_1,
    ME_TD_1,
    ME_TE_1,
    NUM_EVENT
} EVENT_TYPE;


/* cause of transmission field */
enum CAUSE_OF_TX_FIELD
{
    NOT_REPORTED        = 0,    /* object not to be reported        */
    PER_CYC             = 1,    /* periodic/cyclic - timed event    */
    BACK,                       /* background - request from m/s    */
    SPONT,                      /* spontaneous  */
    INIT,                       /* initialized  */
    REQ,                        /* request      */
    ACT,                        /* activation   */
    ACTCON,                     /* activation confirmation      */
    DEACT,                      /* deactivation */
    DEACTCON,                   /* deactivation confirmation    */
    ACTTERM,                    /* activation termination   */
    RETREM,                     /* not supported, return info caused by remote cmd  */
    RETLOC,                     /* not supported, return info caused by local cmd   */
    FILE_TRANSFER,              /* not supported, file transfer */
    /* 14 to 19 reserved */
    INROGEN             = 20,   /* general interrogation    */
    INRO1,                      /* group 1 interrogation    */
    INRO2,                      /* etc  */
    INRO3,
    INRO4,
    INRO5,
    INRO6,
    INRO7,
    INRO8,
    INRO9,
    INRO10,
    INRO11,
    INRO12,
    INRO13,
    INRO14,
    INRO15,
    INRO16,
    REQCOGEN,                   /* general counter request  */
    REQCO1,                     /* group 1 counter request  */
    REQCO2,                     /* etc  */
    REQCO3,
    REQCO4,
    /* 42 to 43 reserved */
    UNKNOWN_TYPE_ID = 44,       /* Unknown type identification */
    UNKNOWN_COT,                /* Unknown cause of transmission */
    UNKNOWN_COA,                /* Unknown common object address */
    UNKNOWN_IOA                 /* Unknown information object address */
};


/* type ID field - function code */
enum TYPE_ID_FIELD
{
    /* process information */
    M_SP_NA_1 = 1,              /* single point */
    M_SP_TA_1 = 2,              /* single point - time tagged */
    M_DP_NA_1 = 3,              /* double point */
    M_DP_TA_1 = 4,              /* double point - time tagged */
    M_ST_NA_1 = 5,              /* step position */
    M_ST_TA_1 = 6,              /* NOT SUPPORTED: step position - time tagged */
    M_BO_NA_1 = 7,              /* bitstring of 32 bits */
    M_BO_TA_1 = 8,              /* bitstring of 32 bits - time tagged */
    M_ME_NA_1 = 9,              /* measured value - normalised */
    M_ME_TA_1 = 10,             /* NOT SUPPORTED: measured value - time tagged */
    M_ME_NB_1 = 11,             /* measured value - scaled */
    M_ME_TB_1 = 12,             /* NOT SUPPORTED: measured value - scaled time tagged */
    M_ME_NC_1 = 13,             /* NOT SUPPORTED: measured value - short float */
    M_ME_TC_1 = 14,             /* NOT SUPPORTED: measured value - short float time tagged */
    M_IT_NA_1 = 15,             /* integrated total */
    M_IT_TA_1 = 16,             /* integratd total - time tagged */
    M_EP_TA_1 = 17,             /* NOT SUPPORTED: event of protection equipment - time tagged */
    M_EP_TB_1 = 18,             /* NOT SUPPORTED: event of protection equipment - packed time tagged */
    M_EP_TC_1 = 19,             /* NOT SUPPORTED: event of protection equipment - packed O/P circuit time tagged */
    M_PS_NA_1 = 20,             /* packed single point - status change detection */
    M_ME_ND_1 = 21,             /* measured value - normalized without quality descriptor */
    /* v600*/
    /* 22 to 29 reserved */
    M_SP_TB_1 = 30,             /* single point with CP56Time time tag */
    M_DP_TB_1 = 31,             /* double point with CP56Time time tag */
    M_ST_TB_1 = 32,             /* step position point with CP56Time time tag */
    M_BO_TB_1 = 33,             /* bitstring  with CP56Time time tag */
    M_ME_TD_1 = 34,             /* NORMLIZED ME with CP56Time time tag */
    M_ME_TE_1 = 35,             /* SCALED ME with CP56Time time tag */
    M_ME_TF_1 = 36,             /* NOT SUPPORTED, FLOAT ME with CP56Time time tag */
    M_IT_TB_1 = 37,             /* COUNTER point with CP56Time time tag */
    M_EP_TD_1 = 38,             /* NOT SUPPORTED*/
    M_EP_TE_1 = 39,             /* NOT SUPPORTED*/
    M_EP_TF_1 = 40,             /* NOT SUPPORTED*/

    C_SC_NA_1 = 45,             /* single command */
    C_DC_NA_1 = 46,             /* double command */
    C_RC_NA_1 = 47,             /* regulating step command */
    C_SE_NA_1 = 48,             /* set point command - normalized */
    C_SE_NB_1 = 49,             /* set point command - scaled */
    C_SE_NC_1 = 50,             /* NOT SUPPORTED: set point command - short float */
    C_BO_NA_1 = 51,             /* NOT SUPPORTED: bitstring of 32 bits */
    /* 52 to 57 reserved */
    C_SC_TA_1 = 58,             /* single command with CP56Time time tag*/
    C_DC_TA_1 = 59,             /* DOUBLE command with CP56Time time tag*/
    C_RC_TA_1 = 60,             /* REGULATING command with CP56Time time tag*/
    C_SE_TA_1 = 61,             /* SET POINT, NORMLIZED command with CP56Time time tag*/
    C_SE_TB_1 = 62,             /* SET POINT, SCALED command with CP56Time time tag*/
    C_SE_TC_1 = 63,             /* NOT SUPPORTED, SET POINT, FLOAT command with CP56Time time tag*/
    C_BO_TA_1 = 64,             /* NOT SUPPORTED*/
    /* 65 to 69 reserved */
    /* system information */
    M_EI_NA_1 = 70,             /* end of initialization */
    /* 71 to 99 reserved */
    C_IC_NA_1 = 100,            /* interrogation command */
    C_CI_NA_1 = 101,            /* counter interrogation command */
    C_RD_NA_1 = 102,            /* read command */
    C_CS_NA_1 = 103,            /* clock sync command */
    C_TS_NA_1 = 104,            /* test command */
    C_RP_NA_1 = 105,            /* reset process command */
    C_CD_NA_1 = 106,            /* delay acquisition command */
    C_TS_TA_1 = 107,            /* test command with CP56Time tag*/
    /* 108 to 109 reserved */
    /* parameters */
    P_ME_NA_1 = 110,            /* measurand - normalized */
    P_ME_NB_1 = 111,            /* measurand - scaled */
    P_ME_NC_1 = 112,            /* NOT SUPPORTED: measurand - short float */
    P_AC_NA_1 = 113,            /* parameter activation */
    /* 114 to 119 reserved */
    /* file transfer */
    F_FR_NA_1 = 120,            /* file ready */
    F_SR_NA_1 = 121,            /* section ready */
    F_SC_NA_1 = 122,            /* call directory, select file, call file, call section */
    F_LS_NA_1 = 123,            /* last section, last segment */
    F_AF_NA_1 = 124,            /* ack file, ack section */
    F_SG_NA_1 = 125,            /* segment */
    F_DR_TA_1 = 126             /* directory */
                /* 127 reserved */
};

enum SQ_TYPE
{
    SINGLE = 0,
    SEQUENCE        /* sequence reply only used for non-buffered event data */
};


/* useful once off calculated cfg related parameters */
typedef struct
{
    int global_link_addr;   /* global link address for this LRU */
    int ft12_fixed_len;     /* num of octets used for fixed FT12 frame
                             * (including FT12_OVERHEAD + control + address */
    int ft12_len;           /* num of octets used for variable FT12 frame
                             * (including FT12_OVERHEAD + control + address */
} CALC_COM_CFG;

typedef struct
{
    int global_link_addr;   /* global link address for this LRU */
    int global_asdu_addr;   /* global asdu address for this LRU */
    int dui_len;            /* num of octets used for data unit identifier */
    int max_asdu_len;       /* max. length of ASDU in reply msg */
    int max_back_asdu_len;  /* max. length of ASDU in class 2 reply */
} CALC_LRU_CFG;

/* status information on the last Rx select control request */
typedef struct
{
    ulong   time;           /* time of last Rx select ctl */
    ulong   obj_addr;       /* obj_addr of last Rx select ctl */
    uint    qualifier;      /* qualifier of ctl of last Rx select AO/DO ctl
                             * NOTE, DO qualifier of ctl also holds DO value
                             */
    int     value;          /* AO value of last Rx select ctl */
    enum TYPE_ID_FIELD  obj_id;    /*v500: selected object type*/
} SELECT_CONTROL_STATUS;

/* user data pending flags */

#define CONFIRM_USER_DATA                   0x01
#define BUFFERED_USER_DATA                  0x02
#define NONBUFFERED_USER_DATA               0x04
#define REQUESTED_USER_DATA                 0x08
#define EOI_USER_DATA                       0x10
#define ACTTERM_OUTPUT_USER_DATA            0x20
#define ACTTERM_INTER_USER_DATA             0x40
#define ACTTERM_COUNT_INTER_USER_DATA       0x80


/* IEC time format ('borrowed' from A098-0) */

/* offset into array of chars representing 7 octet IEC time info */
#define NUM_IEC_TIME_BYTES 7
#define MSEC_OFFSET_LSB 0
#define MSEC_OFFSET_MSB 1
#define MIN_OFFSET      2
#define HOUR_OFFSET     3
#define DAY_OFFSET      4
#define MONTH_OFFSET    5
#define YEAR_OFFSET     6

/* masks for the array of chars representing 7 octet IEC time info */

/* note, due to the 'octet shifting bug (?)' (refer top of this file), the
 * IEC masks have been cast to long (ie. using solution 2).
 */

#define IEC_IV          0x80L   /* invalid value mask */
#define IEC_MINUTE      0x3FL   /* minute mask */
#define IEC_SU          0x80L   /* summer time mask */
#define IEC_HR          0x1FL   /* hour mask */
#define IEC_DAYOFWEEK   0xE0L   /* day of week mask */
#define IEC_DAYOFMONTH  0x1FL   /* day of month mask */
#define IEC_MONTH       0x0FL   /* month mask */
#define IEC_YEAR        0x7FL   /* year mask */

/* various macros */
#define IEC_MSEC_LSB(x)   ((x) & 0x00FF)
#define IEC_MSEC_MSB(x)  (((x) & 0xFF00) >> 8)
#define IEC_SEC(x)        ((x) & 0x000000FF)
#define IEC_MIN(x)       (((x) & 0x0000FF00) >>  8)
#define IEC_HOUR(x)      (((x) & 0x00FF0000) >> 16)

#define PSOS_SEC(x)       ((x) & 0x000000FF)
#define PSOS_MIN(x)      (((x) & 0x0000FF00) >>  8)
#define PSOS_HOUR(x)     (((x) & 0x00FF0000) >> 16)
#define PSOS_DAY(x)       ((x) & 0x000000FF)
#define PSOS_MONTH(x)    (((x) & 0x0000FF00) >>  8)
#define PSOS_YEAR(x)     (((x) & 0xFFFF0000) >> 16)
#define PSOS_TIME_MASK   0x00FFFFFF;

#define DECEMBER        12
#define HOURS_IN_DAY      24L
#define MS_IN_HOUR   3600000L   /* number of milliseconds in an hour      */
#define MS_IN_MIN      60000L   /* number of milliseconds in a minute     */
#define MS_IN_SEC       1000L   /* number of milliseconds in a second     */

enum OCTET_TIME
{
    TWO_OCTET_TIME,
    THREE_OCTET_TIME,
    SEVEN_OCTET_TIME
};

/****************************************************************************/
/*                     Compatibility related bit definitions                */
/****************************************************************************/
#define A101_ENABLE_ZERO_THRESHOLD    0x0001  /* V810 */
#define A101_IEC103_EDITION_2         0x0002  /* V820 */
#define A101_PACK_SINGLE_POINTS       0x0004  /* 820  */

/****************************************************************************/
/*                      870-5-1 Related Definitions                         */
/****************************************************************************/

/* fixed octet values */
#define FT12_START          0x68
#define FT12_FIXED_START    0x10
#define FT12_END            0x16
#define FT12_SINGLE_ACK_1   0xE5    /* assumed to be pos ACK (not defined in std) */
#define FT12_SINGLE_ACK_2   0xA2    /* not used 870-5-101 pg 21 */

/* indexes into the frame header */
#define FT12_START_INDEX_1      0
#define FT12_LEN_INDEX_1        1
#define FT12_LEN_INDEX_2        2
#define FT12_START_INDEX_2      3
#define FT12_CONTROL_INDEX      4
#define FT12_ADDRESS_INDEX      5

#define FT12_FIXED_CONTROL_INDEX 1
#define FT12_FIXED_ADDRESS_INDEX 2  /* 1 or 2 octets, depending on link_addr_len */
#define FT12_FIXED_CHKSUM_INDEX  3  /* depending on length of addr */
/*#define FT12_FIXED_END_INDEX     4*/  /* depending on length of addr */

/* size of overhead characters (start/stop, length, checksum chars) */
#define FT12_HEADER_OVERHEAD       4
#define FT12_TAIL_OVERHEAD         2
#define FT12_OVERHEAD              (FT12_HEADER_OVERHEAD + FT12_TAIL_OVERHEAD)
#define FT12_FIXED_HEADER_OVERHEAD 1
#define FT12_FIXED_TAIL_OVERHEAD   2
#define FT12_FIXED_OVERHEAD        (FT12_FIXED_HEADER_OVERHEAD + FT12_FIXED_TAIL_OVERHEAD)

/* size of communications buffer */
#define MAX_FRAME_LEN       255         /* length is only 1 byte */
#define BUF_SIZE            (MAX_FRAME_LEN + FT12_OVERHEAD + 1)

/*
 * v860: The ASDU length is no longer a fixed constant. Therefore, the
 *       MAX_104_ASDU_LEN parameter must be removed.
 */
/*v600*/
/*#define MAX_104_ASDU_LEN    249 */        /* max length for 104 ASDU*/
/* end v860 */

/*v820*/
#define MAX_CNFRM_LEN       16



/****************************************************************************/
/*                      870-5-2 Related Definitions                         */
/****************************************************************************/

/* offsets into the ASDU, indexed from the beginning of the asdu_data */
#define TYPE_ID_OFFSET      0
#define VSQ_OFFSET          1
#define COT_OFFSET          2
/*#define ASDU_OFFSET         3 */  /* only for when COT is 1 octet */


/* control field masks */
#define CONTROL_RES         0x80
#define CONTROL_PRM         0x40
#define CONTROL_FCB         0x20
#define CONTROL_ACD         0x20
#define CONTROL_FCV         0x10
#define CONTROL_DFC         0x10
#define CONTROL_FUNCTION    0x0f

/* variable structure qualiifer masks */
#define VSQ_SQ_MASK             0x80
#define VSQ_NUM_MASK            0x7f
#define VSQ_MAX_NUM             0x7f
#define COT_TEST_MASK           0x80
#define COT_NEG_CONFIRM_MASK    0x40
#define COT_CAUSE_MASK          0x2f

enum FT12_RX_MSG_STATUS
{
    FT12_MSG_OK = 0,
    FT12_INVALID_FRAME,
    FT12_NO_START_1,          /* waiting for msg */
    FT12_NO_START_2,
    FT12_WRONG_LEN,
    FT12_NO_END,

    FT12_INVALID_CONTROL_RES, /* if reserve bit set */
    FT12_INVALID_CONTROL_PRM, /* if not set by m/s */
    FT12_INVALID_CONTROL_FCV, /* if set/cleared for a function that should not
                               *    have this bit set/cleared */
    FT12_INVALID_CONTROL_FCB, /* if (i)  alternated when fcv = 0
                               *    (ii) not alternated when fcv = 1 (if the Rx
                               *         function is not same as last Rx function */
    FT12_INVALID_CONTROL_FUNCTION, /* 10, control function is incorrect */
    FT12_INVALID_CHECKSUM,    /* checksum is incorrect */
    FT12_UNKNOWN_LINK_ADDR,   /* link addr doesn't belong to any LRUs on this COM */
    FT12_RESEND_MESSAGE       /* last Rx msg is to be resent */
};

enum ASDU_RX_MSG_STATUS
{
    ASDU_MSG_OK = 0,
    ASDU_UNSUPPORTED_TYPE_ID, /* type id is not supported (obviously) */
    ASDU_INVALID_TYPE_ID,     /* type id not valid for a m/s message */
    ASDU_INVALID_VSQ,         /* invalid VSQ */
    ASDU_UNKNOWN_ASDU_ADDR,   /* ASDU addr doesn't belong to this LRU */
    ASDU_INVALID_OBJ_ADDR,    /* obj addr is invalid (ie. != 0 for system msgs) */
    ASDU_INVALID_CAUSE_TYPE,  /* cause type_id is unknown or not valid for obj_type */
    ASDU_UNKNOWN_OBJ_ADDR,    /* no matching info object addr in LRU */
    ASDU_INVALID_ASDU_SIZE,   /* ASDU size is incorrect */
    ASDU_WRONG_OBJ_TYPE,      /* object at given addr does not match msg type_id */
    ASDU_FAIL_SE,        /* 10 */     /* select execute testing failed */
    ASDU_WRONG_PULSE_TYPE,    /* pulse type specified does not match MT02 cfg */
    ASDU_INVALID_CMD_STATE,   /* not used, invalid cmd state in qual of cmd */
    ASDU_CONTROL_LOCAL_MODE,  /* control rejected, LRU is in local mode */
    ASDU_CONTROL_NOT_CONFIG,  /* sys pnt cfg as not used */
    ASDU_CONTROL_NOT_ACKED,   /* previous controls still not acked */
    ASDU_CONTROL_OFFLINE,     /* control point is offline */
    ASDU_CONTROL_WRONG_TYPE,  /* control cfg type is incorrect */
    ASDU_CONTROL_WIN_ERROR,   /* WIN write error */
    ASDU_CONTROL_DIRECT_PNT,  /* select cmd issued to a direct execute pnt */
    ASDU_INVALID_TIME,        /* time field invalid (eg. IV set) */
    ASDU_INVALID_TEST_PATTERN,/* 20 */ /* test pattern Rx was not correct */
    ASDU_INVALID_QUALIFIER,   /* qualifer of command had an invalid value
                               * (ie. not used) or a reserved value */
    ASDU_END_OF_INIT_NOT_TX,  /* since RTU boot-up, M_EI hasn't been Tx */
    ASDU_COUNTER_FRZ_FAILED,  /* WIN error, unable to freeze requested counters */
    ASDU_COUNTER_CLR_FAILED,  /* WIN error, unable to clear requested counters */
    ASDU_UNSUPPORTED_QUALIFIER,/* requested qualifier of command is unsupported */
    ASDU_NON_CYCLIC_OBJ,      /* param act cmd on an obj not cfg to be reported
                               * as periodic/cyclic */
    ASDU_POINTS_NOT_INIT,     /* initialisation of pnts has not finished */
    ASDU_UNEXPECTED,           /* unexpected ASDU during File Transfer*/
    ASDU_INVALID_TT           /* invalid time tag for TT control*/
};

enum PRIMARY_LINK_FUNCTION_CODE
{
    RESET_REMOTE_LINK       = 0,
    RESET_USER_PROCESS      = 1,
    USER_DATA_CONFIRM       = 3,
    USER_DATA_NO_REPLY      = 4,
    REQUEST_ACCESS_DEMAND   = 8,
    REQUEST_STATUS_OF_LINK  = 9,
    REQUEST_USER_CLASS_1    = 10,
    REQUEST_USER_CLASS_2    = 11
};

enum SECONDARY_LINK_FUNCTION_CODE
{
    ACK_CONFIRM             = 0,
    NACK_CONFIRM            = 1,    /* as DFC != 1, this msg is never used */
    USER_DATA               = 8,
    NACK_RESPOND            = 9,
    LINK_STATUS_ACCESS_DEMAND = 11,
    LINK_NOT_FUNCTIONING    = 14,   /* msg is never used, can't respond if their
                                     * is no working link */
    LINK_NOT_IMPLEMENTED    = 15    /* msg is never used, can't respond if their
                                     * is no working link */
};

/* frame type of reply message */
enum FRAME_TYPE
{
    SINGLE_ACK = 0,
    SINGLE_NACK,    /* not used 870-5-101 pg 21 */
    FIXED_FRAME,
    VARIABLE_FRAME
};

typedef struct
{
    unsigned res:1;             /* reserved bit - should always be zero */
    unsigned prm:1;             /* who sent the message */
    unsigned fcb:1;             /* access demand (I have class one data) */
    unsigned fcv:1;             /* data flow control (I am full) */
    enum PRIMARY_LINK_FUNCTION_CODE function; /* data link function code */
} PRIMARY_CONTROL_FIELD ;

typedef struct
{
    unsigned res:1;             /* reserved bit - should always be zero */
    unsigned prm:1;             /* who sent the message */
    unsigned acd:1;             /* access demand (I have class one data) */
    unsigned dfc:1;             /* data flow control (I am full) */
    enum SECONDARY_LINK_FUNCTION_CODE function; /* data link function code */
} SECONDARY_CONTROL_FIELD ;

typedef struct
{
    /* last Rx msg state */
    int fcb;                    /* fcb of last Rx message */
    int function;               /* function type of last Rx message */

    /* last Tx msg state */
    uchar tx_buffer[BUF_SIZE];   /* last Tx message */
    int tx_buf_len;             /* size of msg stored in tx_buffer */
} LAST_MSG_STATUS;  /* for specific LRUs, stores status of last Tx & Rx msg */


/****************************************************************************/
/*                      870-5-3 Related Definitions                         */
/****************************************************************************/

typedef struct
{
    enum SQ_TYPE sq;    /* defines whether num = num of info elements or objects */
    int num;    /* define num of info elements/objects */
} VARI_STRUCT_QUAL;

typedef struct
{
    int test;           /* TRUE if msg is only a test message */
    int neg_confirm;    /* TRUE if msg is a negative confirmation */
    enum CAUSE_OF_TX_FIELD cause;
    uchar orig_addr;
} CAUSE_OF_TX;

typedef struct
{
    enum TYPE_ID_FIELD  type_id;
    VARI_STRUCT_QUAL    vsq;
    CAUSE_OF_TX         cot;
    uint                asdu_addr;
} DATA_UNIT_IDENTIFIER;

/* info object from USER_DATA function sent by m/s */
typedef struct
{
    ulong               obj_addr;   /* obj_addr of first object */
    uint                qualifier;  /* qualifier of first object */
    ulong               value;      /* value octet(s) of first object */
    uchar               *sync_time; /* pointer to the C_CS time info within
                                     * the asdu_data msg */
} RX_INFO_OBJECT;


typedef struct
{
    DATA_UNIT_IDENTIFIER dui;

    uchar asdu_data[MAX_FRAME_LEN]; /* holds the ASDU data */
    /* note, the max array size really req. is actually =
     * (MAX_FRAME_LEN - control_len - link_addr_len)
     */
    int asdu_len;
/*    int info_obj_len;*/  /* num of octets in info object 1, 2, ... */
} SECONDARY_APPL_SERVICE_DATA_UNIT;

typedef struct
{
    DATA_UNIT_IDENTIFIER dui;
    RX_INFO_OBJECT info_obj; /* holds info object for USER_DATA msg from m/s */

    uchar asdu_data[MAX_FRAME_LEN]; /* holds the ASDU data */
    /* note, the max array size really req. is actually =
     * (MAX_FRAME_LEN - control_len - link_addr_len)
     */
    int asdu_len;
/*    int info_obj_len;*/  /* num of octets in info object 1, 2, ... */
} PRIMARY_APPL_SERVICE_DATA_UNIT;

#define A101_GUARANTEED_TX      0x0001 /*v512*/
#define A101_HOLD_TIME          300L   /*V512*/
#define A101_FAILHOLD_TX        0x0002 /*v600: event messages*/
#define A101_RESPONSE_TX        0x0004 /*v600: DPA message*/
#define A101_QUERY_TX           0x0008 /*v600: no use so far*/

/* protocol message from secondary to primary */
typedef struct
{
    int len;            /* size of user data (control, addr, & ASDU data) */
    SECONDARY_CONTROL_FIELD control; /* type of mesage */
    int link_addr;          /* octets 1 & 2 */
    SECONDARY_APPL_SERVICE_DATA_UNIT asdu;
    enum FRAME_TYPE  frame_type; /* type of frame to Tx */

    /*v512: fix time sync problem*/
    int  service;              /* service */
    unsigned long   tx_time;  /* rtc time to send the message*/

} SECONDARY_PROTOCOL_MSG;

/* protocol message from primary to secondary */
typedef struct
{
    int len;            /* size of user data (control, addr, & ASDU data) */
    PRIMARY_CONTROL_FIELD control;  /* type of mesage */
    int link_addr;      /* octets 1 & 2 */
    PRIMARY_APPL_SERVICE_DATA_UNIT asdu;    /* type, VSQ, cause 1/2, ASDU addr */
    int lru_num;        /* lru number that msg belongs to FOR THIS COM PORT */
    /*v511: fix time sync problem*/
    unsigned long   first_time;  /* rtc time when first bit is received*/
} PRIMARY_PROTOCOL_MSG;


/****************************************************************************/
/*                      870-5-4 Related Definitions                         */
/****************************************************************************/


/****************************************************************************/
/*                      870-5-5 Related Definitions                         */
/****************************************************************************/


/***************************************************************************/
/*         General and Configuration Related definitions                   */
/***************************************************************************/

/* control mapping table */
enum CONTROL_TYPE
{
    TRIP_CLOSE      = 0,
    PULSE           = 1,
    LATCHING        = 2,
    ILLEGAL_CONTROL
};

/* control & setpoint param masks */
#define DIRECT_EXECUTE  0x1
#define GROUP_OWNERSHIP 0x2

/* comms cfg flow control masks */
#define RTS_FLOW_MASK   0x1
#define CTS_FLOW_MASK   0x2
#define DCD_FLOW_MASK   0x4

/* counter mapping table */
#define COUNTER_TRANSITION      0x1
#define COUNTER_FREEZE_ENABLE   0x2
#define COUNTER_RESET_ENABLE    0x4
#define COUNTER_RETURN_FROZEN   0x8

/* Whether to include or exclude object data in the Interrogation Response */
#define EXCLUDE_OBJ 0
#define INCLUDE_OBJ 1
/* end v860 */

/* V900: added the following definitions: */

#define SYNC_MSG_DISABLED   0 /* Whether hourly clock sync message is disabled */

/* End V900 */

/**************************************************************************/
/*      DPA Mapping Table Configuration Definitions                       */
/**************************************************************************/

typedef struct
{
    int pnt_num;            /* system point number                       */
    int inv_stat;           /* invert status */
} BINARY_IN_MAP;

typedef struct
{
    int pnt_num_on;         /* system point number 1 for ON request     */
    int pnt_num_off;        /* system point number 1 for OFF request    */
    int type;               /* type of control output                    */
    int short_pulse;        /* duration of short pulse */
    int long_pulse;         /* duration of long  pulse */
    int ctl_param;          /* direct execute, ... */
} BINARY_OUT_MAP;

typedef struct
{
    int pnt_num;            /* system point number                       */
    int threshold;          /* threshold value before being reported as event */
    int count_flag;         /* counter type - pulse/trans, freeze/reset, ... */
} COUNTER_MAP;

typedef struct
{
    int multiplier;         /* Scaled Value = Raw * multiplier + offset  */
    int divisor;            /*                ---------------            */
    int offset;             /*                  divisor                  */
    int unipolar;           /* TRUE if AI / AO value is a unipolar value */
} ANALOG_SCALE;

typedef struct
{
    int  threshold;          /* threshold value before reported as event */
    int  lo;                 /* low alarm for measured value             */
    int  hi;                 /* high alarm for measured value             */
    int  smooth;             /* smoothing factor for measured value
                              * (not supported)
                              */
} ANALOG_PARAM;

typedef struct
{
    int pnt_num;             /* system point number                       */
    ANALOG_SCALE scale;      /* scaling function for this entry           */
    ANALOG_PARAM param;      /* parameters for event reporting */
} ANALOG_INPUT_MAP;

typedef struct
{
    int pnt_num;             /* system point number                       */
    ANALOG_SCALE scale;      /* scaling function for this entry           */
    int set_param;           /* direct execute, .. */
    int Send_Actterm;        /* Boolean indication of whether to send an C_SE actterm */
} ANALOG_OUTPUT_MAP;


/**************************************************************************/
/*      Data Processing Application Configuration definitions             */
/**************************************************************************/

typedef struct               /* Communication Configuration Table         */
{
    /* communications port parameters */
    char port[6];           /* serial comms port name                    */
    uint baud;              /* the baud rate ( not the control word )    */
    int rts_on;             /* RTS pretransmission delay                 */
    int rts_off;            /* RTS post-transmission delay               */
    int rx_squelch;         /* time DCD must be high before data is Rx */
    int flow_ctl;           /* flow control - enable/disable RTS, CTS, DCD */
    int msg_timeout;        /* inter frame timeout - max time to Rx entire frame */

    /* communications fail parameters */
    int com_fail;           /* maximum idle time on comms line - time between
                             * Rx messages (don't have to be valid msg) */
    int com_fail_pnt;       /* point to operate when comms have failed (msg timeout) */
    int type;               /* type of control output */
    int pulse_time;         /* pulse duration to operate control output */

    /* 870-5-101 specifics */
    int unbal_tx_mode;      /* unbalanced or balanced Tx - for future use only */
    int max_frame_len;      /* max number of chars in reply frame */
    int max_back_resp_len;  /* max number of octets in background response frames to class 2 requests */
    int link_addr_len;      /* number of octets in link address          */
    int link_addr_type;     /* NOT REALLY USED - structured or unstructured link addr used */

    /* non IEC standard compatibility parametes */
    int single_ack_enable;  /* set if LRUs can reply with single character ACKs */
    char parity[2];         /* selectable parity - 101 std specifies even parity */

    /* LRU cross reference parameters */
    int  xref_offset;       /* offset into the lru xref config table     */
    int  num_lru;           /* number of lrus  configured for this port  */
} COM_CFG ;

typedef struct lru_xref
{
    uint link_addr;         /* LRU link address octets 1 & 2 */
    int lru_offset;         /* offset into the lru config table          */
} LXR_CFG;

#define A101_BALANCED_MODE      1 /* flag for balanced mode*/
#define A101_UNBALANCED_MODE    0 /* flag for balanced mode*/

#define A101_CONFIRMED_SERVICE      1
#define A101_UNCONFIRMED_SERVICE    0

#define CTRL_OWNERSHIP_MANUAL         0
#define CTRL_OWNERSHIP_AUTOMATIC      1

typedef struct
{
    uint asdu_addr;         /* common address of ASDU octets 1 & 2, if balanced mode, it is appl address */
    int time_mode;          /* determines whether m/s time syncs set main RTU or local LRU time */
    int buf_size;           /* total number of events that can be buffered */
    int local_remote_pnt;   /* local remote point to monitor             */
    int local_mode_state;   /* state of local remote point to enable local operation mode */
    int time_sync_valid;    /* max time between time syncs before SOEs are marked as invalid */
    int select_timeout;     /* select/execute timeout in milliseconds    */
    int dpi_valid_time;     /* min time that both points must be stable before */
                            /* reported as valid ON/OFF state */
    int dpi_supress_time;   /* min time that both points must be stable before */
                            /* reported as indeterminate state */
    int cause_tx_len;       /* number of octets in cause of transmission field */
    int asdu_addr_len;      /* number of octets in common ASDU address octets */
    int obj_addr_len;       /* number of octets in info object address */
    int new_config;         /* TRUE if this config has just been downloaded */
    int obj_offset;         /* offset into the object table (first object) */
    int num_obj;            /* number of objects in object table */
    int ev_buf_of_pnt;      /* point to operate when event buffer overflow
                             * occurs
                             */
    int type;               /* type of control output */
    int pulse_time;         /* pulse duration to operate control output */
    int ft_report_class;    /* file transfer report class (1 or 2)*/
    int commit_delay;       /* time (in second) of delay committing new config*/

    /*v400:*/
    int buflow_threshold;    /* If this % of event buffers are used, generate buffer low event*/
    int buflow_of_pnt;      /* point to operate when free event buffers are low*/
    int buflow_type;        /* type of control output */
    int ctrl_pulse_time;    /* pulse duration to operate control output */

    /*v500: */
    int cancel_do;          /* if TRUE, cancel pending DO commands when DI changes occur.*/
    /*v600*/
    int ctrl_tm_window;     /* time range in ms to valid the time tagged control request*/
    int lru_mode;           /* balanced = 1 or unbalanced = 0*/

    uint peer_addr;         /* BridgeMAN remote master application address*/

    /* v860 */
    int protocol;           /* Protocol to use in determining the length of APDU & ASDU */
    int max_APDU_ASDU_len;  /* Configurable maximum APDU/ASDU Length */
    /* end v860 */

    int  dl_confirm;        /* If TRUE, confirmed DL transmission service is used*/
    int num_of_buffers;     /* 710: changed to number of tx buffers in the pool*/
    uint holding_time;      /* spontaneous reporting max delay in ms*/
    uint retry_time;        /* the periodic to re-send failed message*/
    int com_fail_pnt;       /* point to operate when comms have failed (msg timeout) */
    int com_fail_type;       /* type of control output */
    int com_fail_pulse_time; /* pulse duration to operate control output */
    int buffer_ai;           /* TRUE then buffer AI changes including periodic ones*/
    /*V700*/
    int rpt_time_spont_only; /* TRUE if time tag is to be reported only on spontaneous data */
/*    uint ACTCON_Class;       / * V800, Class for ACTCON responses. Removed in 820 */
    uint Compatibility;      /* V810: Compatibility bit field */
    uint CtrlOwnership;      /* control ownership mode; manual/automatic */
    uint AllowClockSync;	 /* V900:  Enable/ disable spontaneous hourly time report C_CS_NA_1 */

} LRU_CFG ;

typedef struct
{
    int type;               /* what sort of data for this object         */
    ulong obj_addr;         /* object address octets 1, 2, & 3 - including filler byte */
    int group;              /* interrogation group for this object       */
    int report_class;       /* class 1 or class 2 reporting */
    int periodic_mode;      /* use cyclic/background mode for this object */
    int periodic_interval;  /* time between periodic updates */
    int report_power_up;    /* specifies if obj grp should be reported on power-up */
    int dpi_valid_time;     /* min time that both points must be stable before */
                            /* reported as valid ON/OFF state */
    int dpi_supress_time;   /* min time that both points must be stable before */
                            /* reported as indeterminate state */
    int interrogation_resp; /* v860: whether to include/exclude the object data
                             * in a response to a station interrogation request
                             */
    int offset;             /* index into MT0x first mapped info element */
    int num_pnts;           /* number of information elements */
} OBJECT_CFG;


/****************************************************************************/
/*           Mapping Table Related unions and stuctures                     */
/****************************************************************************/

typedef union
{
    BINARY_IN_MAP      *bin;
    BINARY_OUT_MAP     *bout;
    COUNTER_MAP        *count;
    ANALOG_INPUT_MAP   *ain;
    ANALOG_OUTPUT_MAP  *aout;
    void *ptr;
} MAPPING_TABLE ;            /* ptr to the configuration map table        */


/**************************************************************************/
/*              Actual RTU database                                       */
/**************************************************************************/

/* used to store ALL object data in a common form */
typedef struct
{
    OBJECT_CFG  *obj_cfg;   /* object cfg record for this obj_addr
                             * req. for setpoint/control requests */
    int         map_pnt;    /* index into MT0x - req. for direct m/s requests */
    ulong       obj_addr;   /* info object address */
    ulong       current;    /* current value in WESDAC table */
    ulong       last;       /* last value reported to the m/s */
    ulong       time;       /* pSOS time info */
    ulong       unix_time;  /* v811: unix time info, num sec since 1970-1-1 */
    uint        unix_ticks; /* v811: milli-sec portion of unix time info */
    uint        quality;    /* quality descriptors for monitored types
                             * qualifier of command for control types */
    enum CAUSE_OF_TX_FIELD
    cause;      /* cause of Tx field */

    ulong       last_periodic_time; /* time of the last periodic reporting
                                     * for this object */
    uint        report_periodic;   /* TRUE if obj is cfg to report periodically
                                    * (either cyclic/periodic or background) */
    uint        reported_GI; /* v820: GI have been reproted */
} EVENT_DATA;


/**************************************************************************/
/*                 event buffer definitions                               */
/**************************************************************************/

/* supported buffer operations */
typedef enum
{
    ADD_BUFFER,
    GET_BUFFER,
    RESET_BUFFER
} BUFFER_OPERATION;

/* event buffer information */
typedef struct event_buf
{
    EVENT_DATA data;            /* event data to store in the buffer */
    struct event_buf *prev;     /* pointer to the previous event buffer */
    struct event_buf *next;     /* point to the next event buffer */
} EVENT_BUF;

/* event buffer head used for both the event & free buffers (mem pool) */
typedef struct
{
    int num_used;           /* number of used event buffers */
    EVENT_BUF *head;        /* first event buffer to be removed */
} EVENT_BUF_HEADER;

/**************************************************************************/
/*                 double status point definition                         */
/**************************************************************************/

typedef struct
{
    EVENT_DATA *obj_ptr;    /* Object data associated with this point    */
    EVENT_DATA obj_data;    /* state of the object to report             */
    ulong report_time;      /* time this dbl event should report         */
    ulong last_time;        /* Last time the point changed state         */
    ulong last_value;       /* last valid state of pnt */
    FLAG  suppress;         /* Is the point currently suppressed         */
} DOUBLE_LIST;

/*
 * v820: table 18 note 20 issue
 */

/* information relating to pending event data */
typedef struct
{
    ulong   first_class1;       /* first class 1 object to report */
    ulong   first_class2;       /* first class 2 object to report */
    ulong   last_class1;        /* last class 1 object to report */
    ulong   last_class2;        /* last class 2 object to report */

    uchar   class1_pending;     /* set if class 1 data available in LRU */
    uchar   class2_pending;     /* set if class 2 data available in LRU */


    uchar   term_asdu[MAX_FRAME_LEN]; /* mirrored ASDU ACTTERM msg */
    int     term_asdu_len;      /* num of octets in ASDU ACTTERM msg */
    ulong   term_interrogation; /* ACTTERM interrogation bit field */
    uchar   term_count_quality[5]; /* counter interrogation qualifier of cmd
                                    * for group 1, 2, 3, 4, & general */
    uint    eoi_qualifier;      /* qualifier of initialisation for M_EI obj */
    FLAG    eoi_already_tx;     /* true if M_EI has already been Tx */

    EVENT_DATA *requested_obj_data; /* obj_data requested by the last read
                                     * command (class 2 only) */
} REPORT_USER_DATA;

/**************************************************************************/
/*              Reverse Mapping Table related definitions                 */
/**************************************************************************/

/* used as index into the obj_data structure */
typedef struct
{
    ulong obj_addr;
    ulong num_addr;
    long data_index;    /* index into the event/object data table */
} OBJECT_ADDRESS;

typedef struct
{
    OBJECT_CFG *obj_cfg;    /* OBJECT that contains this point            */
    int obj_num;            /* not really req. - but useful for debugging */
    int map_pnt;            /* offset into MT0x table */
    ulong obj_data_index;   /* index into the obj_data table */
    int obj_pnt_index;      /* specifies which bit in the obj_addr belongs
                             * to this point */
} REVERSE;

typedef struct
{
    int num_mappings;         /* number of mappings that exists for a point */
    int rev_index;            /* index into reverse table info              */
} LOGICAL_MAP;

typedef struct
{
    int start;              /* first point of this type on this RTU       */
    int span;               /* span of points mapped to this RTU          */
    LOGICAL_MAP *log;       /* maps back into the DPA logical structure   */
    REVERSE *rtu_points;    /* actual point information for the rtu       */
} DPA_REVERSE_TABLE;


/**************************************************************************/
/*          File Transfer related definitions                             */
/**************************************************************************/


/* supported download and upload configuration file name*/
#define FT_CFG_FILE_NAME    1

/* File Transfer status*/

enum FT_STATUS
{
    W_FILE_READY = 0,
    W_SECTION_READY,
    W_SEGMENT,
    SECTION_RECEIVED,
    MAX_FT_STATUS
};

/* flags for message pending*/
enum FT_MSG
{
    MSG_NONE = 0,
    MSG_CALL_FILE,
    MSG_CALL_SECTION,
    MSG_ACK_FILE,
    MSG_ACK_SECTION,
    MSG_DIR,
    MSG_FILE_READY,
    MSG_SECTION_READY,
    MSG_SEGMENT,
    MSG_LAST_SEGMENT,
    MSG_LAST_SECTION
};

/* type of TX message to update */
enum TX_MSG_TYPE
{
    NEW_TX_MSG = 1,
    BUF_TX_MSG
};

/* File Transfer control variable*/

typedef struct
{
    enum FT_STATUS   state;     /* file transfer control state*/
    enum FT_MSG msg_pending;    /* indicate whether or not any File Transfer
                                 * message is pending.
                                 */
    int expected_section;       /* the section name (number) we expect to receive*/
    long seg_offset;            /* store bytes we have received in the current section*/
    long received_length;       /* correctly received bytes in acknowledged sections*/
    int current_section;        /* the section we are receiving now*/
    unsigned int current_file;  /* the file we are rreceiving now*/
    unsigned long file_length, section_length; /* length of file and section*/
    unsigned int asdu_file;     /* file name used in outgoing PDU*/
    int asdu_section;           /* section name used in outgoing PDU*/
    uchar qualifier;            /* ASDU qualifier for File Transfer*/
    CAUSE_OF_TX  cot;           /* cause of transmission*/
    uchar section_sum;          /* check sum for the current section*/
    uchar file_sum;             /* check sum for the current file*/
    FLAG  committing;           /* TRUE if we are ready to commit new cfg*/
    unsigned long commit_time_rtc; /* rtc count at which to start committing*/
    FILE    *fp;                /* downloaded file pointer*/
    unsigned long file_ioa;
} FT_CTRL;

/* balanced mode message pool structure*/

typedef struct _a101_msg
{
    unsigned char msg[MAX_FRAME_LEN];   /*  app message*/
    int     length;                     /* message length in byte*/
    int     service;                    /* tx service: Guaranteed, or failhold */
    unsigned long   time;               /* time message received, or guaranteed tx time*/
    unsigned int    id;                 /* transmission id, id = 0 for not yet sent */
    unsigned int    channel;            /* BridgeMAN channel*/
    int             status;             /* BM Send message status */
    struct _a101_msg  *next;            /* pointer to next one*/
}A101_MSG ;

#define A101_MSG_SIZE  sizeof(A101_MSG)

/* transmission management state machine structure*/

#define     A101_TX_INIT            0
/* #define     A101_TX_PENDING         1  710: Removed this state */
#define     A101_TX_CONNECTED       2
#define     A101_TX_DISCONNECTED    3
/* #define     A101_TX_FAILED          4  710: Removed this state */


typedef struct _a101_tx_man
{
    int         state;           /* status */
    int         comm_state;      /* state of communication indication control point*/
    int         active_ch;       /* active channel, != -1*/
    int         last_sent_status;/* 710: last BM send status returned */
    A101_MSG    *tx_msg;         /* 710: ptr message to send*/
    A101_MSG    *tx_msg_head;    /* 710: ptr message to send*/
    A101_MSG    *tx_msg_tail;    /* 710: ptr message to send*/
    unsigned int num_tx_msg_buffered;     /* 710: number of tx message buffered but not yet sent */
    unsigned int num_tx_msg_cfm_pending;  /* 710: number of tx message buffered but awaiting cfm */

    unsigned long num_tx_status;  /* 710: some additional run-time stats */
    unsigned long num_tx_p;
    unsigned long num_tx_n;
    unsigned long num_bm_send_success;
    unsigned long num_bm_send_no_buffers;
    unsigned long num_bm_send_failures;

    PRIMARY_PROTOCOL_MSG  rx_msg;/* contains application message from master*/
}A101_TX_MAN;

/* delay for getting buffer retry*/
#define A101_GET_BUFFER_DELAY   50L


/**************************************************************************/
/*          Process information object definition                         */
/**************************************************************************/

/*
** This structure is used for a lookup table indexed by process information
** object type. The table is declared in OBJ_INFO.C.
*/
typedef struct
{
    enum TYPE_ID_FIELD obj_type;        /* Object type this record represents */
    enum TYPE_ID_FIELD non_tt_type;     /* Same object type without time tag */
    int has_time_tag;                   /* True if object is reported with a time tag */
    int can_be_sequenced;               /* True if objects can be reported in a sequence */
    int obj_len_wo_addr;                /* Length of object without address */
} PROCESS_INFO_TYPE;

/**************************************************************************/
/*              LRU datbase related definitions                           */
/**************************************************************************/

typedef struct
{
    /* File Transfer control variable*/
    FT_CTRL     ft_control;

    /* flag TRUE if it has been referenced by a LXR record*/
    FLAG    is_referenced;

    /* flag TRUE if it has been spawned. note, LRU may not be spwaned because of cfg err*/
    FLAG    is_spawned;

    /* balanced mode TX message buffer pool*/
    A101_MSG    *tx_buffers;

    /* balanced mode RX message buffer pool*/
    A101_MSG    *rx_buffers;


    char        recv_name[10];   /* process name  for receive process*/

    /* balanced mode tx state machine*/
    A101_TX_MAN     tx_man;

    /* test pattern for 104 new test command*/
    uint        test_seq;

    /* WIN and PSOS data elements */

    char        name[10];   /* process name                               */
    int         lru_num;    /* LRU number = record index in lru_cfg       */

    /* double point process information */
    char        dblname[10];/* double point process name */
    char        dbllogbuf[LOG_BUF_SIZE]; /* logerr char buf */
    ulong       num_dpi;    /* number of DPI objects in the LRU           */

    struct dpa_data *dpa;   /* Pointer to DPA spawner struct */

    /* Configuration Pointers     */
    LRU_CFG     *lru_cfg;   /* LRU config                                 */
    OBJECT_CFG  *obj_cfg;   /* OBJECT config                              */

    ulong       first_frz_it, last_frz_it;

    DPA_REVERSE_TABLE    rev[NUM_WESDAC]; /* reverse mapping tables          */
    FLAG class1_ahead, class2_ahead; /* TRUE if class 1 or class 2 buffered
                                      * objects must be sent first.
                                      */
    ulong   next_hour_exp_rtc;       /* the RTC count for the prejected next top of
                                      * hour.
                                      */
    FLAG    process_hour_update;     /* TRUE if the hour update has to be processed*/
    EVENT_BUF_HEADER    hour_buf;    /* contains hour update objects*/
    EVENT_BUF_HEADER    event_buf[NUM_CLASS][NUM_EVENT]; /* contains the 14 event buffers */
    EVENT_BUF_HEADER    free_buf;    /* contains the pointer to the free buffer memory
                                      * pool. when free_buf.num_used<=0, buffer overflow
                                      * occurs.
                                      */

    FLAG    event_buf_overflow_processed;    /* TRUE if binary output performed*/
    FLAG    event_buf_low_processed;   /*v400: TRUE if binary output performed*/
    int     init_count;                /* counts system pnts not initialised         */

    /* Data/Communication Buffers */

    SECONDARY_PROTOCOL_MSG    tx_msg;     /* buffer for message to send                 */
    PRIMARY_PROTOCOL_MSG     *rx_msg;    /* point to received message data */
    enum ASDU_RX_MSG_STATUS   msg_status; /* status of FT12 framing */

    /* general RTU runtime data */
    EVENT_DATA       *obj_data;     /* run time object data with addr & group params */
    REPORT_USER_DATA  report;       /* information relating to pending event data */

    ulong       num_obj_data; /* number of configured obj elements/addresses */
    int         num_obj;    /* number of object records configured in LRU */

    int         ack_count;  /* control requests acks outstanding          */

    SELECT_CONTROL_STATUS  select_ctl; /* holds info about the last Rx select cmd Rx */

    /* time related functions */
    LOCAL_TIME  LocalTime;          /* local time keeping functions */
    ulong       time_set;           /* last time the RTU time was set */
    FLAG        invalid_time;       /* set if time sync has failed */

    /*v500: for delay measurement*/
    long        sdt_time;           /* time value from master */
    long        sdt_rtc;            /* RTU rtc counter when receiving master sdt*/
    long        delay_time;         /* communication delay time received from
                                     * master spontaneous delay measurement message.
                                     */

    FLAG        ci_freezing;        /* TRUE if counter freezing is caused by couter
                                     * interrogation command
                                     */
    unsigned long lru_smid;         /* local semphore id of mutual exclusion between
                                     * LRU and its event handler.
                                     */
    unsigned int  idle_check_tid;   /* 710: idle check timer ID */
    unsigned int  resend_tx_msg_tid;/* 710: resend_tx_msg timer ID */

    /* calculated configuration parameters */
    CALC_LRU_CFG  calc;             /* useful one-off calculated cfg related params */

    int  tx_buffer_already_logged;  /* flag indicates no tx buffer warning already logged once */
    char logbuf[LOG_BUF_SIZE];      /* logerr character buffer for error log        */
    /* V715 */
    char GIComplete;                /* Flag to indicate whether GI report data should be sent
                                     * before other types of data
                                     */
    char GIPending;                 /* V715: Flag to indicate if a GI report is pending */

    int     lru_ready;               /* V820: Flag to indicate LRU is ready to receive messages */

    /* V830: Indeces and Flags to indicate GI and Group interogations */
    ulong   InterrogationPending;
    long    InterrogationIndex[17]; /* Indicates the object index of next object to be reported in interrogation */
    long    directory_base;
    int     file_count;

} LRU_DATA;


typedef struct
{
    /* PSOS related Elements */

    int         fd;         /* file desc for communications port          */
    char        name[10];   /* process name                               */

    /* Configuration Pointers */

    COM_CFG     *com_cfg;   /* communications config                      */
    LXR_CFG     *xref;      /* first LXR_CFG record for this COM */

    /* LRU Communications data */

    LRU_DATA    *lru;       /* pointer to the very first LRU_DATA record
                             * corresponding to first lru_cfg record (ie. lru_num=0)
                             * NOTE, this is NOT a pointer to the first LRU for this COM
                             */
    uchar        rx_buffer[BUF_SIZE];/* raw rx buffer                     */
    uchar        tx_buffer[BUF_SIZE];/* final tx buffer                   */
    SECONDARY_PROTOCOL_MSG
    *tx_msg;                /* pointer to the LRUs tx buffer              */
    PRIMARY_PROTOCOL_MSG
    rx_msg;                 /* processed rx buffer (protocol dependent)   */
    enum FT12_RX_MSG_STATUS
    msg_status;             /* status of FT12 framing                     */

    /* Communication Runtime Data */
    long        com_fail_time; /* time when communications will fail      */
    FLAG        com_failed;    /* have the communications failed          */
    LAST_MSG_STATUS
    *last_msg;                 /* fcb and function of last msg for each LRU*/

    /* calculagted configuration parameters */
    CALC_COM_CFG calc;            /* useful once off calculated cfg related params */

    char logbuf[LOG_BUF_SIZE]; /* logerr character buffer for error log   */

} COM_DATA;


typedef struct dpa_data
{
    COM_DATA   *com;          /* pointer to communication info           */
    LRU_DATA   *lru;          /* pointer to lru info                     */
    COM_CFG    *com_cfg;      /* start of com config table               */
    LXR_CFG    *xref;         /* lru cross reference config table        */
    LRU_CFG    *lru_cfg;      /* start of the LRU config table           */
    OBJECT_CFG *obj_cfg;      /* start of the OBJECT config table        */
    int        num_com;       /* number of communication records in conf */
    int        num_lru;       /* number of LRU records in the config     */
    int        num_xref;      /* number of XREF records in the config    */
    int        num_obj;       /* number of OBJECT records in the config  */
    char       logbuf[LOG_BUF_SIZE];   /* logger character buffer for error log */
} DPA_DATA;

/* support for event_buffer overflow bin output type*/

enum EV_BUF_OUTPUT
{
    EV_BUF_LATCH_ON =0,
    EV_BUF_LATCH_OFF,
    EV_BUF_TRIP,
    EV_BUF_CLOSE
};

/* v518: used by time sync*/
#define HIGHEST_PRIORITY    255
#define OVERFLOW_PRIORITY_P  0x81

/**************************************************************************/
/*                Function Proto type List                                */
/**************************************************************************/

/* COM routines */

void    a101_com_task       (COM_DATA *);
void    a101_com_proc       (COM_DATA *);
void    a101_setup_com_port (COM_DATA *);
enum INTERNAL_MESSAGE  a101_rx_message     (COM_DATA *, int *,int *);
void    a101_tx_message     (COM_DATA *, pSOS_MESSAGE *, int);
int     a101_strip_ft12_frame (COM_DATA *);
void    a101_init_com       (COM_DATA *com);
void    a101_build_ft12_frame (COM_DATA *);
/* double point routines */

void    a101_double_task    (LRU_DATA *);

/* LRU routines */

/* setup/init routines */
void    a101_lru_task       (LRU_DATA *);
void    a101_lru_process    (LRU_DATA *, pSOS_MESSAGE *);
void    a101_setup_map_tables (LRU_DATA *);
void    a101_check_lru      (LRU_DATA *);
void    a101_setup_reverse  (LRU_DATA *);
void    a101_setup_lru      (LRU_DATA *);
int     a101_strip_asdu     (LRU_DATA *lru);
void    a101_init_lru       (LRU_DATA *lru);

/* FT12 function routines */
void    a101_reset_remote_link(LRU_DATA *);
void    a101_reset_user_process (LRU_DATA *, REPLY_TYPES);
void    a101_user_data (LRU_DATA *);
void    a101_request_access_demand(LRU_DATA *lru);
void    a101_request_status_link (LRU_DATA *);
void    a101_request_user_data (LRU_DATA *, CLASS_TYPES);

/* msg framing/formatting routines */
void    a101_update_ft12_frame(LRU_DATA *, enum SECONDARY_LINK_FUNCTION_CODE);
void    a101_build_dui      (LRU_DATA *);
void    a101_build_iec_time (uchar *, long, long, int, enum OCTET_TIME, FLAG);
void    a101_strip_iec_time_psos (uchar *, long *, long *, int *, FLAG *);
int     a101_add_object_to_asdu (LRU_DATA *, EVENT_DATA *, enum SQ_TYPE,
                enum CAUSE_OF_TX_FIELD, enum TYPE_ID_FIELD, uchar, int, int);
void    a101_build_confirmation_reply(LRU_DATA *, CAUSE_OF_TX *);
void    a101_build_termination_reply (LRU_DATA *, CAUSE_OF_TX *);
int     a101_init_cause_of_tx (LRU_DATA *, CAUSE_OF_TX *);

/* user data (command) routines */
int     a101_digital_control_command(LRU_DATA *, CAUSE_OF_TX *, EVENT_DATA *);
int     a101_analog_control_command (LRU_DATA *, CAUSE_OF_TX *, EVENT_DATA *);
int     a101_clock_sync_command(LRU_DATA *, CAUSE_OF_TX *);
int     a101_delay_aquisition_command(LRU_DATA *, CAUSE_OF_TX *);
int     a101_test_command   (LRU_DATA *, CAUSE_OF_TX *);
int     a101_read_command   (LRU_DATA *, EVENT_DATA *);
int     a101_reset_process_command(LRU_DATA *, CAUSE_OF_TX *);
int     a101_interrogation_command(LRU_DATA *, CAUSE_OF_TX *);
int     a101_counter_interrogation_command(LRU_DATA *, CAUSE_OF_TX *);
int     a101_parameter_measured_command(LRU_DATA *, CAUSE_OF_TX *, EVENT_DATA *);
int     a101_parameter_activation_command(LRU_DATA *, CAUSE_OF_TX *, EVENT_DATA *);


/* linked list buffer routines */
void    a101_create_buffer_header(EVENT_BUF_HEADER *, int);
void    a101_add_event_buffer(EVENT_BUF_HEADER *, EVENT_BUF *);
EVENT_BUF *a101_get_event_buffer(EVENT_BUF_HEADER *);
int     a101_event_buffer(BUFFER_OPERATION, EVENT_BUF_HEADER *, EVENT_DATA *, EVENT_BUF_HEADER *);
int     a101_obj_to_buffer_index(int);
int     a101_buffer_index_to_obj(int);
void    a101_reset_buffer (LRU_DATA *, RESET_OPERATION);


/* information object routines */
void    a101_setup_object_data(LRU_DATA *);
int     a101_obj_to_wesdac  (int);
int     a101_obj_addr_info  (int, int, int *);
int     a101_binary_search  (EVENT_DATA *, ulong, ulong, ulong *);
/* Rev 851: modified prototype to fix DR 8136: GI reporting optimization */
PROCESS_INFO_TYPE* a101_get_proc_info_type ( enum TYPE_ID_FIELD proc_info_typeid, enum CAUSE_OF_TX_FIELD cause, LRU_DATA *lru );

/* Time related functions for setting/getting local LRU & main RTU time */
int     a101_get_t          (long * , long * , int * , LOCAL_TIME *);
int     a101_set_t          (long, long, int, LOCAL_TIME * );
void    a101_convert_counter(ulong, long * , long *, int *, LOCAL_TIME *);
void    a101_unix_to_psos_time(unsigned long ,long *,long *);
unsigned long a101_psos_to_unix_time (long lDate, long lTime);

void    a101_time_adjust    (long, LOCAL_TIME *);
int     a101_time_out       (ulong start_time, ulong timeout);

/* Rev 851: new function added for RTC counter comparson */
long   a101_time_diff	(ulong time1, ulong time2 );

/* Analog Routines */
int     a101_scale          (int , ANALOG_SCALE *, FLAG *);


/* general data/allocation related routines  */
int     a101_findtbl_getptr (char *,void **, const int );
void    a101_psos_name      (char *, char , int , char, int , enum pSOS_NAME_TYPE );
void    a101_check_config   (DPA_DATA *);
void    a101_error          (char * , ...);
void    a101_warning        (char * , ...);


/*  WIN related routines */
void    a101_report_enable  ( LRU_DATA * );
void    a101_setup_win_dpa  ( LRU_DATA * );
void    a101_init_ownership ( LRU_DATA * );
//void    a101_pnt_num        ( POINT_TABLE *, int, int *);
void    a101_event          ( LRU_DATA *, unsigned int ,void * );
int     a101_analog_output  ( LRU_DATA *, ANALOG_OUTPUT_MAP *, int );
int     a101_control        ( LRU_DATA * lru, BINARY_OUT_MAP * boutmap, int value, int on_time,
                              int req_num, int last, int do_commfail);
int     a101_group_control  (LRU_DATA *lru, BINARY_OUT_MAP *bout_map);
void    a101_config_table_change(LRU_DATA *lru, char *tbl_name, uint start_rec, uint num_rec);
int     a101_group_number( BINARY_OUT_MAP *bout_map);

/* standard routines without standard header files */

int     logerr              (char *,...);

/* calculate two flags for hour update object report*/
void a101_obj_ahead_hour(LRU_DATA *lru);

/* function used to handle file transfer */
int a101_ft_fsm(LRU_DATA *lru, CAUSE_OF_TX *cot);

/* two functions from objahead.c for hour update handling*/

void a101_add_next_hour_obj(LRU_DATA *lru);
void a101_adjust_next_hour_obj(LRU_DATA *lru);
void a101_clock_sync(LRU_DATA *lru, unsigned int event, void *eventdata);

/* four IFS file operation APIs for file transfer */

int ft_start_cfg(LRU_DATA *lru);
int ft_append_cfg(LRU_DATA *lru, unsigned char *cfg_data, unsigned char len);
int ft_finish_cfg(LRU_DATA *lru);
int ft_abort_cfg(LRU_DATA *lru);
int ft_commit_cfg(LRU_DATA *lru);
int ft_rollback_cfg(LRU_DATA *lru, long offset);
int ft_length_cfg(LRU_DATA *lru, long * length);
int ft_create_time_cfg(LRU_DATA *lru, unsigned char * iec_time);
int ft_get_file_info(LRU_DATA *, char *, unsigned long *, unsigned char *);
void *a101_malloc(long size);
int A101_rxer_proc(register LRU_DATA *lru);

/* transmission related balanced mode funtion*/
int a101_b_req_proc(LRU_DATA *lru, pSOS_MESSAGE *rx_message);
int a101_b_ls_p_proc(LRU_DATA *lru, pSOS_MESSAGE *rx_message);
int a101_b_ls_n_proc(LRU_DATA *lru, pSOS_MESSAGE *rx_message);
int a101_b_tx_p_proc(LRU_DATA *lru, pSOS_MESSAGE *rx_message);
int a101_b_tx_n_proc(LRU_DATA *lru, pSOS_MESSAGE *rx_message);
int a101_b_rx_msg_proc(LRU_DATA *lru, pSOS_MESSAGE *rx_message);
int a101_b_resend_proc(LRU_DATA *lru);
int a101_b_send_msg(LRU_DATA *lru, A101_MSG *tx_msg);
int a101_b_pick_msg (LRU_DATA *lru);
int a101_validate_tt_control(LRU_DATA *lru);
int a101_buffer_ai(LRU_DATA *lru, EVENT_DATA *obj_data);
void a101_resend_timeout(unsigned int tid, LRU_DATA *lru);
void a101_b_send_msg_update(LRU_DATA *lru, A101_MSG *req_msg, enum TX_MSG_TYPE type);
void a101_b_add_msg_to_buffer(LRU_DATA *lru, A101_MSG *req_msg);
int  a101_b_remove_msg_from_buffer(LRU_DATA *lru, unsigned int id);
A101_MSG  *a101_b_find_msg_with_id(LRU_DATA *lru, unsigned int id);
A101_MSG  *a101_b_next_buffered_msg(LRU_DATA *lru);
int a101_general_interrogation_data (LRU_DATA *, SECONDARY_PROTOCOL_MSG *);
int a101_interrogation_type(int);

/* V830: detect if a control actcon is pending */
int a101_control_actcon_pending (LRU_DATA *);

/*v830: end if initialization qualifier function */
uint    a101_eoi_qualifier(void);


/*Rev 851: new function to estimate if obj_data should be sent in sequence */
enum SQ_TYPE a101_obj_should_be_sequenced ( EVENT_DATA *obj_data, ulong num_obj, int gi, LRU_DATA *lru );

};

#endif // PROTOCOL_IEC101_104_H


