/*
  mcp_can.cpp
  2012 Copyright (c) Seeed Technology Inc.  All right reserved.
  2014 Copyright (c) Cory J. Fowler  All Rights Reserved.

  Author: Loovee
  Contributor: Cory J. Fowler
  2014-1-16
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-
  1301  USA
*/
#include "mcp_can.h"

#define spi_readwrite SPI.transfer
#define spi_read() spi_readwrite(0x00)

/*********************************************************************************************************
** Function name:           mcp2515_reset
** Descriptions:            Performs a software reset
*********************************************************************************************************/
void MCP_CAN::mcp2515_reset(void)                                      
{
    MCP2515_SELECT();
    spi_readwrite(MCP_RESET);
    MCP2515_UNSELECT();
    delay(10);
}

/*********************************************************************************************************
** Function name:           mcp2515_readRegister
** Descriptions:            Read data register
*********************************************************************************************************/
INT8U MCP_CAN::mcp2515_readRegister(const INT8U address)                                                                     
{
    INT8U ret;

    MCP2515_SELECT();
    spi_readwrite(MCP_READ);
    spi_readwrite(address);
    ret = spi_read();
    MCP2515_UNSELECT();

    return ret;
}

/*********************************************************************************************************
** Function name:           mcp2515_readRegisterS
** Descriptions:            Reads sucessive data registers
*********************************************************************************************************/
void MCP_CAN::mcp2515_readRegisterS(const INT8U address, INT8U values[], const INT8U n)
{
	INT8U i;
	MCP2515_SELECT();
	spi_readwrite(MCP_READ);
	spi_readwrite(address);
	// mcp2515 has auto-increment of address-pointer
	for (i=0; i<n; i++) 
    {
		values[i] = spi_read();
	}
	MCP2515_UNSELECT();
}

/*********************************************************************************************************
** Function name:           mcp2515_setRegister
** Descriptions:            Sets data register
*********************************************************************************************************/
void MCP_CAN::mcp2515_setRegister(const INT8U address, const INT8U value)
{
    MCP2515_SELECT();
    spi_readwrite(MCP_WRITE);
    spi_readwrite(address);
    spi_readwrite(value);
    MCP2515_UNSELECT();
    delayMicroseconds(250);
}

/*********************************************************************************************************
** Function name:           mcp2515_setRegisterS
** Descriptions:            Sets sucessive data registers
*********************************************************************************************************/
void MCP_CAN::mcp2515_setRegisterS(const INT8U address, const INT8U values[], const INT8U n)
{
    INT8U i;
    MCP2515_SELECT();
    spi_readwrite(MCP_WRITE);
    spi_readwrite(address);
       
    for (i=0; i<n; i++) 
    {
        spi_readwrite(values[i]);
    }
    MCP2515_UNSELECT();
    delayMicroseconds(250);
}

/*********************************************************************************************************
** Function name:           mcp2515_modifyRegister
** Descriptions:            Sets specific bits of a register
*********************************************************************************************************/
void MCP_CAN::mcp2515_modifyRegister(const INT8U address, const INT8U mask, const INT8U data)
{
    MCP2515_SELECT();
    spi_readwrite(MCP_BITMOD);
    spi_readwrite(address);
    spi_readwrite(mask);
    spi_readwrite(data);
    MCP2515_UNSELECT();
    delayMicroseconds(250);
}

/*********************************************************************************************************
** Function name:           mcp2515_readStatus
** Descriptions:            Reads status register
*********************************************************************************************************/
INT8U MCP_CAN::mcp2515_readStatus(void)                             
{
	INT8U i;
	MCP2515_SELECT();
	spi_readwrite(MCP_READ_STATUS);
	i = spi_read();
	MCP2515_UNSELECT();
	return i;
}

/*********************************************************************************************************
** Function name:           setMode
** Descriptions:            Sets control mode
*********************************************************************************************************/
INT8U MCP_CAN::setMode(const INT8U opMode)
{
    INT8U i;

    mcpMode = opMode;

    i = mcp2515_setCANCTRL_Mode(mcpMode);

    return i;
}

/*********************************************************************************************************
** Function name:           mcp2515_setCANCTRL_Mode
** Descriptions:            Set control mode
*********************************************************************************************************/
INT8U MCP_CAN::mcp2515_setCANCTRL_Mode(const INT8U newmode)
{
    INT8U i;

    mcp2515_modifyRegister(MCP_CANCTRL, MODE_MASK, newmode);

    i = mcp2515_readRegister(MCP_CANCTRL);
    i &= MODE_MASK;

    if ( i == newmode ) 
    {
        return MCP2515_OK;
    }

    return MCP2515_FAIL;
}

