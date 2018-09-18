# Pwnsy
Penetration Testing Tool for Windows 7 -> 10

Idea : Peensy (Offensive Security)  
Thanks to : @samratashok for the tiny (~280 characters) PowerShell TCP Client

## What does it do
It allows to execute a reverse shell on any Windows machine with PowerShell enabled within a minimal  
amount of time, and in a stealthy manner. 

## How does it works
It uses the abilities of the Teensy boards to deploy a small powershell script, using keyboard emulation.  
The goal of this project was to find a way to launch a TCP Client that would connect to a netcat listener.  

In order to type and execute the payload, we use the explorer address bar in order to type powershell commands  
that can be longer than 260 chars. That's why we don't use Windows + R "Run" window. 

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

I advice you to run it inside of a loop in bash, so you can exit it with Ctrl+C, so you can reconnect to your client if needed.

Once you recieve a connexion, you can start executing powershell commands / scripts on the client, for example :  
```  
whoami
ipconfig
```  

## Demo : 
[![5 Seconds Reverse Shell](https://img.youtube.com/vi/8NpX56rHsYY/0.jpg)](https://www.youtube.com/watch?v=8NpX56rHsYY)

## Example of Post-Exploitation payload : 
[![Screenshot Retrieval from Reverse Shell](https://img.youtube.com/vi/HPO-l4Br-zY/0.jpg)](https://www.youtube.com/watch?v=HPO-l4Br-zY)

### Notes
Intended for legal usage ONLY, I'm not liable for anything you do with this code.  
Feel free to give feedback and ideas in order to improve it!
