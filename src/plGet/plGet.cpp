/************************************************************************/
/* Filename:	plGet.cpp												*/
/* Description: Source code for the function plGet						*/
/* Authors:		L.I.Oei, M.A.E.Bakker 									*/
/*				maarten@panic.et.tudelft.nl, l.i.oei@its.tudelft.nl		*/
/* Date:		2002/08/01												*/
/* Updates:																*/		
/************************************************************************/
/* plGet returns the device's handle if 1 argument is given, and		*/
/* returns the value of the given parameter if 2 arguments are given.	*/
/************************************************************************/
/* Input:	prhs[0] = (struct) handle for the device or					*/
/*					  (U32)	   serial number of the device				*/
/*			prhs[1] = (string) name of the parameter (optional)			*/
/* Output:	plhs[0] = (struct) handle for the device (with 1 argument)	*/
/*			plhs[0] = value of the given parameter (with 2 arguments)	*/
/* Syntax:	plGet(serialnumber) or plGet(serialnumber, parametername)	*/
/*			plGet(handle) or plGet(handle, parametername)				*/
/************************************************************************/


#define WIN32_LEAN_AND_MEAN	/* Exclude rarely-used stuff from Windows headers */
#include <windows.h>

#include "C:\Program Files\MATLAB\R2006a\extern\include\mex.h"
/* with "C:\Program Files\MATLAB\R2006a\extern\lib\win32\microsoft\libmex.lib" */
/* and  "C:\Program Files\MATLAB\R2006a\extern\lib\win32\microsoft\libmx.lib" */
#include "C:\Program Files\PixeLINK4.1\include\PixeLINKApi.h"
/* with "C:\Program Files\PixeLINK4.1\lib\PxLAPI40.lib" */

#include "..\plCreateDeviceHandle.h"
#include "..\plGetValue.h"

#ifdef __cplusplus
    extern "C" {
#endif

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	mxArray *serialNumberField;
	U32		serialNumber;		/* the serial number of the device */

	mxArray *lhs[1], *rhs[2];		/* to call fgdevices */
	double *px;
	int isOpen = 0;

	int buflen;
	char *parametername = "";

	/* prhs[0] should be the device handle (struct) or serial number (int) */
	/* prhs[1] should be the parameter name (optional) */
	if ((nrhs==1) || (nrhs ==2))
	{
		/* get the serialNumber */
		if (mxIsStruct(prhs[0]))
		{
			serialNumberField = mxGetField(prhs[0], 0, "HardwareVersion.SerialNumber");
			serialNumber = (U32)mxGetScalar(serialNumberField);
			
			
		}
		else if (mxIsDouble(prhs[0]))
		{
			serialNumber = (U32)mxGetScalar(prhs[0]);
			
		}
		else
		{
			mexPrintf("PixeLINK Interface error.\n");
			mexPrintf("plGet: Wrong argument given.\n");
			mexPrintf("First argument should be the device handle or the serial number.\n");
			mexErrMsgTxt("\n");
		}

		/* check whether the device has been opened */
		rhs[0] = mxCreateString("isopen");

		rhs[1] = mxCreateDoubleMatrix(1,1,mxREAL);
		px = mxGetPr(rhs[1]);
		px[0] = (U32)serialNumber;
		mexPrintf(" serial number %lu.\n", serialNumber);
		mexCallMATLAB(1, lhs, 2, rhs, "plDevices");
		isOpen = (int)mxGetScalar(lhs[0]);
		
		

		if (!isOpen)
		{
			mexPrintf("plGet: Device with serial number %lu has not been opened.\n", serialNumber);
			mexPrintf("Cannot get settings.\n");
			mexErrMsgTxt("\n");
		}

		if (nrhs==1) /* 1 argument: return device handle */
		{
			/* create the device handle (return structure) */
			plCreateDeviceHandle(lhs, serialNumber);
			plhs[0] = lhs[0];
		}
		else /* nrhs==2: 2 arguments: return parameter value */
		{
			
			/* get the parameter name */
			if (!mxIsChar(prhs[1]))	/* 2nd argument (parametername) should be a string */
			{
				mexPrintf("plGet: Wrong argument given.\n");
				mexPrintf("Second argument should be a string with the parametername.\n");
				mexErrMsgTxt("\n");
			}
			
			buflen = (mxGetM(prhs[1]) * mxGetN(prhs[1]) * sizeof(mxChar)) + 1;
			parametername = (char *)mxCalloc(buflen, sizeof(char));
			mxGetString(prhs[1], parametername, buflen);

			/* get the settings for this parameter */
			plGetValue(lhs, serialNumber, parametername);
			plhs[0] = lhs[0];
			
		}
	}
	else /* ! ((nrhs==1) || (nrhs==2)) */
	{
		mexPrintf("PixeLINK Interface error.\n");
		mexPrintf("plGet: Wrong number of arguments given. One or two arguments needed:\n");
		mexPrintf("handle or serial number and (optional) a parametername.\n");
		mexPrintf("Example: plGet(handle) or plGet(handle, parametername)\n");
		mexPrintf("     or: plGet(serialnumber) or plGet(serialnumber, parametername)\n");
		mexErrMsgTxt("\n");
	}
}

#ifdef __cplusplus
    }	/* extern "C" */
#endif
