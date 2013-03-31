/************************************************************************/
/* Filename:	plError.cpp												*/
/* Description:	Source code for the error checking of the pl FGI		*/
/* Authors:		Jian Huang, M.A.E.Bakker, L.I.Oei 						*/
/* Date:		2009/12/20												*/
/* Updates:																*/
/************************************************************************/
/* plError translates a PixeLINK API status code into a useful error	*/
/* message.																*/
/************************************************************************/
/* Input:	(PXL_RETURN_CODE) the API's return code that should be		*/
/*			checked														*/
/*			(string) description when the possible fault occurred		*/
/* Output:	(int)	0 if no error										*/
/*				1 on an error that should be interpreted as a warning	*/
/*				2 on an error that should be interpreted as fatal		*/
/* Syntax:	i = plError(PXL_RETURN_CODE result, char* error);			*/
/************************************************************************/

#define WIN32_LEAN_AND_MEAN	/* Exclude rarely-used stuff from Windows */
#include <windows.h>
#include "C:\Program Files\MATLAB\R2006a\extern\include\mex.h"
#include "C:\Program Files\PixeLINK4.1\include\PixeLINKApi.h"

int plError(PXL_RETURN_CODE result, char *error)
{
	char errorStr[49+33];
	strcpy(errorStr, "The device's API encountered a problem while ");
	strncat(errorStr, error, 33);
	strcat(errorStr, ":\n");
	if (result != ApiSuccess)
	{
		mexPrintf(errorStr);
	}
	/* Error codes are defined in PixeLINKApi.h*/
	switch (result)
	{
		case ApiSuccess:			return 0;
		case ApiInvalidHandleError:	mexPrintf("NULL-handle: the device was not sucessfully initialised.\n"); return 2;
		case ApiNullPointerError:	mexPrintf("NULL-pointer: problem allocating required memory.\n"); return 2;
		case ApiNoCameraAvailableError:		mexPrintf("The camera may be disconnected.\n"); return 1;
		case ApiBufferTooSmall:		mexPrintf("A buffer passed as parameter is too small.\n"); return 2;
		case ApiNoSerialNumberError:mexPrintf("The serial number could not be obtained from the camera.\n"); return 2;
		case ApiUnknownError:		mexPrintf("Error unknown to the API.\n"); return 2;
		case ApiOSVersionError:		mexPrintf("The API cannot run on the current operating system.\n");	return 2;
		case ApiHardwareError:		mexPrintf("Hardware error.\n"); return 2;
		case ApiInvalidSerialNumberError:	mexPrintf("A camera with that serial number could not be found.\n"); return 2;
		case ApiDiskFullError:		mexPrintf("Not enough disk space to complete an IO operation.\n"); return 2;
		case ApiIOError:			mexPrintf("An error occurred during an IO operation.\n"); return 2;
		case ApiStreamStopped:		mexPrintf("Application requested streaming termination.\n"); return 2;
		case ApiCreatePreviewWndError:	mexPrintf("Error creating the preview window.\n"); return 2;
		case ApiSuccessParametersChanged:		mexPrintf("Indicates that a set feature is successful but one or more parameter had to be changed (ROI) .\n"); return 2;
		case ApiOutOfRangeError:	mexPrintf("Indicates that a feature set value is out of range.\n"); return 2;
		case ApiInvalidCameraName:	mexPrintf("Indicated that the name specified is not a valid camera name.\n"); return 2;
		case ApiGetNextFrameBusy:	mexPrintf("PxLGetNextFrame cannot be called at this time because it is in use by an overlay callback function.\n"); return 2;
		default:					mexPrintf("Error unknown to the FGI.\n"); return 1;
	}
}
