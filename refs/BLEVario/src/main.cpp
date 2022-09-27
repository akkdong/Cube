// main.cpp
//

#include <Arduino.h>
#include "BLEVario.h"

#define POWER_ON		4

#define BTN_SEL			26
#define BTN_LEFT		34
#define BTN_RIGHT		35
#define BTN_UP			5
#define BTN_DOWN		2

// button
typedef struct _key_map
{
	uint8_t		pin;
	uint8_t		active; // 0: ACTIVE_LOW, others: ACTIVE_HIGH
	uint8_t	    key;
    uint8_t     state;  // 0: release, others: press
} key_map;


static key_map _m1[] =
{
	{ BTN_SEL, 		0, 	KEY_RETURN, 0 },
	{ BTN_LEFT, 	1, 	KEY_LEFT_ARROW, 0 },
	{ BTN_RIGHT, 	1, 	KEY_RIGHT_ARROW, 0 },
	{ BTN_UP, 		1, 	KEY_UP_ARROW, 0 },
	{ BTN_DOWN, 	1, 	KEY_DOWN_ARROW, 0 },
};

BLEVario bleDevice;

void setup() {
	// initialize pin-mode
	pinMode(POWER_ON, OUTPUT);
	pinMode(BTN_SEL, INPUT);
	pinMode(BTN_LEFT, INPUT);
	pinMode(BTN_RIGHT, INPUT);
	pinMode(BTN_UP, INPUT);	
	pinMode(BTN_DOWN, INPUT);

    digitalWrite(POWER_ON, HIGH);

    //
    Serial.begin(115200);
    bleDevice.begin();
}

void loop() {
    //
    while (Serial.available())
    {
        int ch = Serial.read();
        bleDevice.write(ch);
    }

    while (bleDevice.available())
    {
        int ch = bleDevice.read();
        Serial.write(ch);
    }

    if (bleDevice.isConnected())
    {
        for (size_t i = 0; i < sizeof(_m1) / sizeof(_m1[0]); i++)
        {
            key_map* kmap = &_m1[i];
            int value = digitalRead(kmap->pin);
            uint8_t state = 0; // release
            if ((kmap->active == 0 && value == 0) || (kmap->active != 0 && value != 0))
                state = 1; // press

            if (kmap->state != state)
            {
                if (state)
                {
                    bleDevice.press(kmap->key);
                    Serial.printf("Press: %02X\n", kmap->key);
                }
                else
                {
                    bleDevice.release(kmap->key);
                    Serial.printf("Release: %02X\n", kmap->key);
                }

                kmap->state = state;
            }
        }
    }
}
