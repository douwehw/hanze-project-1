#include <Arduino.h>
#include <vector>

#include <tm1637_display.h>
#include <timer.h>
#include <hx711_loadcell.h>

// Pins for loadcells
#define RED_DOUT 26
#define RED_SCK 25
#define GREEN_DOUT 2
#define GREEN_SCK 4

// Pins for the 7-segment display
#define CLK 17
#define DIO 16

// Pins for the LED's
const std::vector<uint8_t> *RED_LEDS = new std::vector<uint8_t>{18, 19, 21};
const std::vector<uint8_t> *GREEN_LEDS = new std::vector<uint8_t>{13, 12, 14};

// Pins for IR emitter/reciever
#define RED_IR_R 34
#define RED_IR_E 35
#define GREEN_IR_R 32
#define GREEN_IR_E 33

// Pin for start button
#define START 23

// Initialising sensor classes on the heap
TM1367_Display *display;
hx711_loadcell *red_loadcell;
hx711_loadcell *green_loadcell;

// Initialising all timers
Timer *timer;
Timer *beam_to_scale;
Timer *puck_on_scale;

// Initialising all bools needed for loop logic
bool beam_broken, puck_in_goal, game_started = false;

uint8_t red_score, green_score = 0;

void setup()
{
    Serial.begin(115200);

    // General initialising for the sensors
    display = new TM1367_Display(CLK, DIO);
    red_loadcell = new hx711_loadcell(RED_DOUT, RED_SCK);
    green_loadcell = new hx711_loadcell(GREEN_DOUT, GREEN_SCK);

    timer = new Timer();
    beam_to_scale = new Timer();
    puck_on_scale = new Timer();

    // Set up all LED pins and ensure they are all off
    for (const uint8_t pin : *RED_LEDS)
    {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, LOW);
    }
    for (const uint8_t pin : *GREEN_LEDS)
    {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, LOW);
    }

    // Setting the IR emitters on and initialising the recievers
    pinMode(RED_IR_R, INPUT);
    pinMode(RED_IR_E, OUTPUT); // could be permanently bound to 3.3v?
    digitalWrite(RED_IR_E, HIGH);

    pinMode(GREEN_IR_R, INPUT);
    pinMode(GREEN_IR_E, OUTPUT); // could be permanently bound to 3.3v?
    digitalWrite(GREEN_IR_E, HIGH);

    // Initialising the start button
    pinMode(START, INPUT);
}

void loop()
{
    // Check if the start button has been pressed
    if (!game_started)
    {
        if (digitalRead(START))
        {
            game_started = true;
        }
        return;
    }

    if (!timer->started())
    {
        timer->start();
    }

    display->showTime(static_cast<int>(timer->seconds()));

    // Green side loop
    if (digitalRead(GREEN_IR_R) == true)
    {
        if (!beam_to_scale->started())
        {
            beam_to_scale->start();
        }
        beam_broken = true;
    }

    if (beam_broken)
    {
        if (green_loadcell->isPressed())
        {
            if (!puck_on_scale->started())
            {
                puck_on_scale->start();
            }
            puck_in_goal = true;
        }

        if (beam_to_scale->seconds() > 3)
        {
            beam_to_scale->stop();
            beam_broken = false;
        }
    }

    if (puck_in_goal)
    {
        if (puck_on_scale->seconds() > 1)
        {
            green_score++;
            puck_in_goal = false;
            beam_to_scale->stop();
            puck_on_scale->stop();
            beam_broken = false;
        }
    }

    for (uint8_t score = 0; score < green_score; score++)
    {
        digitalWrite(GREEN_LEDS->at(score), HIGH);
    }

    if (green_score == 3)
    {
        timer->pause();
        game_started = false;
    }
}
// Red side loop
