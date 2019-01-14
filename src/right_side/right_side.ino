/*For more information see www.ladyada.net/learn/sensors/fsr.html */

#define BASE_PRESSURE  0

#define LEFT_PRESSURE  420
#define DOWN_PRESSURE  560
#define RIGHT_PRESSURE 350
#define UP_PRESSURE    650

#define LOWER_LIMIT_PRESSURE 400

//Times in ms
#define DELAY_TIME 0.25
#define DEBOUNCE_TIME 10

#include <Keyboard.h>

int LURD_pins[4] = {0, 1, 2, 3};
int LURD_State[4] = {0, 0, 0, 0};
int LURD_pressures[4] = {LEFT_PRESSURE, UP_PRESSURE, RIGHT_PRESSURE, DOWN_PRESSURE};
char LURD_Keys[5] = "awds";
const unsigned int MAX_INPUT = 50;

long long last_keypress_time = 0;
long long last_state_change_time[4] = {0, 0, 0, 0};

void setup(void)
{
  Serial.begin(9600);
  Keyboard.begin();
}

// Adjust sensitivity according to the serial command sent from the web app.
void process_data (char* data)
{
  //  TODO: Remove this line. This line is unnecessary since the data comes
  //        with null terminator before this function gets called.
  data[4] = 0;

  // Check to see if the first byte is either 0,1,2,3.
  // If so, it's a pad sensitivity adjustment command.
  if (data[0] - 48 < 5)
  {
    LURD_pressures[data[0] - 48] = atoi((const char *) &(data[1]));
  }

  // Output the current pad sensitivities.
  char* headers[4] = {"L pressure: ,", "U pressure: ,",
                      "R pressure: ,","D pressure: ,"};
  char pressure_string_buf[4];

  for (int i = 0; i < 4; i++)
  {
    sprintf(pressure_string_buf, "%03d", LURD_pressures[i]);

    Serial.print(headers[i]);
    Serial.print(&(pressure_string_buf[0]));
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

  long long current_time = micros();

  // Read keyboard input depending on whether the panel is held down or not.
  for (int i = 0; i < 4; i++)
  {
    int sensor_value = analogRead(LURD_pins[i]);

    // Not making it a float is intentional here.
    int state_change_time_diff_in_ms = (current_time - last_state_change_time[i]) / 1000;

    if (sensor_value > (LURD_pressures[i] + BASE_PRESSURE))
    {
      if ((LURD_State[i] == 0) &&
          (state_change_time_diff_in_ms > DEBOUNCE_TIME))
      {
        LURD_State[i] = 1;
        last_state_change_time[i] = current_time;
      }
    }

   else if ((sensor_value < LOWER_LIMIT_PRESSURE) &&
            (state_change_time_diff_in_ms > DEBOUNCE_TIME))
   {
     if (LURD_State[i] == 1)
     {
       LURD_State[i] = 0;
       last_state_change_time[i] = current_time;
     }
   }
 }

  current_time = micros();

  if ((current_time - last_keypress_time) > (DELAY_TIME * 1000))
  {
    for (int i = 0; i < 4; i++)
    {
      if (LURD_State[i] == 1) Keyboard.press(LURD_Keys[i]);
      else Keyboard.release(LURD_Keys[i]);
    }

    last_keypress_time = current_time;
  }
}
