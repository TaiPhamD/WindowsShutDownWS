// shutdownDLL.cpp : Defines the exported functions for the DLL application.
// code adopted from: https://serverfault.com/questions/813695/how-do-i-stop-windows-10-install-from-modifying-bios-boot-settings

#include "stdafx.h"


#include <windows.h>
#include <iomanip>
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "advapi32.lib")


struct CloseHandleHelper {
  void operator()(void *p) const { CloseHandle(p); }
};

BOOL SetPrivilege(HANDLE process, LPCWSTR name, BOOL on) {
  HANDLE token;
  if (!OpenProcessToken(process, TOKEN_ADJUST_PRIVILEGES, &token))
    return FALSE;
  std::unique_ptr<void, CloseHandleHelper> tokenLifetime(token);
  TOKEN_PRIVILEGES tp;
  tp.PrivilegeCount = 1;
  if (!LookupPrivilegeValueW(NULL, name, &tp.Privileges[0].Luid))
    return FALSE;
  tp.Privileges[0].Attributes = on ? SE_PRIVILEGE_ENABLED : 0;
  return AdjustTokenPrivileges(token, FALSE, &tp, sizeof(tp), NULL, NULL);
}


void startShutdown() {
  // Get required shutdown priviledges
  SetPrivilege(GetCurrentProcess(), SE_SHUTDOWN_NAME, TRUE);
  // Go ahead and shut down
  InitiateSystemShutdownEx(NULL, NULL, 0, FALSE, FALSE, 0);

}


void changeBootAndRestart(uint32_t *mode) {
  // Mode 1 - restart to Windows, Mode 2: restart to MacOS
  // Get required UEFI write priviledges
  SetPrivilege(GetCurrentProcess(), SE_SYSTEM_ENVIRONMENT_NAME, TRUE);

  // Update UEFI
  const int bootOrderSize = 2;
  const BYTE windowsBoot[] = {0x00, 0x00};
  const BYTE macBoot[] = {0x80, 0x00}; //bytes are reversed
  DWORD bootOrderLength = 0;
  const TCHAR bootOrderName[] = TEXT("BootOrder");
  const TCHAR globalGuid[] = TEXT("{8BE4DF61-93CA-11D2-AA0D-00E098032B8C}");
  DWORD bootOrderAttributes = 7; // VARIABLE_ATTRIBUTE_NON_VOLATILE |
                                 // VARIABLE_ATTRIBUTE_BOOTSERVICE_ACCESS |
                                 // VARIABLE_ATTRIBUTE_RUNTIME_ACCESS
  BYTE payload[bootOrderSize];

  if (*mode==1) {
    memcpy_s(payload, bootOrderSize, windowsBoot, sizeof(windowsBoot));
  } 
  else if (*mode == 2) {
    memcpy_s(payload, bootOrderSize, macBoot, sizeof(macBoot));
  } 
  else {
    //default to windows mode for all other
    memcpy_s(payload, bootOrderSize, windowsBoot, sizeof(windowsBoot));
  }

  SetFirmwareEnvironmentVariableEx(bootOrderName, globalGuid, &payload,
                                   bootOrderSize,
                                   bootOrderAttributes);


    // Get required restart priviledges
  SetPrivilege(GetCurrentProcess(), SE_SHUTDOWN_NAME, TRUE);
  InitiateSystemShutdownEx(NULL, NULL, 2, FALSE, TRUE, 0);
}

extern "C" {
__declspec(dllexport) bool MySystemShutdown(uint32_t *mode) {
          if (*mode == 0) {
            // we are just doign a normal shutdown
            startShutdown();
          } else {
            changeBootAndRestart(mode);
          }

          // shutdown was successful
          return TRUE;
    }
}
