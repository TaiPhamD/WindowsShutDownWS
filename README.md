# WindowsShutDownWS
This WebService will host an endpoint locally on the computer to listen for an HTTP request/post to initiate windows shutdown. The shutdown process is done by calling the windows shutdown procedure via a windows DLL:
https://github.com/TaiPhamD/shutdownDLL 

Binary link: 
- shutdownDLL : https://github.com/TaiPhamD/shutdownDLL/files/2568215/shutdownDLL.zip)
- webserverApp : https://github.com/TaiPhamD/WindowsShutDownWS/files/2568218/shutdown_server.zip

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
Also remember to place the shutdownDLL in C:\windows\system32 or in the same path as shutdown_server.exe
