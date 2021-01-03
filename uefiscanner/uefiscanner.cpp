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


void printUEFI(const LPCWSTR &globalGuid) {
 const int BUFFER_SIZE = 4096;
  BYTE bootOrderBuffer[BUFFER_SIZE];
  DWORD bootOrderLength = 0;
  const TCHAR bootOrderName[] = TEXT("BootOrder");
  DWORD bootOrderAttributes = 7; //VARIABLE_ATTRIBUTE_NON_VOLATILE | VARIABLE_ATTRIBUTE_BOOTSERVICE_ACCESS | VARIABLE_ATTRIBUTE_RUNTIME_ACCESS

  bootOrderLength = GetFirmwareEnvironmentVariableEx(
      bootOrderName, globalGuid, bootOrderBuffer, BUFFER_SIZE,
      &bootOrderAttributes);
  if (bootOrderLength == 0) {
    std::cout << "Failed getting BootOrder with error " << GetLastError()
              << std::endl;
    return;
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

}


int main()
{

  SetPrivilege(GetCurrentProcess(), SE_SYSTEM_ENVIRONMENT_NAME, TRUE);

  //GUID ID obtained from : https://github.com/acidanthera/OpenCorePkg/blob/2f1043d989a8dc86afb3330c29fad3414c843598/User/Library/UserGlobalVar.c

  //Print boot info for GLOBAL UEFI GUID
  //{ 0x8BE4DF61, 0x93CA, 0x11D2, { 0xAA, 0x0D, 0x00, 0xE0, 0x98, 0x03, 0x2B, 0x8C }};
  std::cout << "Global Boot GUID" << std::endl;
  printUEFI(TEXT("{8BE4DF61-93CA-11D2-AA0D-00E098032B8C}"));


  //Print boot info for APPLE UEFI GUID
  //{ 0X7C436110, 0XAB2A, 0X4BBB, { 0XA8, 0X80, 0XFE, 0X41, 0X99, 0X5C, 0X9F, 0X82 }};
  std::cout << "Apple Boot GUID" << std::endl;
  printUEFI(TEXT("{7C436110-AB2A-4BBB-A880-FE41995C9F82}"));

  
  //Print boot info for OC UEFI GUID
  //{ 0x4D1FDA02, 0x38C7, 0x4A6A, { 0x9C, 0xC6, 0x4B, 0xCC, 0xA8, 0xB3, 0x01, 0x02 }};
  std::cout << "OC Boot GUID" << std::endl;
  printUEFI(TEXT("{4D1FDA02-38C7-4A6A-9CC6-4BCCA8B30102}"));    

  // shutdown was successful
  return TRUE;
}
