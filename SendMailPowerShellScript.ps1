Param([String]$attachment, [String]$subject, [String]$body)

Function sendMail {
    Param( [Parameter(` Mandatory=$true)] [String]$to, [Parameter(` Mandatory=$true)] [String]$from, 
           [Parameter(` Mandotory=$true)] [String]$password, [Parameter(` Mandatory=$true)] [String]$subject,
           [Parameter(` Mandatory=$true)] [String]$attachment
          )

    try {
        $msg = New-Object System.Net.Mail.MailMessage($from, $to, $subject, $body)
        $server = "smtp.gmail.com"
        
        if($attachment -ne $null) {
            try {
                $attachments = $attachment -split ("\:\:");
                ForEach($val in $attachments) {
                    $newAttachment = New-Object System.Net.Mail.Attachment($val)
                    $msg.Attachments.Add($newAttachment)
                }   
            } catch {
                exit 2;
            }  

            $client = New-Object Net.Mail.smtpClient($server, 587)
            $client.EnableSsl = $true
            $client.Credentials = New-Object System.Net.NetworkCredential($from.split("@")[0], $password)
            $client.Send($msg)

            Remove-Variable -Name client
            Remove-Variable -Name password
            exit 7;

        }   
    } catch {
        exit 3;
    }
}

try {
    sendMail
        -attachment $attachment
        -to "Recipient Email Address"
        -body $body
        -subject $subject
        -password "MyPassword"
        -from "Sender Email Address"
} catch {
    exit 4;
}