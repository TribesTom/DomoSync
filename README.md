DomoSync
========
DomoSync is a logicel suite to enhance my domotic installation.

It contain  : 

Arduino code using Cosa libraries : https://github.com/mikaelpatel/Cosa

C daemon inspired from UUGear code : https://github.com/shawnu/UUGear
                  
C client and Pyton client to interface with Domoticz ( TODO )


DomoSync daemon can connect to multiple Arduino Board at same time and clients can access all arduino board at same time.
It enhance UUGear at this point. It can detect new Arduino board connected and dispose of Arduino board disconnected.
It is a multithread daemon : one main thread sending message from client to arduino board, one thread for each arduino board listening to response, one thread to listen to udev events. 

Arduino sketch can use Button classe from Cosa library which is usefull in domotic.
