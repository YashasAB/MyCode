#include "cs225/PNG.h"
#include <list>
#include <iostream>

#include "colorPicker/ColorPicker.h"
#include "imageTraversal/ImageTraversal.h"

#include "Point.h"
#include "Animation.h"
#include "FloodFilledImage.h"

using namespace cs225;
using namespace std;

/**
 * Constructs a new instance of a FloodFilledImage with a image `png`.
 *
 * @param png The starting image of a FloodFilledImage
 */
FloodFilledImage::FloodFilledImage(const PNG & png) {
  /** @todo [Part 2] */
  newframe = png;

}

/**
 * Adds a FloodFill operation to the FloodFillImage.  This function must store the operation,
 * which will be used by `animate`.
 *
 * @param traversal ImageTraversal used for this FloodFill operation.
 * @param colorPicker ColorPicker used for this FloodFill operation.
 */
void FloodFilledImage::addFloodFill(ImageTraversal & traversal, ColorPicker & colorPicker) {
  /** @todo [Part 2] */

  floodpath.push_back(&traversal);
  tofill.push_back(&colorPicker);

}

/**
 * Creates an Animation of frames from the FloodFill operations added to this object.
 *
 * Each FloodFill operation added by `addFloodFill` is executed based on the order
 * the operation was added.  This is done by:
 * 1. Visiting pixels within the image based on the order provided by the ImageTraversal iterator and
 * 2. Updating each pixel to a new color based on the ColorPicker
 *
 * While applying the FloodFill to the image, an Animation is created by saving the image
 * after every `frameInterval` pixels are filled.  To ensure a smooth Animation, the first
 * frame is always the starting image and the final frame is always the finished image.
 *
 * (For example, if `frameInterval` is `4` the frames are:
 *   - The initial frame
 *   - Then after the 4th pixel has been filled
 *   - Then after the 8th pixel has been filled
 *   - ...
 *   - The final frame, after all pixels have been filed)
 */
Animation FloodFilledImage::animate(unsigned frameInterval) const {
  Animation animation;
  /** @todo [Part 2] */
  PNG image = newframe;

  animation.addFrame(image);

  // Initially thought I would use a queue of PNG's
  // std::vector<*ImageTraversal>::iterator it = floodpath.begin();
  // std::vector<*ImageTraversal>::iterator itc = tofill.begin();
  // while(it != floodpath.end()){
  //   Point lol = *it;
    // x=lol.x;
    // y=lol.y;
    // HSLAPixel fillx = colorPicker.getColor(x,y);
  //   PNG  start = floodpath.front();
  //   PNG newframe = new PNG(start);
  //   HSLAPixel & pixel = newframe.getPixel(x,y);
  //   pixel = fillx;
  //   floodpath.push_back(newframe);
  //   it++;
  // }
  unsigned x,y;
  int frameNumber = 1;
  //fill all pixels in order given by floodpath and add frame to animation if framenumber is multiple of frameinterval
  for(unsigned i = 0; i<floodpath.size(); i++){
    for (const Point & lol : *floodpath[i]){
      x=lol.x;
      y=lol.y;
      HSLAPixel fillx = tofill[i]->getColor(x,y);
      HSLAPixel & pixel = image.getPixel(x,y);
      pixel = fillx;
      //check if current frame is needed for animation
      if(frameNumber%frameInterval==0){
      animation.addFrame(image);
    }
    frameNumber++;
  }
  if((frameNumber-1)%frameInterval!=0){
  animation.addFrame(image);
     }
  }

  return animation;
}
