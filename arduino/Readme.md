For Uno.ino the intended Setup is to only detach the PWM cable from your Fan connector, and connect Those to Pins 9 and 10 respectively (Uno only has one PWM Timer so only 2 Fans supported), And plug the "remaining" Fan Plug in your Mainboard as usual. Due to the simplicity of this Setup, Schematic Graphic seems overkill.


![Arduino Micro schematics](./micro_schematics.svg)

See the docs at https://afancontrol.readthedocs.io/#pwm-fans-via-arduino
