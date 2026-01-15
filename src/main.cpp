#include <Arduino.h>

#include <tm1637_display.h>
#include <timer.h>
#include <hx711_loadcell.h>

#define SCALE_TIME_DELAY 3

// Pins for loadcells
#define RED_DOUT 26
#define RED_SCK 25
#define GREEN_DOUT 22
#define GREEN_SCK 23

// Pins for the 7-segment display
#define CLK 16
#define DIO 17

// Pins for the LED's
const uint8_t GREEN_LEDS[] = {13, 12, 14};
const uint8_t GREEN_LED_COUNT = 3;
const uint8_t RED_LEDS[] = {18, 19, 21};
const uint8_t RED_LED_COUNT = 3;

// Pin for start button
#define START 35

// Initialising sensor classes on the heap
TM1367_Display *display;
hx711_loadcell *green_loadcell;
hx711_loadcell *red_loadcell;

// Initialising all timers
Timer *timer;
Timer *green_puck_timer;
Timer *red_puck_timer;

// Initialising all bools needed for loop logic
volatile bool game_started = false;
bool puck_in_goal = false;

uint8_t green_score = 0;
uint8_t red_score = 0;
uint8_t prev_green_score = 0;
uint8_t prev_red_score = 0;

enum GameState
{
    NOT_STARTED,
    ONGOING,
    PUCK_IN_GOAL,
    GOAL_SCORED,
    FINISHED,
};

const char *game_state_to_string(GameState s)
{
    switch (s)
    {
    case NOT_STARTED:
        return "NOT_STARTED";
    case ONGOING:
        return "ONGOING";
    case PUCK_IN_GOAL:
        return "PUCK_IN_GOAL";
    case GOAL_SCORED:
        return "GOAL_SCORED";
    case FINISHED:
        return "FINISHED";
    default:
        return "UNKNOWN";
    }
}

GameState game_state = NOT_STARTED;
GameState prev_game_state = NOT_STARTED;

/// @brief Method tied to the interrupt on the start button.
void startGame();
/// @brief Handles the scores of both teams. Increments the LEDs based on the score.
void handleScores();
/// @brief Handles the logic of the RED side of the playing field. This method counts the GREEN team's points, so it checks the weight sensor on the RED side.
void handleGreenSideLogic();
/// @brief Handles the logic of the RED side of the playing field. This method counts the GREEN team's points, so it checks the weight sensor on the RED side.
void handleRedSideLogic();
/// @brief Keeps track of the new gameState and prints it to the Serial monitor
void updateGameState();


void setup()
{
    Serial.begin(115200);

    // General initialising for the sensors
    display = new TM1367_Display(CLK, DIO);
    display->showTime(0);
    green_loadcell = new hx711_loadcell(GREEN_DOUT, GREEN_SCK, 110000);
    red_loadcell = new hx711_loadcell(RED_DOUT, RED_SCK, 180000);

    Serial.println("Calibrating red load cell...");
    red_loadcell->calibrate();
    Serial.println("Red load cell calibrated!");

    Serial.println("Calibrating green load cell...");
    green_loadcell->calibrate();
    Serial.println("Green load cell calibrated!");


    timer = new Timer();
    green_puck_timer = new Timer();
    red_puck_timer = new Timer();

    // Set up all LED pins and ensure they are all off
    for (uint8_t i = 0; i < GREEN_LED_COUNT; i++)
    {
        pinMode(GREEN_LEDS[i], OUTPUT);
        digitalWrite(GREEN_LEDS[i], LOW);
    }

    for (uint8_t i = 0; i < RED_LED_COUNT; i++)
    {
        pinMode(RED_LEDS[i], OUTPUT);
        digitalWrite(RED_LEDS[i], LOW);
    }

    Serial.println("Remove the puck, game starting in 5 seconds.");
    delay(2000);
    Serial.println("3");
    delay(1000);
    Serial.println("2");
    delay(1000);
    Serial.println("1");
    delay(1000);
    Serial.println("Game started");
    startGame();
}

