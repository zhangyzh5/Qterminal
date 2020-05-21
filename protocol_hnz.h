#ifndef PROTOCOL_HNZ
#define PROTOCOL_HNZ

#endif // PROTOCOL_HNZ

/*
 *   Function: PROTOCOL_HNZ.H
 *   Author:   Robert Zhang
 *   Date:     15 June 2015
 *
 *      ((C)) Copyright Smart Grid Communication, 2021.
 *                          All rights reserved.
 *
 *   Description:
 *
 *   This is the include file for a Data Processing Application
 *   Revision History:
 *
 *  Num Modified by         Date            Reason
 *  --- -------------       ---------       ----------------------
 *  100 Robert Zhang        15-June-2015    Created from HNZ v204
 *
 */

/* Usefull macros */
#define CHAR char
//#define INT int
//#define ULONG unsigned long
#define LONG long
#define VOID void
#define HNZ_SECONDS(time)              ( time & 0x000000ff )
#define HNZ_MINUTES(time)              ((time & 0x0000ff00) >> 8)
#define HNZ_HOURS(time)                ((time & 0x00ff0000) >> 16)

#define HNZ_TIME(hour,minute,second)   (((LONG) hour   << 16) | \
                                        ((LONG) minute <<  8) | \
                                        ((LONG) second ))

#define HNZ_SWAP(x)            ((x << 8) | (x >> 8))


/* Defines the APPLICATION Number for this DPA */

#define APPL    "HNZ"
#define APPLNUM   84

/* General Defines */
#define HNZ_SUCCESS    0
#define HNZ_FAILURE    -1
#define HNZ_DISABLED   -1

#define HNZ_VALID      0
#define HNZ_INVALID    1

#define HNZ_NUM_EVENTS     23     /*R200+  19 */
#define HNZ_NUM_TABLES     9
#define HNZ_NUM_PARSE_FUNC 12

#define HNZ_STEADY     'S'
#define HNZ_FLEETING   'F'

#define HNZ_NORMAL     'N'
#define HNZ_INVERTED   'I'

#define HNZ_OLDEST     'O'
#define HNZ_NEWEST     'N'

#define HNZ_TRANSITION 'T'
#define HNZ_PULSE      'P'

#define HNZ_YES        'Y'
#define HNZ_NO         'N'

#define HNZ_BIPOLAR    'B'
#define HNZ_UNIPOLAR   'U'

#define HNZ_CRC_POLY        0x8408
#define HNZ_CRC_INIT        0xFFFF

#define HNZ_STATUS_ON      0x08
#define HNZ_STATUS_OFF     0x00
#define HNZ_VALIDITY_ON    0x10
#define HNZ_VALIDITY_OFF   0x00
#define HNZ_OPEN           0x10
#define HNZ_CLOSE          0x08


#define HNZ_DONT_REPEAT   0

#define HNZ_RUN_INTERVAL   60000L
#define HNZ_INIT_INTERVAL   1000L

#define HNZ_MAX_AO_VAL     32767L
#define HNZ_MIN_AO_VAL    -32768L


#define HNZ_SPS_NUM    1
#define HNZ_DPS_NUM    2

#define HNZ_LRU_STARTUP 1      /* Lru startup flag*/
#define HNZ_LRU_RUNNING 0      /* Lru running flag*/

#define HNZ_DI_MOD_ADDR 1      /*number of addresses per di module*/
#define HNZ_CI_MOD_ADDR 4      /*number of addresses per ci module*/
#define HNZ_AI_MOD_ADDR 16     /*number of addresses per ai module*/
#define HNZ_AO_MOD_ADDR 4      /*number of addresses per ao module*/
#define HNZ_SPS_POINTS  8      /*number of SPS points per address*/
#define HNZ_DPS_POINTS  4      /*number of DPS points per address*/

/* Masks */
#define HNZ_LONG_TO_BYTE       0x000000ff
#define HNZ_INT_TO_BYTE        0x00ff
#define HNZ_TOP_OF_BYTE        0x00f0
#define HNZ_BOTTOM_OF_BYTE     0x000f
#define HNZ_PM_MASK            0x80      /* mask for Master time sync PM bit*/
#define HNZ_PM_MASK_SOE        0x01      /*R200 mask for SOE event report */
#define HNZ_GROUP_MASK         0X0F
#define RTU_ID_MSB_SHIFT        1

