#include<iostream>
#include <string>
using std::string;
#include<cmath>
#include<cstdlib>
#include "Image.h"



  void Image::lighten(){
    unsigned width=this->width();
    unsigned height=this->height();
    for (unsigned x = 0; x < width; x++) {
      for (unsigned y = 0; y < height; y++) {
        HSLAPixel & pixel = this->getPixel(x, y);
        if(pixel.l+0.1<1){
        pixel.l = pixel.l+0.1;
      }
      else{
        pixel.l=1;
      }
      }
    }
  }
  void Image::lighten(double amount){
    unsigned width=this->width();
    unsigned height=this->height();
    for (unsigned x = 0; x < width; x++) {
      for (unsigned y = 0; y < height; y++) {
        HSLAPixel & pixel = this->getPixel(x, y);
        if(pixel.l+amount<1){
        pixel.l = pixel.l+amount;
      }
      else{
        pixel.l=1;
      }
      }
    }
  }
  void Image::darken(){
    unsigned width=this->width();
    unsigned height=this->height();
    for (unsigned x = 0; x < width; x++) {
      for (unsigned y = 0; y < height; y++) {
        HSLAPixel & pixel = this->getPixel(x, y);
        if(pixel.l-0.1>0){
        pixel.l = pixel.l-0.1;
      }
      else{
        pixel.l=0;
      }
      }
    }
  }
  void Image::darken(double amount){
    unsigned width=this->width();
    unsigned height=this->height();
    for (unsigned x = 0; x < width; x++) {
      for (unsigned y = 0; y < height; y++) {
        HSLAPixel & pixel = this->getPixel(x, y);
        if(pixel.l-amount>0){
        pixel.l = pixel.l-amount;
      }
      else{
        pixel.l=0;
      }
      }
    }
  }
  void Image::saturate(){
    unsigned width=this->width();
    unsigned height=this->height();
    for (unsigned x = 0; x < width; x++) {
      for (unsigned y = 0; y < height; y++) {
        HSLAPixel & pixel = this->getPixel(x, y);
        if(pixel.s+0.1<1){
        pixel.s = pixel.s+0.1;
      }
      else{
        pixel.s=1;
      }
      }
    }
  }
  void Image::saturate(double amount){
    unsigned width=this->width();
    unsigned height=this->height();
    for (unsigned x = 0; x < width; x++) {
      for (unsigned y = 0; y < height; y++) {
        HSLAPixel & pixel = this->getPixel(x, y);
        if(pixel.s+amount<1){
        pixel.s = pixel.s+amount;
      }
      else{
        pixel.s=1;
      }
      }
    }
  }
  void Image::desaturate(){
  unsigned width=this->width();
    unsigned height=this->height();
    for (unsigned x = 0; x < width; x++) {
      for (unsigned y = 0; y < height; y++) {
        HSLAPixel & pixel = this->getPixel(x, y);
        if(pixel.s-0.1>0){
        pixel.s = pixel.s-0.1;
      }
      else{
        pixel.s=0;
      }
      }
    }
  }
  void Image::desaturate(double amount){
    unsigned width=this->width();
    unsigned height=this->height();
    for (unsigned x = 0; x < width; x++) {
      for (unsigned y = 0; y < height; y++) {
        HSLAPixel & pixel = this->getPixel(x, y);
        if(pixel.s-amount>0){
        pixel.s = pixel.s-amount;
      }
      else{
        pixel.s=0;
      }
      }
    }
  }
  void Image::grayscale(){
    unsigned width=this->width();
    unsigned height=this->height();
    for (unsigned x = 0; x < width; x++) {
      for (unsigned y = 0; y < height; y++) {
        HSLAPixel & pixel = this->getPixel(x, y);
        pixel.s=0;
      }
    }
  }
  void Image::rotateColor(double degrees){
    unsigned width=this->width();
    unsigned height=this->height();
    for (unsigned x = 0; x < width; x++) {
      for (unsigned y = 0; y < height; y++) {
        HSLAPixel & pixel = this->getPixel(x, y);
        pixel.h = pixel.h+degrees;
       if(pixel.h>360){
        pixel.h=pixel.h-360;
      }
      if(pixel.h<0){
        pixel.h=pixel.h+360;
      }
      }
    }
  }
  void Image::Illinify(){
    unsigned width=this->width();
    unsigned height=this->height();
    for (unsigned x = 0; x < width; x++) {
      for (unsigned y = 0; y < height; y++) {
        HSLAPixel & pixel = this->getPixel(x, y);
        if (pixel.h>113.5 and pixel.h<293.5){
          pixel.h=216;
        }
        else {
          pixel.h=11;
        }
      }
    }
  }
  void Image::scale(double factor){
    
    unsigned width=this->width();
     unsigned height=this->height();
     unsigned nw=width*factor;
     unsigned nh=height*factor;
     double wr=width/(double)nw;
     double hr=height/(double)nh;

     if(wr<hr){
       hr=wr;
     }
     else {
       wr=hr;
     }

    cs225::PNG scaledim(*this);
     this->resize(ceil(nw),ceil(nh));
     for (unsigned x = 0; x < nw; x++) {
       for (unsigned y = 0; y < nh; y++) {
         HSLAPixel & pixel = this->getPixel(x, y);
         int xx=x*wr;
         int yy=y*hr;
         pixel=scaledim.getPixel(xx,yy);
       }
     }

       }
  void Image::scale(unsigned w, unsigned h){
    unsigned width=this->width();
    unsigned height=this->height();
    unsigned nw=w;
    unsigned nh=h;
    double wr=nw/width;
    double hr=nh/height;

    if(width*wr<=w && height*wr<=h){
      scale(wr);
    }
    else {
      scale(hr);
    }

  }
