const PROGMEM int list[] = {100, 110, 120, 130, 140, 150, 160, 170, 180, 190};
const PROGMEM char bas[][WIDTH] = {
{"100 LET L = 0\n"},
{"110 LET I = 0\n"},
{"120 LET I = I + 1\n"},
{"130 IF I > 1000 THEN GOTO 150\n"},
{"140 GOTO 120\n"},
{"150 IF L = 1 THEN GOTO 180\n"},
{"160 LET L = 1\n"},
{"170 GOTO 110\n"},
{"180 LET L = 0\n"},
{"190 GOTO 110\n"},
};
