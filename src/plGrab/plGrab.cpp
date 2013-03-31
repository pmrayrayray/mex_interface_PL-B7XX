/************************************************************************/
/* Filename:	plGrab.cpp												*/
/* Description: Source code for the function plGrab						*/
/* Authors:		M.A.E.Bakker, L.I.Oei 									*/
/*				maarten@panic.et.tudelft.nl, l.i.oei@its.tudelft.nl		*/
/* Date:		2002/08/08												*/
/* Updates:																*/		
/************************************************************************/
/* plGrab grabs a frame from the PixeLINK device and places it into a	*/
/* mxArray.																*/
/************************************************************************/
/* Input:	prhs[0] = 	(U32) serialnumber for the device				*/
/*						(struct) handle for the device					*/
/* Output:	plhs[0] = 	(mxArray) grabbed image							*/
/* Syntax:	imagematrix = plGrab(serialnumber)							*/
/*			imagematrix = plGrab(handle)								*/
/************************************************************************/
/*RGB to BGR switch, Smutny 6.9.2002*/
#define WIN32_LEAN_AND_MEAN	/* Exclude rarely-used stuff from Windows */
#include <windows.h>


#include "C:\Program Files\MATLAB\R2006a\extern\include\mex.h"
/* with "C:\Program Files\MATLAB\R2006a\extern\lib\win32\microsoft\libmex.lib" */
/* and  "C:\Program Files\MATLAB\R2006a\extern\lib\win32\microsoft\libmx.lib" */
#include "C:\Program Files\PixeLINK4.1\include\PixeLINKApi.h"
/* with "C:\Program Files\PixeLINK4.1\lib\PxLAPI40.lib" */

#include "..\plError.h"	/* the pixelink error checking subroutine */
#include "..\plTypes.h"

