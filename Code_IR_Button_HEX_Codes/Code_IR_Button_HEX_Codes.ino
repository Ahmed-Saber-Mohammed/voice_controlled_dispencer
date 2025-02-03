 /**********************************************************************************
 *  TITLE: Get the Hex code from any IR remote.
 *  Download the Libraries
 *********************************************************************************/

#include <IRremote.h>
 
int IR_RECV_PIN = A0;  //Update the pin as per circuit
 
IRrecv irrecv(IR_RECV_PIN);
decode_results results;
 
void setup()
{
  Serial.begin(115200);
  irrecv.enableIRIn(); // Start the receiver
}
 
void loop()
{
  if (irrecv.decode(&results))
  {
  Serial.print("0x");
  Serial.println(results.value, HEX); //print the HEX code
  irrecv.resume();
  }
}
