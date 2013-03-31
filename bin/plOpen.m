%plOpen: a function to open a PixeLINK camera device.
%
%SYNTAX m=plopen(serialnumber);
%
%DESCRIPTION This function will open a PixeLINK camera
%for use by other plFGI functions, or re-obtain the
%handle of a device which is already open.
%
%INPUT The serial number of the device to be opened.
%
%REMARKS It will be verified whether the input is a
%number. If a camera with the given serial number is
%not found, a list will be printed of all available
%cameras.
%
%OUTPUT If the input isn't a number, an error message 
%will be shown to the user. If it is an unknown serial
%number, a list with available numbers will be printed.
%If the serial number is recognised, plOpen will try
%to open it and return a handle structure. When the
%camera is already open, a message will be printed
%and the handle structure is returned.
%
%EXAMPLES To attempt to open a camera having serial
%number 75122, and store it's handle in mxArray m,
%type in the Matlab Command Window : 
% 
%	EXAMPLE 1: 	m=plopen(75122);
%

disp('Error: plOpen not found.')
