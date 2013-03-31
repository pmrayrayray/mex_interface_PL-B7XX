%plGet: a function to get (read) parameters from a PixeLINK
%camera device.
%
%SYNTAX plget(handle); or plget(handle, parametername);
%plget(serialnumber); or plget(serialnumber, parametername);
%
%DESCRIPTION This function will get all parameters (in
%the form of an up-to-date handle) or a specified parameter
%from an opened PixeLINK camera device.
%
%INPUT The handle structure or the serial number of the
%device from which parameters should be obtained, and
%as an option, the name of a parameter.
%
%REMARKS It will be verified whether the input is a valid 
%handle structure or number.
%
%OUTPUT If the input isn't valid or the framegrabber 
%can't be reached for some reason, an error message 
%will be shown to the user. If everything's ok, plGet
%will either return a complete handle which reflects the
%most recent settings of the device, or if a parameter
%name was given, it will return the value or structure
%belonging to that parameter.
%
%EXAMPLES
% 
%	EXAMPLE 1: 		plget(m)
%	EXAMPLE 2:		plget(75122, 'SubWindowSize');
% 

disp('Error: plGet not found.')
