
OpenStage microscope stage controller for Arduino


This is a hardware and software solution for cheaply building a motorized microscope stage. We have successfully worked with OpenStage on a 2-photon imaging system for many months. The characteristics of the system are described in our PlosONE paper (http://www.plosone.org/article/info%3Adoi%2F10.1371%2Fjournal.pone.0088977). The three-axis system described in that paper cost us about $1000 (controller + motors + motor coupling hardware). Competing commercial motor solutions cost in the region of $5,000 to $10,000. None of these costs include bread-boards, linear translation stages, construction posts, etc, since all of this hardware is required no matter what motor solution is used. 

The breakdown of the costs is as follows:
* Costs for motors and motor couplers are in the region of $100 to $200 per axis. 
* Costs for micrometers, which act as the gear mechanism, are in the region of $80 to $150 per axis. 
* Linear translation stages start at about $200 per axis. 
* A 4 axis controller unit can be assembled for under $500. 


The cost savings come from the fact that we are using regular, cheap, stepper motors instead of expensive linear actuators or servos. Further, the gears in our system are regular micrometers, which are easy to source and can even be bought at a discount from e-bay. The main trade off compared to commercial systems is bulk, since connecting a stepper motor to a micrometer requires a relatively long (at least 8" or so) flexible shaft. A second trade off is that our system does not track stage position with encoders and so position is inferred by counting the number of steps taken by the motors. In practice, this is not an issue as a properly assembled stage that is not driven beyond its operating limits will not lose position. We have never had accuracy issues with our stage. 


Our controller is based around an Arduino Mega and can accommodate up to 4 axes of motion, but we have only tested up to 3 axes. You can also build a single-axis controller around an Arduino Uno for as little as $50. The controller will communicate with a PC via serial or virtual serial (USB) ports. An external LCD display and hand-held controller can also be added. Please see our paper and website (http://turnerlab.cshl.edu/openstage.html) for more details.
