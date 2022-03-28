/*

  Copyright (c) 2022, Steven Chesser
  All rights reserved.

  This source code is licensed under the BSD-style license found in the
  LICENSE file in the root directory of this source tree.

*/

/* Define some constants for LED, buttons, relays, ect */

int pushbuttonLED = 0;
int brakeswitchLED = 1;
int pushbutton = 2;
int brakebutton = 3;
int relayPIN = 7;
int potPIN = A6;

/* Define variables */

int blinkcode = 0;
bool buttonpressed = false;
bool brakepressed = false;
int potvalue = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("Smartish Lag Switch");

  pinMode(pushbutton, INPUT);
  pinMode(brakebutton, INPUT);
  pinMode(pushbuttonLED, OUTPUT);
  pinMode(brakeswitchLED, OUTPUT);
  pinMode(relayPIN, OUTPUT);
  pinMode(potPIN, INPUT);

  digitalWrite(pushbuttonLED, LOW); // turn led off
  digitalWrite(brakeswitchLED, LOW); // turn led off
  digitalWrite(pushbutton, HIGH); //activate arduino internal pull up
  digitalWrite(brakebutton, HIGH); //activate arduino internal pull up
  digitalWrite(relayPIN, HIGH); // make sure IAT sensor connected
}

void loop() {



  potvalue = analogRead(potPIN); // read in POT knob

  if (potvalue < 0 ) potvalue = 0; // if oddly under zero... make it zero.

  if (potvalue > 0 ) {
    potvalue = (round(potvalue / 100) * 100); // if potvalue > 0 lets turn this potvalue into a 1/10th second potvalue. 100ms increments.
    Serial.print("IAT Re-enabled time in ms : ");
    Serial.println(potvalue);
  }

  if ((digitalRead(pushbutton) == LOW) and (brakepressed == false) and (buttonpressed == false)) { // Driver presses PUSH Button and holds it to get blink code of how long POT potvalue was set to.
    digitalWrite(pushbuttonLED, LOW);
    delay(250); // 1/4 second delay while this button is down.
    Serial.print("PUSHED ");
    blinkcode++;
    Serial.println(blinkcode);
    return;
  }

  if ( (blinkcode >= 4) and (potvalue > 0) ) { // Driver wants to know how long POT potvalue was set to.
    Serial.println("Showing countdown");
    digitalWrite(pushbuttonLED, LOW);
    for (int x = 0; (x < ( potvalue / 100)); x++) {
      Serial.println(x);
      digitalWrite(pushbuttonLED, HIGH);
      delay(500);
      digitalWrite(pushbuttonLED, LOW);
      delay(500);
    }
    blinkcode = 0;
  }


  if (digitalRead(brakebutton) == HIGH) { // Driver holding down BRAKES while staging.
    Serial.println("");
    Serial.println("Brake button enaged. Waiting for brake to be released!");
    Serial.println("");
    digitalWrite(brakeswitchLED, HIGH); // turn the LED on
    delay(250); // 1/4 second delay for message
    brakepressed = true;
  } else
  {
    Serial.println(""); // Driver let off the BRAKES.
    Serial.println("Brake button NOT enaged. Waiting for brake to be pressed!");
    Serial.println("");
    digitalWrite(brakeswitchLED, LOW); // turn the LED off
    delay(250); // 1/4 second delay for message
    brakepressed = false;
  }


  // Once Driver has brake down, the driver can press the IAT push button to cut off IAT SENSOR

  if ((digitalRead(pushbutton) == LOW) and (brakepressed == true)) { // Driver presses PUSH Button
    Serial.println("");
    Serial.println("Push button enaged. Waiting for brake switch!");
    Serial.println("");
    Serial.println("IAT SENSOR HAS BEEN DISCONNECTED!");
    Serial.println("");

    digitalWrite(pushbuttonLED, HIGH);
    delay(250); // 1/4 second delay for message
    digitalWrite(relayPIN, LOW); // IAT now is disconnected!
    buttonpressed = true;
  }


  // Driver is ready. Brakes are down. IAT is cut. Lets make boost and then go!

  if ( (buttonpressed == true) and (brakepressed == true) ) {
    Serial.println("");
    Serial.println("Push button was engaged and brake is currently down.  Let off brake to go!");
    delay(250); // 1/4 second delay for message

    while (1 == 1) {
      delay(1); // 1ms delay for looping

      if (digitalRead(brakebutton) == LOW) { // Driver let go of brakes! Lets zoom zoom!
        Serial.println("");
        Serial.println("Brake has been let go of. Time to go!");
        Serial.println("IAT Sensor enabled!");
        Serial.println("");

        if (potvalue > 0 ) { // if we had A delay potvalue set by the POT knob, then lets wait....
          Serial.print("Waiting ");
          Serial.print(potvalue);
          Serial.print(" milliseconds before enabling IAT SENSOR.");
          Serial.println("");
          Serial.println("");
          delay(potvalue);
        }

        digitalWrite(pushbuttonLED, LOW); // turn led off
        digitalWrite(brakeswitchLED, LOW); // turn led off
        digitalWrite(relayPIN, HIGH); // make sure IAT now is connected. ZOOM ZOOM TIME!

        // reset vars
        buttonpressed = false;
        brakepressed = false;
        blinkcode = 0;
        break; // break out of this crazy loop
      }
    }
  }
}
