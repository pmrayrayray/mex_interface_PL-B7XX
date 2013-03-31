/************************************************************************/
/* Filename:	plPrintPossibleValues.cpp								*/
/* Description: Source code for plPrintPossibleValues					*/
/* Authors:		Jian Huang, M.A.E.Bakker, L.I.Oei 						*/
/* Date:		2009/12/20												*/
/* Updates:																*/		
/************************************************************************/
/* plPrintPossibleValues: prints the possible values for 'parametername'*/
/* called when plSet gets 2 arguments									*/
/************************************************************************/
/* Input:	parametername (string) name of the parameter				*/
/* Output:	-															*/
/* Syntax:	plPrintPossibleValues(parametername)						*/
/************************************************************************/


#define WIN32_LEAN_AND_MEAN	/* Exclude rarely-used stuff from Windows headers */
#include <windows.h>
#include "C:\Program Files\MATLAB\R2006a\extern\include\mex.h"
#include "C:\Program Files\PixeLINK4.1\include\PixeLINKApi.h"
#include "plError.h"
#include "plTypes.h"


void plPrintPossibleValues(char* parametername)
{
	/* ExposureTime */
	if (!strcmp(parametername, "ExposureTime"))
	{
		mexPrintf("ExposureTime: float fExposure, string bChangeClockSpeed='FALSE'\n");
		mexPrintf("fExposure is the exposure time, in seconds (0.000063-2.0).\n");
	}
	/* Gamma */
	else if (!strcmp(parametername, "Gamma"))
	{
		mexPrintf("Gamma: float\n");
		mexPrintf("To turn of gamma correction, specify a value of 0.1 or 4.\n");
	}
	/* CircuitGain */
	else if (!strcmp(parametername, "CircuitGain"))
	{
		mexPrintf("CircuitGain: U32\n");
		mexPrintf("Value should not exceed %lu.\n", PLA780_MAX_GAIN);
	}
	/* ROI */
	else if (!strcmp(parametername, "ROI"))
	{
		mexPrintf("ROI:\n");
		mexPrintf("U32 uStartColumn\n");
		mexPrintf("U32 uStartRow\n");
		mexPrintf("U32 uNumberColumns\n");
		mexPrintf("U32 uNumberRows\n");
	}
	/* GrabOutputType */
	else if (!strcmp(parametername, "GrabOutputType"))
	{
		mexPrintf("GrabOutputType: string\n");
		mexPrintf("Valid values are:\n");
		mexPrintf("'RAW'    Image exactly as produced by the camera\n");
		mexPrintf("'IMAGE'  24 bpp colour bitmap converted to Matlab image format\n");
		mexPrintf("'RGB24'  24 bpp colour bitmap in RGB24 format\n");
	}
	else
	{
		mexPrintf("plSet: Unknown parameter name.\n");
		mexErrMsgTxt("\n");
	}
}
