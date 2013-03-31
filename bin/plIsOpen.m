%plIsOpen: a function to check if a PixeLINK
%camera device is open.
%
%SYNTAX b = plIsopen(handle); or
%b=plisopen(serialnumber);
%
%DESCRIPTION This function will check if a PixeLINK
%camera device is open.
%
%INPUT The handle structure or the serial number of the
%device.
%
%REMARKS It will be verified whether the input is a valid 
%handle structure or number. If the camera was unplugged
%whithout being closed first, this function will still
%show it as being open.
%
%OUTPUT If the input isn't valid or the framegrabber 
%can't be reached for some reason, an error message 
%will be shown to the user. If everything's ok, the
%value 1 will be returned if the camera is open, the
%value 0 will be returned if the camera is closed.
%
%EXAMPLES To check if a camera with its handle 
%stored in matlab variable m and serial number 75122
%is open, type in the Matlab Command Window : 
% 
%	EXAMPLE 1: 		b=plisopen(m);
%	EXAMPLE 2:		b=plisopen(75122);  
%

disp('Error: plIsOpen not found.')
