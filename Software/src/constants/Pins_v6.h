#ifndef V6_BOARD_PINS
#define V6_BOARD_PINS

namespace Pins {
        namespace Display {
                // This pin is used by u8g2 to reset the display.
                // Use -1 if this is shared with the board reset pin.
                constexpr int oledReset = -1;

                // Pin used by RGB LED
                constexpr int ledPin = 6;
            }
        namespace Remote {

                constexpr int speedPotPin = 1;

                // This switch occurs when you press the right button in.
                // With the current state of the code this will send out a "ButtonPress"
                // event automatically.
                constexpr int encoderSwitch = 2;

                // The rotary encoder requires at least two pins to function.
                constexpr int encoderA = 41;
                constexpr int encoderB = 42;
                constexpr int encoderPower =
                -1; /* Put -1 of Rotary encoder Vcc is connected directly to 3,3V;
                        else you can use declared output pin for powering rotary
                        encoder */

                constexpr int displayData = 8;
                constexpr int displayClock = 9;
                constexpr int encoderStepsPerNotch = 2;

                constexpr int ethLedRead = 43;
                constexpr int ethLedWrite = 44;
        }
        namespace Driver {
                constexpr int currentSensorPin = 15;

                // Pin that pulses on servo/stepper steps - likely labelled PUL on
                // drivers.
                constexpr int motorStepPin = 14;
                // Pin connected to driver/servo step direction - likely labelled DIR on
                // drivers. N.b. to iHSV57 users - DIP switch #5 can be flipped to
                // invert motor direction entirely
                constexpr int motorDirectionPin = 21;
                // Pin for motor enable - likely labelled ENA on drivers.
                constexpr int motorEnablePin = 47;

                constexpr int motorPedPin = 46;
                constexpr int motorAlarmPin = 3;

        }
        namespace Wifi {
            // Pin for Wi-Fi reset button (optional)
            constexpr int resetPin = 0;
        }
};

#endif
