# A self contained blinking relay circuit

Tonight's tinkering was more a hardware exercise.

Too tired to do this in fritzing, so I'll try to lay it out in text.

I have a suaoki solar generator. It's a 12v battery in a box with an integrated solar charge controller and multiple outputs. On one side, it can output 12v via 5.5mm DC barrel jacks.

On the other side it outputs 5v via USB jacks, for charging cell phones.

I connected the a USB cable from the sauoki to the USB micro input on the esp32 dev board.
Then, I connected the ground from a 5.5mm barrel jack to the ground of a 12v LED strip. I ran the positive line through a 3v3 controlled relay module that supports up to 28v DC @ up to 10 amps (or 10a 125v AC).

I connected the input signal to GPIO 12 of the edp32, VCC to the 3.3v output of the esp32 and GND to GND on the esp2.

Then, I uploaded this sketch.

```cpp
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(12,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(12,HIGH);
  delay(1000);
  digitalWrite(12,LOW);
  delay(1000);
}
```

Not terribly exciting, but this was the result:

![Blinky LED Strip](./blinky-led-strip.gif)

12v will absolutely fry an esp32, so it's nice that we can power larger loads with such a small device. Also means that we can trigger lights (or sirens, or fog machines, or linear actuators, or compressed air solendoids, etc. ad infinitum) remotely, or via a low voltage trigger, such as a reed switch.