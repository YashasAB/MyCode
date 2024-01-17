#include<iostream>
#include "StickerSheet.h"
using cs225::PNG;
using cs225::HSLAPixel;
namespace cs225{

void StickerSheet::copy(const StickerSheet &other){
  ct=other.ct;
  mx=other.mx;
  sheet = new Image*[mx];
  xi=new int[mx];
  yi=new int[mx];
  presence= new int[mx];
  for(unsigned i=0; i<mx; i++){
    if(other.presence[i]!=0){
    sheet[i] = new Image(*other.sheet[i]);
    xi[i]=other.xi[i];
    yi[i]=other.yi[i];
    presence[i]=other.presence[i];
    }
    else{
      sheet[i] = NULL;
      xi[i]=0;
      yi[i]=0;
      presence[i]=0;
    }
  }
  picture = new Image(*other.picture);

}
void StickerSheet::destroy(){
  delete[] xi;
  xi=NULL;
  delete[] yi;
  yi=NULL;
  for(unsigned i=0; i<mx; i++){
    sheet[i]=NULL;
  }
  delete[] sheet;
  sheet=NULL;
  delete picture;
  picture=NULL;
  delete[] presence;
  presence=NULL;
}

StickerSheet::StickerSheet (const Image &picture, unsigned max){
  ct  = 0;
  mx = max;
  sheet = new Image*[mx];
  xi=new int[mx];
  yi=new int[mx];
  presence = new int[mx];
  for(unsigned i=0; i<mx; i++){
    sheet[i]=NULL;
    xi[i]=0;
    yi[i]=0;
    presence[i]=0;
  }
  this->picture = new Image(picture);

}


StickerSheet::~StickerSheet (){

    destroy();

  }

StickerSheet::StickerSheet(const StickerSheet &other){
  copy(other);
}


const StickerSheet& StickerSheet::operator=(const StickerSheet &other){

  if(this==&other){
    return *this;
  }
  else{
    destroy();
    copy(other);
    return *this;
  }
}


void StickerSheet::changeMaxStickers (unsigned max){
  if(mx==max){
    return;
  }

  Image * npicture= new Image(*picture);
  Image * *nsheet = new Image*[max];
  int * nxi=new int[max];
  int * nyi=new int[max];
  int * npresence = new int[max];
  int nct=ct;

  if(mx<max){
    for(unsigned j=mx; j<max; j++){
      nsheet[j]=NULL;
      nxi[j]=0;
      nyi[j]=0;
      npresence[j]=0;
      }
   }
  if(mx>max){
    for(unsigned i=max; i<mx; i++){
      sheet[i]=NULL;
      xi[i]=0;
      yi[i]=0;
      if (presence[i]!=0){
      nct--;
    }
      presence[i]=0;
    }
    mx=max;
  }
  for(unsigned i=0; i<mx; i++){
    if(presence[i]!=0){
    nsheet[i] = new Image(*sheet[i]);
    nxi[i]=xi[i];
    nyi[i]=yi[i];
    npresence[i]=presence[i];
  }
  else{
    nsheet[i]= NULL;
    nxi[i]=0;
    nyi[i]=0;
    npresence[i]=0;
  }
      }

  destroy();
  xi=nxi;
  yi=nyi;
  mx=max;
  ct=nct;
  sheet=nsheet;
  presence=npresence;
  picture=npicture;
}


int StickerSheet::addSticker (Image &sticker, unsigned x, unsigned y){
  if(ct<mx){

  for (unsigned i=0; i<mx; i++){
    if(presence[i]==0){
      xi[i]=x;
      yi[i]=y;
      presence[i]=1;
      sheet[i]=&sticker;
      ct++;
      return i;
    }
  }
}
  return -1;
}

bool StickerSheet::translate (unsigned index, unsigned x, unsigned y){
  if(presence[index]==0 || index>=mx){
  return false;
}
else{
  xi[index]=x;
  yi[index]=y;
  return true;
}
}

void StickerSheet::removeSticker (unsigned index){
  if(presence[index]==0){
    return;
  }
  presence[index]=0;
  ct--;
  sheet[index]=NULL;
  xi[index]=0;
  yi[index]=0;
    }
Image * StickerSheet::getSticker (unsigned index){
  if(presence[index]==0){
    return NULL;
  }
  return sheet[index];
}

Image StickerSheet::render () const {
  unsigned ww = picture->width();
  unsigned hh= picture->height();
  Image * outpic = new Image(*picture);
  for(unsigned i=0; i<mx; i++){
    if(presence[i]!=0){
    unsigned w = xi[i] + sheet[i]->width();
    unsigned h = yi[i] + sheet[i]->height();
    if(w>ww){
      ww=w;
    }
    if(hh<h){
      hh=h;
    }
  }
}
    outpic->resize(ww,hh);

    for(unsigned i=0; i<mx; i++){
      if(presence[i]!=0){
        unsigned wid = sheet[i]->width();
        unsigned hei = sheet[i]->height();
        for(unsigned j=0; j<wid; j++){
          for(unsigned k=0; k<hei; k++){
            HSLAPixel &stick = sheet[i]->getPixel(j,k);
            HSLAPixel &pic = outpic->getPixel(xi[i]+j,yi[i]+k);
            if(stick.a!=0){
              pic.a=stick.a;
              pic.s=stick.s;
              pic.l=stick.l;
              pic.h=stick.h;
            }
          }
        }
      }
    }
    Image out = *outpic;
    delete outpic;
    outpic=NULL;
    return out;
}
}