/*********************************************************************************************************
** Function name:           mcp2515_configRate
** Descriptions:            Set baudrate
*********************************************************************************************************/
INT8U MCP_CAN::mcp2515_configRate(const INT8U canSpeed, const INT8U canClock)            
{
    INT8U set, cfg1, cfg2, cfg3;
    set = 1;
    switch (canClock)
    {
	    // Add support for boards operating at 8MHz 
	/*
	case (MCP_8MHZ):
        switch (canSpeed) 
        {
            case (CAN_5KBPS):                                               //   5KBPS                  
            cfg1 = MCP_8MHz_5kBPS_CFG1;
            cfg2 = MCP_8MHz_5kBPS_CFG2;
            cfg3 = MCP_8MHz_5kBPS_CFG3;
            break;

            case (CAN_10KBPS):                                              //  10KBPS                  
            cfg1 = MCP_8MHz_10kBPS_CFG1;
            cfg2 = MCP_8MHz_10kBPS_CFG2;
            cfg3 = MCP_8MHz_10kBPS_CFG3;
            break;

            case (CAN_20KBPS):                                              //  20KBPS                  
            cfg1 = MCP_8MHz_20kBPS_CFG1;
            cfg2 = MCP_8MHz_20kBPS_CFG2;
            cfg3 = MCP_8MHz_20kBPS_CFG3;
            break;

            case (CAN_40KBPS):                                              //  40Kbps
            cfg1 = MCP_8MHz_40kBPS_CFG1;
            cfg2 = MCP_8MHz_40kBPS_CFG2;
            cfg3 = MCP_8MHz_40kBPS_CFG3;
            break;

            case (CAN_50KBPS):                                              //  50Kbps
            cfg1 = MCP_8MHz_50kBPS_CFG1;
            cfg2 = MCP_8MHz_50kBPS_CFG2;
            cfg3 = MCP_8MHz_50kBPS_CFG3;
            break;

            case (CAN_80KBPS):                                              //  80Kbps
            cfg1 = MCP_8MHz_80kBPS_CFG1;
            cfg2 = MCP_8MHz_80kBPS_CFG2;
            cfg3 = MCP_8MHz_80kBPS_CFG3;
            break;

            case (CAN_100KBPS):                                             // 100Kbps
            cfg1 = MCP_8MHz_100kBPS_CFG1;
            cfg2 = MCP_8MHz_100kBPS_CFG2;
            cfg3 = MCP_8MHz_100kBPS_CFG3;
            break;

            case (CAN_125KBPS):                                             // 125Kbps
            cfg1 = MCP_8MHz_125kBPS_CFG1;
            cfg2 = MCP_8MHz_125kBPS_CFG2;
            cfg3 = MCP_8MHz_125kBPS_CFG3;
            break;

            case (CAN_200KBPS):                                             // 200Kbps
            cfg1 = MCP_8MHz_200kBPS_CFG1;
            cfg2 = MCP_8MHz_200kBPS_CFG2;
            cfg3 = MCP_8MHz_200kBPS_CFG3;
            break;

            case (CAN_250KBPS):                                             // 250Kbps
            cfg1 = MCP_8MHz_250kBPS_CFG1;
            cfg2 = MCP_8MHz_250kBPS_CFG2;
            cfg3 = MCP_8MHz_250kBPS_CFG3;
            break;

            case (CAN_500KBPS):                                             // 500Kbps
            cfg1 = MCP_8MHz_500kBPS_CFG1;
            cfg2 = MCP_8MHz_500kBPS_CFG2;
            cfg3 = MCP_8MHz_500kBPS_CFG3;
            break;
        
            case (CAN_1000KBPS):                                            //   1Mbps
            cfg1 = MCP_8MHz_1000kBPS_CFG1;
            cfg2 = MCP_8MHz_1000kBPS_CFG2;
            cfg3 = MCP_8MHz_1000kBPS_CFG3;
            break;  

            default:
            set = 0;
            break;
        }
        break;
	*/
        case (MCP_16MHZ):
        switch (canSpeed) 
        {
            case (CAN_5KBPS):                                               //   5Kbps
            cfg1 = MCP_16MHz_5kBPS_CFG1;
            cfg2 = MCP_16MHz_5kBPS_CFG2;
            cfg3 = MCP_16MHz_5kBPS_CFG3;
            break;

            case (CAN_10KBPS):                                              //  10Kbps
            cfg1 = MCP_16MHz_10kBPS_CFG1;
            cfg2 = MCP_16MHz_10kBPS_CFG2;
            cfg3 = MCP_16MHz_10kBPS_CFG3;
            break;

            case (CAN_20KBPS):                                              //  20Kbps
            cfg1 = MCP_16MHz_20kBPS_CFG1;
            cfg2 = MCP_16MHz_20kBPS_CFG2;
            cfg3 = MCP_16MHz_20kBPS_CFG3;
            break;

            case (CAN_40KBPS):                                              //  40Kbps
            cfg1 = MCP_16MHz_40kBPS_CFG1;
            cfg2 = MCP_16MHz_40kBPS_CFG2;
            cfg3 = MCP_16MHz_40kBPS_CFG3;
            break;

            case (CAN_50KBPS):                                              //  50Kbps
            cfg2 = MCP_16MHz_50kBPS_CFG2;
            cfg3 = MCP_16MHz_50kBPS_CFG3;
            break;

            case (CAN_80KBPS):                                              //  80Kbps
            cfg1 = MCP_16MHz_80kBPS_CFG1;
            cfg2 = MCP_16MHz_80kBPS_CFG2;
            cfg3 = MCP_16MHz_80kBPS_CFG3;
            break;

            case (CAN_100KBPS):                                             // 100Kbps
            cfg1 = MCP_16MHz_100kBPS_CFG1;
            cfg2 = MCP_16MHz_100kBPS_CFG2;
            cfg3 = MCP_16MHz_100kBPS_CFG3;
            break;

            case (CAN_125KBPS):                                             // 125Kbps
            cfg1 = MCP_16MHz_125kBPS_CFG1;
            cfg2 = MCP_16MHz_125kBPS_CFG2;
            cfg3 = MCP_16MHz_125kBPS_CFG3;
            break;

            case (CAN_200KBPS):                                             // 200Kbps
            cfg1 = MCP_16MHz_200kBPS_CFG1;
            cfg2 = MCP_16MHz_200kBPS_CFG2;
            cfg3 = MCP_16MHz_200kBPS_CFG3;
            break;

            case (CAN_250KBPS):                                             // 250Kbps
            cfg1 = MCP_16MHz_250kBPS_CFG1;
            cfg2 = MCP_16MHz_250kBPS_CFG2;
            cfg3 = MCP_16MHz_250kBPS_CFG3;
            break;

            case (CAN_500KBPS):                                             // 500Kbps
            cfg1 = MCP_16MHz_500kBPS_CFG1;
            cfg2 = MCP_16MHz_500kBPS_CFG2;
            cfg3 = MCP_16MHz_500kBPS_CFG3;
            break;
        
            case (CAN_1000KBPS):                                            //   1Mbps
            cfg1 = MCP_16MHz_1000kBPS_CFG1;
            cfg2 = MCP_16MHz_1000kBPS_CFG2;
            cfg3 = MCP_16MHz_1000kBPS_CFG3;
            break;  

            default:
            set = 0;
            break;
        }
        break;
        
        case (MCP_20MHZ):
        switch (canSpeed) 
        {
            case (CAN_40KBPS):                                              //  40Kbps
            cfg1 = MCP_20MHz_40kBPS_CFG1;
            cfg2 = MCP_20MHz_40kBPS_CFG2;
            cfg3 = MCP_20MHz_40kBPS_CFG3;
            break;

            case (CAN_50KBPS):                                              //  50Kbps
            cfg1 = MCP_20MHz_50kBPS_CFG1;
            cfg2 = MCP_20MHz_50kBPS_CFG2;
            cfg3 = MCP_20MHz_50kBPS_CFG3;
            break;

            case (CAN_80KBPS):                                              //  80Kbps
            cfg1 = MCP_20MHz_80kBPS_CFG1;
            cfg2 = MCP_20MHz_80kBPS_CFG2;
            cfg3 = MCP_20MHz_80kBPS_CFG3;
            break;

            case (CAN_100KBPS):                                             // 100Kbps
            cfg1 = MCP_20MHz_100kBPS_CFG1;
            cfg2 = MCP_20MHz_100kBPS_CFG2;
            cfg3 = MCP_20MHz_100kBPS_CFG3;
            break;

            case (CAN_125KBPS):                                             // 125Kbps
            cfg1 = MCP_20MHz_125kBPS_CFG1;
            cfg2 = MCP_20MHz_125kBPS_CFG2;
            cfg3 = MCP_20MHz_125kBPS_CFG3;
            break;

            case (CAN_200KBPS):                                             // 200Kbps
            cfg1 = MCP_20MHz_200kBPS_CFG1;
            cfg2 = MCP_20MHz_200kBPS_CFG2;
            cfg3 = MCP_20MHz_200kBPS_CFG3;
            break;

            case (CAN_250KBPS):                                             // 250Kbps
            cfg1 = MCP_20MHz_250kBPS_CFG1;
            cfg2 = MCP_20MHz_250kBPS_CFG2;
            cfg3 = MCP_20MHz_250kBPS_CFG3;
            break;

            case (CAN_500KBPS):                                             // 500Kbps
            cfg1 = MCP_20MHz_500kBPS_CFG1;
            cfg2 = MCP_20MHz_500kBPS_CFG2;
            cfg3 = MCP_20MHz_500kBPS_CFG3;
            break;
        
            case (CAN_1000KBPS):                                            //   1Mbps
            cfg1 = MCP_20MHz_1000kBPS_CFG1;
            cfg2 = MCP_20MHz_1000kBPS_CFG2;
            cfg3 = MCP_20MHz_1000kBPS_CFG3;
            break;  

            default:
            set = 0;
            break;
        }
        break;
        
        default:
        set = 0;
        break;
    }

    if (set) {
        mcp2515_setRegister(MCP_CNF1, cfg1);
        mcp2515_setRegister(MCP_CNF2, cfg2);
        mcp2515_setRegister(MCP_CNF3, cfg3);
        return MCP2515_OK;
    }
    else {
        return MCP2515_FAIL;
    }
}

