#include "daisy_seed.h"

using namespace daisy;

DaisySeed hardware;

int main(void)
{
    bool ledState;
    ledState = true;

    hardware.Configure();
    hardware.Init();

    while (1) {
        hardware.SetLed(ledState);
        ledState = !ledState;

        System::Delay(250.0f);
    }
}