/* DO Constants */
#define HNZ_CTL_SUCCESS        0x01
#define HNZ_CTL_WRONG_MSG      0x02
#define HNZ_CTL_INVALID        0x03
#define HNZ_CTL_DISCREP        0x04
#define HNZ_CTL_FAILURE        0x05

#define HNZ_CTL_BUSY           0x07  /*R200*/
#define HNZ_CTL_INVAL_STATUS   0x09  /*R200*/
#define HNZ_CTL_STAT_BEFORE    0x0a  /*R200*/
#define HNZ_CTL_LOCAL          0x0b  /*R200*/




#define HNZ_DO_SUCCESS         0x01
#define HNZ_DO_WRONG_MSG       0x02
#define HNZ_DO_DISCREP         0x03
#define HNZ_DO_FAILURE         0x04


/* AO Constants */
#define HNZ_AO_SUCCESS         0x01
#define HNZ_AO_WRONG_MSG       0x02
#define HNZ_AO_DISCREP         0x03
#define HNZ_AO_INVALID         0x04

/* AI Constants */

#define HNZ_MAX_UP    4095  /* Maximum Unipolar Value */
#define HNZ_MAX_BP    2047  /* Maximum Bipolar Value */
#define HNZ_MIN_BP   -2048  /* Maximum Bipolar Value */
#define HNZ_MAX_CAP      1  /* Maximum value exceeded */
#define HNZ_NO_CAP       0  /* No cap */
#define HNZ_MIN_CAP     -1  /* Minimum value exceeded */
#define HNZ_MAX_GROUPS   7  /* Maximum number of groups */
#define HNZ_MIN_VAL_BP   2047  /* neg value = signe (0x800) + pos value 1 to 0x7FF */
#define HNZ_AI_NEG_SIGNE 2048  /* neg value = signe (0x800) + pos value 1 to 0x7FF */



/*  message defines */
#define HNZ_ADDR_OFFSET    2  /* offset to address field       */
#define HNZ_MSG_OFFSET     4  /* offset to the message field   */
#define HNZ_CTRL_OFFSET    3  /* offset to the control field   */
#define HNZ_STARTFLG_OFFSET 0 /* offset to the start flag      */
#define HNZ_ENDFLG_OFFSET  4  /* offset from the end to the end flag   */

#define HNZ_MIN_RX_LENGTH  8  /* Minimum message length    */
#define HNZ_MAX_INF_LENGTH 37 /* Maximum size of the information field */
#define HNZ_MAX_MSG_LENGTH 31 /* Maximum size of a message */
#define HNZ_MAX_ADDR_BYTES 4  /* Maximum number of bytes in address field*/

#define HNZ_BROADCAST      0

#define HNZ_HEADER_LENGTH  4
#define HNZ_CRC_LENGTH     2
#define HNZ_CRC_OFFSET     4

#define HNZ_DLE            0x10
#define HNZ_STX            0x02
#define HNZ_ETX     (CHAR) 0x03

#define HNZ_F_BIT          0x10
#define HNZ_INFO_BIT       0x01
#define HNZ_MAX_FRAME      7

/* Message header characters */
#define NR_SHIFT    5       /* received frame number position               */
#define NR_MASK     0xE0    /* sent frame number mask                       */
#define NS_SHIFT    1       /* sent frame number position                   */
#define NS_MASK     0x0E    /* received frame number mask                   */

#define RESTART_CTRL_WORD     0x93  /* control word to indicate restart     */
#define RESTART_CTRL_ACK      0x73  /* control word ack from slave          */
#define DISCONNECT_CTRL_WORD  0x1F  /* control word to indicate disconnect  */

/* Priority Measurand Stuff */
#define HNZ_PRI_MEASURANDS 8  /* Maximum number of priority measurands*/
#define HNZ_NORMAL_MSRD    0  /* Normal measurand */
#define HNZ_PRIORITY_MSRD  1  /* Priority measurand */
#define HNZ_PRI_ACK        1
#define HNZ_PRI_ERR_2      2
#define HNZ_PRI_ERR_4      4
#define HNZ_PRI_ERR_8      8

