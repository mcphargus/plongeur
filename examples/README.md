# simple wifi example

This example borrows from the great tutorials over at [random nerd tutorials](https://randomnerdtutorials.com/projects-esp32/).

## config-wifi.cpp

This project uses the Preferences library to set a key value pair in non-volatile memory. This way we can store our password by uploading a sketch once, and then our regular sketch can do the heavy lifting.

## mqtt-led-example.cpp

- Gathers credentials from preferences nvm
- Uses those credentials to connect to a wifi network
- sets the system time from an ntp server
- subscribes to an mqtt topic
- listens for "on" or "off" on the mqtt topic, and sets the onboard LED accordingly