/*********************************************************************************************************
** Function name:           mcp2515_initCANBuffers
** Descriptions:            Initialize Buffers, Masks, and Filters
*********************************************************************************************************/
void MCP_CAN::mcp2515_initCANBuffers(void)
{
    INT8U i, a1, a2, a3;
    
    INT8U std = 0;               
    INT8U ext = 1;
    INT32U ulMask = 0x00, ulFilt = 0x00;


    mcp2515_write_mf(MCP_RXM0SIDH, ext, ulMask);			/*Set both masks to 0           */
    mcp2515_write_mf(MCP_RXM1SIDH, ext, ulMask);			/*Mask register ignores ext bit */
    
                                                                        /* Set all filters to 0         */
    mcp2515_write_mf(MCP_RXF0SIDH, ext, ulFilt);			/* RXB0: extended               */
    mcp2515_write_mf(MCP_RXF1SIDH, std, ulFilt);			/* RXB1: standard               */
    mcp2515_write_mf(MCP_RXF2SIDH, ext, ulFilt);			/* RXB2: extended               */
    mcp2515_write_mf(MCP_RXF3SIDH, std, ulFilt);			/* RXB3: standard               */
    mcp2515_write_mf(MCP_RXF4SIDH, ext, ulFilt);
    mcp2515_write_mf(MCP_RXF5SIDH, std, ulFilt);

                                                                        /* Clear, deactivate the three  */
                                                                        /* transmit buffers             */
                                                                        /* TXBnCTRL -> TXBnD7           */
    a1 = MCP_TXB0CTRL;
    a2 = MCP_TXB1CTRL;
    a3 = MCP_TXB2CTRL;
    for (i = 0; i < 14; i++) {                                          /* in-buffer loop               */
        mcp2515_setRegister(a1, 0);
        mcp2515_setRegister(a2, 0);
        mcp2515_setRegister(a3, 0);
        a1++;
        a2++;
        a3++;
    }
    mcp2515_setRegister(MCP_RXB0CTRL, 0);
    mcp2515_setRegister(MCP_RXB1CTRL, 0);
}