/* Functioning Mode stuff */
#define HNZ_NEVER_REPORT   0xffff
#define HNZ_FM_00          0x00
#define HNZ_FM_01          0x01
#define HNZ_FM_FF          0xff

/* Counter stuff */

#define HNZ_MAX_CI_VAL     0x003fffff  /* maximum sendable counter */
#define HNZ_NUM_CI_BYTES   3   /* Number of sent bytes per CI value */
#define HNZ_04_VAL_LENGHT  4   /* number of bites per value sent*/
#define HNZ_TRANS_DIV      0   /* Transition Divisor */
#define HNZ_PULSE_DIV      1   /* Pulse Divisor */

/* Time Definitions                 */

#define ONE_SECOND     1000L
#define ONE_MINUTE     60L * ONE_SECOND
#define WIN_ACK_WAIT   10L

#define TX_BUFF_SIZE   1024
#define RX_BUFF_SIZE   1024

/*   Process Stack size definitions */

#define SPAWN_PRIORITY    50
#define SPAWN_SUPER_STACK 512
#define SPAWN_USER_STACK  1024

#define COM_PRIORITY      195
#define COM_STK           0x01000300

#define LRU_PRIORITY      185
#define LRU_STK           0x01000800

/*  Module Types  */


/*
 * If you change this you must also change the number of
 * points allowed per module in data.c*/
#define HNZ_SPS    0   /* Single pole signaling */
#define HNZ_DPS    1   /* Double pole signaling */
#define HNZ_CI_FZ  4   /* Counter  */
#define HNZ_CI_RUN 5   /* Counter  */
#define HNZ_DC_TC  6   /* Digital Control (Trip/Close) */
#define HNZ_DC_P   7   /* Digital Control (Pulse) */
#define HNZ_DO     8   /* Digital Output   */
#define HNZ_AI     9   /* Analog Input */
#define HNZ_AO    10   /* Analog Output    */

#define HNZ_NUM_TYPES  11

#define HNZ_MODULUS_MULT      300000L   /* modulus multiplier */
#define HNZ_TIME_CODE_MULT         5L   /* time code multiplier */
#define HNZ_MS_DAY          86400000L   /* number of ms in a day */
#define HNZ_MS_HOUR          3600000L   /* number of ms in an hour */
#define HNZ_MS_MINUTE          60000L   /* number of ms in a minute */
#define HNZ_MS_SECOND           1000L   /* number of ms in a second */
#define HNZ_HALF_DAY        43200000L   /* number of ms in half a day */


/* Message Sizes */
#define HNZ_01_SIZE        9
#define HNZ_06_SIZE        4
#define HNZ_09_SIZE        3
#define HNZ_0A_SIZE        3
#define HNZ_0B_SIZE        6       /*R200 */
#define HNZ_0D_SIZE        3
#define HNZ_0F_SIZE        2
#define HNZ_1A_SIZE        4
#define HNZ_1B_SIZE        4
#define HNZ_1C_SIZE        4
#define HNZ_1D_SIZE        5
#define HNZ_18_SIZE        2
#define HNZ_20_SIZE        3
#define HNZ_22_SIZE        2
#define HNZ_40_SIZE        4
#define HNZ_41_SIZE        4

#define HNZ_MAX_04_SIZE    10
#define HNZ_MAX_26_SIZE    10

/* Message Codes */
#define HNZ_01_CODE    0x01
#define HNZ_02_CODE    0x02
#define HNZ_04_CODE    0x04
#define HNZ_09_CODE    0x09
#define HNZ_0A_CODE    0x0a
#define HNZ_0B_CODE    0x0b
#define HNZ_0D_CODE    0x0d
#define HNZ_16_CODE    0x16
#define HNZ_18_CODE    0x18
#define HNZ_1A_CODE    0x1a
#define HNZ_1D_CODE    0x1d
#define HNZ_20_CODE    0x20
#define HNZ_22_CODE    0x22
#define HNZ_26_CODE    0x26
#define HNZ_32_CODE    0x32
#define HNZ_3D_CODE    0x3d
#define HNZ_40_CODE    0x40
#define HNZ_41_CODE    0x41

/* Special Message Codes */
#define HNZ_SIGN_CHK_END  0x0c /* End of signaling overall check */
#define HNZ_MSRD_BLK_END  0x0e /* End of measurand blocks */


