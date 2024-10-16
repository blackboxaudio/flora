#include "daisy_seed.h"

using namespace daisy;

DaisySeed hardware;

int main(void)
{
    bool led_state;
    led_state = true;

    hardware.Configure();
    hardware.Init();

    while(1)
    {
        // Set the onboard LED
        hardware.SetLed(led_state);

        // Toggle the LED state for the next time around.
        led_state = !led_state;

        // Wait 500ms
        System::Delay(125);
    }
}
