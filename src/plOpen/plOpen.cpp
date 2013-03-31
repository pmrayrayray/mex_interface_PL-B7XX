/************************************************************************/
/* Filename:	plOpen.cpp												*/
/* Description: Source code for the function plOpen						*/
/* Authors:		Jian Huang, M.A.E.Bakker, L.I.Oei 						*/
/* Date:		2009/12/20												*/
/* Updates:																*/		
/************************************************************************/
/* plOpen opens a pixelink device and returns the device's handle.		*/
/************************************************************************/
/* Input:	prhs[0] = (int)	   serial number of the device to be opened	*/
/* Output:	plhs[0] = (struct) handle for the device					*/
/* Syntax:	m = plOpen(serialnumber)									*/
/************************************************************************/


#define WIN32_LEAN_AND_MEAN	/* Exclude rarely-used stuff from Windows headers */
#include <windows.h>
/* Include mex header of Matlab */
#include "C:\Program Files\MATLAB\R2006a\extern\include\mex.h"
/* Include the header of Pixelink4.1 Api */
#include "C:\Program Files\PixeLINK4.1\include\PixeLINKApi.h"

#include "..\plCreateDeviceHandle.h"
#include "..\plError.h"

#ifdef __cplusplus
    extern "C" {
#endif

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	PXL_RETURN_CODE nRetValue;		/* the return codes of the pixelink functions */
	int		isOpen = 0;
	//char	*deviceAlias = "PixeLINK(tm) 1394 Camera";			/* device alias */
	U32		numberDevices = -1;		/* the number of attached imaging devices with the specified alias */
	U32		index = 0;				/* index of the device (default = 0) */
	bool	found = false;
	HANDLE	deviceId = 0;			/* the deviceId PimMegaInitialize returns */
	U32		serialNumber = -1;		/* the serial number of the device */
	U32		*serialNumberArray = NULL;
	U32		thisSerialNumber = -1;
	int		i = 0;
	U32		availableSerialNumbers[32]; /* to store all available serial numbers */
	mxArray *lhs[1], *rhs[3];		/* to call fgdevices */
	double *px;

	/* prhs[0] should be the serial number */
	if ((nrhs==1) && mxIsDouble(prhs[0]))
	{
		/* get the serial number */
		serialNumber = (U32)mxGetScalar(prhs[0]);
		/* check whether the device has been opened before */
		rhs[0] = mxCreateString("isopen");
		rhs[1] = mxCreateDoubleMatrix(1,1,mxREAL);
		px = mxGetPr(rhs[1]);
		px[0] = (U32)serialNumber;
		mexCallMATLAB(1, lhs, 2, rhs, "plDevices");
		isOpen = (int)mxGetScalar(lhs[0]);

		/* open the device if it hasn't already been opened */
		if (!isOpen)
		{
			/* get the number of attached imaging devices */
			//nRetValue = PimMegaGetNumberDevices(deviceAlias, &numberDevices); for SDK 3.1.
			nRetValue = PxLGetNumberCameras(NULL, &numberDevices);
			serialNumberArray = (U32*)malloc( numberDevices* sizeof(U32) );
			nRetValue = PxLGetNumberCameras(serialNumberArray, &numberDevices);
			if (ApiSuccess != nRetValue)
			{
				mexPrintf("PixeLINK Interface error.\n");
				mexPrintf("plOpen: Error on checking number of available devices.\n");
				mexErrMsgTxt("\n");
			}
			if (numberDevices < (unsigned long) 1)
			{
				mexPrintf("plOpen: No devices available.\n");
				mexErrMsgTxt("\n");
			}
			/* initialize array with available serial numbers with 0 */
			for (i = 0; i < 32; i++)
				availableSerialNumbers[i] = 0;

			/* open the device with the correct serial number */
			for (index = (unsigned long) 0; (index < numberDevices) && !found; index++)
			{
				/* initialize the device */
				//nRetValue = PimMegaInitialize(deviceAlias, index, &deviceId);
				nRetValue =PxLInitialize(*(serialNumberArray+index),&deviceId);
				if (plError(nRetValue, "initializing device"))
				{
					mexErrMsgTxt("\n");
				}
				/* get the device's serial number ... */
				//nRetValue = PimMegaGetSerialNumber(deviceId, &thisSerialNumber);
				thisSerialNumber=*(serialNumberArray+index);
				if (plError(nRetValue, "getting serial number"))
				{
					mexErrMsgTxt("\n");
				}
				if (index < 32)
					availableSerialNumbers[index] = thisSerialNumber;
				/* check whether this is the device we're looking for */
				if (serialNumber == thisSerialNumber)
				{
					found = true;
					break;			/* if the correct device is found: break */
				}
				/* if this was not the correct device: uninitialize the device */
				//nRetValue = PimMegaUninitialize(&deviceId);
				nRetValue = PxLUninitialize(deviceId);
				if (plError(nRetValue, "closing the device"))
				{
					mexErrMsgTxt("\n");
				}
				//free serialNumberArray
				free(serialNumberArray);
				serialNumberArray= NULL;
			}

			if (!found)
			{
				mexPrintf("plOpen: No device found with serial number %lu.\n", serialNumber);
				mexPrintf("Available serial numbers:\n");
				for (i = 0; i < 32; i++)
				{
					if (availableSerialNumbers[i] == (unsigned long)0)
					{
						break;
					}
					mexPrintf("%lu\n", availableSerialNumbers[i]);
				}
				mexErrMsgTxt("\n");
			}
			/* update the open device array in plDevices */
			rhs[0] = mxCreateString("add");
			rhs[1] = mxCreateDoubleMatrix(1,1,mxREAL);
			px = mxGetPr(rhs[1]);
			px[0] = (U32)serialNumber;
			rhs[2] = mxCreateDoubleMatrix(1,1,mxREAL);
			px = mxGetPr(rhs[2]);
			px[0] = (U32)deviceId;
			mexCallMATLAB(1, lhs, 3, rhs, "plDevices");
		} /* end of opening the device */
		else
		{	/*device was already open, print a warning, then proceed to return the handle */
			mexPrintf("plOpen: device already open, returning device handle.\n");
		}
		/* create the device handle (return structure) */
		plCreateDeviceHandle(lhs, serialNumber);
		plhs[0] = lhs[0];
		mexPrintf("ok,plopen.\n");
	}
	else /* ! ((nrhs==1) && mxIsDouble(prhs[0])) */
	{
		mexPrintf("PixeLINK Interface error.\n");
		mexPrintf("plOpen: Wrong arguments given. One argument needed (serial number).\n");
		mexErrMsgTxt("\n");
	}
}

#ifdef __cplusplus
    }	/* extern "C" */
#endif
