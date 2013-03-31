/************************************************************************/
/* Filename:	plIsOpen.cpp											*/
/* Description: Source code for the function plIsOpen					*/
/* Authors:		Jian Huang, M.A.E.Bakker, L.I.Oei 						*/
/* Date:		2009/12/20												*/
/* Updates:																*/		
/************************************************************************/
/* plIsOpen checks whether the device has been opened. It returns 0 if	*/
/* the device has not been opened, and 1 if it has been opened. 		*/
/************************************************************************/
/* Input:	prhs[0] = (struct)	device handle, or						*/
/*					  (U32)		serial number of the device				*/
/* Output:	plhs[0] = (double)	0 if the device has not beeen opened	*/
/*								1 if the device has been opened			*/
/* Syntax:	plIsOpen(handle) or plIsOpen(serialnumber)					*/
/************************************************************************/

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>

#include "C:\Program Files\MATLAB\R2006a\extern\include\mex.h"
#include "C:\Program Files\PixeLINK4.1\include\PixeLINKApi.h"

#ifdef __cplusplus
    extern "C" {
#endif

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	mxArray *serialNumberField;
	U32		serialNumber = -1;		/* the serial number of the device */
	mxArray *rhs[2];
	double	*px;
	/* prhs[0] should be the device handle (struct) or serial number (int) */
	if (nrhs==1)
	{
		/* get the serial number */
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
			mexPrintf("plIsOpen: Wrong arguments given. One argument needed (device handle or serial number).\n");
			mexErrMsgTxt("\n");
		}
		/* check whether the device is open */
		rhs[0] = mxCreateString("isopen");
		rhs[1] = mxCreateDoubleMatrix(1,1,mxREAL);
		px = mxGetPr(rhs[1]);
		px[0] = (U32)serialNumber;
		mexCallMATLAB(1, plhs, 2, rhs, "plDevices");
	}
	else /* ! (nrhs==1) */
	{
		mexPrintf("PixeLINK Interface error.\n");
		mexPrintf("plIsOpen: Wrong arguments given. One argument needed (device handle or serial number).\n");
		mexErrMsgTxt("\n");
	}
}

#ifdef __cplusplus
    }	/* extern "C" */
#endif