/*********************************************************************************************************
** Function name:           mcp2515_init
** Descriptions:            Initialize the controller
*********************************************************************************************************/
INT8U MCP_CAN::mcp2515_init(const INT8U canIDMode, const INT8U canSpeed, const INT8U canClock)
{

  INT8U res;

    mcp2515_reset();
    
    mcpMode = MCP_LISTENONLY;

    res = mcp2515_setCANCTRL_Mode(MODE_CONFIG);
    if(res > 0)
    {
#if DEBUG_MODE
      Serial.print("Entering Configuration Mode Failure...\r\n"); 
#endif
      return res;
    }
#if DEBUG_MODE
    Serial.print("Entering Configuration Mode Successful!!!\r\n");
#endif

                                                                        /* set boadrate                 */
    if(mcp2515_configRate(canSpeed, canClock))
    {
#if DEBUG_MODE
      Serial.print("Setting Baudrate Failure...\r\n");
#endif
      return res;
    }
#if DEBUG_MODE
    Serial.print("Setting Baudrate Successful!!!\r\n");
#endif

    if ( res == MCP2515_OK ) {

                                                                        /* init canbuffers              */
        mcp2515_initCANBuffers();

                                                                        /* interrupt mode               */
        mcp2515_setRegister(MCP_CANINTE, MCP_RX0IF | MCP_RX1IF);

        switch(canIDMode)
        {
            case (MCP_ANY):
            mcp2515_modifyRegister(MCP_RXB0CTRL,
            MCP_RXB_RX_MASK | MCP_RXB_BUKT_MASK,
            MCP_RXB_RX_ANY | MCP_RXB_BUKT_MASK);
            mcp2515_modifyRegister(MCP_RXB1CTRL, MCP_RXB_RX_MASK,
            MCP_RXB_RX_ANY);
            break;

            case (MCP_STD): 
            mcp2515_modifyRegister(MCP_RXB0CTRL,
            MCP_RXB_RX_MASK | MCP_RXB_BUKT_MASK,
            MCP_RXB_RX_STD | MCP_RXB_BUKT_MASK );
            mcp2515_modifyRegister(MCP_RXB1CTRL, MCP_RXB_RX_MASK,
            MCP_RXB_RX_STD);
            break;

            case (MCP_EXT): 
            mcp2515_modifyRegister(MCP_RXB0CTRL,
            MCP_RXB_RX_MASK | MCP_RXB_BUKT_MASK,
            MCP_RXB_RX_EXT | MCP_RXB_BUKT_MASK );
            mcp2515_modifyRegister(MCP_RXB1CTRL, MCP_RXB_RX_MASK,
            MCP_RXB_RX_EXT);
            break;

            case (MCP_STDEXT): 
            mcp2515_modifyRegister(MCP_RXB0CTRL,
            MCP_RXB_RX_MASK | MCP_RXB_BUKT_MASK,
            MCP_RXB_RX_STDEXT | MCP_RXB_BUKT_MASK );
            mcp2515_modifyRegister(MCP_RXB1CTRL, MCP_RXB_RX_MASK,
            MCP_RXB_RX_STDEXT);
            break;
    
            default:
#if DEBUG_MODE        
            Serial.print("`Setting ID Mode Failure...\r\n");
#endif           
            return MCP2515_FAIL;
            break;
}    


        res = mcp2515_setCANCTRL_Mode(mcpMode);                                                                
        if(res)
        {
#if DEBUG_MODE        
          Serial.print("Returning to Previous Mode Failure...\r\n");
#endif           
          return res;
        }

    }
    return res;

}

