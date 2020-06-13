/**
 * @file maptiles.cpp
 * Code for the maptiles function.
 */

#include <iostream>
#include <map>
#include "maptiles.h"
//#include "cs225/RGB_HSL.h"

using namespace std;


Point<3> convertToXYZ(LUVAPixel pixel) {
    return Point<3>( pixel.l, pixel.u, pixel.v );
}

MosaicCanvas* mapTiles(SourceImage const& theSource,
                       vector<TileImage>& theTiles)
{
    /**
     * @todo Implement this function!
     */

    //create vector that will be used to create KDTree
    vector<Point<3>> tiles;

    //create a map where the point values of everything in theTiles and their indices are present
    map<Point<3>, unsigned> tilemap;

    tiles.resize(theTiles.size());


    for(unsigned i =0; i<theTiles.size(); i++){
      LUVAPixel lol = theTiles[i].getAverageColor();
      Point<3> lmao = convertToXYZ(lol);
      //insert Point values of avg color of imgs in thetiles to tiles
      tiles[i]=lmao;

      // insert the point and index of the point in tiles to tilemap
      pair<Point<3>,unsigned> tileind = pair<Point<3>,unsigned>(lmao,i);
      tilemap.insert(tileind);

    }

    // create a mosaic canvas storing pointers to closest tile

    //create kdtree used to find closest tile
    KDTree<3> * tiletree = new KDTree<3>(tiles);

    //now set the canvas
    int rows = theSource.getRows();
    int cols = theSource.getColumns();
    MosaicCanvas * mos = new MosaicCanvas(rows,cols);

    for(int i =0; i<rows; i++){
      for(int j=0; j<cols; j++){
        LUVAPixel lol = theSource.getRegionColor(i,j);
        Point<3> lmao = convertToXYZ(lol);
        //find closest point to point of region color found in tiletree
        Point<3> closest = tiletree->findNearestNeighbor(lmao);

        //find index of the image in theTiles whose point is the closest point found
        //point of image in theTiles[i] is stored in tiles[i]
        unsigned ind = tilemap[closest];

        //insert into mos
        TileImage * set = &theTiles[ind];
        mos->setTile(i,j,set);

      }
    }

    delete tiletree;

    return mos;

}
