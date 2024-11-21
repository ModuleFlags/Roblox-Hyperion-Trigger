#include "pch.h"
#include "..\hyperiontrigger+\hyperion\xorstr\xorstr.hpp"
#include <iostream>
#include <string>

std::string messagePart1 = XorStr("We’ve detected the use of unauthorized software or a suspicious modification while playing. This behavior is in direct violation of our game’s rules and could result in further actions.\n\n"
    "Warning: Using cheats, hacks, or third-party software compromises the integrity of the game and ruins the experience for others.\n\n"
    "Your actions have been flagged, and further investigation will be conducted.\n\n"
    "Do not attempt to rejoin or bypass this detection. Continued use of unauthorized software may lead to severe consequences.\n\n"
    "Important: Please remove any cheating software immediately. Any further attempts to exploit the game will not be tolerated.\n\n");

std::string messagePart2 = XorStr("If you believe this is an error, please reach out to support with your case details. However, be aware that cheating accusations are taken seriously, and revalidation may be required.");

extern "C" __declspec(dllexport) LRESULT CALLBACK NextHook(int nCode, WPARAM wParam, LPARAM lParam) {
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        // Encrypt the message using XOR

        MessageBoxA(
            NULL,
            (messagePart1 + messagePart2).c_str(),  // Combine both parts and show the decrypted message
            XorStr("Roblox"),  // Decrypted title
            MB_OK | MB_ICONERROR  // Error icon with OK button
        );
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