#ifdef __cplusplus
    extern "C" {
#endif

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	PXL_RETURN_CODE nRetValue;		/* the return codes of the pixelink functions */

	mxArray *serialNumberField;
	U32		serialNumber;		/* the serial number of the device */

	mxArray *lhs[1], *rhs[3];		/* to call fgdevices */
	double *px;
	int isOpen = 0;

	HANDLE deviceId;				/* the deviceId PimMegaInitialize returns */

	//U32 currentVideoMode;			/* to get the current video mode; if currentVideoMode != VIDEO_MODE, set VideoMode to 'VIDEO_MODE' */

	/* image properties */
	U32		currentFlag;
	U32		numberParms;

	float pixelFormat;	
	float region[4];
	float decimation[2];

	FRAME_DESC	Descriptor;
	//U32 dataTransferSize;			/* DataTransferSize: DATA_8BIT_SIZE or DATA_16BIT_SIZE */
	//U32 decimation, width, height;

	
	int grabOutputType;					/* what sort of data should be returned to Matlab */

	int *size;						/* array used to build the Matlab array (with mxCreateNumericArray) */
	int dim;						/* number of dimensions of the Matlab array */

	U32 rawImgSize, rawImgByteSize, RGB24ImgSize;

	void *imgOutPtr = NULL;			/* pointer to the mxArray in which the image should be stored (matlab array) */
	void *imgCapturePtr = NULL;		/* pointer to the capture array for conversion from Bayer or I to RGB24 */
	void *img24BppPtr = NULL;		/* pointer to capture array for conversion from RGB24 to MatLAB image */

	U32 pixelWidth, pixelHeight;
	U32 i, j;

	if ((nrhs == 1) && (nlhs == 1))
	{

/********************************************************************/
/*      Get the serialNumber and deviceId							*/
/********************************************************************/

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
			mexPrintf("plGrab: Wrong arguments given. First argument should be the device handle or serial number.\n");
			mexErrMsgTxt("\n");
		}

		/* get the deviceId */
		rhs[0] = mxCreateString("get");

		rhs[1] = mxCreateDoubleMatrix(1,1,mxREAL);
		px = mxGetPr(rhs[1]);
		px[0] = (U32)serialNumber;

		mexCallMATLAB(1, lhs, 2, rhs, "plDevices");
		deviceId = (HANDLE *)(int)mxGetScalar(lhs[0]);

/********************************************************************/
/*      Open the video stream and set VideoMode	to 'VIDEO_MODE'		*/
/********************************************************************/

		/* open video stream first to do acquisition */
		//nRetValue = PimMegaStartVideoStream(deviceId);
		nRetValue = PxLSetStreamState(deviceId, START_STREAM);
		if (plError(nRetValue, "calling PxLSetStreamState"))
		{
			mexErrMsgTxt("\n");
		}


/********************************************************************/
/*      Get some camera settings									*/
/********************************************************************/

		/* get image properties */
		//nRetValue = PimMegaGetImagerType(deviceId, &imagerType);	
		numberParms=1;
		nRetValue = PxLGetFeature (deviceId, FEATURE_PIXEL_FORMAT ,&currentFlag,&numberParms,&pixelFormat);
		if (plError(nRetValue, "calling FEATURE_PIXEL_FORMAT"))
		{
			//PimMegaStopVideoStream(deviceId);
			nRetValue = PxLSetStreamState(deviceId, STOP_STREAM);
			mexErrMsgTxt("\n");
		}

		//nRetValue = PimMegaGetSubWindowSize(deviceId, &decimation, &width, &height);
		numberParms=4;
		nRetValue = PxLGetFeature (deviceId, FEATURE_ROI ,&currentFlag,&numberParms,region);
		if (plError(nRetValue, "calling PxLGetFeature"))
		{
			nRetValue = PxLSetStreamState(deviceId, STOP_STREAM);
			mexErrMsgTxt("\n");
		}
		numberParms=2;
		nRetValue = PxLGetFeature (deviceId, FEATURE_DECIMATION,&currentFlag,&numberParms,decimation);
		if (plError(nRetValue, "calling PxLGetFeature"))
		{
			nRetValue = PxLSetStreamState(deviceId, STOP_STREAM);
			mexErrMsgTxt("\n");
		}

		pixelWidth = (U32) (region[2] / decimation[0]);
		pixelHeight = (U32) (region[3] / decimation[0]);



		/* get raw output flag from stored configuration in plDevices */
		/* grabOutputType = plDevices("getpar", serialNumber, "GrabOutputType") */
		rhs[0] = mxCreateString("getpar");
		rhs[1] = mxCreateDoubleMatrix(1, 1, mxREAL);

		px = mxGetPr(rhs[1]);
		px[0] = (U32)serialNumber;
		rhs[2] = mxCreateString("GrabOutputType");

		mexCallMATLAB(1, lhs, 3, rhs, "plDevices");
		grabOutputType = (int)mxGetScalar(lhs[0]);			/* what sort of data should be returned to Matlab? */

/********************************************************************/
/*      Allocate memory for the arrays in which the image is stored */
/********************************************************************/

		/* calculate array properties
		 *
		 * 3 arrays are used:
		 *  - caputure array: array returned by the pixelink camera; size (pixelWidth * pixelHeight)
		 *  - matlab array:   array returned to matlab; size depends on postprocessing
		 *  - RGB24 array:    array in which the output of the PimMegaConvert function is stored (not with raw images)
		 *
		 * size:		array used to build the Matlab array (with mxCreateNumericArray)
		 * dim:			number of dimensions of the Matlab array
		 */
		switch (grabOutputType)
		{
			case (RAW):
			{	/* raw output: image is returned to matlab exactly the way it is captured.
				 * matlab array should be (pixelWidth * pixelHeight) for both colour and monochrome
				 */
				size = (int *)mxCalloc(2, sizeof(int));
				size[0] = pixelWidth;
				size[1] = pixelHeight;
				dim = 2;	/* Raw image array has 2 dimensions */
				break;
			}
			case (IMAGE):
			{	/* processed output: image is processed before it is returned to matlab.
				 * matlab array should be (3 * pixelWidth * pixelHeight)
				 */
				size = (int *)mxCalloc(3, sizeof(int));
				size[0] = pixelWidth;
				size[1] = pixelHeight;
				size[2] = 3;
				dim = 3;	/* RGB image array has 3 dimensions */
				break;
			}
			case (RGB24):
			{	/* RGB24 output: image is returned as a consequent piece of memory
				 * of size (3 * pixelWidth * pixelHeight)
				 */
				size = (int *)mxCalloc(2, sizeof(int));
				size[0] = (pixelWidth * pixelHeight * 3);
				size[1] = 1;
				dim = 2;	/* RGB image array has 2 dimensions */
				break;
			}
			default:
			{
				mexPrintf("plGrab: unknown GrabOutputType, should be RAW, IMAGE or RGB24.");
				nRetValue = PxLSetStreamState(deviceId, STOP_STREAM);
				mexErrMsgTxt("\n");
			}
		}

		rawImgSize = (pixelWidth * pixelHeight);	/* number of memory locations needed for storing the raw image */
		RGB24ImgSize = rawImgSize * 3;
		mexPrintf("rawImgSize.%lu\n",rawImgSize);
		/* allocate memory for the matlab and capture arrays */
		switch ((int)pixelFormat)
		{	/* dimension matrix for pixeldepth */
			case (PIXEL_FORMAT_MONO8):	/* 8 bit pixel depth, use 8 bit integer per pixel */
									plhs[0] = mxCreateNumericArray(dim, size, mxUINT8_CLASS, mxREAL);
									imgCapturePtr = (U8 *)mxCalloc(rawImgSize, sizeof(U8));
									rawImgByteSize = rawImgSize;
									
									break;
			case (PIXEL_FORMAT_MONO16):	/* 10 bit pixel depth, use 16 bit integer per pixel */
									if (grabOutputType == RAW)
										plhs[0] = mxCreateNumericArray(dim, size, mxUINT16_CLASS, mxREAL);
									else /* IMAGE or RGB24 is always 8 bit per subpixel */
										plhs[0] = mxCreateNumericArray(dim, size, mxUINT8_CLASS, mxREAL);
									imgCapturePtr = (U16 *)mxCalloc(rawImgSize, sizeof(U16));
									rawImgByteSize = rawImgSize * 2;
									break;
			default:				mexPrintf("plGrab: Unknown pixel depth, should be PIXEL_FORMAT_MONO8 or PIXEL_FORMAT_MONO16.\n");
									nRetValue = PxLSetStreamState(deviceId, STOP_STREAM);
									mexErrMsgTxt("\n");
									return;
		}
		if (plhs[0] == NULL)
		{	/* mxCreateNumericArray returns NULL in case of problems, mxCalloc doesn't return at all */
			nRetValue = PxLSetStreamState(deviceId, STOP_STREAM);
			mexErrMsgTxt("plGrab: Cannot allocate enough MatLAB memory to store the captured image.\n");
			return;
		}
		imgOutPtr = (int *)mxGetPr(plhs[0]);
		/* allocate memory for the RGB24 array if needed */
		if (grabOutputType != RAW) img24BppPtr = (U8 *)mxCalloc(RGB24ImgSize, sizeof(U8));

/********************************************************************/
/*      Capture the image and close the video stream				*/
/********************************************************************/

		/* capture the image */
		//nRetValue = PimMegaReturnVideoData(deviceId, rawImgByteSize, imgCapturePtr);
		nRetValue = PxLSetStreamState(deviceId, START_STREAM);
		nRetValue = PxLGetNextFrame(deviceId,rawImgByteSize,imgCapturePtr,&Descriptor);
		if (plError(nRetValue, "calling PxLGetNextFrame"))
		{
			nRetValue = PxLSetStreamState(deviceId, STOP_STREAM);
			mexErrMsgTxt("\n");
		}

		/* close the video stream */
		nRetValue = PxLSetStreamState(deviceId, STOP_STREAM);
		if (plError(nRetValue, "calling PxLSetStreamState"))
		{
			mexErrMsgTxt("\n");
		}

/********************************************************************/
/*      Postprocess the image (if necessary) and return the image	*/
/********************************************************************/

		/* return the image as is specified by the GrabColorConversion and GrabOutputType variables */
		if (grabOutputType == RAW)
		{	/* 8 bits transfer, just return the image */
			if (pixelFormat == PIXEL_FORMAT_MONO8)
				memcpy(imgOutPtr, imgCapturePtr, rawImgByteSize);
			else if (pixelFormat == PIXEL_FORMAT_MONO16)
			{	/* 16 bits transfer: shift some bits */
				j = 0;
				for (i = 0; i < rawImgSize; i++)
				{
					((PU16)imgOutPtr)[i] = (((PU8)imgCapturePtr)[j] >> 6) + (U16)(((PU8)imgCapturePtr)[j+1]) * 4;
					j+=2;
				}
			}
			else
			{
				mexPrintf("plGrab: unknown pixel depth.\n");
				mexErrMsgTxt("\n");
				return;
			}
		}
		else /* grabOutputType != RAW */
		{	
			

				
			if (grabOutputType == IMAGE)
			{	/* transpose the RGB24 array (rgbrgbrgb) to a MatLAB image array (rrrgggbbb) */
				/* copy image into the matlab array in such a way that it is displayed correctly */
				nRetValue = PxLFormatImage((PU8)imgCapturePtr, &Descriptor, IMAGE_FORMAT_RAW_RGB24, (PU8)img24BppPtr, &RGB24ImgSize );
				plError(nRetValue, "converting raw mono 8 to 24 bpp");
				memcpy(imgOutPtr, img24BppPtr, RGB24ImgSize);
				/*
				for (i = 0; i < pixelHeight; i++)
				{
					for (j = 0; j < pixelWidth; j++)
					{
						((PU8)imgOutPtr)[((j+1) * pixelHeight) - i - 1] = ((PU8)img24BppPtr)[((i * pixelWidth) + j) * 3 + 2];
						((PU8)imgOutPtr)[((j+1) * pixelHeight) - i - 1 + (pixelWidth * pixelHeight)] = ((PU8)img24BppPtr)[(((i * pixelWidth) + j) * 3) + 1];
						((PU8)imgOutPtr)[((j+1) * pixelHeight) - i - 1 + (pixelWidth * pixelHeight * 2)] = ((PU8)img24BppPtr)[(((i * pixelWidth) + j) * 3)];
					}
				}
				*/
			}
			else if (grabOutputType == RGB24)
			{	/* just copy the RGB24 image into the matlab array */
				nRetValue = PxLFormatImage((PU8)imgCapturePtr, &Descriptor, IMAGE_FORMAT_RAW_RGB24, (PU8)img24BppPtr, &RGB24ImgSize );
				plError(nRetValue, "converting raw mono 8 to 24 bpp");
				memcpy(imgOutPtr, img24BppPtr, RGB24ImgSize);
			}
			else /* grabOutputType is neither RAW, RGB24 nor IMAGE */
			{
				mexPrintf("plGrab: unknown value of GrabOutputType: should be RAW, RGB24 or IMAGE.\n");
				mexErrMsgTxt("\n");
				return;
			}
		}
	}
	else /* !((nrhs==1) && (nlhs == 1)) */
	{
		mexPrintf("plGrab: Wrong arguments given. Calling syntax: image = plGrab(handle), or image = plGrab(serialnumber)\n");
		mexErrMsgTxt("\n");
	}
}

#ifdef __cplusplus
    }	/* extern "C" */
#endif
