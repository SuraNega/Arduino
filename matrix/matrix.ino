 
#define FRAME_RATE_HZ 50 // This feels sufficient
#define SUBFRAME_RATE_HZ (FRAME_RATE_HZ * N_SUBFRAMES) // N_SUBFRAMES is in ledmatrix.h
#define SUBFRAME_DELAY_US (1000000 / SUBFRAME_RATE_HZ)
 
int main(void)
{
 struct ledmatrix_frame fB = // struct ledmatrix is a 5-bytes struct holding a 7x5 frame.
  LEDMATRIX_FRAME_INIT( // A helper macro to write frames easily
   11110,  // This structure contains a &quot;B&quot;
   10001,  // 1 means LED is on
   10001,  // 0 means LED is off
   11110,  // they are transformed into binary numbers by &quot;magic&quot; in LEDMATRIX_FRAME_INIT()
   10001,
   10001,
   11110);
 
 ledmatrix_setup(); // Initializes ports and stuff.
 
 while(1)
 {
  ledmatrix_draw_next_subframe(&amp;fB); // Turns on the LEDs of the next subframe.
  _delay_us(SUBFRAME_DELAY_US); // This loop+delay might be replaced by a periodic interrupt.
 }
 return 0;
}
