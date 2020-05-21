
#include    "protocol_hnz.h"
HNZ_TOOL_CFG hnz_tool_settings;

unsigned int HNZ_calculate_crc(unsigned char *pBuffer, unsigned int uNumBytes)
{
    char  cData;
    unsigned char cuBitCtr;

    unsigned int uCrc = HNZ_CRC_INIT;
    while( uNumBytes-- )
    {
        cData = *pBuffer++;


        for ( cuBitCtr = 0xff; cuBitCtr; cuBitCtr >>= 1)
        {

            if ( (uCrc ^ cData) & 1)
            {
                uCrc >>= 1;
                uCrc ^= HNZ_CRC_POLY;
            }
            else
                uCrc >>= 1;

            cData >>= 1;
        }
    }

    return ( uCrc ^ HNZ_CRC_INIT ) ;
}