/*********************************************************************************************************
** Function name:           mcp2515_write_id
** Descriptions:            Write CAN ID
*********************************************************************************************************/
void MCP_CAN::mcp2515_write_id( const INT8U mcp_addr, const INT8U ext, const INT32U id )
{
    uint16_t canid;
    INT8U tbufdata[4];

    canid = (uint16_t)(id & 0x0FFFF);

    if ( ext == 1) 
    {
        tbufdata[MCP_EID0] = (INT8U) (canid & 0xFF);
        tbufdata[MCP_EID8] = (INT8U) (canid >> 8);
        canid = (uint16_t)(id >> 16);
        tbufdata[MCP_SIDL] = (INT8U) (canid & 0x03);
        tbufdata[MCP_SIDL] += (INT8U) ((canid & 0x1C) << 3);
        tbufdata[MCP_SIDL] |= MCP_TXB_EXIDE_M;
        tbufdata[MCP_SIDH] = (INT8U) (canid >> 5 );
    }
    else 
    {
        tbufdata[MCP_SIDH] = (INT8U) (canid >> 3 );
        tbufdata[MCP_SIDL] = (INT8U) ((canid & 0x07 ) << 5);
        tbufdata[MCP_EID0] = 0;
        tbufdata[MCP_EID8] = 0;
    }
    
    mcp2515_setRegisterS( mcp_addr, tbufdata, 4 );
}

