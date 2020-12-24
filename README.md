# WindowsShutDownWS
This WebService will host an endpoint locally on the computer to listen for an HTTP request/post to initiate windows shutdown or restart (including changing UEFI BootOrder id). A good example would to use IFTTT webhook to trigger an HTTP post after receiving a Google Assistant or Alexa command. The shutdown process is done by calling the windows shutdown procedure via a windows DLL:
https://github.com/TaiPhamD/shutdownDLL 

Currently the restart command will change UEFI BootOrder ID based on Google Assistant Ingredient. However, the BootOrder ID is hardcoded to set 0x0080 for MacOS and 0x0000 for Windows. Please edit [this](https://github.com/TaiPhamD/shutdownDLL/blob/3e484fe36e3a3006a17bd99db79ff02f418547c7/shutdownDLL.cpp#L47) if you want to set a diffrent BootOrder ID. I will make this into a configurable item later. 

You can use Linux efibootmgr --verbose to figure out your bootID for example on my computer I have 0x0000 for Windows and 0x0080 for MacOS:

```
efibootmgr --verbose
Timeout: 1 seconds
BootOrder: 0080
Boot0000* Windows Boot Manager	HD(2,GPT,3df99478-b7c8-4c1b-b16c-d994132ddd6a,0x40800,0x2b800)/File(\EFI\Microsoft\Boot\bootmgfw.efi)WINDOWS.........x...B.C.D.O.B.J.E.C.T.=.{.9.d.e.a.8.6.2.c.-.5.c.d.d.-.4.e.7.0.-.a.c.c.1.-.f.3.2.b.3.4.4.d.4.7.9.5.}...o................
Boot0080* Mac OS X	PciRoot(0x0)/Pci(0x1d,0x0)/Pci(0x0,0x0)//HD(2,GPT,4ba9ab95-5cd6-4388-aaa0-5ca49161ca57,0x64028,0x3b97d260)/VenMedia(be74fcf7-0b7c-49f3-9147-01f4042e6842,6ff82f809160a14780cbbf4d161cbff1)/File(\D6C224AB-6106-4CC1-889A-BF0F17E0EA53\System\Library\CoreServices\boot.efi)
Boot0081* Mac OS X	PciRoot(0x0)/Pci(0x1d,0x0)/Pci(0x0,0x0)//HD(2,GPT,4ba9ab95-5cd6-4388-aaa0-5ca49161ca57,0x64028,0x3b97d260)/VenMedia(be74fcf7-0b7c-49f3-9147-01f4042e6842,6ff82f809160a14780cbbf4d161cbff1)/File(\5E921AB6-7A04-442E-BFB5-052A0469E665\System\Library\CoreServices\boot.efi)
```

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
  "Mode": 0 //0: shutdown else any other number will call shutdown DLL based on Ingredient
  "Ingredient: "text phrase" This is the text ingredient that will feed into shutdown DLL. It will call mode 1 if text phrase is "Windows" else it will call mode 2
}
```

# IFTTT example

Please read from www.ifttt.com to learn more about IFTTT.  Some examples of integrating Google Assistant with this application via IFTTT Webhook configuration shown

 - [shutdown applet](https://github.com/TaiPhamD/WindowsShutDownWS/blob/master/IFTTT_EXAMPLE_SHUTDOWN.jpg)
 - [restart to Mac/Windows applet](https://github.com/TaiPhamD/WindowsShutDownWS/blob/master/IFTTT_EXAMPLE_RESTART.jpg)
 - [turn on PC -Wake on Lan applet](https://github.com/TaiPhamD/WindowsShutDownWS/blob/master/IFTTT_EXAMPLE_TURN_ON.jpg)
    - Also if you want to automatically turn on your computer by using Wake on Lan service please see this [repository](https://github.com/TaiPhamD/WOLWebService)
       - You probably want to host this on a dedicated server like a raspberry pi
    - If you are using IntelMausi kext follow this to enable WOL: https://github.com/Mieze/IntelMausiEthernet/issues/17#issuecomment-424290662

# Build from Source 

## Manual compile/install

1. run "go build" command to create the exe
1. setup config.txt in the same path as the compiled .exe
1. Create windows service to launch the compiled .exe ( see create_service_example.bat)
1. Compile [shutdown.dll](https://github.com/TaiPhamD/shutdownDLL) and place it in the same directory as the WindowsShutdownWS.exe

## Extra: creating your own .MSI installer using https://github.com/wixtoolset if you want to distribute it using .MSI

1. Install wixtoolset (remember the bin path since you will use it later)
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

