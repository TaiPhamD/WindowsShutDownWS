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


MSI installer will install app here:
```C:\Program Files (x86)\WindowsShutdownWS\``` . The installer will also create a windows service called
```ShutdownWS``` .

1. configure your ```C:\Program Files (x86)\WindowsShutdownWS\config.txt``` to change your password and web port.  Restart ShutdownWS windows service for it to take affect.

1. must configure windows firewall to allow ```C:\Program Files (x86)\WindowsShutdownWS\shutdown_service.exe``` to listen on the port

1. Must configure your router to forward the port to your computer

