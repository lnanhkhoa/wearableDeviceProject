/******************************************************************************

 @file  simple_gatt_profile.c

 @brief This file contains the Simple GATT profile sample GATT service profile
        for use with the BLE sample application.

 Group: WCS, BTS
 Target Device: CC2650, CC2640, CC1350

 ******************************************************************************
 
 Copyright (c) 2010-2016, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 Release Name: ble_sdk_2_02_00_31
 Release Date: 2016-06-16 18:57:29
 *****************************************************************************/

/*********************************************************************
 * INCLUDES
 */

#include <string.h>

#include "bcomdef.h"
#include "osal.h"
#include "linkdb.h"
#include "att.h"
#include "gatt.h"
#include "gatt_uuid.h"
#include "gattservapp.h"
#include "gapbondmgr.h"

#include "Abattservice.h"

#include "Asimple_gatt_profile.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */
// Simple GATT Profile Service UUID: 0xFFF0
CONST uint8 simpleProfileServUUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(SIMPLEPROFILE_SERV_UUID), HI_UINT16(SIMPLEPROFILE_SERV_UUID)
};

// Characteristic 1 UUID: 0xFFF1
CONST uint8 simpleProfilechar1UUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(SIMPLEPROFILE_CHAR1_UUID), HI_UINT16(SIMPLEPROFILE_CHAR1_UUID)
};

// Characteristic 2 UUID: 0xFFF2
CONST uint8 simpleProfilechar2UUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(SIMPLEPROFILE_CHAR2_UUID), HI_UINT16(SIMPLEPROFILE_CHAR2_UUID)
};

// Characteristic 3 UUID: 0xFFF3
CONST uint8 simpleProfilechar3UUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(SIMPLEPROFILE_CHAR3_UUID), HI_UINT16(SIMPLEPROFILE_CHAR3_UUID)
};

// Characteristic 4 UUID: 0xFFF4
CONST uint8 simpleProfilechar4UUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(SIMPLEPROFILE_CHAR4_UUID), HI_UINT16(SIMPLEPROFILE_CHAR4_UUID)
};

// Characteristic 5 UUID: 0xFFF5
CONST uint8 simpleProfilechar5UUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(SIMPLEPROFILE_CHAR5_UUID), HI_UINT16(SIMPLEPROFILE_CHAR5_UUID)
};

// Characteristic 6 UUID: 0xFFF6
CONST uint8 simpleProfilechar6UUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(SIMPLEPROFILE_CHAR6_UUID), HI_UINT16(SIMPLEPROFILE_CHAR6_UUID)
};

// Characteristic 7 UUID: 0xFFF7
CONST uint8 simpleProfilechar7UUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(SIMPLEPROFILE_CHAR7_UUID), HI_UINT16(SIMPLEPROFILE_CHAR7_UUID)
};
/*********************************************************************
 * EXTERNAL VARIABLES
 */
static simpleProfileCBs_t *simpleProfile_AppCBs = NULL ;

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */


/*********************************************************************
 * Profile Attributes - variables
 */

// Simple Profile Service attribute
static CONST gattAttrType_t simpleProfileService = { ATT_BT_UUID_SIZE, simpleProfileServUUID };


// Simple Profile Characteristic 1 Properties
static uint8 simpleProfileChar1Props = GATT_PROP_READ | GATT_PROP_WRITE;

// Characteristic 1 Value
uint8 simpleProfileChar1[SIMPLEPROFILE_CHAR1_LEN] = {0x01, 0x23, 0x45, 0x67, 0x89, 0x01, 0x20}; // ss, mm, hh, DD, MM, YY, CC

// Simple Profile Characteristic 1 User Description
static uint8 simpleProfileChar1UserDesp[17] = "DateTime";


// Simple Profile Characteristic 2 Properties
static uint8 simpleProfileChar2Props = GATT_PROP_READ | GATT_PROP_WRITE;

// Characteristic 2 Value
static uint8 simpleProfileChar2[SIMPLEPROFILE_CHAR2_LEN] = {0x00, 0x01, 0x02};