void loop()
{
    if (!game_started)
    {
        return;
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
    if (game_started)
    {
        return;
    }

    green_score = 0;
    red_score = 0;

    // Turn off all LEDs
    for (uint8_t i = 0; i < GREEN_LED_COUNT; i++)
    {
        digitalWrite(GREEN_LEDS[i], LOW);
    }
    for (uint8_t i = 0; i < RED_LED_COUNT; i++)
    {
        digitalWrite(RED_LEDS[i], LOW);
    }

    game_started = true;
    timer->start();
    game_state = ONGOING;
}

void handleScores()
{
    if (prev_green_score == green_score && prev_red_score == red_score)
    {
        return;
    }

    // Turn on LEDs for green score
    for (uint8_t score = 0; (score < green_score) && (score < GREEN_LED_COUNT); score++)
    {
        digitalWrite(GREEN_LEDS[score], HIGH);
    }

    // Turn on LEDs for red score
    for (uint8_t score = 0; (score < red_score) && (score < RED_LED_COUNT); score++)
    {
        digitalWrite(RED_LEDS[score], HIGH);
    }

    // Check for game end condition
    if (green_score >= 3 || red_score >= 3)
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
    bool scale_pressed = green_loadcell->isPressed();

    // Phase 1: Waiting for goal (scale pressed, no goal yet)
    if (scale_pressed && !puck_in_goal)
    {
        game_state = PUCK_IN_GOAL;

        if (!green_puck_timer->started())
        {
            // Start timing how long puck is on scale
            green_puck_timer->start();
        }
        else
        {
            // Puck is on the scale and the timer is already running
            if (green_puck_timer->seconds() >= SCALE_TIME_DELAY)
            {
                green_puck_timer->stop();
                green_score++;
                puck_in_goal = true;
                game_state = GOAL_SCORED;
            }
        }
    }
    // Phase 1: Scale released before 1 second - cancel goal attempt
    else if (!scale_pressed && !puck_in_goal && game_state == PUCK_IN_GOAL)
    {
        green_puck_timer->stop();
        game_state = ONGOING;
    }
    // Phase 2: Goal scored, waiting for scale release
    else if (!scale_pressed && puck_in_goal)
    {
        if (!green_puck_timer->started())
        {
            // Start timing how long puck has been off the scale
            green_puck_timer->start();
        }
        else
        {
            // Puck has been off the scale for the required time
            if (green_puck_timer->seconds() >= SCALE_TIME_DELAY)
            {
                puck_in_goal = false;
                green_puck_timer->stop();
                game_state = ONGOING;
            }
        }
    }
    // Phase 2: Scale pressed again during release wait - cancel release timer
    else if (scale_pressed && puck_in_goal)
    {
        green_puck_timer->stop();
        game_state = PUCK_IN_GOAL;
    }
}

void handleRedSideLogic()
{
    bool scale_pressed = red_loadcell->isPressed();

    // Phase 1: Waiting for goal (scale pressed, no goal yet)
    if (scale_pressed && !puck_in_goal)
    {
        game_state = PUCK_IN_GOAL;

        if (!red_puck_timer->started())
        {
            // Start timing how long puck is on scale
            red_puck_timer->start();
        }
        else
        {
            // Puck is on the scale and the timer is already running
            if (red_puck_timer->seconds() >= SCALE_TIME_DELAY)
            {
                red_puck_timer->stop();
                red_score++;
                puck_in_goal = true;
                game_state = GOAL_SCORED;
            }
        }
    }
    // Phase 1: Scale released before 1 second - cancel goal attempt
    else if (!scale_pressed && !puck_in_goal && game_state == PUCK_IN_GOAL)
    {
        red_puck_timer->stop();
        game_state = ONGOING;
    }
    // Phase 2: Goal scored, waiting for scale release
    else if (!scale_pressed && puck_in_goal)
    {
        if (!red_puck_timer->started())
        {
            // Start timing how long puck has been off the scale
            red_puck_timer->start();
        }
        else
        {
            // Puck has been off the scale for the required time
            if (red_puck_timer->seconds() >= SCALE_TIME_DELAY)
            {
                puck_in_goal = false;
                red_puck_timer->stop();
                game_state = ONGOING;
            }
        }
    }
    // Phase 2: Scale pressed again during release wait - cancel release timer
    else if (scale_pressed && puck_in_goal)
    {
        red_puck_timer->stop();
        game_state = PUCK_IN_GOAL;
    }
}

void updateGameState()
{
    if (prev_game_state != game_state)
    {
        Serial.printf("Game state transitioned to %s from %s\n",
                      game_state_to_string(game_state),
                      game_state_to_string(prev_game_state));
        prev_game_state = game_state;
    }
}
