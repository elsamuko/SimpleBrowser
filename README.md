SimpleBrowser
=============

This is a simple sample for a MFC webbrowser control with pluggable protocol for asynchronous http and https requests.

Folders
=======

 * Registry: Contains two registry scripts, which switch the compat mode for the IE engine between 8 and 11
 * Server: Contains a simple webserver in groovy, which serves the sample html sites on localhost
 * SimpleBrowser: The source code for the MFC application


Reason
======

The pluggable protocol and the webbrowser control behave defectively, if the custom protocoll passes CSS stylesheets with the MIME type "text/css".  
See also this question from me on stackoverflow:  
http://stackoverflow.com/questions/21552785/mfc-mimetype-text-css-not-recognized-by-iinternetprotocolsink
