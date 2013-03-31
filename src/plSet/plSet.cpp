/************************************************************************/
/* Filename:	plSet.cpp												*/
/* Description: Source code for the function plSet						*/
/* Authors:		Jian Huang, M.A.E.Bakker, L.I.Oei 						*/
/* Date:		2009/12/20												*/
/* Updates:																*/		
/************************************************************************/
/* plSet sets the value of a given parameter. When 1 argument is given	*/
/* a list of all possible paramters is given. When 2 arguments are		*/
/* given, the possible values of the given parameters are given.		*/
/************************************************************************/
/* Input:	prhs[0] = (struct) handle for the device or					*/
/*					  (int)	   serial number of the device				*/
/*			prhs[1] = (string) name of the parameter (optional)			*/
/*			prhs[2...n] = value of the given parameter (optional)		*/
/* Output:	A list of all possible paramters (with 1 argument)			*/
/*			The possible values of the parameter (with 2 arguments)		*/
/* Syntax:	plSet(serialnumber), plSet(serialnumber, parametername), or	*/
/*			plSet(serialnumber, parametername, value)					*/
/************************************************************************/


#define WIN32_LEAN_AND_MEAN	/* Exclude rarely-used stuff from Windows headers */
#include <windows.h>
#include "C:\Program Files\MATLAB\R2006a\extern\include\mex.h"
#include "C:\Program Files\PixeLINK4.1\include\PixeLINKApi.h"

#include "..\plPrintPossibleValues.h"
#include "..\plSetValue.h"

#ifdef __cplusplus
    extern "C" {
#endif

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	mxArray *serialNumberField;
	U32		serialNumber = -1;		/* the serial number of the device */
	mxArray *lhs[1], *rhs[5];		/* to call fgdevices */
	double *px;
	int isOpen = 0;
	int buflen, i;
	char *parametername = "";
	/* prhs[0] should be the device handle (struct) or serial number (int) */
	/* prhs[1] should be the parameter name (optional) */
	/* prhs[2...n] should be the parameter value (optional) */
	if (nrhs >= 1)
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
			mexPrintf("plSet: Wrong argument given.\n");
			mexPrintf("First argument should be the device handle or the serial number.\n");
			mexErrMsgTxt("\n");
		}
		/* check whether the device has been opened */
		rhs[0] = mxCreateString("isopen");
		rhs[1] = mxCreateDoubleMatrix(1,1,mxREAL);
		px = mxGetPr(rhs[1]);
		px[0] = (U32)serialNumber;
		mexCallMATLAB(1, lhs, 2, rhs, "plDevices");
		isOpen = (int)mxGetScalar(lhs[0]);
		if (!isOpen)
		{
			mexPrintf("plSet: Device with serial number %lu has not been opened.\n", serialNumber);
			mexErrMsgTxt("\n");
		}
		/* 1 argument: give list of possible parameters */
		if (nrhs == 1)
		{
			mexPrintf("Possible parameters (if supported):\n");
			mexPrintf("CurrentFrameRate\t Pixel Format\t ExposureTime\t Gamma\t CircuitGain\t SubWindow\t Decimation\t Trigger\t GrabOutputType\n");
		}
		else /* 2 or 3 arguments */
		{
			/* get the parameter name */
			if (!mxIsChar(prhs[1]))	/* 2nd argument (parametername) should be a string */
			{
				mexPrintf("plSet: Wrong argument given.\n");
				mexPrintf("Second argument should be a string with the parametername.\n");
				mexErrMsgTxt("\n");
			}
			buflen = (mxGetM(prhs[1]) * mxGetN(prhs[1]) * sizeof(mxChar)) + 1;
			parametername = (char *)mxCalloc(buflen, sizeof(char));
			mxGetString(prhs[1], parametername, buflen);
			/* 2 arguments: give possible values for this parameter */
			if (nrhs == 2)
			{
				plPrintPossibleValues(parametername);
			}
			/* 3 or more arguments: set value for this parameter */
			else
			{
				/* copy value arguments into an mxArray */
				for (i = 2; i < nrhs; i++)
				{
					rhs[i-2] = (struct mxArray_tag *)prhs[i];
				}
				/* call plSetValue to set these values */
				plSetValue(serialNumber, parametername, nrhs-2, rhs);
			}
		}
	}
	else /* ! (nrhs >= 1) */
	{
		mexPrintf("PixeLINK Interface error.\n");
		mexPrintf("plSet: Wrong number of arguments given.\n");
		mexPrintf("One or more arguments needed:\n");
		mexPrintf("1.    handle or serial number\n");
		mexPrintf("2.    parametername (optional)\n");
		mexPrintf("3...n parametervalue (optional)\n");
		mexPrintf("Example: plSet(handle), plSet(handle, parametername), or\n");
		mexPrintf("         plSet(handle, parametername, value)\n");
		mexPrintf("     or: plSet(serialnumber), plSet(serialnumber, parametername), or\n");
		mexPrintf("         plSet(serialnumber, parametername, value)\n");
		mexErrMsgTxt("\n");
	}
}

#ifdef __cplusplus
    }	/* extern "C" */
#endif
