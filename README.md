# WindowsShutDownWS
This WebService will host an endpoint locally on the computer to listen for an HTTPS request/post to initiate windows shutdown or restart (including changing UEFI BootOrder id). A good example would to use IFTTT webhook to trigger an HTTPS post after receiving a Google Assistant or Alexa command. The shutdown process is done by calling the windows shutdown procedure via a windows DLL.

Currently the restart command will change UEFI BootOrder ID based on Google Assistant Ingredient. The mapping of the Ingrediant and BootOrder ID is based on config.json. You can run uefiscanner.exe to figure out your BootOrderID and use decimal ID (not hex) when configuring JSON. You can also use Linux efibootmgr --verbose to figure out your bootID however remember to convert to Decimal from hex.

Binary link: 
- webserverApp : https://github.com/TaiPhamD/WindowsShutDownWS/releases

# binary release download

1. copy the zip content for example to C:\shutdown_server

1. Run git-bash and [generate self-sign cert](https://github.com/TaiPhamD/WindowsShutDownWS/blob/master/examples/genreate_self_cert.txt) for HTTPS.

1. Run [examples\create_service_example.bat](https://github.com/TaiPhamD/WindowsShutDownWS/blob/master/examples/create_service_example.bat) to generate windows service to run this web service application. Must run with administrative command prompt.

1. Create a config.json and place it in the same folder as 1. above. You can see [examples\config_example.json](https://github.com/TaiPhamD/WindowsShutDownWS/blob/master/examples/config_example.json) on the formatting


1. must configure windows firewall to allow ```your_folder\shutdownWS.exe``` to listen on the port

1. Must configure your router to forward the port to your computer

1. Open windows services and find "ShutDownWebServer" service to start it
   - Check out info.log file from path defined in 1. for logging and trouble shooting
# Testing before you connect to IFTTT service

Send https request to YOURIP:YOURPORT with the body as JSON

```
{
  "Password":"Yourpassword"
  "Mode": 0 //0: shutdown else any other number will initiate a restart based on Ingredient
  "Ingredient: "text phrase" This is the text ingredient that will feed into shutdown DLL for restart. It will set UEFI BootOrder id based on the {OS,BootID} dictionary defined in config.json
}
```

# IFTTT example

Please read from www.ifttt.com to learn more about IFTTT.  Some examples of integrating Google Assistant with this application via IFTTT Webhook configuration shown

 - [shutdown applet](https://github.com/TaiPhamD/WindowsShutDownWS/blob/master/examples/IFTTT_EXAMPLE_SHUTDOWN.jpg)
 - [restart to Mac/Windows applet](https://github.com/TaiPhamD/WindowsShutDownWS/blob/master/examples/IFTTT_EXAMPLE_RESTART.jpg)
 - [turn on PC -Wake on Lan applet](https://github.com/TaiPhamD/WindowsShutDownWS/blob/master/examples/IFTTT_EXAMPLE_TURN_ON.jpg)
    - Also if you want to automatically turn on your computer by using Wake on Lan service please see this [repository](https://github.com/TaiPhamD/WOLWebService)
       - You probably want to host this on a dedicated server like a raspberry pi
    - If you are using IntelMausi kext follow this to enable WOL: https://github.com/Mieze/IntelMausiEthernet/issues/17#issuecomment-424290662

# Build from Source 

## Prerequisites
1. Install Visual Studio Community (2017 or 2019)
1. Install GoLang
1. Install Cmake

## Manual compile/install


1. Open cmd.exe
1. run build.bat from this source folder
1. The compiled binaries should be in build/dist
