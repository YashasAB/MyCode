#include "../cs225/HSLAPixel.h"
#include "../Point.h"

#include "ColorPicker.h"
#include "MyColorPicker.h"

using namespace cs225;

/**
 * Picks the color for pixel (x, y).
 * Using your own algorithm
 */
HSLAPixel MyColorPicker::getColor(unsigned x, unsigned y) {
  /* @todo [Part 3] */
  int h = (5+x)%360;
  if(5*x-2*y>0){
  int h = ((5*x)-(2*y))%360;
}


  return HSLAPixel(h, 1, 0.5);



}