// Simple Profile Characteristic 2 User Description
static uint8 simpleProfileChar2UserDesp[17] = "Profiles";


// Simple Profile Characteristic 3 Properties
static uint8 simpleProfileChar3Props = GATT_PROP_READ;

// Characteristic 3 Value
uint8 simpleProfileChar3 = 0;

// Simple Profile Characteristic 3 User Description
static uint8 simpleProfileChar3UserDesp[17] = "Battery Level";


// Simple Profile Characteristic 1 Properties
static uint8 simpleProfileChar4Props = GATT_PROP_READ;

// Characteristic 1 Value
uint8 simpleProfileChar4[SIMPLEPROFILE_CHAR4_LEN] = {0x03, 0x23, 0x06, 0x00, 0x00};

// Simple Profile Characteristic 1 User Description
static uint8 simpleProfileChar4UserDesp[20] = "step.distance.calo";


// Simple Profile Characteristic 5 Properties
static uint8 simpleProfileChar5Props = GATT_PROP_READ;

// Characteristic 5 Value
uint8 simpleProfileChar5[SIMPLEPROFILE_CHAR5_LEN] = { 0x02, 0x05, 0x0F };

// Simple Profile Characteristic 5 User Description
static uint8 simpleProfileChar5UserDesp[17] = "Heart Rate Value";

// Simple Profile Characteristic 6 Properties
static uint8 simpleProfileChar6Props = GATT_PROP_READ | GATT_PROP_NOTIFY;

// Characteristic 5 Value
uint8 simpleProfileChar6[SIMPLEPROFILE_CHAR6_LEN] = {
  0x02, 0x05, 0x05, 
  0x01, 0x02, 0x03, 0x04, 0x05,
  0x01, 0x02, 0x03, 0x04, 0x05,
  0x01, 0x02, 0x03, 0x04, 0x05, 0x00, 0x00, 0x00
};

// Simple Profile Characteristic 6 Configuration Each client has its own
// instantiation of the Client Characteristic Configuration. Reads of the
// Client Characteristic Configuration only shows the configuration for
// that client and writes only affect the configuration of that client.
static gattCharCfg_t *simpleProfileChar6Config;
// Simple Profile Characteristic 6 User Description
//static uint8 simpleProfileChar6UserDesp[17] = "Heart Rate Raw";

// Simple Profile Characteristic 7 Properties
static uint8 simpleProfileChar7Props = GATT_PROP_WRITE;

// Characteristic 1 Value
uint8 simpleProfileChar7 = 0x30; // ON/OFF 0x30/0x31

// Simple Profile Characteristic 1 User Description
static uint8 simpleProfileChar7UserDesp[17] = "Heart Rate Config";



/*********************************************************************
 * Profile Attributes - Table
 */

