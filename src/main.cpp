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
const std::vector<uint8_t> *GREEN_LEDS = new std::vector<uint8_t>{13, 12, 14};
const std::vector<uint8_t> *RED_LEDS = new std::vector<uint8_t>{18, 19, 21};

// Pin for start button
#define START 23

// Initialising sensor classes on the heap
TM1367_Display *display;
hx711_loadcell *green_loadcell;
hx711_loadcell *red_loadcell;

// Initialising all timers
Timer *timer;
Timer *puck_on_scale_timer;

// Initialising all bools needed for loop logic
volatile bool game_started = false;
bool puck_in_goal = false;

uint8_t green_score, red_score = 0;
uint8_t prev_green_score, prev_red_score = 0;

enum GameState
{
    NOT_STARTED,
    ONGOING,
    PUCK_IN_GOAL,
    GOAL_SCORED,
    FINISHED,
};

GameState game_state = NOT_STARTED;
GameState prev_game_state = game_state;

void startGame();
void handleScores();
void handleGreenSideLogic();
void handleRedSideLogic();
void updateGameState();
void disableLEDs();

void setup()
{
    Serial.begin(115200);

    // General initialising for the sensors
    display = new TM1367_Display(CLK, DIO);
    green_loadcell = new hx711_loadcell(GREEN_DOUT, GREEN_SCK, 150000); //}
    red_loadcell = new hx711_loadcell(RED_DOUT, RED_SCK, 150000);       //} one of these needs to be calibrated to ~120k and the other to ~200k

    timer = new Timer();
    puck_on_scale_timer = new Timer();

    // Set up all LED pins and ensure they are all off
    for (const uint8_t pin : *GREEN_LEDS)
    {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, LOW);
    }
    for (const uint8_t pin : *RED_LEDS)
    {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, LOW);
    }

    // Initialising the start button
    pinMode(START, INPUT);

    attachInterrupt(START, startGame, RISING); // no matter when you press the button, it will always start the game
}

void loop()
{
    if (!game_started)
    { // do nothing if the game hasn't started
        return;
    }

    if (!timer->started())
    { // start a timer, the game has started
        timer->start();
        game_state = ONGOING;
    }

    // cast the timer's seconds to int to get rid of the floating point
    display->showTime(static_cast<int>(timer->seconds()));

    handleGreenSideLogic();
    handleRedSideLogic();

    handleScores();

    updateGameState();
}

void startGame()
{
    for (const uint8_t pin : *GREEN_LEDS)
    {
        digitalWrite(pin, LOW);
    }
    for (const uint8_t pin : *RED_LEDS)
    {
        digitalWrite(pin, LOW);
    }

    game_started = true;
}

void handleScores()
{
    if (prev_green_score == green_score && prev_red_score == red_score)
    {
        return;
    }

    for (uint8_t score = 0; score < green_score; score++)
    {
        digitalWrite(GREEN_LEDS->at(score), HIGH);
    }

    for (uint8_t score = 0; score < red_score; score++)
    {
        digitalWrite(RED_LEDS->at(score), HIGH);
    }

    if (green_score == 3 || red_score == 3)
    {
        timer->pause();
        game_started = false;
        game_state = FINISHED;
    }

    prev_green_score = green_score;
    prev_red_score = red_score;
}

void handleGreenSideLogic()
{
    if (green_loadcell->isPressed() && !puck_in_goal)
    { // Scale tipped
        game_state = PUCK_IN_GOAL;

        if (!puck_on_scale_timer->started())
        { // start timing how long puck is on scale
            puck_on_scale_timer->start();
        }
        else
        { // Puck is on the scale and the timer is already running
            if (puck_on_scale_timer->seconds() >= 1.0)
            {
                puck_in_goal = true;
                puck_on_scale_timer->stop();
                green_score++;
                game_state = GOAL_SCORED;
            }
            else
            { // It hasn't been a full second since the puck has been on the scale
                if (!green_loadcell->isPressed())
                {
                    puck_on_scale_timer->stop();
                    game_state = ONGOING;
                }
            }
        }
    }

    if (puck_in_goal && !green_loadcell->isPressed())
    { // Scale no longer tipped
        if (!puck_on_scale_timer->started())
        { // yes i am reusing the scale timer to check for how long it has not been on the timer
            puck_on_scale_timer->start();
        }
        else
        { // puck isn't on the scale anymore and the timer is already running
            if (puck_on_scale_timer->seconds() >= 1.0)
            {
                puck_in_goal = false;
                puck_on_scale_timer->stop();

                game_state = ONGOING;
            }
            else
            { // It hasn't been a full second since the puck has been off the scale
                if (green_loadcell->isPressed())
                {
                    puck_on_scale_timer->stop();
                    game_state = PUCK_IN_GOAL;
                }
            }
        }
    }
}

void handleRedSideLogic()
{
    if (red_loadcell->isPressed() && !puck_in_goal)
    { // Scale tipped
        game_state = PUCK_IN_GOAL;

        if (!puck_on_scale_timer->started())
        { // start timing how long puck is on scale
            puck_on_scale_timer->start();
        }
        else
        { // Puck is on the scale and the timer is already running
            if (puck_on_scale_timer->seconds() >= 1.0)
            {
                puck_in_goal = true;
                puck_on_scale_timer->stop();
                red_score++;
                game_state = GOAL_SCORED;
            }
            else
            { // It hasn't been a full second since the puck has been on the scale
                if (!red_loadcell->isPressed())
                {
                    puck_on_scale_timer->stop();
                    game_state = ONGOING;
                }
            }
        }
    }

    if (puck_in_goal && !red_loadcell->isPressed())
    { // Scale no longer tipped
        if (!puck_on_scale_timer->started())
        { // yes i am reusing the scale timer to check for how long it has not been on the timer
            puck_on_scale_timer->start();
        }
        else
        { // puck isn't on the scale anymore and the timer is already running
            if (puck_on_scale_timer->seconds() >= 1.0)
            {
                puck_in_goal = false;
                puck_on_scale_timer->stop();
                game_state = ONGOING;
            }
            else
            { // It hasn't been a full second since the puck has been off the scale
                if (red_loadcell->isPressed())
                {
                    puck_on_scale_timer->stop();
                    game_state = PUCK_IN_GOAL;
                }
            }
        }
    }
}

void updateGameState()
{
    if (prev_game_state != game_state)
    {
        Serial.printf("Game state transitioned to %s from %s", game_state, prev_game_state);
        prev_game_state = game_state;
    }
}
