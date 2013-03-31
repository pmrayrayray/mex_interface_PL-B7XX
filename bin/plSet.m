%plSet: a function to set (write) parameters for a PixeLINK
%camera device.
%
%SYNTAX plset(handle); or plset(handle, parametername); or
%plset(handle, parametername, value1, ..., valueN); or
%plset(serialnumber); or plset(serialnumber, parametername); or
%plset(serialnumber, parametername, value1, ..., valueN);
%
%DESCRIPTION When called with only the handle or serial number, this
%function will print a list of all parameters that can be set. When
%called with also a parameter name, some more information on setting
%the specified parameter is given. Finally, when called with 1 or more
%values, those will be written to the specified parameter of the
%PixeLINK camera.
%
%INPUT The handle structure or the serial number of the
%device, and as optional input, the name of a parameter and the
%value(s) that should be set.
%
%REMARKS It will be verified whether the input is a valid 
%handle structure or number.
%
%OUTPUT If the input isn't valid or the framegrabber 
%can't be reached for some reason, an error message 
%will be shown to the user. If everything's ok, plSet
%will act as specified in the description.
%
%EXAMPLE 1:	plSet(m);
%EXAMPLE 2:	plSet(75122, 'SubWindowSize');
%EXAMPLE 3:	plset(m, 'SubWindowSize', 'PCS2112_NO_DECIMATION', 480, 640);
%EXAMPLE 4:	plset(75122, 'Gamma', 0.7631);
% 

disp('Error: plSet not found.')
