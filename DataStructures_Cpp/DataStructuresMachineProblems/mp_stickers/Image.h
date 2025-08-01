/**
 * @file Image.h
 * Contains your declaration of the interface for the Image class.
 */

#pragma once

#include <string>
using std::string;
#include "cs225/PNG.h"
#include "cs225/HSLAPixel.h"


using cs225::PNG;
using cs225::HSLAPixel;

  class Image: public PNG {
    public:
      void lighten();
      void lighten(double amount);
      void darken();
      void darken(double amount);
      void saturate();
      void saturate(double amount);
      void desaturate();
      void desaturate(double amount);
      void grayscale();
      void rotateColor(double degrees);
      void Illinify();
      void scale(double factor);
      void scale(unsigned w, unsigned h);

  };
