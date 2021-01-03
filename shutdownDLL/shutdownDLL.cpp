// shutdownDLL.cpp : Defines the exported functions for the DLL application.
// code adopted from: https://serverfault.com/questions/813695/how-do-i-stop-windows-10-install-from-modifying-bios-boot-settings

#include <windows.h>
#include <iomanip>
#include <iostream>
//#pragma comment(lib, "user32.lib")
//#pragma comment(lib, "advapi32.lib")

struct CloseHandleHelper
{
  void operator()(void *p) const { CloseHandle(p); }
};

/** Function to obtain required priviledges to issue shutdown or restart**/
BOOL SetPrivilege(HANDLE process, LPCWSTR name, BOOL on)
{
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

/**Shutdown function**/
void startShutdown()
{
  // Get required shutdown priviledges
  SetPrivilege(GetCurrentProcess(), SE_SHUTDOWN_NAME, TRUE);
  // Go ahead and shut down
  InitiateSystemShutdownEx(NULL, NULL, 0, FALSE, FALSE, 0);
}

void changeBootAndRestart(uint16_t *data)
{

  // Get required UEFI write priviledges
  SetPrivilege(GetCurrentProcess(), SE_SYSTEM_ENVIRONMENT_NAME, TRUE);
  // Update UEFI
  const int bootOrderBytes = 2;
  const TCHAR bootOrderName[] = TEXT("BootOrder");
  const TCHAR globalGuid[] = TEXT("{8BE4DF61-93CA-11D2-AA0D-00E098032B8C}");
  DWORD bootOrderAttributes = 7; // VARIABLE_ATTRIBUTE_NON_VOLATILE |
                                 // VARIABLE_ATTRIBUTE_BOOTSERVICE_ACCESS |
                                 // VARIABLE_ATTRIBUTE_RUNTIME_ACCESS

  SetFirmwareEnvironmentVariableEx(bootOrderName,
                                   globalGuid,
                                   data,
                                   bootOrderBytes,
                                   bootOrderAttributes);
  // Get required restart priviledges
  SetPrivilege(GetCurrentProcess(), SE_SHUTDOWN_NAME, TRUE);
  InitiateSystemShutdownEx(NULL, NULL, 2, FALSE, TRUE, 0);
}

extern "C"
{
  __declspec(dllexport) bool MySystemShutdown()
  {
    // we are just doign a normal shutdown
    startShutdown();
    // shutdown was successful
    return TRUE;
  }

  __declspec(dllexport) bool MySystemRestart(uint16_t *data)
  {
    // we are just doign a normal shutdown
    changeBootAndRestart(data);
    // shutdown was successful
    return TRUE;
  }
}
