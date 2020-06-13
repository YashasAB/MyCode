#include "Image.h"
#include "StickerSheet.h"
using namespace cs225;
int main() {

  Image bg;
  Image s1;
  Image s2;
  Image s3;
  Image s4;
  s1.readFromFile("goku.png");
  s2.readFromFile("vegeta.png");
  s3.readFromFile("char.png");
  s4.readFromFile("i.png");
  bg.readFromFile("naruto.png");

  StickerSheet mine = StickerSheet(bg, 4);
  mine.addSticker (s1, 20, 60);
  mine.addSticker (s2, 500, 300);
  mine.addSticker(s3, 1000, 450);
  mine.addSticker(s4, 1000, 50);
 Image myout = mine.render();
 myout.writeToFile("myImage.png");

  return 0;
}
