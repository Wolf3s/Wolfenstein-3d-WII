/* ================================================
   Wolfenstein 3D (SDL) Wii
   Based on Wolf4SDL v.1.6
   Ported to Wii by Nick [MrPeanut] Cavallo
   ================================================

   ================================================
   file: wiistub.h author: MrPeanut 
   purpose: all things wii!
   ================================================
*/

int wiiKbHit(void);
short wiiShortSwap (short l);
int wiiLongSwap (int l);
void wiiInitializeLogFile(void);
void wiiLogWriteLine( const char *msg, ... );
void wiiSleepUntilKbHit(void);
void wiiProcessFont(int chunk);
void wiiProcessSegs(word *segs);
void wiiCloseLogFile(void);
