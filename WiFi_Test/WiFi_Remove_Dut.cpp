#include "stdafx.h"
#include "TestManager.h"
#include "WiFi_Test.h"
#include "WiFi_Test_Internal.h"
#include "IQmeasure.h"
#include "vDUT.h"

using namespace std;

// Input Parameter Container
map<string, WIFI_SETTING_STRUCT> l_removeDutParamMap;

// Return Value Container 
map<string, WIFI_SETTING_STRUCT> l_removeDutReturnMap;

struct tagReturn
{
    char ERROR_MESSAGE[MAX_BUFFER_SIZE];
} l_removeDutReturn;

void ClearRemoveDutReturn(void)
{
	l_removeDutParamMap.clear();
	l_removeDutReturnMap.clear();
}

#ifndef WIN32 
int initRemoveDutContainers = InitializeRemoveDutContainers();
#endif

//! Remove WiFi DUT
/*!
 * Input Parameters
 *
 *  - None
 *
 * Return Values
 *      -# A string for possible error message
 *
 * \return 0 No error occurred
 * \return -1 DUT failed to insert.  Please see the returned error message for details
 */
WIFI_TEST_API int WiFi_Remove_Dut(void)
{
    int  err = ERR_OK;
    int  dummyValue = 0;
	char vErrorMsg[MAX_BUFFER_SIZE]  = {'\0'};
	char logMessage[MAX_BUFFER_SIZE] = {'\0'};

    /*---------------------------------------*
     * Clear Return Parameters and Container *
     *---------------------------------------*/
	ClearReturnParameters(l_removeDutReturnMap);

    /*------------------------*
     * Respond to QUERY_INPUT *
     *------------------------*/
    err = TM_GetIntegerParameter(g_WiFi_Test_ID, "QUERY_INPUT", &dummyValue);
    if( ERR_OK==err )
    {
        RespondToQueryInput(l_removeDutParamMap);
        return err;
    }
	else
	{
		// do nothing
	}

    /*-------------------------*
     * Respond to QUERY_RETURN *
     *-------------------------*/
    err = TM_GetIntegerParameter(g_WiFi_Test_ID, "QUERY_RETURN", &dummyValue);
    if( ERR_OK==err )
    {
        RespondToQueryReturn(l_removeDutReturnMap);
        return err;
    }
	else
	{
		// do nothing
	}

	try
	{
	   /*-----------------------------------------------------------*
		* Both g_WiFi_Test_ID and g_WiFi_Dut need to be valid (>=0) *
		*-----------------------------------------------------------*/
		if( g_WiFi_Test_ID<0 || g_WiFi_Dut<0 )  
		{
			err = -1;
			LogReturnMessage(logMessage, MAX_BUFFER_SIZE, LOGGER_ERROR, "[WiFi] WiFi_Test_ID or WiFi_Dut not valid. WiFi_Test_ID = %d and WiFi_Dut = %d.\n", g_WiFi_Test_ID, g_WiFi_Dut);
			throw logMessage;
		}
		else
		{
			LogReturnMessage(logMessage, MAX_BUFFER_SIZE, LOGGER_INFORMATION, "[WiFi] WiFi_Test_ID = %d and WiFi_Dut = %d.\n", g_WiFi_Test_ID, g_WiFi_Dut);
		}
		
		TM_ClearReturns(g_WiFi_Test_ID);
        
        // Error return of this function is irrelevant
        CheckDutTransmitStatus();
 
        // REMOVE_DUT
		err = ::vDUT_Run(g_WiFi_Dut, "REMOVE_DUT");		
		if ( ERR_OK!=err )
		{	// Check vDut return "ERROR_MESSAGE" or not, if "Yes", must handle it.
			err = ::vDUT_GetStringReturn(g_WiFi_Dut, "ERROR_MESSAGE", vErrorMsg, MAX_BUFFER_SIZE);
			if ( ERR_OK==err )	// Get "ERROR_MESSAGE" from vDut
			{
				err = -1;	// set err to -1, means "Error".
				LogReturnMessage(logMessage, MAX_BUFFER_SIZE, LOGGER_ERROR, vErrorMsg);
				throw logMessage;
			}
			else	// Just return normal error message in this case
			{
				LogReturnMessage(logMessage, MAX_BUFFER_SIZE, LOGGER_ERROR, "[WiFi] vDUT_Run(REMOVE_DUT) return error.\n");
				throw logMessage;
			}
		}
		else
		{
			LogReturnMessage(logMessage, MAX_BUFFER_SIZE, LOGGER_INFORMATION, "[WiFi] vDUT_Run(REMOVE_DUT) return OK.\n");
		}
	    
		// If specified to reload the DUT control DLL every time
		if( 0!=g_ReloadDutDll && -1!=g_WiFi_Dut )
		{
			/*****************************************************************************
			 * NOTE: vDUT_UnregisterDutDll() will unload the DUT control DLL from memory *
			 *****************************************************************************/
			err = ::vDUT_UnregisterDutDll("WIFI", &g_WiFi_Dut);
			if ( ERR_OK!=err )
			{
				LogReturnMessage(logMessage, MAX_BUFFER_SIZE, LOGGER_ERROR, "[WiFi] Unload the DUT control DLL from memory vDUT_UnregisterDutDll() return error.\n");
				throw logMessage;
			}
			else
			{
				LogReturnMessage(logMessage, MAX_BUFFER_SIZE, LOGGER_INFORMATION, "[WiFi] Unload the DUT control DLL from memory vDUT_UnregisterDutDll() return OK.\n");
			}
		}
		else
		{
			// do nothing
		}

		/*-----------------------*
		 *  Return Test Results  *
		 *-----------------------*/
		if (ERR_OK==err)
		{
			sprintf_s(l_removeDutReturn.ERROR_MESSAGE, MAX_BUFFER_SIZE, "[Info] Function completed.\n");
			ReturnTestResults(l_removeDutReturnMap);
		}
	}
	catch(char *msg)
    {
        ReturnErrorMessage(l_removeDutReturn.ERROR_MESSAGE, msg);
    }
    catch(...)
    {
		ReturnErrorMessage(l_removeDutReturn.ERROR_MESSAGE, "[WiFi] Unknown Error!\n");
		err = -1;
    }

    return err;
}

int InitializeRemoveDutContainers(void)
{
    /*------------------*
     * Input Parameters: *
     * IQTESTER_IP01    *
     *------------------*/
    l_removeDutParamMap.clear();

    WIFI_SETTING_STRUCT setting;
    setting.unit = "";
    setting.helpText = "";


    /*----------------*
     * Return Values: *
     * ERROR_MESSAGE  *
     *----------------*/
    l_removeDutReturnMap.clear();

    l_removeDutReturn.ERROR_MESSAGE[0] = '\0';
    setting.type = WIFI_SETTING_TYPE_STRING;
    if (MAX_BUFFER_SIZE==sizeof(l_removeDutReturn.ERROR_MESSAGE))    // Type_Checking
    {
        setting.value       = (void*)l_removeDutReturn.ERROR_MESSAGE;
        setting.unit        = "";
        setting.helpText    = "Error message occurred";
        l_removeDutReturnMap.insert( pair<string,WIFI_SETTING_STRUCT>("ERROR_MESSAGE", setting) );
    }
    else    
    {
        printf("Parameter Type Error!\n");
        exit(1);
    }

    return 0;
}
