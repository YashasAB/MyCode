#include "cs225/PNG.h"
#include "cs225/HSLAPixel.h"



using namespace cs225;

void rotate(std::string inputFile, std::string outputFile) {
  // TODO: Part 2
  cs225::PNG input;
  input.readFromFile(inputFile);
  unsigned w,h;

  w=input.width();
  h=input.height();
  cs225::PNG newimg(w,h);

  for (unsigned x = 0; x < w; x++) {
    for (unsigned y = 0; y < h; y++) {

      HSLAPixel & p1 = input.getPixel(x, y);
      HSLAPixel & p01 = newimg.getPixel(w-x-1, h-y-1);
      p01=p1;
          }
        }
        newimg.writeToFile(outputFile);
      }


cs225::PNG myArt(unsigned int width, unsigned int height) {
  cs225::PNG img(width, height);
  // TODO: Part 3
  unsigned w,h;
  w=img.width();
  h=img.height();

  for (unsigned x = 0; x < w; x++) {
    for (unsigned y = 0; y < h; y++) {

      HSLAPixel & p1 = img.getPixel(x, y);



      if (x%2==0){
        p1.s=0.5;
        p1.l=0.5;
        p1.a=1/8;
        p1.h=20;

      }
      if (x%12==0){
        p1.s=0.5;
        p1.l=0.5;
        p1.a=1/8;
        p1.h=230;

      }
      if (x%7==0){
        p1.s=0.6;
        p1.l=0.7;
        p1.a=0.33;
        p1.h=250;
      }
      if (x%6==0){
        p1.s=0.4;
        p1.l=0.4;
        p1.a=1/4;
        p1.h=80;
      }
      if (y%7==0){

        p1.s=0.6;
        p1.l=0.5;
        p1.a=1/3;
        p1.h=130;
      }
      if (y%9==0){

        p1.s=0.6;
        p1.l=0.5;
        p1.a=1/3;
        p1.h=25;
      }
      if (y%5==0 &&  y%2!=0){

        p1.s=0.7;
        p1.l=0.6;
        p1.a=1/8;
        p1.h=330;
      }
      if (p1.h==0 or p1.s==0){
        if((x-1)%3==0 or (x-1)%4==0){
        p1.h=38;
        p1.s=0.5;
        p1.a=0.66;
        p1.l=0.4;
      }
      else   {
        p1.h=169;
        p1.s=0.5;
        p1.l=0.2;
        p1.a=0.6;
      }
    }

          }
        }

    for (unsigned x = 0; x < w; x=x+3) {
      for (unsigned y = 0; y < h; y=y+2) {
        HSLAPixel & p1 = img.getPixel(x, y);
        if(p1.h>100){
          p1.h=p1.h-12;
        }
      }
    }



  return img;
}
