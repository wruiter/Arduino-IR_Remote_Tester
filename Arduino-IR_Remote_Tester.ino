//------------------------------------------------------------------------------
// Include the IRremote library header
//
#include <IRremote.h>

// include the library code:
#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 10, d0 = 9, d1 = 8, d2 = 7, d3 = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d0, d1, d2, d3, d4, d5, d6, d7);

//------------------------------------------------------------------------------
// Tell IRremote which Arduino pin is connected to the IR Receiver (TSOP4838)
//
int recvPin = 11;
IRrecv irrecv(recvPin);

//+=============================================================================
// Configure the Arduino
//
void  setup ( )
{
   // set up the LCD's number of columns and rows:
  lcd.begin(20, 4);
  delay(100);
  lcd.setCursor(0, 0);  lcd.print("   Remote control   ");
  lcd.setCursor(0, 1);  lcd.print("       Tester       ");
  lcd.setCursor(0, 2);  lcd.print("   Omega Software   ");
  lcd.setCursor(0, 3);  lcd.print("       Service      ");
  Serial.begin(9600);   // Status message will be sent to PC at 9600 baud
  irrecv.enableIRIn();  // Start the receiver
  

  
}

//+=============================================================================
// Display IR code
//
void  ircode (decode_results *results)
{
  // Panasonic has an Address
  if (results->decode_type == PANASONIC) {
    Serial.print(results->address, HEX);
    Serial.print(":");
  }

  // Print Code
  Serial.print(results->value, HEX);
  lcd.setCursor(0, 2);  lcd.print("                    ");
  lcd.setCursor(0, 2);  lcd.print("Code (");
  lcd.setCursor(6, 2);  lcd.print(results->bits, DEC);  lcd.print(" bits) ");
  lcd.setCursor(0, 3);  lcd.print("                    ");
  lcd.setCursor(0, 3);  lcd.print(results->value, HEX);

}

//+=============================================================================
// Display encoding type
//
void  encoding (decode_results *results)
{
     String Encoding; 
  switch (results->decode_type) {

    default:
    case UNKNOWN:      Encoding = "UNKNOWN";       break ;
    case NEC:          Encoding = "NEC";           break ;
    case SONY:         Encoding = "SONY";          break ;
    case RC5:          Encoding = "RC5";           break ;
    case RC6:          Encoding = "RC6";             break ;
    case DISH:         Encoding = "DISH";          break ;
    case SHARP:        Encoding = "SHARP";         break ;
    case JVC:          Encoding = "JVC";           break ;
    case SANYO:        Encoding = "SANYO";         break ;
    case MITSUBISHI:   Encoding = "MITSUBISHI";    break ;
    case SAMSUNG:      Encoding = "SAMSUNG";       break ;
    case LG:           Encoding = "LG";            break ;
    case WHYNTER:      Encoding = "WHYNTER";       break ;
    case AIWA_RC_T501: Encoding = "AIWA_RC_T501";  break ;
    case PANASONIC:    Encoding = "PANASONIC";     break ;
    case DENON:        Encoding = "Denon";         break ;
  }
  Serial.print(Encoding);
  lcd.setCursor(0, 1);  lcd.print("                    ");
  lcd.setCursor(0, 1);  lcd.print("Encoding :");
  lcd.setCursor(11, 1);  lcd.print(Encoding);

}

//+=============================================================================
// Dump out the decode_results structure.
//
void  dumpInfo (decode_results *results)
{
  // Check if the buffer overflowed
  if (results->overflow) {
    Serial.println("IR code too long. Edit IRremoteInt.h and increase RAWBUF");
    return;
  }

  // Show Encoding standard
  Serial.print("Encoding  : ");
  encoding(results);
  Serial.println("");

  // Show Code & length
  Serial.print("Code      : ");
  ircode(results);
  Serial.print(" (");
  Serial.print(results->bits, DEC);
  Serial.println(" bits)");
}

//+=============================================================================
// Dump out the decode_results structure.
//
void  dumpRaw (decode_results *results)
{
  // Print Raw data
  Serial.print("Timing[");
  Serial.print(results->rawlen-1, DEC);
  Serial.println("]: ");

  for (int i = 1;  i < results->rawlen;  i++) {
    unsigned long  x = results->rawbuf[i] * USECPERTICK;
    if (!(i & 1)) {  // even
      Serial.print("-");
      if (x < 1000)  Serial.print(" ") ;
      if (x < 100)   Serial.print(" ") ;
      Serial.print(x, DEC);
    } else {  // odd
      Serial.print("     ");
      Serial.print("+");
      if (x < 1000)  Serial.print(" ") ;
      if (x < 100)   Serial.print(" ") ;
      Serial.print(x, DEC);
      if (i < results->rawlen-1) Serial.print(", "); //',' not needed for last one
    }
    if (!(i % 8))  Serial.println("");
  }
  Serial.println("");                    // Newline
}

//+=============================================================================
// Dump out the decode_results structure.
//
void  dumpCode (decode_results *results)
{
  // Start declaration
  Serial.print("unsigned int  ");          // variable type
  Serial.print("rawData[");                // array name
  Serial.print(results->rawlen - 1, DEC);  // array size
  Serial.print("] = {");                   // Start declaration

  // Dump data
  for (int i = 1;  i < results->rawlen;  i++) {
    Serial.print(results->rawbuf[i] * USECPERTICK, DEC);
    if ( i < results->rawlen-1 ) Serial.print(","); // ',' not needed on last one
    if (!(i & 1))  Serial.print(" ");
  }

  // End declaration
  Serial.print("};");  // 

  // Comment
  Serial.print("  // ");
  encoding(results);
  Serial.print(" ");
  ircode(results);

  // Newline
  Serial.println("");

  // Now dump "known" codes
  if (results->decode_type != UNKNOWN) {

    // Some protocols have an address
    if (results->decode_type == PANASONIC) {
      Serial.print("unsigned int  addr = 0x");
      Serial.print(results->address, HEX);
      Serial.println(";");
    }

    // All protocols have data
    Serial.print("unsigned int  data = 0x");
    Serial.print(results->value, HEX);
    Serial.println(";");
  }
}

//+=============================================================================
// The repeating section of the code
//
void  loop ( )
{
  decode_results  results;        // Somewhere to store the results

  if (irrecv.decode(&results)) {  // Grab an IR code
    lcd.setCursor(0, 0);  lcd.print("                    ");
    lcd.setCursor(0, 0);  lcd.print("Receiving");
    dumpInfo(&results);           // Output the results
    dumpRaw(&results);            // Output the results in RAW format
    dumpCode(&results);           // Output the results as source code
    Serial.println("");           // Blank line between entries
    irrecv.resume();              // Prepare for the next value
    lcd.setCursor(0, 0);  lcd.print("Waiting for next... ");
  }
}
