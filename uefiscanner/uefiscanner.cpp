// EFI.cpp : Quick app to print UEFI boot variables
//

#include <iostream>
#include <sstream>
#include <windows.h>
#include <iomanip>

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

int main()
{

  SetPrivilege(GetCurrentProcess(), SE_SYSTEM_ENVIRONMENT_NAME, TRUE);

  const int BUFFER_SIZE = 4096;
  BYTE bootOrderBuffer[BUFFER_SIZE];
  DWORD bootOrderLength = 0;
  const TCHAR bootOrderName[] = TEXT("BootOrder");
  const TCHAR globalGuid[] = TEXT("{8BE4DF61-93CA-11D2-AA0D-00E098032B8C}");
  DWORD bootOrderAttributes = 7; //VARIABLE_ATTRIBUTE_NON_VOLATILE | VARIABLE_ATTRIBUTE_BOOTSERVICE_ACCESS | VARIABLE_ATTRIBUTE_RUNTIME_ACCESS

  bootOrderLength = GetFirmwareEnvironmentVariableEx(
      bootOrderName, globalGuid, bootOrderBuffer, BUFFER_SIZE,
      &bootOrderAttributes);
  if (bootOrderLength == 0) {
    std::cout << "Failed getting BootOrder with error " << GetLastError()
              << std::endl;
    return 1;
  }
 
  /*
  std::wcout << "boot order length: " << bootOrderLength << std::endl;
  std::wcout << "boot order attributes: " << bootOrderAttributes << std::setw(2)
             << std::hex << std::endl;
  */

  uint16_t data;
  memcpy((char*)&data, bootOrderBuffer, 2);

  //std::wstring CurrBoot(reinterpret_cast<wchar_t *>(bootOrderBuffer + sizeof(uint16_t)));
  std::wcout << "Current boot Integer: " << std::setfill(L'0')  << std::setw(5) << std::dec  << data
             << std::endl;


  for (DWORD i = 0; i < 10000; i ++) {
    std::wstringstream bootOptionNameBuilder;
    bootOptionNameBuilder << "Boot" << std::uppercase << std::setfill(L'0')
                          << std::setw(4) << std::hex
                          << (uint16_t)i;
    
    std::wstring bootOptionName(bootOptionNameBuilder.str());
    BYTE bootOptionInfoBuffer[BUFFER_SIZE];
    DWORD bootOptionInfoLength = GetFirmwareEnvironmentVariableEx(
        bootOptionName.c_str(), globalGuid, bootOptionInfoBuffer, BUFFER_SIZE,
        nullptr);
    if (bootOptionInfoLength == 0) {
      /*std::cout << "Failed getting option info for option at offset " << i
                << std::endl;*/
      continue;
    }
    uint32_t *bootOptionInfoAttributes =
        reinterpret_cast<uint32_t *>(bootOptionInfoBuffer);
    // First 4 bytes make a uint32_t comprised of flags. 0x1 means the boot
    // option is active (not disabled)
    if (((*bootOptionInfoAttributes) & 0x1) != 0) {
      std::wstring description(reinterpret_cast<wchar_t *>(
          bootOptionInfoBuffer + sizeof(uint32_t) + sizeof(uint16_t)));

      std::wcout << "Boot option name:" << bootOptionName << std::endl;
      std::wcout << "Boot description: " << description << " integer ID: " <<  std::dec << i << std::endl
                 << std::endl;
      /*bool isWBM = boost::algorithm::to_upper_copy<std::wstring>(description) ==
                   L"WINDOWS BOOT MANAGER";*/
      // details - keep track of the value of i for the first WBM and non-WBM
      // options you find, and the fact that you found them
    }
  }

  // shutdown was successful
  return TRUE;
}
