// Hyder Shad
// UT EID: hs25796
// 9/6/2018

#include <stdio.h>
#include <stdint.h>
#include "string.h"
#include "ST7735.h"
#include "PLL.h"
#include "fixed.h"
#include "tm4c123gh6pm.h"
#include <stdlib.h>


/****************ST7735_sDecOut2***************
 converts fixed point number to LCD
 format signed 32-bit with resolution 0.01
 range -99.99 to +99.99
 Inputs:  signed 32-bit integer part of fixed-point number
 Outputs: none
 send exactly 6 characters to the LCD 
Parameter LCD display
 12345    " **.**"
  2345    " 23.45"  
 -8100    "-81.00"
  -102    " -1.02" 
    31    "  0.31" 
-12345    "-**.**"
 */ 
void ST7735_sDecOut2(int32_t n) {
	int x = 0;			//store magnitude of n
	
	if(n >= 0) {		//print either minus or space based on sign of n
		printf(" ");
		x = n;				
	}
	else {
		printf("-");
		x = -n;				//store magnitude of n
	}
	
	if(n > 9999 || n < -9999) {	//if out of bounds, print stars
		printf("**.**");			
	}
	else{
		printf("%d.%02d", (x / 100), (x % 100));	//%02 adds 0s for padding and shows 2 digits, x/100 gives decimal portion, x/100 gives integer portion
  }
}

/**************ST7735_uBinOut6***************
 unsigned 32-bit binary fixed-point with a resolution of 1/64. 
 The full-scale range is from 0 to 999.99. 
 If the integer part is larger than 63999, it signifies an error. 
 The ST7735_uBinOut6 function takes an unsigned 32-bit integer part 
 of the binary fixed-point number and outputs the fixed-point value on the LCD
 Inputs:  unsigned 32-bit integer part of binary fixed-point number
 Outputs: none
 send exactly 6 characters to the LCD 
Parameter LCD display
     0	  "  0.00"
     1	  "  0.01"
    16    "  0.25"
    25	  "  0.39"
   125	  "  1.95"
   128	  "  2.00"
  1250	  " 19.53"
  7500	  "117.19"
 63999	  "999.99"
 64000	  "***.**"
*/
void ST7735_uBinOut6(uint32_t n) {
	if(n > 63999) {				//check to see if value is out of range, if so print stars and return
		printf("***.**");
	}
	else{
		n = (n * 125) >> 3;   //n = n * (1000 / 64) since 64 bit resolution = 125 / 8, shifting by 3 gives correct answer, do not divide

		if ((n % 10) >= 5) {
		n = n + 10;						//round up
		}
		n = n/10;							//round down
		printf("%3u.%02u", n / 100, n % 100);	//shows 3 digits for left of decimal, padding of 2 zeros for right of decimal
	}
}

/**************ST7735_XYplotInit***************
 Specify the X and Y axes for an x-y scatter plot
 Draw the title and clear the plot area
 Inputs:  title  ASCII string to label the plot, null-termination
          minX   smallest X data value allowed, resolution= 0.001
          maxX   largest X data value allowed, resolution= 0.001
          minY   smallest Y data value allowed, resolution= 0.001
          maxY   largest Y data value allowed, resolution= 0.001
 Outputs: none
 assumes minX < maxX, and miny < maxY
*/

int32_t s_minX;		//globals for saving min and max xy values when initialization is called 
int32_t s_maxX; 
int32_t s_minY; 	//"s_" for saved value
int32_t s_maxY;

void ST7735_XYplotInit(char *title, int32_t minX, int32_t maxX, int32_t minY, int32_t maxY) {

	s_minX = minX;		//save min and max x-y values to global save variables so XYplot function can use them
	s_maxX = maxX;
	s_minY = minY;
	s_maxY = maxY;

	ST7735_FillScreen(0);  			//reset screen to black
  ST7735_SetCursor(0,0);			//reset cursor
	printf("%s", title);				//print the title of the plot to be drawn

}

/**************ST7735_XYplot***************
 Plot an array of (x,y) data
 Inputs:  num    number of data points in the two arrays
          bufX   array of 32-bit fixed-point data, resolution= 0.001
          bufY   array of 32-bit fixed-point data, resolution= 0.001
 Outputs: none
 assumes ST7735_XYplotInit has been previously called
 neglect any points outside the minX maxY minY maxY bounds
*/
void ST7735_XYplot(uint32_t num, int32_t bufX[], int32_t bufY[]) {
	int i = 0;
	int new_x = 0;		//new pixel coordinates after it has been mapped
	int new_y = 0;
	for(i = 0; i < num; i = i+1) {//go through each pixel in the buffers
		if ((bufX[i] < s_maxX) || (bufX[i] > s_minX) || (bufY[i] < s_maxY) || (bufY[i] > s_minY)) {	//check to see if coordinates are in plot area, ignore points outside area
		new_x = (127 * (bufX[i] - s_minX)) / (s_maxX - s_minX);			//map fixed point, pixels on x axis
		new_y = 32 + (127 * (s_maxY - bufY[i])) / (s_maxY - s_minY);//map fixed point, pixels on y axis, move down 32 pixels as per lab instructions
		ST7735_DrawPixel(new_x, new_y, ST7735_CYAN);							//draw the mapped pixels using ST7735 library function	
		}
	}
}

