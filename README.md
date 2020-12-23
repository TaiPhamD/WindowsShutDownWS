# WindowsShutDownWS
This WebService will host an endpoint locally on the computer to listen for an HTTP request/post to initiate windows shutdown. A good example would to use IFTTT webhook to trigger an HTTP post after receiving a Google Assistant or Alexa command. The shutdown process is done by calling the windows shutdown procedure via a windows DLL:
https://github.com/TaiPhamD/shutdownDLL 

Binary link: 
- webserverApp : https://github.com/TaiPhamD/WindowsShutDownWS/releases

# Windows Installer from release download
MSI installer will install app here:
```C:\Program Files (x86)\WindowsShutdownWS\``` . The installer will also create a windows service called
```ShutdownWS``` .


1. configure your ```C:\Program Files (x86)\WindowsShutdownWS\config.txt``` to change your password and web port.  Restart ShutdownWS windows service for it to load the updated config.txt.
        

       *config.txt* Syntax:
       your_password
       your_web_service_port
       For example:
       MycoolPassword
       9999



1. must configure windows firewall to allow ```C:\Program Files (x86)\WindowsShutdownWS\WindowsShutdownWS.exe``` to listen on the port

1. Must configure your router to forward the port to your computer


# Testing before you connect to IFTTT service

Send http request to YOURIP:YOURPORT with the body as JSON

```
{
  "Password":"Yourpassword"
}
```

# IFTTT example

Please read from www.ifttt.com to learn more about IFTTT. An example, integrating Google Assistant with this application via IFTTT Webhook configuration shown [here](https://github.com/TaiPhamD/WindowsShutDownWS/blob/master/IFTTT_EXAMPLE.jpg)




# Build from Source 

## Manual compile/install

1. run "go build" command to create the exe
1. setup config.txt in the same path as the compiled .exe
1. Create windows service to launch the compiled .exe ( see create_service_example.bat)
1. Compile [shutdown.dll](https://github.com/TaiPhamD/shutdownDLL) and place it in the same directory as the WindowsShutdownWS.exe

## creating your own .MSI installer using https://github.com/wixtoolset if you want to distribute it using .MSI

1. Install wixtoolset
1. Compile:
   - shutdown.dll
   - WindowsShutDownWS.exe
1. Create a config.txt based on the template
1. Copy results from 2 and 3 above to main root of this repo
1. open CMD.exe and set path=%path%;C:\path_to_\wix311-binaries
1. Run WIX command line to generate .msi

```
   candle shutdown.wxs
   light shutdown.wixobj
```
 You should now have a .msi installer!

