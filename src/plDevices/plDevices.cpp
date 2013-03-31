/************************************************************************/
/* Filename:	plDevices.cpp											*/
/* Description: Source code for the function plDevices					*/
/* Authors:		Jian Huang, M.A.E.Bakker, L.I.Oei 						*/
/* Date:		2009/12/20												*/
/* Updates:																*/
/************************************************************************/
/* plDevices keeps the open device array, in which the list of open		*/
/* devices and some auxilairy parameters that are not stored in the		*/
/* device are stored. This is a list of tuples of the form:				*/
/* {serialnumber, deviceId, parameter, parameter}						*/
/* plDevices provides functionality to check and update the list.		*/
/************************************************************************/
/* Input:	prhs[0]	= (string) name of task to be performed: print,		*/
/*						remove,	isopen, get, add, getpar, setpar		*/
/*			prhs[1]	= (U32) serial number of device; Only for remove,	*/
/*						isopen, get, add, getpar, setpar				*/
/*			prhs[2]	= (int) deviceID; Only for add						*/
/*			prhs[2] = (string) name of parameter; Only for getpar,		*/
/*						 setpar											*/
/*			prhs[3]	= value of parameter; Only for setpar				*/
/* Output:	plhs[0] = (double) 1 if device open, 0 if closed; Only for	*/
/*						isopen											*/
/*			plhs[0] = (int) deviceID if open, -1 if closed; Only for	*/
/*						get												*/
/*			plhs[0] = parameter value, -1 if not found; Only for getpar	*/
/* Syntax:	not to be used directly from matlab							*/
/************************************************************************/

/* Exclude rarely-used stuff from Windows headers */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "C:\Program Files\MATLAB\R2006a\extern\include\mex.h"
#include "C:\Program Files\PixeLINK4.1\include\PixeLINKApi.h"
#include "..\plTypes.h"