/*********************************************************************************************************
** Function name:           mcp2515_write_mf
** Descriptions:            Write Masks and Filters
*********************************************************************************************************/
void MCP_CAN::mcp2515_write_mf( const INT8U mcp_addr, const INT8U ext, const INT32U id )
{
    uint16_t canid;
    INT8U tbufdata[4];

    canid = (uint16_t)(id & 0x0FFFF);

    if ( ext == 1) 
    {
        tbufdata[MCP_EID0] = (INT8U) (canid & 0xFF);
        tbufdata[MCP_EID8] = (INT8U) (canid >> 8);
        canid = (uint16_t)(id >> 16);
        tbufdata[MCP_SIDL] = (INT8U) (canid & 0x03);
        tbufdata[MCP_SIDL] += (INT8U) ((canid & 0x1C) << 3);
        tbufdata[MCP_SIDL] |= MCP_TXB_EXIDE_M;
        tbufdata[MCP_SIDH] = (INT8U) (canid >> 5 );
    }
    else 
    {
        tbufdata[MCP_EID0] = (INT8U) (canid & 0xFF);
        tbufdata[MCP_EID8] = (INT8U) (canid >> 8);
        canid = (uint16_t)(id >> 16);
        tbufdata[MCP_SIDL] = (INT8U) (canid & 0x03);
        tbufdata[MCP_SIDL] += (INT8U) ((canid & 0x07) << 5);
        tbufdata[MCP_SIDH] = (INT8U) (canid >> 3 );
    }
    
    mcp2515_setRegisterS( mcp_addr, tbufdata, 4 );
}

/*********************************************************************************************************
** Function name:           mcp2515_read_id
** Descriptions:            Read CAN ID
*********************************************************************************************************/
void MCP_CAN::mcp2515_read_id( const INT8U mcp_addr, INT8U* ext, INT32U* id )
{
    INT8U tbufdata[4];

    *ext = 0;
    *id = 0;

    mcp2515_readRegisterS( mcp_addr, tbufdata, 4 );

    *id = (tbufdata[MCP_SIDH]<<3) + (tbufdata[MCP_SIDL]>>5);

    if ( (tbufdata[MCP_SIDL] & MCP_TXB_EXIDE_M) ==  MCP_TXB_EXIDE_M ) 
    {
                                                                        /* extended id                  */
        *id = (*id<<2) + (tbufdata[MCP_SIDL] & 0x03);
        *id = (*id<<8) + tbufdata[MCP_EID8];
        *id = (*id<<8) + tbufdata[MCP_EID0];
        *ext = 1;
    }
}

/*********************************************************************************************************
** Function name:           mcp2515_write_canMsg
** Descriptions:            Write message
*********************************************************************************************************/
void MCP_CAN::mcp2515_write_canMsg( const INT8U buffer_sidh_addr)
{
    INT8U mcp_addr;
    mcp_addr = buffer_sidh_addr;
    mcp2515_setRegisterS(mcp_addr+5, m_nDta, m_nDlc );                  /* write data bytes             */
    if ( m_nRtr == 1)                                                   /* if RTR set bit in byte       */
    {
        m_nDlc |= MCP_RTR_MASK;  
    }
    mcp2515_setRegister((mcp_addr+4), m_nDlc );                         /* write the RTR and DLC        */
    mcp2515_write_id(mcp_addr, m_nExtFlg, m_nID );                      /* write CAN id                 */

}

/*********************************************************************************************************
** Function name:           mcp2515_read_canMsg
** Descriptions:            Read message
*********************************************************************************************************/
void MCP_CAN::mcp2515_read_canMsg( const INT8U buffer_sidh_addr)        /* read can msg                 */
{
    INT8U mcp_addr, ctrl;

    mcp_addr = buffer_sidh_addr;

    mcp2515_read_id( mcp_addr, &m_nExtFlg,&m_nID );

    ctrl = mcp2515_readRegister( mcp_addr-1 );
    m_nDlc = mcp2515_readRegister( mcp_addr+4 );

    if ((ctrl & 0x08)) {
        m_nRtr = 1;
    }
    else {
        m_nRtr = 0;
    }

    m_nDlc &= MCP_DLC_MASK;
    mcp2515_readRegisterS( mcp_addr+5, &(m_nDta[0]), m_nDlc );
}

/*********************************************************************************************************
** Function name:           mcp2515_getNextFreeTXBuf
** Descriptions:            Send message
*********************************************************************************************************/
INT8U MCP_CAN::mcp2515_getNextFreeTXBuf(INT8U *txbuf_n)                 /* get Next free txbuf          */
{
    INT8U res, i, ctrlval;
    INT8U ctrlregs[MCP_N_TXBUFFERS] = { MCP_TXB0CTRL, MCP_TXB1CTRL, MCP_TXB2CTRL };

    res = MCP_ALLTXBUSY;
    *txbuf_n = 0x00;

                                                                        /* check all 3 TX-Buffers       */
    for (i=0; i<MCP_N_TXBUFFERS; i++) {
        ctrlval = mcp2515_readRegister( ctrlregs[i] );
        if ( (ctrlval & MCP_TXB_TXREQ_M) == 0 ) {
            *txbuf_n = ctrlregs[i]+1;                                   /* return SIDH-address of Buffer*/
            
            res = MCP2515_OK;
            return res;                                                 /* ! function exit              */
        }
    }
    return res;
}

