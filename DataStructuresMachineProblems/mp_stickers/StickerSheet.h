/**
 * @file StickerSheet.h
 * Contains your declaration of the interface for the StickerSheet class.
 */
#pragma once
#include <string>
using std::string;
#include "Image.h"

using cs225::PNG;
using cs225::HSLAPixel;
namespace cs225 {

class StickerSheet{
public:
  StickerSheet (const Image &picture, unsigned max);
  ~StickerSheet ();
  StickerSheet(const StickerSheet &other);
  const StickerSheet & operator= (const StickerSheet &other);
  void changeMaxStickers (unsigned max);
  int addSticker (Image &sticker, unsigned x, unsigned y);
  bool translate (unsigned index, unsigned x, unsigned y);
  void removeSticker (unsigned index);
  Image * getSticker (unsigned index);
  Image render () const ;
  Image * picture;
  private:
    int *xi;
    int * yi;
    Image * *sheet;
    unsigned mx;
    unsigned ct;
    int * presence;
    void copy(const StickerSheet &other);
    void destroy();

};

}
