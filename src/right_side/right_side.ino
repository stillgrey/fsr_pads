/*For more information see www.ladyada.net/learn/sensors/fsr.html */

#define BASE_PRESSURE  0

#define LEFT_PRESSURE  375       
#define DOWN_PRESSURE  437
#define RIGHT_PRESSURE 380
#define UP_PRESSURE    345

#include <Keyboard.h>
 
int LURD_pins[4] = {0, 1, 2, 3};
int LURD_State[4] = {0, 0, 0, 0};
int LURD_pressures[4] = {LEFT_PRESSURE, UP_PRESSURE, RIGHT_PRESSURE, DOWN_PRESSURE};
char LURD_Keys[5] = "awds";
const unsigned int MAX_INPUT = 50;

void setup(void) 
{
  Serial.begin(9600);  
  Keyboard.begin();
}

// Adjust sensitivity according to the serial command sent from the web app.
void process_data (char * data)
{

  //TODO: Remove this line. This line is unnecessary since the data comes with null terminator
  //      before this function gets called. 
  data[4] = 0;

  // Check to see if the first byte is either 0,1,2,3.
  // If so, it's a pad sensitivity adjustment command.
  if (data[0] - 48 < 5)
  {
   LURD_pressures[data[0] - 48] = atoi((const char *) &(data[1]));
  }
  
  Serial.print("L pressure: ,");
  if (LURD_pressures[0] < 100)
  {
      Serial.print("0");
  }
  if (LURD_pressures[0] < 10)
  {
      Serial.print("0");
  }
  Serial.print(LURD_pressures[0]);
  Serial.println(",");
  
  Serial.print("U pressure: ,");
  if (LURD_pressures[1] < 100)
  {
      Serial.print("0");
  }
  if (LURD_pressures[1] < 10)
  {
      Serial.print("0");
  }
  Serial.print(LURD_pressures[1]);
  Serial.println(",");
  
  Serial.print("R pressure: ,");
  if (LURD_pressures[2] < 100)
  {
      Serial.print("0");
  }
  if (LURD_pressures[2] < 10)
  {
      Serial.print("0");
  }
  Serial.print(LURD_pressures[2]);
  Serial.println(",");
  
  Serial.print("D pressure: ,");
  if (LURD_pressures[3] < 100)
  {
      Serial.print("0");
  }
  if (LURD_pressures[3] < 10)
  {
      Serial.print("0");
  }
  Serial.print(LURD_pressures[3]);
  Serial.println(",");
  Serial.println("");
}

// Puts the serial commands in an array, terminates the commands with a null terminator
// and sends it off to be processed.
  
void processIncomingByte (const byte inByte)
{
  static char input_line [MAX_INPUT];
  static unsigned int input_pos = 0;

  switch (inByte)
  {
    case '\n':   // Newline signifies the end of the text
      input_line [input_pos] = 0;  // Add null terminator at the end.
      process_data (input_line);
      
      input_pos = 0; // Reset the input_pos so that the buffer is in the beginning and ready to be written. 
      break;

    case '\r':   // Discard carriage return. Linux and Windows have different line endings but they both end in '\n' so 
      break;

    default:
      if (input_pos < (MAX_INPUT - 1))
        input_line [input_pos++] = inByte;
      break;
  }    
} 

int counter = 0;
void loop(void) {
 
 counter = (counter + 1) % 10; // Adjust the command polling rate here.
 
 // If there are commands that needs to processed, process them.
 if (counter == 0)
 {
   if (Serial.available() > 0)
   {
     processIncomingByte(Serial.read());
   }
 }

 // Send keyboard input depending on whether the panel is held down or not.
 for (int i = 0; i < 4; i++)
 {
     if (analogRead(LURD_pins[i]) > (LURD_pressures[i] + BASE_PRESSURE))
     {
      if (LURD_State[i] == 0)
      {
        Keyboard.press(LURD_Keys[i]);
        LURD_State[i] = 1;
      }
     }
     
     else
     {
      if (LURD_State[i] == 1)
      {
        Keyboard.release(LURD_Keys[i]);
        LURD_State[i] = 0;
      }
     }
 }
  /*
   * Debugging:
  fsrReading = analogRead(0);
  Serial.print("Analog reading (L)= ");
  Serial.println(fsrReading);
 
  fsrReading = analogRead(1);
  Serial.print("Analog reading (U)= ");
  Serial.println(fsrReading);

  fsrReading = analogRead(2);
  Serial.print("Analog reading (R)= ");
  Serial.println(fsrReading);

  fsrReading = analogRead(3);
  Serial.print("Analog reading (D)= ");
  Serial.println(fsrReading);
 */
}