/*********************************************************************************************************
** Function name:           MCP_CAN
** Descriptions:            Public function to declare CAN class and the /CS pin.
*********************************************************************************************************/
MCP_CAN::MCP_CAN(INT8U _CS)
{
    MCPCS = _CS;
    pinMode(MCPCS, OUTPUT);
    MCP2515_UNSELECT();
}

/*********************************************************************************************************
** Function name:           begin
** Descriptions:            Public function to declare controller initialization parameters.
*********************************************************************************************************/
INT8U MCP_CAN::begin(INT8U idmodeset, INT8U speedset, INT8U clockset)
{
    INT8U res;

    SPI.begin();
    res = mcp2515_init(idmodeset, speedset, clockset);
    if (res == MCP2515_OK) return CAN_OK;
    else return CAN_FAILINIT;
}

/*********************************************************************************************************
** Function name:           init_Mask
** Descriptions:            Public function to set mask(s).
*********************************************************************************************************/
INT8U MCP_CAN::init_Mask(INT8U num, INT8U ext, INT32U ulData)
{
    INT8U res = MCP2515_OK;
#if DEBUG_MODE
    Serial.print("Starting to Set Mask!!!\r\n");
#endif
    res = mcp2515_setCANCTRL_Mode(MODE_CONFIG);
    if(res > 0){
#if DEBUG_MODE
    Serial.print("Entering Configuration Mode Failure...\r\n"); 
#endif
  return res;
}
    
    if (num == 0){
        mcp2515_write_mf(MCP_RXM0SIDH, ext, ulData);

    }
    else if(num == 1){
        mcp2515_write_mf(MCP_RXM1SIDH, ext, ulData);
    }
    else res =  MCP2515_FAIL;
    
    res = mcp2515_setCANCTRL_Mode(mcpMode);
    if(res > 0){
#if DEBUG_MODE
    Serial.print("Entering Previous Mode Failure...\r\nSetting Mask Failure...\r\n"); 
#endif
    return res;
  }
#if DEBUG_MODE
    Serial.print("Setting Mask Successful!!!\r\n");
#endif
    return res;
}

/*********************************************************************************************************
** Function name:           init_Filt
** Descriptions:            Public function to set filter(s).
*********************************************************************************************************/
INT8U MCP_CAN::init_Filt(INT8U num, INT8U ext, INT32U ulData)
{
    INT8U res = MCP2515_OK;
#if DEBUG_MODE
    Serial.print("Starting to Set Filter!!!\r\n");
#endif
    res = mcp2515_setCANCTRL_Mode(MODE_CONFIG);
    if(res > 0)
    {
#if DEBUG_MODE
      Serial.print("Enter Configuration Mode Failure...\r\n"); 
#endif
      return res;
    }
    
    switch( num )
    {
        case 0:
        mcp2515_write_mf(MCP_RXF0SIDH, ext, ulData);
        break;

        case 1:
        mcp2515_write_mf(MCP_RXF1SIDH, ext, ulData);
        break;

        case 2:
        mcp2515_write_mf(MCP_RXF2SIDH, ext, ulData);
        break;

        case 3:
        mcp2515_write_mf(MCP_RXF3SIDH, ext, ulData);
        break;

        case 4:
        mcp2515_write_mf(MCP_RXF4SIDH, ext, ulData);
        break;

        case 5:
        mcp2515_write_mf(MCP_RXF5SIDH, ext, ulData);
        break;

        default:
        res = MCP2515_FAIL;
    }
    
    res = mcp2515_setCANCTRL_Mode(mcpMode);
    if(res > 0)
    {
#if DEBUG_MODE
      Serial.print("Entering Previous Mode Failure...\r\nSetting Filter Failure...\r\n"); 
#endif
      return res;
    }
#if DEBUG_MODE
    Serial.print("Setting Filter Successfull!!!\r\n");
#endif
    
    return res;
}

/*********************************************************************************************************
** Function name:           setMsg
** Descriptions:            Set can message, such as dlc, id, dta[] and so on
*********************************************************************************************************/
INT8U MCP_CAN::setMsg(INT32U id, INT8U ext, INT8U len, INT8U *pData)
{
    int i = 0;
    m_nExtFlg = ext;
    m_nID     = id;
    m_nDlc    = len;
    for(i = 0; i<MAX_CHAR_IN_MESSAGE; i++)
    m_nDta[i] = *(pData+i);
    return MCP2515_OK;
}

