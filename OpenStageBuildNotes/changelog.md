# Changes since initial release

## 2021-14-18

The Sparkfun quad stepper board has been discontinued, so in this revision it has been replaced with four "Step Stick" (aka Pololu Carrier) drivers. These are available from various sources with a range of driver ICs, such as the cheap [A4988](https://www.pololu.com/product/1182), more efficient [DR8825](https://www.pololu.com/product/2133) or the quiet-running [TMC2100](https://uk.farnell.com/trinamic/tmc-silentstepstick/stepper-driver-board-2-phase-motor/dp/2822153) (silent step stick). The pinout of these modules are the same.

At the same time the connectors for the motors have been changed from DB-9 (two per motor) to M8 circular connectors (one per motor). The PCB/panel mount connector is the female [Amphenol M8S-04PFFR-SF8001](https://www.mouser.co.uk/ProductDetail/Amphenol-LTW/M8S-04PFFR-SF8001/) and the cable assembly is the male 
[8A-04AMMM-SL7A01](https://www.mouser.co.uk/ProductDetail/Amphenol-LTW/8A-04AMMM-SL7A01/). The genders of these can be swapped if desired, as the PCB footprint is the same for the male version of the Amphenol connector.

Extra capacitors are neeed for the stepper drivers. [100uF 16V radial, 5mm diameter, 2.5mm lead spacing](https://www.mouser.co.uk/ProductDetail/Panasonic/ECA-1CM101I/)