static gattAttribute_t simpleProfileAttrTbl[] =
{
  // Simple Profile Service
  { 
    { ATT_BT_UUID_SIZE, primaryServiceUUID }, /* type */
    GATT_PERMIT_READ,                         /* permissions */
    0,                                        /* handle */
    (uint8 *)&simpleProfileService            /* pValue */
  },

    // Characteristic 1 Declaration
    { 
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ, 
      0,
      &simpleProfileChar1Props 
    },

      // Characteristic Value 1
      { 
        { ATT_BT_UUID_SIZE, simpleProfilechar1UUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE, 
        0, 
        (uint8 *) simpleProfileChar1 
      },

      // Characteristic 1 User Description
      { 
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ, 
        0, 
        simpleProfileChar1UserDesp 
      },      

    // Characteristic 2 Declaration
    { 
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ, 
      0,
      &simpleProfileChar2Props 
    },

      // Characteristic Value 2
      { 
        { ATT_BT_UUID_SIZE, simpleProfilechar2UUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE, 
        0, 
        (uint8 *) simpleProfileChar2 
      },

      // Characteristic 2 User Description
      { 
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ, 
        0, 
        simpleProfileChar2UserDesp 
      },           
      
    // Characteristic 3 Declaration
    { 
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ, 
      0,
      &simpleProfileChar3Props 
    },

      // Characteristic Value 3
      { 
        { ATT_BT_UUID_SIZE, simpleProfilechar3UUID },
        GATT_PERMIT_READ, 
        0, 
        &simpleProfileChar3 
      },

      // Characteristic 3 User Description
      { 
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ, 
        0, 
        simpleProfileChar3UserDesp 
      },

    // Characteristic 4 Declaration
    { 
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ, 
      0,
      &simpleProfileChar4Props 
    },

      // Characteristic Value 4
      { 
        { ATT_BT_UUID_SIZE, simpleProfilechar4UUID },
        GATT_PERMIT_READ, 
        0, 
        (uint8 *)simpleProfileChar4 
      },

      // Characteristic 4 User Description
      { 
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ, 
        0, 
        simpleProfileChar4UserDesp 
      },
      
    // Characteristic 5 Declaration
    { 
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ, 
      0,
      &simpleProfileChar5Props 
    },

      // Characteristic Value 5
      { 
        { ATT_BT_UUID_SIZE, simpleProfilechar5UUID },
        GATT_PERMIT_READ, 
        0, 
        (uint8 *)simpleProfileChar5 
      },

      // Characteristic 5 User Description
      { 
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ, 
        0, 
        simpleProfileChar5UserDesp 
      },

    // Characteristic 6 Declaration
    { 
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ, 
      0,
      &simpleProfileChar6Props 
    },

      // Characteristic Value 6
      { 
        { ATT_BT_UUID_SIZE, simpleProfilechar6UUID },
        GATT_PERMIT_READ, 
        0, 
        (uint8 *)simpleProfileChar6
      },

      // Characteristic 6 configuration
      { 
        { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE, 
        0, 
        (uint8 *)&simpleProfileChar6Config 
      },

    // Characteristic 7 Declaration
    { 
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ, 
      0,
      &simpleProfileChar7Props 
    },

      // Characteristic Value 7
      { 
        { ATT_BT_UUID_SIZE, simpleProfilechar7UUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE, 
        0, 
        (uint8 *) &simpleProfileChar7
      },

      // Characteristic 7 User Description
      { 
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ, 
        0, 
        simpleProfileChar7UserDesp 
      },      

};

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static bStatus_t simpleProfile_ReadAttrCB(uint16_t connHandle,
                                          gattAttribute_t *pAttr, 
                                          uint8_t *pValue, uint16_t *pLen,
                                          uint16_t offset, uint16_t maxLen,
                                          uint8_t method);
static bStatus_t simpleProfile_WriteAttrCB(uint16_t connHandle,
                                           gattAttribute_t *pAttr,
                                           uint8_t *pValue, uint16_t len,
                                           uint16_t offset, uint8_t method);

/*********************************************************************
 * PROFILE CALLBACKS
 */

// Simple Profile Service Callbacks
// Note: When an operation on a characteristic requires authorization and 
// pfnAuthorizeAttrCB is not defined for that characteristic's service, the 
// Stack will report a status of ATT_ERR_UNLIKELY to the client.  When an 
// operation on a characteristic requires authorization the Stack will call 
// pfnAuthorizeAttrCB to check a client's authorization prior to calling
// pfnReadAttrCB or pfnWriteAttrCB, so no checks for authorization need to be 
// made within these functions.
CONST gattServiceCBs_t simpleProfileCBs =
{
  simpleProfile_ReadAttrCB,  // Read callback function pointer
  simpleProfile_WriteAttrCB, // Write callback function pointer
  NULL                       // Authorization callback function pointer
};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      SimpleProfile_AddService
 *
 * @brief   Initializes the Simple Profile service by registering
 *          GATT attributes with the GATT server.
 *
 * @param   services - services to add. This is a bit map and can
 *                     contain more than one service.
 *
 * @return  Success or Failure
 */
bStatus_t SimpleProfile_AddService( uint32 services )
{
  uint8 status;

  // Allocate Client Characteristic Configuration table
//  simpleProfileChar4Config = (gattCharCfg_t *)ICall_malloc( sizeof(gattCharCfg_t) *linkDBNumConns );
 simpleProfileChar6Config = (gattCharCfg_t *)ICall_malloc( sizeof(gattCharCfg_t) *linkDBNumConns );
 if ( simpleProfileChar6Config == NULL )
 {
   return ( bleMemAllocError );
 }
  
  // Initialize Client Characteristic Configuration attributes
//  GATTServApp_InitCharCfg( INVALID_CONNHANDLE, simpleProfileChar4Config );
 GATTServApp_InitCharCfg( INVALID_CONNHANDLE, simpleProfileChar6Config );
  
  if ( services & SIMPLEPROFILE_SERVICE )
  {
    // Register GATT attribute list and CBs with GATT Server App
    status = GATTServApp_RegisterService( simpleProfileAttrTbl, 
                                          GATT_NUM_ATTRS( simpleProfileAttrTbl ),
                                          GATT_MAX_ENCRYPT_KEY_SIZE,
                                          &simpleProfileCBs );
  }
  else
  {
    status = SUCCESS;
  }

  return ( status );
}

/*********************************************************************
 * @fn      SimpleProfile_RegisterAppCBs
 *
 * @brief   Registers the application callback function. Only call 
 *          this function once.
 *
 * @param   callbacks - pointer to application callbacks.
 *
 * @return  SUCCESS or bleAlreadyInRequestedMode
 */
bStatus_t SimpleProfile_RegisterAppCBs( simpleProfileCBs_t *appCallbacks )
{
  if ( appCallbacks )
  {
    simpleProfile_AppCBs = appCallbacks;
    
    return ( SUCCESS );
  }
  else
  {
    return ( bleAlreadyInRequestedMode );
  }
}

/*********************************************************************
 * @fn      SimpleProfile_SetParameter
 *
 * @brief   Set a Simple Profile parameter.
 *
 * @param   param - Profile parameter ID
 * @param   len - length of data to write
 * @param   value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate 
 *          data type (example: data type of uint16 will be cast to 
 *          uint16 pointer).
 *
 * @return  bStatus_t
 */
bStatus_t   SimpleProfile_SetParameter( uint8 param, uint8 len, void *value )
{
  bStatus_t ret = SUCCESS;
  switch ( param )
  {
    case SIMPLEPROFILE_CHAR1:
      if ( len <= SIMPLEPROFILE_CHAR1_LEN ) 
      {
        // memset(simpleProfileChar1, 0, SIMPLEPROFILE_CHAR1_LEN+1);
        memcpy(simpleProfileChar1, value, len);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case SIMPLEPROFILE_CHAR2:
      if ( len <= SIMPLEPROFILE_CHAR2_LEN ) 
      {
        // memset(simpleProfileChar2, 0, SIMPLEPROFILE_CHAR2_LEN+1);
        memcpy(simpleProfileChar2, value, len);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case SIMPLEPROFILE_CHAR3:
      if ( len == sizeof ( uint8 ) ) 
      {
        simpleProfileChar3 = *((uint8*)value);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case SIMPLEPROFILE_CHAR4:
      if ( len <= SIMPLEPROFILE_CHAR4_LEN ) 
      {
        memcpy(simpleProfileChar4, value, len);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case SIMPLEPROFILE_CHAR5:
      if ( len <= SIMPLEPROFILE_CHAR5_LEN ) 
      {
        memcpy( simpleProfileChar5, value, len );
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case SIMPLEPROFILE_CHAR6:
      if ( len <= SIMPLEPROFILE_CHAR6_LEN ) 
      {
        memcpy( simpleProfileChar6, value, len );
        
        // See if Notification has been enabled
        GATTServApp_ProcessCharCfg( simpleProfileChar6Config, simpleProfileChar6, FALSE,
                                    simpleProfileAttrTbl, GATT_NUM_ATTRS( simpleProfileAttrTbl ),
                                    INVALID_TASK_ID, simpleProfile_ReadAttrCB );
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case SIMPLEPROFILE_CHAR7:
      if ( len == sizeof(uint8_t)) 
      {
        simpleProfileChar7 = *((uint8*) value);
        
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;
      
    default:
      ret = INVALIDPARAMETER;
      break;
  }
  
  return ( ret );
}

/*********************************************************************
 * @fn      SimpleProfile_GetParameter
 *
 * @brief   Get a Simple Profile parameter.
 *
 * @param   param - Profile parameter ID
 * @param   value - pointer to data to put.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate 
 *          data type (example: data type of uint16 will be cast to 
 *          uint16 pointer).
 *
 * @return  bStatus_t
 */
bStatus_t SimpleProfile_GetParameter( uint8 param, uint8_t *value )
{
  bStatus_t ret = SUCCESS;
  switch ( param )
  {
    case SIMPLEPROFILE_CHAR1:
      if(sizeof(value) < SIMPLEPROFILE_CHAR1_LEN)
        memcpy(value, simpleProfileChar1, sizeof(value));
      else
        memcpy(value, simpleProfileChar1, SIMPLEPROFILE_CHAR1_LEN);
      break;

    case SIMPLEPROFILE_CHAR2:
      if(sizeof(value) < SIMPLEPROFILE_CHAR2_LEN)
        memcpy(value, simpleProfileChar2, sizeof(value));
      else 
        memcpy(value, simpleProfileChar2, sizeof(simpleProfileChar2));
      break;      

    case SIMPLEPROFILE_CHAR3:
      *((uint8*)value) = simpleProfileChar3;
      break;  

    case SIMPLEPROFILE_CHAR4:
      if(sizeof(value) <= SIMPLEPROFILE_CHAR4_LEN)
        memcpy(value, simpleProfileChar4, sizeof(value));
      else 
        memcpy(value, simpleProfileChar4, sizeof(simpleProfileChar4));
      break;

    case SIMPLEPROFILE_CHAR5:
      if(sizeof(value) <= SIMPLEPROFILE_CHAR5_LEN)
        memcpy( value, simpleProfileChar5, sizeof(value));
      else 
        memcpy(value, simpleProfileChar5, SIMPLEPROFILE_CHAR5_LEN);
      break;      
      
    case SIMPLEPROFILE_CHAR6:
      if(sizeof(value) <= SIMPLEPROFILE_CHAR6_LEN)
        memcpy( value, simpleProfileChar6, sizeof(value));
      else 
        memcpy(value, simpleProfileChar6, SIMPLEPROFILE_CHAR6_LEN);
      break;      

    case SIMPLEPROFILE_CHAR7:
      *((uint8*)value) = simpleProfileChar7;
      break;

    default:
      ret = INVALIDPARAMETER;
      break;
  }
  
  return ( ret );
}

/*********************************************************************
 * @fn          simpleProfile_ReadAttrCB
 *
 * @brief       Read an attribute.
 *
 * @param       connHandle - connection message was received on
 * @param       pAttr - pointer to attribute
 * @param       pValue - pointer to data to be read
 * @param       pLen - length of data to be read
 * @param       offset - offset of the first octet to be read
 * @param       maxLen - maximum length of data to be read
 * @param       method - type of read message
 *
 * @return      SUCCESS, blePending or Failure
 */
static bStatus_t simpleProfile_ReadAttrCB(uint16_t connHandle,
                                          gattAttribute_t *pAttr,
                                          uint8_t *pValue, uint16_t *pLen,
                                          uint16_t offset, uint16_t maxLen,
                                          uint8_t method)
{
  bStatus_t status = SUCCESS;
  
  // Make sure it's not a blob operation (no attributes in the profile are long)
  if ( offset > 0 )
  {
    return ( ATT_ERR_ATTR_NOT_LONG );
  }
 
  if ( pAttr->type.len == ATT_BT_UUID_SIZE )
  {
    // 16-bit UUID
    uint16 uuid = BUILD_UINT16( pAttr->type.uuid[0], pAttr->type.uuid[1]);
    switch ( uuid )
    {
      // No need for "GATT_SERVICE_UUID" or "GATT_CLIENT_CHAR_CFG_UUID" cases;
      // gattserverapp handles those reads

      // characteristics 1 and 2 have read permissions
      // characteritisc 3 does not have read permissions; therefore it is not
      //   included here
      // characteristic 4 does not have read permissions, but because it
      //   can be sent as a notification, it is included here
      case SIMPLEPROFILE_CHAR1_UUID:
        {
          uint8 len = sizeof(simpleProfileChar1);
          // verify offset
          if (offset > len)
          {
            status = ATT_ERR_INVALID_OFFSET;
          }
          else
          {
            // determine read length (exclude null terminating character)
            *pLen = MIN(maxLen, (len - offset));
            // copy data
            memcpy(pValue, &(pAttr->pValue[offset]), *pLen);
          }
        }
        break;

      case SIMPLEPROFILE_CHAR2_UUID:
        {
          uint8 len = sizeof(simpleProfileChar2);
          // verify offset
          if (offset > len)
          {
            status = ATT_ERR_INVALID_OFFSET;
          }
          else
          {
            // determine read length (exclude null terminating character)
            *pLen = MIN(maxLen, (len - offset));
            // copy data
            memcpy(pValue, &(pAttr->pValue[offset]), *pLen);
          }
        }
        break;

      case SIMPLEPROFILE_CHAR3_UUID:
        {
          uint8 len = sizeof(simpleProfileChar3);
          // verify offset
          if (offset > len)
          {
            status = ATT_ERR_INVALID_OFFSET;
          }
          else
          {
            // determine read length (exclude null terminating character)
            *pLen = MIN(maxLen, (len - offset));
            // copy data
            memcpy(pValue, &(pAttr->pValue[offset]), *pLen);
          }
        }
        break;

      case SIMPLEPROFILE_CHAR4_UUID:
        {
          uint8 len = sizeof(simpleProfileChar4);
          // verify offset
          if (offset > len)
          {
            status = ATT_ERR_INVALID_OFFSET;
          }
          else
          {
            // determine read length (exclude null terminating character)
            *pLen = MIN(maxLen, (len - offset));
            // copy data
            memcpy(pValue, &(pAttr->pValue[offset]), *pLen);
          }
        }
        break;

      case SIMPLEPROFILE_CHAR5_UUID:
        {
          uint8 len = sizeof(simpleProfileChar5);
          // verify offset
          if (offset > len)
          {
            status = ATT_ERR_INVALID_OFFSET;
          }
          else
          {
            // determine read length (exclude null terminating character)
            *pLen = MIN(maxLen, (len - offset));
            // copy data
            memcpy(pValue, &(pAttr->pValue[offset]), *pLen);
          }
        }
        break;

      case SIMPLEPROFILE_CHAR6_UUID:
        {
          uint8 len = sizeof(simpleProfileChar6);
          // verify offset
          if (offset > len)
          {
            status = ATT_ERR_INVALID_OFFSET;
          }
          else
          {
            // determine read length (exclude null terminating character)
            *pLen = MIN(maxLen, (len - offset));
            // copy data
            memcpy(pValue, &(pAttr->pValue[offset]), *pLen);
          }
        break;
        }
        break;
        
      default:
        // Should never get here! (characteristics 3 and 4 do not have read permissions)
        *pLen = 0;
        status = ATT_ERR_ATTR_NOT_FOUND;
        break;
    }
  }
  else
  {
    // 128-bit UUID
    *pLen = 0;
    status = ATT_ERR_INVALID_HANDLE;
  }

  return ( status );
}

/*********************************************************************
 * @fn      simpleProfile_WriteAttrCB
 *
 * @brief   Validate attribute data prior to a write operation
 *
 * @param   connHandle - connection message was received on
 * @param   pAttr - pointer to attribute
 * @param   pValue - pointer to data to be written
 * @param   len - length of data
 * @param   offset - offset of the first octet to be written
 * @param   method - type of write message
 *
 * @return  SUCCESS, blePending or Failure
 */
static bStatus_t simpleProfile_WriteAttrCB(uint16_t connHandle,
                                           gattAttribute_t *pAttr,
                                           uint8_t *pValue, uint16_t len,
                                           uint16_t offset, uint8_t method)
{
  bStatus_t status = SUCCESS;
  uint8 notifyApp = 0xFF;
  
  if ( pAttr->type.len == ATT_BT_UUID_SIZE )
  {
    // 16-bit UUID
    uint16 uuid = BUILD_UINT16( pAttr->type.uuid[0], pAttr->type.uuid[1]);
    switch ( uuid )
    {
      case SIMPLEPROFILE_CHAR1_UUID:
        //Validate the value
        // Make sure it's not a blob oper
        if ( offset == 0 )
        {
          if ( len < 1 || len> sizeof(simpleProfileChar1))
          {
            status = ATT_ERR_INVALID_VALUE_SIZE;
          }
        }
        else
        {
          status = ATT_ERR_ATTR_NOT_LONG;
        }
        
        //Write the value
        if ( status == SUCCESS )
        {
          uint8 *pCurValue = (uint8 *)pAttr->pValue;        
          for(uint8 i = 0; i<len; i++){
            *(pCurValue+i) = pValue[i];
          }

          notifyApp = SIMPLEPROFILE_CHAR1;        
        }
             
        break;

      case SIMPLEPROFILE_CHAR2_UUID:
        //Validate the value
        // Make sure it's not a blob oper
        if ( offset == 0 )
        {
          if ( len < 1 || len> sizeof(simpleProfileChar2))
          {
            status = ATT_ERR_INVALID_VALUE_SIZE;
          }
        }
        else
        {
          status = ATT_ERR_ATTR_NOT_LONG;
        }
        
        //Write the value
        if ( status == SUCCESS )
        {
          uint8 *pCurValue = (uint8 *)pAttr->pValue;        
          for(uint8 i = 0; i<len; i++){
            *(pCurValue+i) = pValue[i];
          }
          notifyApp = SIMPLEPROFILE_CHAR2;        
        }
             
        break;

      case GATT_CLIENT_CHAR_CFG_UUID:
        status = GATTServApp_ProcessCCCWriteReq( connHandle, pAttr, pValue, len,
                                                 offset, GATT_CLIENT_CFG_NOTIFY );
        if (status == SUCCESS)
      {
        uint16 charCfg = BUILD_UINT16(pValue[0], pValue[1]);

        simpleProfile_AppCBs->pfnSimpleProfileChange((charCfg == GATT_CFG_NO_OPERATION) ?
                                HEARTRATE_MEAS_NOTI_DISABLED :
                                HEARTRATE_MEAS_NOTI_ENABLED);
      }
        break;

      case SIMPLEPROFILE_CHAR7_UUID:
        //Validate the value
        // Make sure it's not a blob oper
        if ( offset == 0 )
        {
          if ( len < 1 || len> sizeof(simpleProfileChar7))
          {
            status = ATT_ERR_INVALID_VALUE_SIZE;
          }
        }
        else
        {
          status = ATT_ERR_ATTR_NOT_LONG;
        }
        
        //Write the value
        if ( status == SUCCESS )
        {
          uint8 *pCurValue = (uint8 *)pAttr->pValue;        
          for(uint8 i = 0; i<len; i++){
            *(pCurValue+i) = pValue[i];
          }

          notifyApp = SIMPLEPROFILE_CHAR7;        
        }
             
        break;
        
      default:
        // Should never get here! (characteristics 2 and 4 do not have write permissions)
        status = ATT_ERR_ATTR_NOT_FOUND;
        break;
    }
  }
  else
  {
    // 128-bit UUID
    status = ATT_ERR_INVALID_HANDLE;
  }

  // If a characteristic value changed then callback function to notify application of change
  if ((notifyApp != 0xFF ) && simpleProfile_AppCBs && simpleProfile_AppCBs->pfnSimpleProfileChange)
  {
    simpleProfile_AppCBs->pfnSimpleProfileChange(notifyApp);
  }
  
  return ( status );
}

/*********************************************************************
*********************************************************************/
