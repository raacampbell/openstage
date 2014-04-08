

* Costs for motors and motor couplers are in the region of $100 to $200 per axis. 
* Costs for micrometers, which act as the gear mechanism, are in the region of $80 to $150 per axis. 
* Linear translation stages start at about $200 per axis. 
* A 4 axis controller unit can be assembled for under $500. 

We have successfully worked with OpenStage on a 2-photon imaging system for many months, and it is now being picked up by other labs in our institute. The characteristics of the system are described in our PlosONE paper (http://www.plosone.org/article/info%3Adoi%2F10.1371%2Fjournal.pone.0088977). The three-axis system described in that paper cost us about $1000 (controller + motors + motor coupling hardware). Competing commercial motor solutions cost in the region of $5,000 to $10,000. None of these costs include bread-boards, linear translation stages, construction posts, etc, since all of this hardware is required no matter what motor solution is used. 

The firmware source code is heavily commented and can be used to help wire the system (see OpenStage.ino). Also see our website (http://turnerlab.cshl.edu/openstage.html).

ToDo:
Add option for stage to communicate via virtual serial (ie USB) port.
