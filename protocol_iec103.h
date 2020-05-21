#ifndef PROTOCOL_IEC103_H
#define PROTOCOL_IEC103_H

#include <QtNetwork>
#include <QSerialPort>
#define SEND 1
#define RECEIVE 0
class IEC103
{
public:
    IEC103();

    ~IEC103();
signals:
    void sendData1(QByteArray &);

void  IEC103_processPacket(QSerialPort *serial, QByteArray &data);
void  IEC103_spont(QSerialPort *serial);
void  IEC103_systemTimeTocp56time2a();
void  IEC103_M_SP_DP_TB_1_handle(QSerialPort *serial,unsigned char point_type);
void  IEC103_M_ME_TF_1_handle(QSerialPort *serial,unsigned char point_type);
void  IEC103_C_CS_NA_1_handle(QSerialPort *serial,QByteArray &data);                 //Time Sync;
void  IEC103_M_SP_DP_NA_1_handle(QSerialPort *serial,unsigned long info_object_address, unsigned int points_number,unsigned char point_type,unsigned char cause);
void  IEC103_C_IC_NA_1_handle(QSerialPort *serial,QByteArray &data);                 //Integrity Poll
void  IEC103_Ack_handle(QSerialPort *serial,QByteArray &data);
void  IEC103_Req_C1_handle(QSerialPort *serial,QByteArray &data);
void  IEC103_Req_C2_handle(QSerialPort *serial,QByteArray &data);
void  IEC103_Req_Link_Status_handle(QSerialPort *serial,QByteArray &data);
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
    struct  {
        unsigned char		FUNC    :4;
        unsigned char		FCV_DFC :1;
        unsigned char		FCB_ACD :1;
        unsigned char		PRM     :1;
        unsigned char       RES     :1;
    } bit;

};
control_field_struct control_field_u;

//typedef struct control_field_struct control_field_struct;
#define MAX_INTERRO_POINT 10
typedef struct
{
    unsigned char  su;
    unsigned int   lru_common_address_size;     /*RTU Address bytes*/

    unsigned char  link_address;            /*RTU Address*/
    unsigned int   lru_role;                /*0: Slave; 1: Master*/
    unsigned int   ioBoardNumber;           /* Number of IO Board*/
    unsigned char  acd_flag;
    bool           lru_inited_flag;
    bool           lru_normal_flag;
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

} IEC103_TOOL_CFG;
IEC103_TOOL_CFG cfg;
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
    F_DR_TA_1 = 126,             /* directory */
    M_IRF_NA_3 =5,
    M_MEI_NA_3 =3
                /* 127 reserved */
};

enum SQ_TYPE
{
    SINGLE = 0,
    SEQUENCE        /* sequence reply only used for non-buffered event data */
};

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


/* control mapping table */
enum CONTROL_TYPE
{
    TRIP_CLOSE      = 0,
    PULSE           = 1,
    LATCHING        = 2,
    ILLEGAL_CONTROL
};

/* comms cfg flow control masks */
#define RTS_FLOW_MASK   0x1
#define CTS_FLOW_MASK   0x2
#define DCD_FLOW_MASK   0x4


};

#endif // PROTOCOL_IEC103_H


