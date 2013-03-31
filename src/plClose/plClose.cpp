/************************************************************************/
/* Filename:	plClose.cpp												*/
/* Description: Source code for the function plClose					*/
/* Authors:		Jian Huang, M.A.E.Bakker, L.I.Oei 						*/
/* Date:		2009/12/20												*/
/* Updates:																*/		
/************************************************************************/
/* plClose closes a pixelink device.									*/
/************************************************************************/
/* Input:	prhs[0] = (mxArray)	handle of the device					*/
/*					  (U32)		serial number of the device				*/
/* Output:	-															*/
/* Syntax:	plClose(handle)	or plClose(serialnumber)					*/
/************************************************************************/

#define WIN32_LEAN_AND_MEAN	/* Exclude rarely-used stuff from Windows headers */
#include <windows.h>

#include "C:\Program Files\MATLAB\R2006a\extern\include\mex.h"
#include "C:\Program Files\PixeLINK4.1\include\PixeLINKApi.h"
#include "..\plError.h"

#ifdef __cplusplus
    extern "C" {
#endif

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	PXL_RETURN_CODE nRetValue;		/* the return codes of the pixelink functions */
	mxArray *serialNumberField;
	U32		serialNumber = -1;		/* the serial number of the device */
	HANDLE deviceId;				/* the deviceId PimMegaInitialize returns */
	mxArray *lhs[1], *rhs[2];		/* to call fgdevices */
	double *px;
	int isOpen = 0;
	/* prhs[0] should be the device handle (struct) or serial number (int) */
	if (nrhs==1)
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
			mexPrintf("plClose: Wrong arguments given. One argument needed (device handle or serial number).\n");
			mexErrMsgTxt("\n");
		}
		/* check whether the device has been opened */
		rhs[0] = mxCreateString("isopen");
		rhs[1] = mxCreateDoubleMatrix(1,1,mxREAL);
		px = mxGetPr(rhs[1]);
		px[0] = (U32)serialNumber;
		mexCallMATLAB(1, lhs, 2, rhs, "plDevices");
		isOpen = (int)mxGetScalar(lhs[0]);
		if (isOpen)
		{
			/* get the deviceId */
			rhs[0] = mxCreateString("get");
			rhs[1] = mxCreateDoubleMatrix(1,1,mxREAL);
			px = mxGetPr(rhs[1]);
			px[0] = (U32)serialNumber;
			mexCallMATLAB(1, lhs, 2, rhs, "plDevices");
			deviceId = (HANDLE *)(int)mxGetScalar(lhs[0]);
			/* uninitialize the device */
			nRetValue = PxLUninitialize(deviceId);
			if (plError(nRetValue, "closing the device"))
			{
				mexErrMsgTxt("\n");
			}
			/* update the open device array in plDevices */
			rhs[0] = mxCreateString("remove");
			rhs[1] = mxCreateDoubleMatrix(1,1,mxREAL);
			px = mxGetPr(rhs[1]);
			px[0] = (U32)serialNumber;
			mexCallMATLAB(1, lhs, 2, rhs, "plDevices");
		}
		else /* !isOpen */
		{
			mexPrintf("plClose: cannot close device with serial number %d.\n", serialNumber);
			mexPrintf("Device has not been opened.\n");
			mexErrMsgTxt("\n");
		}
	}
	else /* ! ((nrhs==1) && mxIsStruct(prhs[0])) */
	{
		mexPrintf("PixeLINK Interface error.\n");
		mexPrintf("plClose: Wrong arguments given. One argument needed (device handle or serial number).\n");
		mexErrMsgTxt("\n");
	}
}

#ifdef __cplusplus
    }	/* extern "C" */
#endif
