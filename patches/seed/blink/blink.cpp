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
        hardware.SetLed(led_state);

        led_state = !led_state;

        System::Delay(250.0f);
    }
}