#define HNZ_INT_ADDR   0x02    /* Internal signalings address */

/*R200 DSW definition ( it is DSW02 and DSW03 */
/*DSW02*/
#define HNZ_DSW02            0x02 /* LRU Address of the DSW02 */
                                   /* 0x01 not used */
#define HNZ_DSW02_RTU_LOCAL  0x02 /* RTU switch in LOCAL state*/
#define HNZ_DSW02_RTUINIT    0x04 /* 1 after restart, 0 after report to Master*/
#define HNZ_DSW02_RTU_BAD    0x08 /* CR polarity bit- set if DSW03-0 or DSW03-7 set */
#define HNZ_DSW02_LINK1      0x10 /* Link 1 failure */
#define HNZ_DSW02_LINK2      0x20 /* Link 2 failure */
#define HNZ_DSW02_TEMP_50    0x40 /* RTU temperature above 50 deg*/
#define HNZ_DSW02_TEMP_60    0x80 /* RTU temperature above 60 deg*/

#define DSW02_RTU_LOCAL_ADDR  0x20
#define DSW02_RTU_BAD_ADDR    0x60
#define DSW02_TEMP_50_ADDR    0xc0 /* RTU temperature above 50 deg*/
#define DSW02_TEMP_60_ADDR    0xe0 /* RTU temperature above 60 deg*/



/*DSW03*/
#define HNZ_DSW03            0x03 /* LRU Address of the DSW03 */
#define HNZ_DSW03_DO2_OFF    0x01 /* Remote DO invalid */
#define HNZ_DSW03_DI_OFF     0x02 /* Signaling function invalid */
#define HNZ_DSW03_AI_OFF     0x04 /* AI OFF line */
                                   /* not used */
#define HNZ_DSW03_AO_OFF     0x10 /* AO OFF-line */
#define HNZ_DSW03_ACCU_OFF   0x40 /* Counting function invalid  */
#define HNZ_DSW03_DO1_OFF    0x80 /* Generator setpoints function invalid */

#define HNZ_SOEENABLE        0x01 /*R200 SOE enable flag*/
#define DSW03_DO2_ADDR        0x00 /*R200*/
#define DSW03_DI_ADDR         0x20 /*R200*/
#define DSW03_AI_ADDR         0x40 /*R200*/
#define DSW03_AO_ADDR         0x80 /*R200*/

#define DSW03_ACCU_ADDR       0xc0 /*R200*/
#define DSW03_DO1_ADDR        0xe0 /*R200*/

/*
 * Logical Remote Unit configuration
 */

/*
 * Module Configuration
 */
typedef struct
{
    unsigned int  uModuleType;       /* Type of HNZ Module          */
    unsigned int  uFirstAddress;     /* First Addres of the Module  */

    unsigned int  uFirstPoint;       /* Offset into the corresponding maping table */
    unsigned int  uNumPoints;        /* Number of points in the module */


} HNZ_MOD_CFG;

/*
 * Callout function prototypes
 */