/*********************************************************************************************************
** Function name:           clearMsg
** Descriptions:            Set all messages to zero
*********************************************************************************************************/
INT8U MCP_CAN::clearMsg()
{
    m_nID       = 0;
    m_nDlc      = 0;
    m_nExtFlg   = 0;
    m_nRtr      = 0;
    m_nfilhit   = 0;
    for(int i = 0; i<m_nDlc; i++ )
      m_nDta[i] = 0x00;

    return MCP2515_OK;
}

/*********************************************************************************************************
** Function name:           sendMsg
** Descriptions:            Send message
*********************************************************************************************************/
INT8U MCP_CAN::sendMsg()
{
    INT8U res, res1, txbuf_n;
    uint16_t uiTimeOut = 0;

    do {
        res = mcp2515_getNextFreeTXBuf(&txbuf_n);                       /* info = addr.                 */
        uiTimeOut++;
    } while (res == MCP_ALLTXBUSY && (uiTimeOut < TIMEOUTVALUE));

    if(uiTimeOut == TIMEOUTVALUE) 
    {   
        return CAN_GETTXBFTIMEOUT;                                      /* get tx buff time out         */
    }
    uiTimeOut = 0;
    mcp2515_write_canMsg( txbuf_n);
//    mcp2515_start_transmit( txbuf_n );       // Depreciated this function call, the function calls another function and returns.
    mcp2515_modifyRegister( txbuf_n-1 , MCP_TXB_TXREQ_M, MCP_TXB_TXREQ_M );
    do
    {
        uiTimeOut++;        
        res1 = mcp2515_readRegister(txbuf_n-1);                         /* read send buff ctrl reg 	*/
        res1 = res1 & 0x08;                               		
    }while(res1 && (uiTimeOut < TIMEOUTVALUE));   
    if(uiTimeOut == TIMEOUTVALUE)                                       /* send msg timeout             */	
    {
        return CAN_SENDMSGTIMEOUT;
    }
    return CAN_OK;

}

/*********************************************************************************************************
** Function name:           sendMsgBuf
** Descriptions:            Send message to transmitt buffer
*********************************************************************************************************/
INT8U MCP_CAN::sendMsgBuf(INT32U id, INT8U ext, INT8U len, INT8U *buf)
{
    setMsg(id, ext, len, buf);
    sendMsg();
}

/*********************************************************************************************************
** Function name:           readMsg
** Descriptions:            Read message
*********************************************************************************************************/
INT8U MCP_CAN::readMsg()
{
    INT8U stat, res;

    stat = mcp2515_readStatus();

    if ( stat & MCP_STAT_RX0IF )                                        /* Msg in Buffer 0              */
    {
        mcp2515_read_canMsg( MCP_RXBUF_0);
        mcp2515_modifyRegister(MCP_CANINTF, MCP_RX0IF, 0);
        res = CAN_OK;
    }
    else if ( stat & MCP_STAT_RX1IF )                                   /* Msg in Buffer 1              */
    {
        mcp2515_read_canMsg( MCP_RXBUF_1);
        mcp2515_modifyRegister(MCP_CANINTF, MCP_RX1IF, 0);
        res = CAN_OK;
    }
    else 
    {
        res = CAN_NOMSG;
    }
    return res;
}

/*********************************************************************************************************
** Function name:           readMsgBuf
** Descriptions:            Public function, Reads message from receive buffer.
*********************************************************************************************************/
INT8U MCP_CAN::readMsgBuf(INT32U *id, INT8U *len, INT8U buf[])
{
    readMsg();
    *id  = m_nID;
    *len = m_nDlc;
    for(int i = 0; i<m_nDlc; i++)
    {
      buf[i] = m_nDta[i];
    }
}

/*********************************************************************************************************
** Function name:           checkReceive
** Descriptions:            Public function, Checks for received data.  (Used if not using the interrupt output)
*********************************************************************************************************/
INT8U MCP_CAN::checkReceive(void)
{
    INT8U res;
    res = mcp2515_readStatus();                                         /* RXnIF in Bit 1 and 0         */
    if ( res & MCP_STAT_RXIF_MASK ) 
    {
        return CAN_MSGAVAIL;
    }
    else 
    {
        return CAN_NOMSG;
    }
}

/*********************************************************************************************************
** Function name:           checkError
** Descriptions:            Public function, Returns error register data.
*********************************************************************************************************/
INT8U MCP_CAN::checkError(void)
{
    INT8U eflg = mcp2515_readRegister(MCP_EFLG);

    if ( eflg & MCP_EFLG_ERRORMASK ) 
    {
        return CAN_CTRLERROR;
    }
    else 
    {
        return CAN_OK;
    }
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