#ifdef __cplusplus
    extern "C" {
#endif

/* global variables needed for storing the open device array */
//struct {U32 serialNumber; int deviceID; U32 grabColorConversion; int grabOutputType;} deviceArray[32];
struct {U32 serialNumber; int deviceID;  int grabOutputType;} deviceArray[32];
int deviceCount = 0;

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	double *y;
	int buflen, i, t;
	char *task = "", *parName = "";
	U32 serialnumber;
	/* make sure plDevices stays in memory, even when doing 'clear all' or 'clear mex'. */
	mexLock();
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	y = mxGetPr(plhs[0]);
	y[0]=1;
	if ((nrhs > 0) && mxIsChar(prhs[0]))	/* 1st argument (task) should be a string */
	{
		/* allocate memory for task string and get the 1st argument */
		buflen = (mxGetM(prhs[0]) * mxGetN(prhs[0]) * sizeof(mxChar)) + 1;
		task = (char *)mxCalloc(buflen, sizeof(char));
		mxGetString(prhs[0], task, buflen);
		if (nrhs >= 2) /* if it exists, the 2nd argument is the serial number */
		{
			if (!mxIsDouble(prhs[1]))
			{
				mexPrintf("Internal error: call to plDevices failed.\n");
				mexPrintf("2nd argument should be the serial number.\n");
				mexErrMsgTxt("\n");
			}
			/* get the 2nd argument (serial number) */
			serialnumber = (U32)mxGetScalar(prhs[1]);
		}
		switch (nrhs)
		{
		/* print has 1 argument (task) */
		case 1:		if (!strcmp(task, "print"))
					{
						for (i = 0; i < deviceCount; i++)
						{
							mexPrintf("Device nr: %d --> %d, %d: GrabOutputType = %d\n", i, deviceArray[i].serialNumber, deviceArray[i].deviceID,  deviceArray[i].grabOutputType);
						}
					}
					else
					{
						mexPrintf("Internal error: call to plDevices failed.\n");
						mexPrintf("Wrong number of arguments (1) is given or unknown task %s.", task);
						mexErrMsgTxt("\n");
					}
					break;
		/* remove, isopen, get have 2 arguments (task, serialnumber) */
		case 2:		if (!strcmp(task, "remove"))
					{
						for (i = 0; i < deviceCount; i++)
						{
							if (deviceArray[i].serialNumber == serialnumber)
							{
								for (t = i; t < deviceCount; t++)
								{
									deviceArray[t].serialNumber = deviceArray[t+1].serialNumber;
									deviceArray[t].deviceID = deviceArray[t+1].deviceID;
								}
								deviceCount--;
							}
						}
					}
					else if (!strcmp(task, "isopen"))
					{
						/* initialise return value */
						//plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
						//y = mxGetPr(plhs[0]);
						/* return 0, unless the loop finds a matching serialnumber */
						y[0] = 0;
						for (i = 0; i < deviceCount;i++)
						{
							if (deviceArray[i].serialNumber == serialnumber)
							{
								y[0] = 1; /* found matching serialnumber, return 1 */
								break;
							}
						}
					}
					else if (!strcmp(task, "get"))
					{
						/* initialise return value */
						//plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
						//y = mxGetPr(plhs[0]);
						/* return -1, unless the loop finds a matching serialnumber */
						y[0] = -1;
						for (i = 0; i < deviceCount;i++)
						{
							if (deviceArray[i].serialNumber == serialnumber)
							{
								y[0] = (double)deviceArray[i].deviceID;
								break; /* found a matching serialnumber, end search */
							}
						}
					}
					else
					{
						mexPrintf("Internal error: call to plDevices failed.\n");
						mexPrintf("Wrong number of arguments (2) is given or unknown task %s.", task);
						mexErrMsgTxt("\n");
					}
					break;
		/* add has 3 arguments (task, serialnumber, deviceid)
		 * getpar has 3 arguments (task, serialnumber, parameter name)
		 */
		case 3:		if (!strcmp(task, "add"))
					{
						if (deviceCount >= 32)
							mexErrMsgTxt("Maximum number of open devices (32) reached.\n");
						deviceArray[deviceCount].serialNumber = serialnumber;
						deviceArray[deviceCount].deviceID = (U32)mxGetScalar(prhs[2]);
						//deviceArray[deviceCount].grabColorConversion = PIXEL_FORMAT_BAYER8_GRBG;
						//modified to new version
						deviceArray[deviceCount].grabOutputType = RAW;
						deviceCount++;
					}
					else if (!strcmp(task, "getpar"))
					{	/* allocate memory for parName string and get the 3rd argument */
						buflen = (mxGetM(prhs[2]) * mxGetN(prhs[2]) * sizeof(mxChar)) + 1;
						parName = (char *)mxCalloc(buflen, sizeof(char));
						mxGetString(prhs[2], parName, buflen);
						if (!strcmp(parName, "GrabOutputType"))
						{
							t = 1;
							for (i = 0; i < deviceCount;i++)
							{
								if (deviceArray[i].serialNumber == serialnumber)
								{
									y[0] = (double)deviceArray[i].grabOutputType;
									t = 0;
									break; /* found a matching serialnumber, end search */
								}
							}
							if (t)
							{
								mexPrintf("Internal error: getpar called with unknown serial number %d", serialnumber);
								mexErrMsgTxt("\n");
							}
						}
						else
						{
							mexPrintf("getpar called with unknown parameter name %s.", parName);
							mexErrMsgTxt("\n");
						}
					}
					else
					{
						mexPrintf("Internal error: call to plDevices failed.\n");
						mexPrintf("Wrong number of arguments (3) is given or unknown task %s.", task);
						mexErrMsgTxt("\n");
					}
					break;
		/* setpar has 4 arguments */
		case 4:		if (!strcmp(task, "setpar"))
					{	/* allocate memory for parName string and get the 3rd argument */
						buflen = (mxGetM(prhs[2]) * mxGetN(prhs[2]) * sizeof(mxChar)) + 1;
						parName = (char *)mxCalloc(buflen, sizeof(char));
						mxGetString(prhs[2], parName, buflen);
						if (!strcmp(parName, "GrabOutputType"))
						{
							t = 1;
							for (i = 0; i < deviceCount;i++)
							{
								if (deviceArray[i].serialNumber == serialnumber)
								{
									deviceArray[i].grabOutputType = (U32)mxGetScalar(prhs[3]);
									t = 0;
									break; /* found a matching serialnumber, end search */
								}
							}
							if (t)
							{
								mexPrintf("Internal error: setpar called with unknown serial number %d", serialnumber);
								mexErrMsgTxt("\n");
							}
						}
						else
						{
							mexPrintf("getpar called with unknown parameter name %s.", parName);
							mexErrMsgTxt("\n");
						}
					}
					else
					{
						mexPrintf("Internal error: call to plDevices failed.\n");
						mexPrintf("Wrong number of arguments (4) is given or unknown task %s.", task);
						mexErrMsgTxt("\n");
					}
					break;
		default:	mexPrintf("Internal error: call to plDevices failed.\n");
					mexErrMsgTxt("Number of arguments out of range.\n");
		}
	}
	else
	{
		y[0]=-1;
		mexPrintf("Internal error: call to plDevices failed.\n");
		mexPrintf("First argument should be a string with the name of the task to be performed.\n");
		mexErrMsgTxt("\n");
	}
}

#ifdef __cplusplus
    }	/* extern "C" */
#endif
