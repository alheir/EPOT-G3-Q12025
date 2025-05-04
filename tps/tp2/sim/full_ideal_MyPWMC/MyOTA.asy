Version 4
SymbolType CELL
LINE Normal -32 0 -32 32
LINE Normal -32 -32 -32 0
LINE Normal -16 -16 -24 -16
LINE Normal -16 16 -24 16
LINE Normal -20 -12 -20 -20
LINE Normal 16 -4 -32 -32
LINE Normal 16 4 -32 32
LINE Normal 16 -4 16 4
WINDOW 39 -2 50 Center 2
WINDOW 40 -4 77 Center 2
WINDOW 0 -34 -47 Left 2
SYMATTR SpiceLine Ri= 10e6 Ro=5e6
SYMATTR SpiceLine2 gm=2e-3 Fo=200
SYMATTR ModelFile MyOTA.sub
SYMATTR SpiceModel MyOTA
SYMATTR Description Custom OTA
SYMATTR Prefix X
PIN -32 -16 NONE 8
PINATTR PinName 1
PINATTR SpiceOrder 1
PIN -32 16 NONE 8
PINATTR PinName 2
PINATTR SpiceOrder 2
PIN 16 0 NONE 8
PINATTR PinName 3
PINATTR SpiceOrder 3
