// dllmain.cpp : Definisce il punto di ingresso per l'applicazione DLL.
#include "pch.h"
#include <windows.h>
#include <iostream>
#include <TlHelp32.h>
#include "mem.h"

DWORD WINAPI HackThread(HMODULE hModule)
{
    //Create console
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); //just for text colors

    char startBanner[] =
        "<+-+-+-+-+-+-+-+-+-+-+-+-+-+-+>\n\n"
        " Assault Cube v1.2.0.2 Trainer\n\n"
        "<+-+-+-+-+-+-+-+-+-+-+-+-+-+-+>\n\n";

    char help[] =
        "\n 1 = Semi god-mode\n"
        " 2 = Infinite ammo\n"
        " 3 = No recoil\n"
        " 4 = Rapid fire\n"
        " 6 = SuperJump (Combine with SuperSpeed for a sort of fly hack)\n"
        " 7 = SuperSpeed(NoClip)\n"
        " 0 = eject trainer\n"
        " 9 = display instructions\n";

    std::cout << startBanner;
    std::cout << help;

    //get module base
    uintptr_t moduleBase = (uintptr_t)GetModuleHandle(L"ac_client.exe");

    bool bHealth = false, bAmmo = false, bRecoil = false, bRapid = false, bSuperJump = false, bSpeed = false;

    while (true)
    {
        if (GetAsyncKeyState(VK_NUMPAD0) & 1)
        {
            break;
        }

        if (GetAsyncKeyState(VK_NUMPAD9) & 1)
        {
            SetConsoleTextAttribute(hConsole, 15);
            std::cout << help;
        }

        if (GetAsyncKeyState(VK_NUMPAD1) & 1)
        {
            bHealth = !bHealth;
            if (bHealth)
            {
                SetConsoleTextAttribute(hConsole, 2);
                std::cout << "SEMI GOD MODE ACTIVATED\n";
            }
            else
            {
                SetConsoleTextAttribute(hConsole, 4);
                std::cout << "SEMI GOD MODE DEACTIVATED\n";
            }
        }

        if (GetAsyncKeyState(VK_NUMPAD2) & 1)
        {
            bAmmo = !bAmmo;
            if (bAmmo)
            {
                SetConsoleTextAttribute(hConsole, 2);
                std::cout << "INFINITE AMMO ACTIVATED\n";
            }
            else
            {
                SetConsoleTextAttribute(hConsole, 4);
                std::cout << "INFINITE AMMO DEACTIVATED\n";
            }
        }

        if (GetAsyncKeyState(VK_NUMPAD3) & 1)
        {
            bRecoil = !bRecoil;

            if (bRecoil)
            {
                mem::Nop((BYTE*)(moduleBase + 0x63786), 10);
                SetConsoleTextAttribute(hConsole, 2);
                std::cout << "NO RECOIL ACTIVATED\n";
            }

            else
            {
                mem::Patch((BYTE*)(moduleBase + 0x63786), (BYTE*)"\x50\x8d\x4c\x24\x1c\x51\x8b\xce\xff\xd2", 10);
                SetConsoleTextAttribute(hConsole, 4);
                std::cout << "NO RECOIL DEACTIVATED\n";
            }
        }

        if (GetAsyncKeyState(VK_NUMPAD4) & 1)
        {
            bRapid = !bRapid;

            if (bRapid)
            {
                mem::Nop((BYTE*)(moduleBase + 0x637E4), 2);
                SetConsoleTextAttribute(hConsole, 2);
                std::cout << "RAPID FIRE ACTIVATED\n";
            }

            else
            {
                mem::Patch((BYTE*)(moduleBase + 0x637E4), (BYTE*)"\x89\x0A", 2);
                SetConsoleTextAttribute(hConsole, 4);
                std::cout << "RAPID FIRE DEACTIVATED\n";
            }
        }

        //continuous write/freeze
        uintptr_t* localPlayerPtr = (uintptr_t*)(moduleBase + 0x10f4f4);

        if (localPlayerPtr)
        {
            if (bHealth)
            {
                *(int*)(*localPlayerPtr + 0xf8) = 1337; //continuously write 1337 value at the health address
            }

            if (bAmmo)
            {
                uintptr_t ammoAddr = mem::FindDMAAddy(moduleBase + 0x10f4f4, { 0x374, 0x14, 0x0 });
                int* ammo = (int*)ammoAddr;
                *ammo = 1337; //continuously write 1337 value at the ammo address
            }

            if (GetAsyncKeyState(VK_NUMPAD6) & 1) //activate or deactivate superjump/sort of fly hack
            {
                bSuperJump = !bSuperJump;

                if (bSuperJump)
                {
                    SetConsoleTextAttribute(hConsole, 2);
                    std::cout << "SuperJump activated\n";
                }

                else
                {
                    SetConsoleTextAttribute(hConsole, 4);
                    std::cout << "SuperJump deactivated\n";
                }
            }

            if (bSuperJump)
            {
                if (GetAsyncKeyState(VK_SPACE))
                {
                    uintptr_t z_cordAddr = mem::FindDMAAddy(moduleBase + 0x10f4f4, { 0x3c });
                    float* z_cord = (float*)z_cordAddr;
                    *z_cord = *z_cord + 0.2; //adds 0.2 on the player position Z axis after pressing space
                }

                if (GetAsyncKeyState(VK_LCONTROL))
                {
                    uintptr_t z_cordAddr = mem::FindDMAAddy(moduleBase + 0x10f4f4, { 0x3c });
                    float* z_cord = (float*)z_cordAddr;
                    *z_cord = *z_cord - 0.2; //decreases 0.2 on the player position Z axis after pressing Lctrl
                }
            }

            if (GetAsyncKeyState(VK_NUMPAD7) & 1) //activate or deactivate SuperSpeed and a sort of NoClip
            {
                bSpeed = !bSpeed;

                if (bSpeed)
                {
                    SetConsoleTextAttribute(hConsole, 2); //set green text color
                    std::cout << "SuperSpeed activated\n";
                }

                else
                {
                    SetConsoleTextAttribute(hConsole, 4); //set red text color
                    std::cout << "SuperSpeed deactivated\n";
                }
            }

            //continuosly calculate the x Angle view
            uintptr_t xViewAddr = mem::FindDMAAddy(moduleBase + 0x10f4f4, { 0x40 });
            float* xView = (float*)xViewAddr;

            //continuosly calculate the y Angle view
            uintptr_t yViewAddr = mem::FindDMAAddy(moduleBase + 0x10f4f4, { 0x44 });
            float* yView = (float*)yViewAddr;

            if (bSpeed)
            {
                if (GetAsyncKeyState('W')) //player speed when going forward (continuously add/decrease a value on the player cords when pressing the 'W' key for different viewing angles
                {
                    if (*xView >= 0 && *xView <= 90)
                    {
                        uintptr_t y_cordAddr = mem::FindDMAAddy(moduleBase + 0x10f4f4, { 0x38 });
                        float* y_cord = (float*)y_cordAddr;
                        *y_cord = *y_cord - 0.1;

                        uintptr_t x_cordAddr = mem::FindDMAAddy(moduleBase + 0x10f4f4, { 0x34 });
                        float* x_cord = (float*)x_cordAddr;
                        *x_cord = *x_cord + 0.1;
                    }
                    if (*xView >= 90 && *xView <= 180)
                    {
                        uintptr_t y_cordAddr = mem::FindDMAAddy(moduleBase + 0x10f4f4, { 0x38 });
                        float* y_cord = (float*)y_cordAddr;
                        *y_cord = *y_cord + 0.1;

                        uintptr_t x_cordAddr = mem::FindDMAAddy(moduleBase + 0x10f4f4, { 0x34 });
                        float* x_cord = (float*)x_cordAddr;
                        *x_cord = *x_cord + 0.1;
                    }
                    if (*xView >= 180 && *xView <= 270)
                    {
                        uintptr_t y_cordAddr = mem::FindDMAAddy(moduleBase + 0x10f4f4, { 0x38 });
                        float* y_cord = (float*)y_cordAddr;
                        *y_cord = *y_cord + 0.1;

                        uintptr_t x_cordAddr = mem::FindDMAAddy(moduleBase + 0x10f4f4, { 0x34 });
                        float* x_cord = (float*)x_cordAddr;
                        *x_cord = *x_cord - 0.1;
                    }
                    if (*xView >= 270 && *xView <= 360)
                    {
                        uintptr_t y_cordAddr = mem::FindDMAAddy(moduleBase + 0x10f4f4, { 0x38 });
                        float* y_cord = (float*)y_cordAddr;
                        *y_cord = *y_cord - 0.1;

                        uintptr_t x_cordAddr = mem::FindDMAAddy(moduleBase + 0x10f4f4, { 0x34 });
                        float* x_cord = (float*)x_cordAddr;
                        *x_cord = *x_cord - 0.1;
                    }
                }
                
                if (GetAsyncKeyState('S')) //player speed when going backward (continuously add/decrease a value on the player cords when pressing the 'S' key for different viewing angles
                {
                    if (*xView >= 0 && *xView <= 90)
                    {
                        uintptr_t y_cordAddr = mem::FindDMAAddy(moduleBase + 0x10f4f4, { 0x38 });
                        float* y_cord = (float*)y_cordAddr;
                        *y_cord = *y_cord + 0.1;

                        uintptr_t x_cordAddr = mem::FindDMAAddy(moduleBase + 0x10f4f4, { 0x34 });
                        float* x_cord = (float*)x_cordAddr;
                        *x_cord = *x_cord - 0.1;
                    }
                    if (*xView >= 90 && *xView <= 180)
                    {
                        uintptr_t y_cordAddr = mem::FindDMAAddy(moduleBase + 0x10f4f4, { 0x38 });
                        float* y_cord = (float*)y_cordAddr;
                        *y_cord = *y_cord - 0.1;

                        uintptr_t x_cordAddr = mem::FindDMAAddy(moduleBase + 0x10f4f4, { 0x34 });
                        float* x_cord = (float*)x_cordAddr;
                        *x_cord = *x_cord - 0.1;
                    }
                    if (*xView >= 180 && *xView <= 270)
                    {
                        uintptr_t y_cordAddr = mem::FindDMAAddy(moduleBase + 0x10f4f4, { 0x38 });
                        float* y_cord = (float*)y_cordAddr;
                        *y_cord = *y_cord - 0.1;

                        uintptr_t x_cordAddr = mem::FindDMAAddy(moduleBase + 0x10f4f4, { 0x34 });
                        float* x_cord = (float*)x_cordAddr;
                        *x_cord = *x_cord + 0.1;
                    }
                    if (*xView >= 270 && *xView <= 360)
                    {
                        uintptr_t y_cordAddr = mem::FindDMAAddy(moduleBase + 0x10f4f4, { 0x38 });
                        float* y_cord = (float*)y_cordAddr;
                        *y_cord = *y_cord + 0.1;

                        uintptr_t x_cordAddr = mem::FindDMAAddy(moduleBase + 0x10f4f4, { 0x34 });
                        float* x_cord = (float*)x_cordAddr;
                        *x_cord = *x_cord + 0.1;
                    }
                }
            }
        }
        Sleep(5);
    }

    //cleanup & eject
    fclose(f);
    FreeConsole();
    FreeLibraryAndExitThread(hModule, 0);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, nullptr));
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
