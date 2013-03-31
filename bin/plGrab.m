%plGrab: a function to grab an image from a PixeLINK
%camera device.
%
%SYNTAX imagematrix=plgrab(handle); or
%imagematrix=plgrab(serialnumber);
%
%DESCRIPTION This function will grab an image from an
%opened PixeLINK camera device.
%
%INPUT The handle structure or the serial number of the
%device from which a frame should be grabbed.
%
%REMARKS It will be verified whether the input is a valid 
%handle structure or number. The size and nature of the
%image is determined amongst others by the DataTransferSize,
%GrabColorConversion and GrabOutputType parameters which
%can be set through plSet.
%
%OUTPUT If the input isn't valid or the framegrabber 
%can't be reached for some reason, an error message 
%will be shown to the user. If everything's ok, an image 
%will be returned.
%
%EXAMPLES To grab an image from a camera a with its handle 
%stored in matlab variable m and serial number 75122,
%into a mxArray i, type in the Matlab Command Window : 
% 
%	EXAMPLE 1: 		i=plgrab(m);
%	EXAMPLE 2:		i=plgrab(75122);  
%

disp('Error: plGrab not found.')
