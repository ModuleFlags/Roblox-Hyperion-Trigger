#include "hyperion/hyperionutils.h"
#include "hyperion/xorstr/xorstr.hpp"
#include "hyperion/roblox/roblox.h"

int main() {
    ClearLog();
    DisplayDetectionMessage();
    Sleep(4000);
    //return 0;
    if (TriggerHyperion()) {
        //std::wcout << XorStrW(L"Triggered hyperion successfully.") << std::endl;
    }
    else {
        std::wcerr << XorStrW(L"Process failed.") << std::endl;
    }

    return 0;
}