//INT  HNZ_check_config (L034_TABLE *, L034_MAPPED_DATA *, INT);
//VOID HNZ_init_user_data (L034_TABLE *pCfgTables, VOID *pUserData);
//INT  HNZ_init_lru (L034_LRU_DATA *);
//INT  HNZ_di_data_change (L034_LRU_DATA *, unsigned int, VOID *);
//INT  HNZ_ai_data_change (L034_LRU_DATA *, unsigned int, VOID *);
//INT  HNZ_ai_on_off_line (L034_LRU_DATA *, unsigned int, VOID *);
//INT  HNZ_ci_data_change (L034_LRU_DATA *, unsigned int, VOID *);
//INT  HNZ_ci_data_freeze (L034_LRU_DATA *, unsigned int, VOID *);
//INT  HNZ_ci_on_off_line (L034_LRU_DATA *, unsigned int, VOID *);
//INT  HNZ_di_data_update (L034_LRU_DATA *, unsigned int, VOID *);
//INT  HNZ_ai_data_update (L034_LRU_DATA *, unsigned int, VOID *);
//INT  HNZ_ci_data_update (L034_LRU_DATA *, unsigned int, VOID *);
//INT  HNZ_adjust_time_event (L034_LRU_DATA *, unsigned int, VOID *);
//INT  HNZ_on_off_line (L034_LRU_DATA *, unsigned int, VOID *, VOID *);
//LONG HNZ_data_link_check (L034_COM_PARAM * );
//VOID HNZ_pre_transmit (L034_COM_PARAM *);
//INT  HNZ_appl_layer_check (L034_LRU_DATA *, L034_COM_PARAM *);
//VOID HNZ_spawner(VOID);
int  HNZ_check_crc (CHAR *,unsigned int );
unsigned int HNZ_calculate_crc(unsigned char *, unsigned int );
//VOID HNZ_lru_processing (L034_LRU_DATA *, INT, L034_COM_PARAM *, INT *, INT * );
//VOID HNZ_init_user_data (L034_TABLE * , VOID *);
//INT  HNZ_add_to_queue (VOID **, HNZ_QUEUE_PTS *);
//VOID HNZ_add_rtu_id_to_queue( L034_LRU_DATA * );
//INT  HNZ_get_from_queue (VOID **, HNZ_QUEUE_PTS *);
//INT  HNZ_peek_at_queue (VOID **, HNZ_QUEUE_PTS *);
//VOID HNZ_init_queue (CHAR *, HNZ_QUEUE_PTS *, unsigned int, INT, CHAR);
//VOID HNZ_clear_queue (HNZ_QUEUE_PTS *);
//VOID HNZ_di_to_queue (L034_LRU_DATA  *, unsigned int, INT, INT, CHAR, ULONG );
//INT  HNZ_add_message (L034_LRU_DATA  *, VOID **, HNZ_QUEUE_PTS *);
//VOID HNZ_get_di ( L034_LRU_DATA  * );
//VOID HNZ_user_timer(L034_LRU_DATA *);
//VOID HNZ_di_overall (L034_LRU_DATA *);
//VOID HNZ_ai_overall (L034_LRU_DATA *, INT);
//INT  HNZ_calc_hnz_value ( INT, L034_MT04 *, INT *, unsigned int);
//unsigned int HNZ_calc_sys_value ( INT, L034_MT04 *);
//VOID HNZ_special_message (L034_LRU_DATA  *, CHAR );
//LONG HNZ_get_hnz_time (CHAR *);
//VOID HNZ_put_hnz_time (CHAR *, LONG, INT );
//LONG HNZ_get_time (ULONG);
//VOID HNZ_set_time (ULONG, LONG);
//VOID HNZ_set_interval (L034_LRU_DATA *);
//VOID HNZ_write_data(CHAR *, CHAR *, INT *, INT );
//VOID HNZ_initialize_rtu (L034_LRU_DATA *);

//VOID  HNZ_dsw_event_handler (L034_LRU_DATA *, unsigned int, WIN_SINGLE_EVENT *);


//INT HNZ_funct_mode_any        (L034_LRU_DATA  *, L034_COM_PARAM **, INT *, INT *);
//INT HNZ_funct_mode_all        (L034_LRU_DATA  *, L034_COM_PARAM **, INT *, INT *);
//INT HNZ_accum_reset           (L034_LRU_DATA  *, L034_COM_PARAM **, INT *, INT *);
//INT HNZ_accum_stored_values   (L034_LRU_DATA  *, L034_COM_PARAM **, INT *, INT *);
//INT HNZ_overall_check_request (L034_LRU_DATA  *, L034_COM_PARAM **, INT *, INT *);
//INT HNZ_remote_control        (L034_LRU_DATA  *, L034_COM_PARAM **, INT *, INT *);
//INT HNZ_analog_setpoint       (L034_LRU_DATA  *, L034_COM_PARAM **, INT *, INT *);
//INT HNZ_digital_output        (L034_LRU_DATA  *, L034_COM_PARAM **, INT *, INT *);
//INT HNZ_bias_computation      (L034_LRU_DATA  *, L034_COM_PARAM **, INT *, INT *);
//INT HNZ_clock_update          (L034_LRU_DATA  *, L034_COM_PARAM **, INT *, INT *);
//INT HNZ_rtu_identification    (L034_LRU_DATA  *, L034_COM_PARAM **, INT *, INT *);
//INT HNZ_rtu_init              (L034_LRU_DATA  *, L034_COM_PARAM **, INT *, INT *);

