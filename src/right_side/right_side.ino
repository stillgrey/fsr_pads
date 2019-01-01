/*For more information see www.ladyada.net/learn/sensors/fsr.html */

#define BASE_PRESSURE  0

#define LEFT_PRESSURE  375
#define DOWN_PRESSURE  437
#define RIGHT_PRESSURE 380
#define UP_PRESSURE    345
#define SZ             7

#include <Keyboard.h>

int LURD_pins[4] = {0, 1, 2, 3};
int LURD_State[4] = {0, 0, 0, 0};
int LURD_pressures[4] = {LEFT_PRESSURE, UP_PRESSURE, RIGHT_PRESSURE, DOWN_PRESSURE};
char LURD_Keys[5] = "awds";
const unsigned int MAX_INPUT = 50;

double FIR_coeff[SZ] = {-0.008, 0.0928, 0.263, 0.3544, 0.263, 0.0928, -0.008};

double buf[4][10] =  {{0,0,0,0,0,0,0,0,0,0},
                    {0,0,0,0,0,0,0,0,0,0},
                    {0,0,0,0,0,0,0,0,0,0},
                    {0,0,0,0,0,0,0,0,0,0}};

int buf_pos = SZ;

// Calculates non-negative modulo values.
int mod(int a, int b)
{
    int r = a % b;
    return r < 0 ? r + b : r;
}

void setup(void)
{
  Serial.begin(9600);
  Keyboard.begin();
}

//Formats the pad sensitivity output to have 3 digits consistently.
String format_pad_sensitivity (int input)
{
  String retval = "";

  if (input < 100) retval += "0";
  if (input < 10) retval += "0";
  retval += String(input);

  return retval;
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

  // Output the current pad sensitivities.
  String headers[4] = {"L pressure: ,", "U pressure: ,", "R pressure: ,","D pressure: ,"};
  for (int i = 0; i < 4; i++)
  {
    Serial.print(headers[i]);
    Serial.print(format_pad_sensitivity(LURD_pressures[i]));
    Serial.println(",");
  }
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
    case 'E': 
      // The python server will send 'E' constantly and wait for the Arduino
      // to send a response.

      // If Arduino receives the 'E' then it will return an 'E' to signify
      // that their handshake was successful and they are ready to communicate.
      // ______________________________________________________________________
      // Send whatever we have right now along side a garbage information.

        Serial.print("disregard this buffer please. thanks.");
        Serial.flush();
        
      // Send handshake
        Serial.write("E");
        break;
        
    case '\n': // Newline signifies the end of the text
      input_line [input_pos] = 0; // Add null terminator at the end.
      process_data (input_line);

      input_pos = 0; // Reset the input_pos so that the buffer is in the beginning and ready to be written.
      break;

    case '\r': // Discard carriage return. Linux and Windows have different line endings but they both end in '\n.'
               // So, keep it consistent and disregard carriage return.
      break;

    default: // Add each byte to each index of the array until the buffer runs out.
      if (input_pos < (MAX_INPUT - 1)) input_line[input_pos++] = inByte;
      break;
  }
}

int counter = 0;

void loop(void)
{
 counter = (counter + 1) % 10; // Adjust the command polling rate here.

 // If there are commands that needs to processed, process them.
 if (counter == 0)
 {
   if (Serial.available() > 0) 
   {
    int x = Serial.read();
    processIncomingByte(x);
   }
 }

 // Send keyboard input depending on whether the panel is held down or not.
 for (int i = 0; i < 4; i++)
 {
    buf[i][buf_pos] = analogRead(LURD_pins[i]);
    double temp = 0; 
    for (int j = 0; j < SZ; j++) 
    {
      temp += FIR_coeff[j] * buf[i][mod((buf_pos - j), SZ)];
    } 
    buf[i][buf_pos] = temp; 
    
    if (buf[i][buf_pos] > (LURD_pressures[i] + BASE_PRESSURE))
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

 /* FIR print statements
  
 Serial.print(buf[0][buf_pos]);
 Serial.print(", ");
 Serial.print(buf[1][buf_pos]);
 Serial.print(", ");
 Serial.print(buf[2][buf_pos]);
 Serial.print(", ");
 Serial.println(buf[3][buf_pos]);
 */
 buf_pos = mod((buf_pos + 1),  SZ);
 
  
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
