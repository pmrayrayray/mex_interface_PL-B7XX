/************************************************************************/
/* Filename:	plCreateDeviceHandle.cpp								*/
/* Description: Source code for plCreateDeviceHandle					*/
/* Authors:		Jian Huang, M.A.E.Bakker, L.I.Oei 						*/
/* Date:		2009/12/20												*/
/* Updates:																*/		
/************************************************************************/
/* plCreateDeviceHandle creates the device handle (i.e. the	structure	*/
/* with settings), which has to be returned to the matlab user.			*/
/************************************************************************/
/* Input:	returnarray (mxArray)	mxArray for receiving the structure	*/
/*			serialNumber (U32)		serial number of the device			*/
/* Output:	-															*/
/* Syntax:	plCreateDeviceHandle(returnArray, serialNumber)				*/
/************************************************************************/

#include <windows.h>
#include "C:\Program Files\MATLAB\R2006a\extern\include\mex.h"
#include "C:\Program Files\PixeLINK4.1\include\PixeLINKApi.h"
/* with "C:\Program Files\PixeLINK4.1\lib\PxLAPI40.lib" */
#include <stdlib.h>

#include ".\plTypes.h"

void plCreateDeviceHandle(mxArray *returnArray[], U32 serialNumber)
{
	/* m = mxArray structure to be returned as device handle */
	/* lhs, rhs used to call plDevices */
	mxArray *m, *lhs[1], *rhs[3];
	double *px;
	HANDLE deviceId;
	/* variable for receiving the error codes the pixelink api functions return */
	PXL_RETURN_CODE nRetValue;	
	int nof=24;	/* number of fields of the device handle structure */
	int rows=1;
	int cols=1;
	char *fieldnames[] =	/* names of the fields of the device handle structure */
	{
		"DeviceID", "CurrentFrameRate", "PixelFormat", "ExposureTime", "Gamma", "HardwareVersion.CameraName",
		"HardwareVersion.ModelName", "HardwareVersion.Description", "HardwareVersion.SerialNumber",
		"HardwareVersion.FirmwareVersion","HardwareVersion.FPGAVersion",
		"CircuitGain", "ROI.StartRow", "ROI.StartColumn", "ROI.NumberRows",	"ROI.NumberColumns",
		"Decimation.value",	"Decimation.mode", "Trigger.Mode", "Trigger.Source", "Trigger.Polarity",
		"Trigger.Delay", "Trigger.Parameter",
		//"GrabColorConversion",
		"GrabOutputType"
	};
	mxArray *tmp, *tmp2, *tmp3, *tmp4, *tmp5,*tmp6;
	double *tmppr, *tmppr2, *tmppr3, *tmppr4, *tmppr5;

	/* variables used for receiving the settings */
	U32		currentFlag;
	U32		numberParms;
	float	currentFrameRateValue;
	float		exposureValue = -1;
	float	exposureTimeValue = -1;
	float	gammaValue = -1;
	CAMERA_INFO hardwareVersionValue;
	float	circuitGainValue = -1;
	float	region[4];
	float	decimation[2];
	float   trigger[5];
	float	pixelFormat=0;
	U32		grabOutputTypeValue = -1;
	mexPrintf("start plcreatdevice.\n");

	/* Get the deviceId */
	rhs[0] = mxCreateString("get");
	rhs[1] = mxCreateDoubleMatrix(1,1,mxREAL);
	px = mxGetPr(rhs[1]);
	px[0] = (U32)serialNumber;
	mexCallMATLAB(1, lhs, 2, rhs, "plDevices");
	deviceId = (HANDLE)(U32)mxGetScalar(lhs[0]);
	mexPrintf("ok,plcreatdevice1.%lu\n",deviceId);

	/* Get all settings and store them in the mxArray */
	m = mxCreateStructMatrix(rows, cols, nof, (const char **)fieldnames);
	mexPrintf("ok, plcreatdevice2.\n");

	/* DeviceID */
	tmp = mxCreateDoubleMatrix(1, 1, mxREAL);
	tmppr = mxGetPr(tmp);
	tmppr[0] = (U32)deviceId;
	mxSetField(m, 0, "DeviceID", tmp);
	mexPrintf("ok, plcreatdevice3.\n");

	/* Current Frame Rate */
	//nRetValue = PimMegaGetCurrentFrameRate(deviceId, &currentFrameRateValue);
	numberParms=1;
	nRetValue = PxLGetFeature (deviceId, FEATURE_FRAME_RATE ,&currentFlag, &numberParms, &currentFrameRateValue);
	mexPrintf("ok, plcreatdevice4 %d.\n",nRetValue);
	if (ApiSuccess == nRetValue)
	{
		tmp = mxCreateDoubleMatrix(1, 1, mxREAL);
		tmppr = mxGetPr(tmp);
		tmppr[0] = currentFrameRateValue;
		
	}
	else if (ApiNotSupportedError == nRetValue)
	{
		tmp = mxCreateString("Unsupported");
	}
	else
	{
		tmp = mxCreateString("Could not get value");
	}
	mxSetField(m, 0, "CurrentFrameRate", tmp);

	/* PixelFormat*/
	//nRetValue = PimMegaGetDataTransferSize(deviceId, &dataTransferSizeValue);
	numberParms=1;
	nRetValue = PxLGetFeature (deviceId, FEATURE_PIXEL_FORMAT, &currentFlag, &numberParms, &pixelFormat);
	mexPrintf("ok, plcreatdevice5.\n");
	if (ApiSuccess == nRetValue)
	{
		if (pixelFormat == PIXEL_FORMAT_MONO8)
		{
			tmp = mxCreateString("PIXEL_FORMAT_MONO8");
		}
		else if (pixelFormat == PIXEL_FORMAT_MONO16)
		{
			tmp = mxCreateString("PIXEL_FORMAT_MONO16");
		}
		else
		{
			tmp = mxCreateString("Unknown value");
		}
	}
	else if (ApiNotSupportedError == nRetValue)
	{
		tmp = mxCreateString("Unsupported");
	}
	else
	{
		tmp = mxCreateString("Could not get value");
	}
	mxSetField(m, 0, "PixelFormat", tmp);

	/* Exposure Time */
	//nRetValue = PimMegaGetExposureTime(deviceId, &exposureTimeValue);
	numberParms=1;
	nRetValue = PxLGetFeature (deviceId, FEATURE_SHUTTER, &currentFlag, &numberParms, &exposureTimeValue);

	if (ApiSuccess == nRetValue)
	{
		tmp = mxCreateDoubleMatrix(1, 1, mxREAL);
		tmppr = mxGetPr(tmp);
		tmppr[0] = exposureTimeValue;
	}
	else if (ApiNotSupportedError == nRetValue)
	{
		tmp = mxCreateString("Unsupported");
	}
	else
	{
		tmp = mxCreateString("Could not get value");
	}
	mxSetField(m, 0, "ExposureTime", tmp);

	/* Gamma */
	//nRetValue = PimMegaGetGamma(deviceId, &gammaValue);
	numberParms=1;
	nRetValue = PxLGetFeature (deviceId, FEATURE_GAMMA, &currentFlag, &numberParms, &gammaValue);
	if (ApiSuccess == nRetValue)
	{
		tmp = mxCreateDoubleMatrix(1, 1, mxREAL);
		tmppr = mxGetPr(tmp);
		tmppr[0] = gammaValue;
	}
	else if (ApiNotSupportedError == nRetValue)
	{
		tmp = mxCreateString("Unsupported");
	}
	else
	{
		tmp = mxCreateString("Could not get value");
	}
	mxSetField(m, 0, "Gamma", tmp);

	/* Hardware Version */
	//nRetValue = PimMegaGetHardwareVersion(deviceId, &hardwareVersionValue);
	nRetValue = PxLGetCameraInfo(deviceId, &hardwareVersionValue);
	if (ApiSuccess == nRetValue)
	{
		tmp = mxCreateString((const char *)hardwareVersionValue.CameraName);
		tmp2 = mxCreateString((const char *)hardwareVersionValue.ModelName);
		tmp3 = mxCreateString((const char *)hardwareVersionValue.Description);
		tmp4 = mxCreateDoubleMatrix(1, 1, mxREAL);
		tmppr4 = mxGetPr(tmp4);
		tmppr4[0] =strtoul(hardwareVersionValue.SerialNumber,NULL,10);
		//tmp4 = mxCreateString((const char *)hardwareVersionValue.SerialNumber);
		tmp5 = mxCreateString((const char *)hardwareVersionValue.FirmwareVersion);
		tmp6 = mxCreateString((const char *)hardwareVersionValue.FPGAVersion);
	}
	else if (ApiNotSupportedError == nRetValue)
	{
		tmp = mxCreateString("Unsupported");
		tmp2 = mxCreateString("Unsupported");
		tmp3 = mxCreateString("Unsupported");
		tmp4 = mxCreateString("Unsupported");
		tmp5 = mxCreateString("Unsupported");
		tmp6 = mxCreateString("Unsupported");
	}
	else
	{
		tmp = mxCreateString("Could not get value");
		tmp2 = mxCreateString("Could not get value");
		tmp3 = mxCreateString("Could not get value");
		tmp4 = mxCreateString("Could not get value");
		tmp5 = mxCreateString("Could not get value");
		tmp6 = mxCreateString("Could not get value");
	}
	mxSetField(m, 0, "HardwareVersion.CameraName", tmp);
	mxSetField(m, 0, "HardwareVersion.ModelName", tmp2);
	mxSetField(m, 0, "HardwareVersion.Description", tmp3);
	mxSetField(m, 0, "HardwareVersion.SerialNumber", tmp4);
	mxSetField(m, 0, "HardwareVersion.FirmwareVersion", tmp5);
	mxSetField(m, 0, "HardwareVersion.FPGAVersion", tmp6);

	/* Circuit Gain */
	//nRetValue = PimMegaGetMonoGain(deviceId, &monoGainValue);
	numberParms=1;
	nRetValue = PxLGetFeature (deviceId, FEATURE_GAIN, &currentFlag, &numberParms, &circuitGainValue);
	if (ApiSuccess == nRetValue)
	{
		tmp = mxCreateDoubleMatrix(1, 1, mxREAL);
		tmppr = mxGetPr(tmp);
		tmppr[0] = (U32)circuitGainValue;
	}
	else if (ApiNotSupportedError == nRetValue)
	{
		tmp = mxCreateString("Unsupported");
	}
	else
	{
		tmp = mxCreateString("Could not get value");
	}
	mxSetField(m, 0, "CircuitGain", tmp);

	/* ROI */
	//nRetValue = PimMegaGetSubWindow(deviceId, &subWindowDecimationValue, &subWindowStartColumnValue, &subWindowStartRowValue, &subWindowNumberColumnsValue, &subWindowNumberRowsValue);
	numberParms=4;
	nRetValue = PxLGetFeature (deviceId, FEATURE_ROI ,&currentFlag, &numberParms, region);
	if (ApiSuccess == nRetValue)
	{
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
		
	}
	else if (ApiNotSupportedError == nRetValue)
	{
		//tmp = mxCreateString("Unsupported");
		tmp2 = mxCreateString("Unsupported");
		tmp3 = mxCreateString("Unsupported");
		tmp4 = mxCreateString("Unsupported");
		tmp5 = mxCreateString("Unsupported");
	}
	else
	{
		//tmp = mxCreateString("Could not get value");
		tmp2 = mxCreateString("Could not get value");
		tmp3 = mxCreateString("Could not get value");
		tmp4 = mxCreateString("Could not get value");
		tmp5 = mxCreateString("Could not get value");
	}
	//mxSetField(m, 0, "ROI.Decimation", tmp);
	mxSetField(m, 0, "ROI.StartColumn", tmp2);
	mxSetField(m, 0, "ROI.StartRow", tmp3);
	mxSetField(m, 0, "ROI.NumberColumns", tmp4);
	mxSetField(m, 0, "ROI.NumberRows", tmp5);

	/* Decimation */
	numberParms=2;
	nRetValue = PxLGetFeature (deviceId, FEATURE_DECIMATION ,&currentFlag,&numberParms,decimation);
		if (ApiSuccess == nRetValue)
	{
		tmp = mxCreateDoubleMatrix(1, 1, mxREAL);
		tmppr = mxGetPr(tmp);
		tmppr[0] = (U32)decimation[0];
		tmp2 = mxCreateDoubleMatrix(1, 1, mxREAL);
		tmppr2 = mxGetPr(tmp2);
		tmppr2[0] = (U32)decimation[1];

	}
	mxSetField(m, 0, "Decimation.value", tmp);
	mxSetField(m, 0, "Decimation.mode", tmp2);

	/* Trigger mode and delay */
	numberParms=5;
	nRetValue = PxLGetFeature (deviceId, FEATURE_TRIGGER ,&currentFlag,&numberParms,trigger);
		if (ApiSuccess == nRetValue)
	{
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
	}
	else
	{
		tmp = mxCreateString("Could not get value");
		tmp2 = mxCreateString("Could not get value");
		tmp3 = mxCreateString("Could not get value");
		tmp4 = mxCreateString("Could not get value");
		tmp5 = mxCreateString("Could not get value");
	}
	mxSetField(m, 0, "Trigger.Mode", tmp);
	mxSetField(m, 0, "Trigger.Source", tmp2);
	mxSetField(m, 0, "Trigger.Polarity", tmp3);
	mxSetField(m, 0, "Trigger.Delay", tmp4);
	mxSetField(m, 0, "Trigger.Parameter", tmp5);

	//GrabOutputType 
	rhs[0] = mxCreateString("getpar");
	rhs[1] = mxCreateDoubleMatrix(1,1,mxREAL);
	px = mxGetPr(rhs[1]);
	px[0] = (U32)serialNumber;
	rhs[2] = mxCreateString("GrabOutputType");
	mexCallMATLAB(1, lhs, 3, rhs, "plDevices");
	grabOutputTypeValue = (U32)mxGetScalar(lhs[0]);

	switch (grabOutputTypeValue)
	{
		case RAW:		tmp = mxCreateString("RAW");
										break;
		case IMAGE:		tmp = mxCreateString("IMAGE");
										break;
		case RGB24:		tmp = mxCreateString("RGB24");
										break;
		default:		tmp = mxCreateString("Unknown value");
										break;
	}
	mxSetField(m, 0, "GrabOutputType", tmp);

	/* return the device handle array */
	returnArray[0] = m;
	return;
}