//void HNZ_ai_gr2_scan (unsigned int, L034_LRU_DATA *); /*R201. function does not need to return anything*/
//void HNZ_ai_gr3_scan (unsigned int, L034_LRU_DATA *); /*R201. function does not need to return anything*/
//void HNZ_ai_gr4_scan (unsigned int, L034_LRU_DATA *); /*R201. function does not need to return anything*/
//void HNZ_ai_gr5_scan (unsigned int, L034_LRU_DATA *); /*R201. function does not need to return anything*/
//void HNZ_ai_gr6_scan (unsigned int, L034_LRU_DATA *); /*R201. function does not need to return anything*/
//void HNZ_ai_gr7_scan (unsigned int, L034_LRU_DATA *); /*R201. function does not need to return anything*/
//void HNZ_ai_gr8_scan (unsigned int, L034_LRU_DATA *); /*R201. function does not need to return anything*/
//VOID HNZ_test_AI_point (L034_LRU_DATA *, LONG);
//VOID HNZ_ctrl_tout (unsigned int, L034_LRU_DATA *);
//int  HNZ_ao_on_off_line (L034_LRU_DATA *, unsigned int, VOID *);
//INT  HNZ_do_on_off_line (L034_LRU_DATA *, unsigned int, VOID *);

/*
 * Configuration errors (200 - 299)
 */
#define FATAL_DUP_MOD_ADDR  "F001: Duplicate address %d mapped in MOD records %d and %d"
#define FATAL_INC_MOD_TYPE  "F002: Incorrect Module Type %d mapped in MOD record %d"
#define FATAL_AI_RANGE_ERR  "F003: Range should not be set to 0 in MT04 record %d"
#define FATAL_AO_RANGE_ERR  "F004: Range should not be set to 0 in MT05 record %d"
#define FATAL_DUP_LRU_ADDR  "F005: Duplicate address %ld mapped in LXR record %d"
#define FATAL_MAX_POINTS    "F006: Maximum number of points exceeded in MOD record %d"
#define FATAL_LRU_TBL_NFOUND "F008: HNZ_LRU_CFG table not found"
#define FATAL_MOD_TBL_NFOUND "F009: HNZ_MOD_CFG table not found"
#define FATAL_SOE_INIT_ERROR "F010: Could not initialize SOE sorting buffer"
#define FATAL_INIT_POINTS_ERROR "F011: Error %d, initializing points"
#define FATAL_DO_UN_ERROR    "F012: Only one point is undefined in MT02 records %d and %d"
#define FATAL_DPS_UN_ERROR   "F013: Only one point is undefined in MT01 records %d and %d"
#define FATAL_ODD_ERROR      "F014: Even number of points should be defined in MOD rec %d"
#define FATAL_AI_GROUP       "F015: Group %d missing in MT04 table for LRU %d"

#define HIGH_QUEUE_OVERFLOW "I001: High Priority Queue overflow"
#define LOW_QUEUE_OVERFLOW  "I002: Low Priority Queue overflow"
/*R201 Pulse DI not mapped error message added*/
#define PULSE_DI_NOT_MAPPED "I003: LRU = %d pulse type (MT02 entry %d) DI not mapped "

#define SOE_GET_ERROR       "LRU %d: Error %d getting queued SOE"
#define HNZ_BOARD_MAX   256

enum ioType{
    SPS=0,
    DPS=1,
    COUNTER=2,
    DOTC=3,
    DOPULSE=4,
    DOLATCH=5,
    AI=6,
    AO=7
};


typedef struct
{
    int   lru_address_size;     /*RTU Address bytes*/

    int   lru_address;          /*RTU Address*/
    int   lru_role;                 /*0: Slave; 1: Master*/
    int   ioBoardNumber;        /* Number of IO Board*/
    ioType ioBoardType[HNZ_BOARD_MAX];   /*  0:DI Single;
                                        1:DI Double;
                                        2:Counter Frozen;
                                        3:DO T/C;
                                        4:DO Pulse
                                        5:DO Latch
                                        6:AI
                                        7:AO */
    unsigned int  uNumMod;           /* Number of Modules          */

} HNZ_TOOL_CFG;

