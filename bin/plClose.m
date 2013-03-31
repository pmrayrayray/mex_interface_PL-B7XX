%plClose: a function to close a PixeLINK camera device.
%
%SYNTAX plclose(handle); or plclose(serialnumber);
%
%DESCRIPTION This function will close an opened
%PixeLINK camera device.
%
%INPUT The handle structure or the serial number of the
%device that should be closed.
%
%REMARKS It will be verified whether the input is a valid 
%handle structure or number. If it's valid, plClose
%will try to close the device.
%
%OUTPUT If the input isn't valid or the framegrabber 
%can't be closed for some reason, an error message 
%will be shown to the user. If everything's ok, the 
%device will be closed and the internal list of open
%devices will be updated.
%
%EXAMPLES To close a camera with its handle 
%stored in matlab variable m and serial number
%75122, type in the Matlab Command Window : 
% 
%	EXAMPLE 1: 		plclose(m);
%	EXAMPLE 2:		plclose(75122);  
% 

disp('Error: plClose not found.')
