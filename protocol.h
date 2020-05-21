#ifndef PROTOCOL_H
#define PROTOCOL_H

enum protocol_Id{
    NONE=0,
    DNP_SERIAL=1,
    DNP_TCP=2,
    IEC_101=3,
    IEC_103=4,
    IEC_104=5,
    MODBUS_RTU=6,
    MODBUS_TCP=7,
    MODBUS_ASCII=8,
    IEC61850_GOOSE=9,
    IEC61850_MMS=10,
    PPC_HNZ=11,
    CDC_TYPEII=12,
    PGE=13,
    SC1801=14,
    TEJAS5_1=15,
    OTHER=0xff
};


#endif // PROTOCOL_H

