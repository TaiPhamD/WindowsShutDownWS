# WindowsShutDownWS
This is WebService will host an api end point to shutdown your computer by calling the windows shutdown procedure via a windows DLL:
https://github.com/TaiPhamD/shutdownDLL (Binary link: https://github.com/TaiPhamD/shutdownDLL/files/2568215/shutdownDLL.zip)


The configuration for the webservice: 
place config.txt --> in C:\shutdown_server\config.txt

*config.txt* Syntax:
```
your_password
your_web_service_port
```
For example:
```
MycoolPassword
9999
```
run cmd.exe as adminstrator to install this app as a service:
```
sc create ShutdownWebService binPath="C:\your_path_to\shutdown_server.exe"
```
