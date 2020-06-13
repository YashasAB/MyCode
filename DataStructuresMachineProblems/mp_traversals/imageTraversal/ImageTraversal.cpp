#include <cmath>
#include <iterator>
#include <iostream>

#include "../cs225/HSLAPixel.h"
#include "../cs225/PNG.h"
#include "../Point.h"

#include "ImageTraversal.h"

/**
 * Calculates a metric for the difference between two pixels, used to
 * calculate if a pixel is within a tolerance.
 *
 * @param p1 First pixel
 * @param p2 Second pixel
 * @return the difference between two HSLAPixels
 */
double ImageTraversal::calculateDelta(const HSLAPixel & p1, const HSLAPixel & p2) {
  double h = fabs(p1.h - p2.h);
  double s = p1.s - p2.s;
  double l = p1.l - p2.l;

  // Handle the case where we found the bigger angle between two hues:
  if (h > 180) { h = 360 - h; }
  h /= 360;

  return sqrt( (h*h) + (s*s) + (l*l) );
}

/**
 * Default iterator constructor.
 */
 ImageTraversal::Iterator::Iterator() {
     path = NULL;
     done = true;
 }

ImageTraversal::Iterator::Iterator(ImageTraversal *it, Point start, double tolerance, PNG image) {
  /** @todo [Part 1] */
  //Initializing route, startpoint and tolerance and image to be traversed.
  path = it;
  st = start;
  tol = tolerance;
  img = image;

  //If there is nothing to traverse
  if(it->empty()){
    done = true;
    }
  else{
    done = false;
     }

  w = image.width();
  h = image.height();

  //Creating a matrix of vectors that will help keep track of visited HSLAPixels

  // start with width and resize every width to height so accessing the attendance of a visit of a pixel is of the form visit [w],[h]
  visits.resize(w);

  for(unsigned i = 0; i<w; i++){
    visits[i].resize(h);
  }

 //Setting visit attendance to 0
 for(unsigned i = 0; i<w; i++){
   for(unsigned j = 0; j<h; j++){
     visits[i][j]=0;
   }
 }

}

/**
 * Iterator increment opreator.
 *
 * Advances the traversal of the image.
 */
ImageTraversal::Iterator & ImageTraversal::Iterator::operator++() {
  /** @todo [Part 1] */
  Point curr = path->pop();
  unsigned x = curr.x;
  unsigned y = curr.y;

  HSLAPixel &  pc = img.getPixel(x,y);

  // mark attandance of startpoint as present

  visits[x][y]=1;

  if(x+1<w){
    if(visits[x+1][y]==0){
      HSLAPixel &  pn = img.getPixel(x+1,y);
      double diff = ImageTraversal::calculateDelta(pc,pn);
      if(diff<tol){
      path->add(Point(x+1,y));
     }
    }
  }

  if(y+1<h){
    if(visits[x][y+1]==0){
      HSLAPixel &  pn = img.getPixel(x,y+1);
      double diff = ImageTraversal::calculateDelta(pc,pn);
      if(diff<tol){
      path->add(Point(x,y+1));
     }
    }
  }

  if(x>=1){
    if(visits[x-1][y]==0){
      HSLAPixel &  pn = img.getPixel(x-1,y);
      double diff = ImageTraversal::calculateDelta(pc,pn);
      if(diff<tol){
      path->add(Point(x-1,y));
     }
    }
  }

  if(y>=1){
    if(visits[x][y-1]==0){
      HSLAPixel &  pn = img.getPixel(x,y-1);
      double diff = ImageTraversal::calculateDelta(pc,pn);
      if(diff<tol){
      path->add(Point(x,y-1));
     }
    }
  }

  // the top of the queue is the next point to be visited.- so if its attandance is marked pop it
  // keep popping the path until we reach a point who's attendance is 0 or path is empty.

  while(!path->empty()){
    if(visits[path->peek().x][path->peek().y]==0){
      break ;   //continue and return current path if next to be visited has attendance of 0 or not visited
    }
    else{
      Point lol = path->pop();
    }
  }

  if(path->empty()){
    done = true;
  }

  return *this;
}

/**
 * Iterator accessor opreator.
 *
 * Accesses the current Point in the ImageTraversal.
 */
Point ImageTraversal::Iterator::operator*() {
  /** @todo [Part 1] */
  return path->peek();
}

/**
 * Iterator inequality operator.
 *
 * Determines if two iterators are not equal.
 */
bool ImageTraversal::Iterator::operator!=(const ImageTraversal::Iterator &other) {
  /** @todo [Part 1] */
  if(done!=other.done){
    return true;
  }
  else{
  return false;
  }
}
