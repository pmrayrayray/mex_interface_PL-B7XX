/************************************************************************/
/* Filename:	plSetValue.cpp											*/
/* Description: Source code for plSetValue								*/
/* Authors:		Jian Huang, M.A.E.Bakker, L.I.Oei 						*/
/* Date:		2009/12/20												*/
/* Updates:																*/		
/************************************************************************/
/* plSetValue sets the value of a given parameter.						*/
/************************************************************************/
/* Input:	serialNumber (U32)		serialNumber of the device			*/
/*			parametername (string)	name of the parameter				*/
/*			nvalues (int)			number of values					*/
/*			values (mxArray)		mxArray with values					*/
/* Output:	-															*/
/* Syntax:	plSetValue(serialNumber, parametername, nvalues, values)	*/
/************************************************************************/

#define WIN32_LEAN_AND_MEAN	/* Exclude rarely-used stuff from Windows headers */
#include <windows.h>
#include "C:\Program Files\MATLAB\R2006a\extern\include\mex.h"
#include "C:\Program Files\PixeLINK4.1\include\PixeLINKApi.h"
#include "plError.h"
#include "plTypes.h"
#include "plPrintPossibleValues.h"


void plSetValue(U32 serialNumber, char* parametername, int nvalues, mxArray *values[])
{
	/* m = mxArray structure to be returned as device handle */
	mxArray *lhs[1], *rhs[4];
	double *px;
	HANDLE deviceId;
	/* variable for receiving the error codes the pixelink api functions return */
	PXL_RETURN_CODE nRetValue;	
	int		buflen, i;
	float	tmp;
	char*	tmpstring;
	//float	tmpfloat;
	float	region[4];
	float	decimation[2];
	float	trigger[5];
	//BOOL	tmpBOOL;

	/*          Get the deviceId										*/
	rhs[0] = mxCreateString("get");
	rhs[1] = mxCreateDoubleMatrix(1,1,mxREAL);
	px = mxGetPr(rhs[1]);
	px[0] = (U32)serialNumber;
	mexCallMATLAB(1, lhs, 2, rhs, "plDevices");
	deviceId = (HANDLE)(int)mxGetScalar(lhs[0]);

	/*          Check whether the correct parameters are given.			*/
	/* U32 */
	if (!strcmp(parametername, "ExposureTime") ||
		!strcmp(parametername, "CircuitGain") ||
		!strcmp(parametername, "Gamma"))	
	{
		if (! ( (nvalues == 1) && mxIsDouble(values[0]) ) )
		{
			mexPrintf("plSetValue: wrong number of parameters given or wrong parametertype.\n");
			mexPrintf("%s expects 1 parameter of type U32.\n", parametername);
			mexErrMsgTxt("\n");
		}
	}
	/* string */
	else if (!strcmp(parametername, "PixelFormat") ||
		!strcmp(parametername, "GrabOutputType"))
	{
		if (! ( (nvalues == 1) && mxIsChar(values[0]) ) )
		{
			mexPrintf("plSetValue: wrong number of parameters given or wrong parametertype.\n");
			mexPrintf("%s expects 1 parameter of type string.\n", parametername);
			mexErrMsgTxt("\n");
		}
	}
	/* string, U32, U32, U32, U32 */
	else if (!strcmp(parametername, "ROI"))
	{
		if (! (nvalues == 4))
		{
			mexPrintf("plSetValue: wrong number of parameters given.\n");
			mexPrintf("%s expects 4 parameters of types U32, U32, U32, U32.\n", parametername);
			mexErrMsgTxt("\n");
		}

		for (i = 0; i < 4; i++)
		{
			if (! mxIsDouble(values[i]) )
			{
				mexPrintf("plSetValue: wrong parametertype given.\n");
				mexPrintf("%s expects 4 parameters of types U32, U32, U32, U32.\n", parametername);
				mexErrMsgTxt("\n");
			}
		}
	}
	else if (!strcmp(parametername, "Decimation"))
	{
		if (! (nvalues == 2))
		{
			mexPrintf("plSetValue: wrong number of parameters given.\n");
			mexPrintf("%s expects 2 parameters of types U32, U32.\n", parametername);
			mexErrMsgTxt("\n");
		}

		for (i = 0; i < 2; i++)
		{
			if (! mxIsDouble(values[i]) )
			{
				mexPrintf("plSetValue: wrong parametertype given.\n");
				mexPrintf("%s expects 2 parameters of types U32, U32.\n", parametername);
				mexErrMsgTxt("\n");
			}
		}
	}
	else if (!strcmp(parametername, "Trigger"))
	{
		if (! (nvalues == 5))
		{
			mexPrintf("plSetValue: wrong number of parameters given.\n");
			mexPrintf("%s expects 5 parameters of types U32, U32, U32, U32,U32.\n", parametername);
			mexErrMsgTxt("\n");
		}

		for (i = 0; i < 5; i++)
		{
			if (! mxIsDouble(values[i]) )
			{
				mexPrintf("plSetValue: wrong parametertype given.\n");
				mexPrintf("%s expects 5 parameters of types U32, U32, U32, U32,U32.\n", parametername);
				mexErrMsgTxt("\n");
			}
		}
	}

	/*          Set the value of the given parameter					*/
	/* Pixel Format */
	if (!strcmp(parametername, "PixelFormat"))
	{
		if (! ( (nvalues == 1) && mxIsChar(values[0]) ) )
		{
			mexPrintf("plSetValue: wrong number of parameters given or wrong parametertype.\n");
			mexPrintf("%s expects 1 parameter of type string.\n", parametername);
			mexErrMsgTxt("\n");
		}
		buflen = (mxGetM(values[0]) * mxGetN(values[0]) * sizeof(mxChar)) + 1;
		tmpstring = (char *)mxCalloc(buflen, sizeof(char));
		mxGetString(values[0], tmpstring, buflen);
		/* check whether this is a known value */
		if (!strcmp(tmpstring, "PIXEL_FORMAT_MONO8"))
		{
			tmp = PIXEL_FORMAT_MONO8;
		}
		else if (!strcmp(tmpstring, "PIXEL_FORMAT_MONO16"))
		{
			tmp= PIXEL_FORMAT_MONO16;
		}
		else
		{
			mexPrintf("plSetValue: Error setting DataTransferSize: unknown value.\n");
			mexPrintf("Valid values are 'PIXEL_FORMAT_MONO8' and 'PIXEL_FORMAT_MONO16'\n");
			mexErrMsgTxt("\n");
		}
		/* set the value */
		//nRetValue = PimMegaSetDataTransferSize(deviceId, tmpU32);
		nRetValue = PxLSetFeature (deviceId,FEATURE_PIXEL_FORMAT,FEATURE_FLAG_MANUAL,1,&tmp);
		if (plError(nRetValue, "setting PixelFormat"))
		{
			mexErrMsgTxt("\n");
		}
	}

	/* ExposureTime */
	else if (!strcmp(parametername, "ExposureTime"))
	{
		if (! ( (nvalues == 1) && mxIsDouble(values[0]) ) )
		{
			mexPrintf("plSetValue: wrong number of parameters given or wrong parametertype.\n");
			mexPrintf("%s expects 1 parameter of type float.\n", parametername);
			mexErrMsgTxt("\n");
		}
		tmp = (float)mxGetScalar(values[0]);
		/* check whether the given value is within the correct range */
		if (tmp> PLA780_MAX_EXPOSURETIME)
		{
			mexPrintf("plSetValue: Error setting Exposure: value too large.\n");
			mexPrintf("Exposure value should not exceed %lu.\n", PLA780_MAX_EXPOSURETIME);
			mexErrMsgTxt("\n");
		}
		/* set the value */
		//nRetValue = PimMegaSetExposureTime(deviceId, tmpfloat, tmpBOOL);
		nRetValue = PxLSetFeature (deviceId,FEATURE_SHUTTER,FEATURE_FLAG_MANUAL,1,&tmp);

		if (plError(nRetValue, "setting ExposureTime"))
		{
			mexErrMsgTxt("\n");
		}
	}

	/* Gamma */
	else if (!strcmp(parametername, "Gamma"))
	{
		tmp = (float)mxGetScalar(values[0]);
		/* check whether the given value is within the correct range */
		if (tmp> PLA780_MAX_GAMMA||tmp<PLA780_MIN_GAMMA)
		{
			mexPrintf("plSetValue: Error setting Exposure: value is not in the correct range.\n");
			//mexPrintf("Exposure value should not exceed %lu.\n", PLA780_MAX_GAMMA);
			mexErrMsgTxt("\n");
		}
		//nRetValue = PimMegaSetGamma(deviceId, tmpfloat);
		nRetValue = PxLSetFeature (deviceId,FEATURE_GAMMA,FEATURE_FLAG_MANUAL,1,&tmp);
		if (plError(nRetValue, "setting Gamma"))
		{
			mexErrMsgTxt("\n");
		}
	}

	/* CircuitGain */
	else if (!strcmp(parametername, "CircuitGain"))
	{
		tmp= (float)mxGetScalar(values[0]);

		/* check whether the given value is within the correct range */
		if (tmp> PLA780_MAX_GAIN)
		{
			mexPrintf("plSetValue: Error setting MonoGain: value too large.\n");
			mexPrintf("CircuitGain value should not exceed %lu.\n", PLA780_MAX_GAIN);
			mexErrMsgTxt("\n");
		}
		/* set the value */
		//nRetValue = PimMegaSetMonoGain(deviceId, tmpU32);
		nRetValue = PxLSetFeature (deviceId,FEATURE_GAIN,FEATURE_FLAG_MANUAL,1,&tmp);

		if (plError(nRetValue, "setting CircuitGain"))
		{
			mexErrMsgTxt("\n");
		}
	}
	/* SubWindow */
	/* ROI*/
	else if (!strcmp(parametername, "ROI"))
	{
		region[0] = (float)mxGetScalar(values[0]); /* uStartColumn */
		region[1] = (float)mxGetScalar(values[1]); /* uStartRow */
		region[2] = (float)mxGetScalar(values[2]); /* uNumberColumns */
		region[3] = (float)mxGetScalar(values[3]); /* uNumberRows */
		/* check whether the given values are within the correct range */
		if ((region[0] + region[2]) > PLA780_MAX_WIDTH)
		{
			mexPrintf("plSetValue: Error setting SubWindow: value too large.\n");
			mexPrintf("(uStartColumn + uNumberColumns) should not exceed %lu.\n", PLA780_MAX_WIDTH);
			mexErrMsgTxt("\n");
		}
		if ((region[1] + region[3]) > PLA780_MAX_HEIGHT)
		{
			mexPrintf("plSetValue: Error setting SubWindow: value too large.\n");
			mexPrintf("(uStartRow + uNumberRows) should not exceed %lu.\n", PLA780_MAX_HEIGHT);
			mexErrMsgTxt("\n");
		}
		if (region[2]< PLA780_MIN_WIDTH)
		{
			mexPrintf("plSetValue: Error setting SubWindow: value too small.\n");
			mexPrintf("uNumberColumns must be at least %lu.\n", PLA780_MIN_WIDTH);
			mexErrMsgTxt("\n");
		}
		if (region[3] < PLA780_MIN_HEIGHT)
		{
			mexPrintf("plSetValue: Error setting SubWindow: value too small.\n");
			mexPrintf("uNumberRows must be at least %lu.\n", PLA780_MIN_HEIGHT);
			mexErrMsgTxt("\n");
		}
		/* set the values */
		//nRetValue = PimMegaSetSubWindow(deviceId, tmpU32, tmpU322, tmpU323, tmpU324, tmpU325);
		mexPrintf("region[0]=%f\n",region[0]);
		mexPrintf("region[1]=%f\n",region[1]);
		mexPrintf("region[2]=%f\n",region[2]);
		mexPrintf("region[3]=%f\n",region[3]);
		nRetValue = PxLSetFeature (deviceId,FEATURE_ROI,FEATURE_FLAG_MANUAL,4,region);
		if (plError(nRetValue, "setting ROI"))
		{
			mexErrMsgTxt("\n");
		}
	}

	/* Decimation*/
	else if (!strcmp(parametername, "Decimation"))
	{
		decimation[0]= (float)mxGetScalar(values[0]); 
		decimation[1] = (float)mxGetScalar(values[1]); 
		mexPrintf("decimation[0]=%f\n",decimation[0]);
		mexPrintf("decimation[1]=%f\n",decimation[1]);				
		nRetValue = PxLSetFeature (deviceId,FEATURE_DECIMATION,FEATURE_FLAG_MANUAL,2,decimation);
		if (plError(nRetValue, "setting Decimation"))
		{
			mexErrMsgTxt("\n");
		}
	}
	/* Trigger option*/
	else if (!strcmp(parametername, "Trigger"))
	{
		trigger[0]= (float)mxGetScalar(values[0]); 
		trigger[1] = (float)mxGetScalar(values[1]);
		trigger[2] = (float)mxGetScalar(values[2]); 
		trigger[3] = (float)mxGetScalar(values[3]); 
		trigger[4] = (float)mxGetScalar(values[4]); 		
		nRetValue = PxLSetFeature (deviceId,FEATURE_TRIGGER ,FEATURE_FLAG_MANUAL,5,trigger);
		if (plError(nRetValue, "setting Trigger option"))
		{
			mexErrMsgTxt("\n");
		}
	}
}