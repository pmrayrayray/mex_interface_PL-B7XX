/************************************************************************/
/* Filename:	plGetValue.cpp											*/
/* Description: Source code for plGetValue								*/
/* Authors:		Jian Huang, M.A.E.Bakker, L.I.Oei 						*/
/* Date:		2009/12/20												*/
/* Updates:																*/		
/************************************************************************/
/* plGetValue returns the value of a given parameter.					*/
/************************************************************************/
/* Input:	returnarray (mxArray)	mxArray for receiving the structure	*/
/*			serialNumber (U32)		serialNumber of the device			*/
/*			parametername (string)	name of the parameter				*/
/* Output:	-															*/
/* Syntax:	plGetValue(returnArray, serialNumber, parametername)		*/
/************************************************************************/

#include <windows.h>
#include "C:\Program Files\MATLAB\R2006a\extern\include\mex.h"
#include "C:\Program Files\PixeLINK4.1\include\PixeLINKApi.h"
#include "plError.h"
#include "plTypes.h"

void plGetValue(mxArray *returnArray[], U32 serialNumber, char* parametername)
{	/* m = mxArray structure to be returned as device handle */
	/* lhs, rhs used to call plDevices */
	mxArray *m, *lhs[1], *rhs[3];
	double *px;
	HANDLE deviceId;
	/* variable for receiving the error codes the pixelink api functions return */
	PXL_RETURN_CODE nRetValue;	
	char *fieldnames[6];
	mxArray *tmp, *tmp2, *tmp3, *tmp4, *tmp5, *tmp6;
	double *tmppr, *tmppr2, *tmppr3, *tmppr4, *tmppr5;
	U32		currentFlag;
	U32		numberParms;
	float	currentFrameRateValue = -1;
	float		exposureValue = -1;
	float	exposureTimeValue = -1;
	float	gammaValue = -1;
	CAMERA_INFO hardwareVersionValue;
	float	circuitGainValue = -1;
	float	region[4];
	float	decimation[2];
	float   trigger[5];
	float	pixelFormat=0;
	U32		grabColorConversion = -1;
	U32		grabOutputTypeValue = -1;

	/*          Get the deviceId										*/
	rhs[0] = mxCreateString("get");
	rhs[1] = mxCreateDoubleMatrix(1,1,mxREAL);
	px = mxGetPr(rhs[1]);
	px[0] = (U32)serialNumber;
	mexCallMATLAB(1, lhs, 2, rhs, "plDevices");
	deviceId = (HANDLE)(int)mxGetScalar(lhs[0]);

	/*          Get the value of the given parameter					*/

	if (!strcmp(parametername, "DeviceID"))
	{	/* DeviceID */
		m = mxCreateDoubleMatrix(1, 1, mxREAL);
		tmppr = mxGetPr(m);
		tmppr[0] = (int)deviceId;
	}
	else if (!strcmp(parametername, "CurrentFrameRate"))
	{	/* Current Frame Rate */
		numberParms=1;
		nRetValue = PxLGetFeature (deviceId, FEATURE_FRAME_RATE ,&currentFlag,&numberParms,&currentFrameRateValue);
		if (plError(nRetValue, "getting CurrentFrameRate value"))
		{
			mexErrMsgTxt("\n");
		}
		m = mxCreateDoubleMatrix(1, 1, mxREAL);
		tmppr = mxGetPr(m);
		tmppr[0] = currentFrameRateValue;
	}
	else if (!strcmp(parametername, "PixelFormat"))
	{	/* PixelFormat*/
		numberParms=1;
		nRetValue = PxLGetFeature (deviceId, FEATURE_PIXEL_FORMAT,&currentFlag,&numberParms,&pixelFormat);
		if (plError(nRetValue, "getting PixelFormat"))
		{
			mexErrMsgTxt("\n");
		}
		if (pixelFormat == PIXEL_FORMAT_MONO8)
		{
			m = mxCreateString("PIXEL_FORMAT_MONO8");
		}
		else if (pixelFormat == PIXEL_FORMAT_MONO16)
		{
			m = mxCreateString("PIXEL_FORMAT_MONO16");
		}
		else
		{
			m = mxCreateString("Unknown value");
		}
	}
	else if (!strcmp(parametername, "ExposureTime"))
	{	/* Exposure Time */
		numberParms=1;
		nRetValue = PxLGetFeature (deviceId, FEATURE_SHUTTER,&currentFlag,&numberParms,&exposureTimeValue);
		if (plError(nRetValue, "getting ExposureTime value"))
		{
			mexErrMsgTxt("\n");
		}
		m = mxCreateDoubleMatrix(1, 1, mxREAL);
		tmppr = mxGetPr(m);
		tmppr[0] = exposureTimeValue;
	}
	else if (!strcmp(parametername, "Gamma"))
	{	/* Gamma */
		numberParms=1;
		nRetValue = PxLGetFeature (deviceId, FEATURE_GAMMA,&currentFlag,&numberParms,&gammaValue);
		if (plError(nRetValue, "getting Gamma value"))
		{
			mexErrMsgTxt("\n");
		}
		m = mxCreateDoubleMatrix(1, 1, mxREAL);
		tmppr = mxGetPr(m);
		tmppr[0] = gammaValue;
	}
	else if (!strcmp(parametername, "HardwareVersion"))
	{	/* Hardware Version */
		nRetValue = PxLGetCameraInfo(deviceId,&hardwareVersionValue);
		if (plError(nRetValue, "getting HardwareVersion value"))
		{
			mexErrMsgTxt("\n");
		}
		tmp = mxCreateString((const char *)hardwareVersionValue.CameraName);
		tmp2 = mxCreateString((const char *)hardwareVersionValue.ModelName);
		tmp3 = mxCreateString((const char *)hardwareVersionValue.Description);
		tmp4 = mxCreateString((const char *)hardwareVersionValue.SerialNumber);
		tmp5 = mxCreateString((const char *)hardwareVersionValue.FirmwareVersion);
		tmp6 = mxCreateString((const char *)hardwareVersionValue.FPGAVersion);
		fieldnames[0] = "HardwareVersion.CameraName";
		fieldnames[1] = "HardwareVersion.ModelName";
		fieldnames[2] = "HardwareVersion.Description";
		fieldnames[3] = "HardwareVersion.SerialNumber";
		fieldnames[4] = "HardwareVersion.FirmwareVersion";
		fieldnames[5] = "HardwareVersion.FPGAVersion";
		m = mxCreateStructMatrix(1, 1, 6, (const char **)fieldnames);
		mxSetField(m, 0, "HardwareVersion.CameraName", tmp);
		mxSetField(m, 0, "HardwareVersion.ModelName", tmp2);
		mxSetField(m, 0, "HardwareVersion.Description", tmp3);
		mxSetField(m, 0, "HardwareVersion.SerialNumber", tmp4);
		mxSetField(m, 0, "HardwareVersion.FirmwareVersion", tmp5);
		mxSetField(m, 0, "HardwareVersion.FPGAVersion", tmp6);
	}
	else if (!strcmp(parametername, "CircuitGain"))
	{	/* Circuit Gain */
		numberParms=1;
		nRetValue = PxLGetFeature (deviceId, FEATURE_GAIN,&currentFlag,&numberParms,&circuitGainValue);
		if (plError(nRetValue, "getting CircuitGain value"))
		{
			mexErrMsgTxt("\n");
		}
		m = mxCreateDoubleMatrix(1, 1, mxREAL);
		tmppr = mxGetPr(m);
		tmppr[0] = (U32)circuitGainValue;
	}
	else if (!strcmp(parametername, "ROI"))
	{	/* ROI */
		numberParms=4;
		nRetValue = PxLGetFeature (deviceId, FEATURE_ROI ,&currentFlag,&numberParms,region);
		if (plError(nRetValue, "getting ROI value"))
		{
			mexErrMsgTxt("\n");
		}
		tmp2 = mxCreateDoubleMatrix(1, 1, mxREAL);
		tmppr2 = mxGetPr(tmp2);
		tmppr2[0] = (U32)region[0];
		tmp3 = mxCreateDoubleMatrix(1, 1, mxREAL);
		tmppr3 = mxGetPr(tmp3);
		tmppr3[0] = (U32)region[1];
		tmp4 = mxCreateDoubleMatrix(1, 1, mxREAL);
		tmppr4 = mxGetPr(tmp4);
		tmppr4[0] = (U32)region[2];
		tmp5 = mxCreateDoubleMatrix(1, 1, mxREAL);
		tmppr5 = mxGetPr(tmp5);
		tmppr5[0] = (U32)region[3];
		fieldnames[0] = "ROI.StartColumn";
		fieldnames[1] = "ROI.StartRow";
		fieldnames[2] = "ROI.NumberColumns";
		fieldnames[3] = "ROI.NumberRows";
		m = mxCreateStructMatrix(1, 1, 4, (const char **)fieldnames);
		mxSetField(m, 0, "ROI.StartColumn", tmp2);
		mxSetField(m, 0, "ROI.StartRow", tmp3);
		mxSetField(m, 0, "ROI.NumberColumns", tmp4);
		mxSetField(m, 0, "ROI.NumberRows", tmp5);
	}
	else if (!strcmp(parametername, "Decimation"))
	{
		/* Decimation */
		numberParms=2;
		nRetValue = PxLGetFeature (deviceId, FEATURE_DECIMATION ,&currentFlag,&numberParms,decimation);
		if (plError(nRetValue, "getting Decimation value"))
		{
		mexErrMsgTxt("\n");
		}
		tmp = mxCreateDoubleMatrix(1, 1, mxREAL);
		tmppr = mxGetPr(tmp);
		tmppr[0] = (U32)decimation[0];
		tmp2 = mxCreateDoubleMatrix(1, 1, mxREAL);
		tmppr2 = mxGetPr(tmp2);
		tmppr2[0] = (U32)decimation[1];
		fieldnames[0] = "Decimation.value";
		fieldnames[1] = "Decimation.mode";
		m = mxCreateStructMatrix(1, 1, 2, (const char **)fieldnames);
		mxSetField(m, 0, "Decimation.value", tmp);
		mxSetField(m, 0, "Decimation.mode", tmp2);
	}

	else if (!strcmp(parametername, "Trigger"))
	{
		/* Trigger */
		numberParms=5;
		nRetValue = PxLGetFeature (deviceId, FEATURE_TRIGGER ,&currentFlag,&numberParms,trigger);
		if (plError(nRetValue, "getting Trigger option"))
		{
		mexErrMsgTxt("\n");
		}
		tmp = mxCreateDoubleMatrix(1, 1, mxREAL);
		tmppr = mxGetPr(tmp);
		tmppr[0] = (U32)trigger[0];
		tmp2 = mxCreateDoubleMatrix(1, 1, mxREAL);
		tmppr2 = mxGetPr(tmp2);
		tmppr2[0] = (U32)trigger[1];
		tmp3 = mxCreateDoubleMatrix(1, 1, mxREAL);
		tmppr3 = mxGetPr(tmp3);
		tmppr3[0] = (U32)trigger[2];
		tmp4 = mxCreateDoubleMatrix(1, 1, mxREAL);
		tmppr4 = mxGetPr(tmp4);
		tmppr4[0] = (U32)trigger[3];
		tmp5 = mxCreateDoubleMatrix(1, 1, mxREAL);
		tmppr5 = mxGetPr(tmp5);
		tmppr5[0] = (U32)trigger[4];
		fieldnames[0] = "Trigger.Mode";
		fieldnames[1] = "Trigger.Source";
		fieldnames[2] = "Trigger.Polarity";
		fieldnames[3] = "Trigger.Delay";
		fieldnames[4] = "Trigger.Parameter";
		m = mxCreateStructMatrix(1, 1, 5, (const char **)fieldnames);
		mxSetField(m, 0, "Trigger.Mode", tmp);
		mxSetField(m, 0, "Trigger.Source", tmp2);
		mxSetField(m, 0, "Trigger.Polarity", tmp3);
		mxSetField(m, 0, "Trigger.Delay", tmp4);
		mxSetField(m, 0, "Trigger.Parameter", tmp5);
	}
	else if (!strcmp(parametername, "GrabOutputType"))
	{	/* GrabOutputType is kept in plDevices and used in plGrab */
		rhs[0] = mxCreateString("getpar");
		rhs[1] = mxCreateDoubleMatrix(1,1,mxREAL);
		px = mxGetPr(rhs[1]);
		px[0] = (U32)serialNumber;
		rhs[2] = mxCreateString("GrabOutputType");
		mexCallMATLAB(1, lhs, 3, rhs, "plDevices");
		grabOutputTypeValue = (U32)mxGetScalar(lhs[0]);
		switch (grabOutputTypeValue)
		{
			case (RAW):		m = mxCreateString("RAW");
							break;
			case (IMAGE):	m = mxCreateString("IMAGE");
							break;
			case (RGB24):	m = mxCreateString("RGB24");
							break;
			default:		m = mxCreateString("Unknown value");
		}
	}
	else
	{
		mexPrintf("plGetValue: Unknown parameter.\n");
		mexPrintf("possible parameters are:\n");
		mexPrintf("DeviceID\n");
		mexPrintf("CurrentFrameRate\n");
		mexPrintf("ExposureTime\n");
		mexPrintf("Gamma\n");
		mexPrintf("HardwareVersion\n");
		mexPrintf("CircuitGain\n");
		mexPrintf("ROI\n");
		mexPrintf("Decimation\n");
		mexPrintf("Trigger\n");
		mexPrintf("GrabOutputType\n");
		mexErrMsgTxt("\n");
	}
	returnArray[0] = m;
	return;
}
