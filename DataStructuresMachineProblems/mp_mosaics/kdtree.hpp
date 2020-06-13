/**
 * @file kdtree.cpp
 * Implementation of KDTree class.
 */

#include <utility>
#include <algorithm>

using namespace std;


template <int Dim>
bool KDTree<Dim>::smallerDimVal(const Point<Dim>& first,
                                const Point<Dim>& second, int curDim) const
{
    /**
     * @todo Implement this function!
     */

    if(first[curDim]<second[curDim]){
      return true;
    }
    if(first[curDim]==second[curDim]){
      return first<second;
    }
    if(first[curDim]>second[curDim]){
      return false;
    }
    return false;
}

template <int Dim>
bool KDTree<Dim>::shouldReplace(const Point<Dim>& target,
                                const Point<Dim>& currentBest,
                                const Point<Dim>& potential) const
{
    /**
     * @todo Implement this function!
     */
     int currd=0;
     int pod=0;

     for(unsigned i=0; i<Dim ; i++){
       currd+=pow((currentBest[i]-target[i]),2);
       pod+=pow((potential[i]-target[i]),2);
     }

     if(pod<currd){
       return true;
     }
     if(pod==currd){
       return potential<currentBest;
     }
     if(pod>currd){
       return false;
     }

     return false;
}

template <int Dim>
KDTree<Dim>::KDTree(const vector<Point<Dim>>& newPoints)
{
    /**
     * @todo Implement this function!
     */
     createkd(root,newPoints,0);
}

template <int Dim>
KDTree<Dim>::KDTree(const KDTree<Dim>& other) {
  /**
   * @todo Implement this function!
   */

}


template <int Dim>
const KDTree<Dim>& KDTree<Dim>::operator=(const KDTree<Dim>& rhs) {
  /**
   * @todo Implement this function!
   */

  return *this;
}

template <int Dim>
KDTree<Dim>::~KDTree() {
  /**
   * @todo Implement this function!
   */
   destkd(root);
}

template <int Dim>
unsigned KDTree<Dim>::findmed(vector<Point<Dim>> & lol, int dim){
  //copy everything to a non-constant vector lol
  // stl:vector<Point<Dim>> lol;
  // lol.resize(newpts.size());
  // unsigned lmao =0;
  // for (auto it = newpts.cbegin(); it != newpts.cend(); ++it){
  // lol[lmao]=*it;
  // lmao++;
  // }

  //sort lol
  unsigned min;
  for(unsigned i=0;i<lol.size() ;i++ ){
    min = i;
    for(unsigned j=i+1;j<lol.size() ;j++ ) {
      if(smallerDimVal(lol[j],lol[min],dim)){
         min = j;
      }
     }
      if(min!=i){
        Point<Dim> temp = lol[i];
        lol[i]=lol[min];
        lol[min]=temp;
      }
    }

  // finding median
  unsigned med = (lol.size()-1)/2;

  return med;
}

template <int Dim>
void KDTree<Dim>::createkd(KDTreeNode * & nod, vector<Point<Dim>> newpts, int dim){
  if(newpts.size()==0){
      nod=NULL;
      return;
    }
  if(newpts.size()==1){
      nod = new KDTreeNode(newpts[0]);
      return;
    }

  unsigned median = findmed(newpts,dim);
  nod = new KDTreeNode(newpts[median]);
  //recursively construct left subtrees then right subtrees
  if(median>0){
  std::vector<Point<Dim>> lmao(newpts.cbegin(), newpts.cbegin() + median);
  createkd(nod->left,lmao, (dim+1)%Dim);
  }

  std::vector<Point<Dim>> lmfao(newpts.cbegin()+1+median,newpts.cend());
  createkd(nod->right,lmfao,(dim+1)%Dim );
  return;
  
}

template <int Dim>
void KDTree<Dim>::destkd(KDTreeNode*nod){
  if (nod == NULL){
      return;
    }
    destkd(nod->right);
    destkd(nod->left);
    delete nod;
    return;
}

template <int Dim>
Point<Dim> KDTree<Dim>::findNearestNeighbor(const Point<Dim>& query) const
{
    /**
     * @todo Implement this function!
     */
     Point<Dim> currentBest = root->point;
     Point<Dim> q = query;
     findnn(root, currentBest, q, 0);
     return currentBest;
}

template <int Dim>
void KDTree<Dim>::findnn(KDTreeNode* nod, Point<Dim> &currentBest,Point<Dim> &q,  int dim) const{
   if (nod==NULL){
      return;
    }
    KDTreeNode* target;
    KDTreeNode* notarget;

    if(smallerDimVal(nod->point, q, dim)){
      target = nod->right;
      notarget = nod->left;
    }
    else{
      notarget = nod->right;
      target = nod->left;
    }

    //keep traversing right or left based on weather querry is larger than current point
    //increase the dimension every recursive call so it is traversing the way the tree was constructed.

    findnn(target, currentBest, q, (dim+1)%Dim);

    //if node reached is closer update currentBest to current node
    if(shouldReplace(q, currentBest, nod->point)){
      currentBest = nod->point;
    }

  //Find distance between querry and current best
  double rad = 0;
  for(unsigned i=0; i<Dim ; i++){
    rad+=pow((currentBest[i]-q[i]),2);
  }

  // double pod = 0;
  // for(unsigned i=0; i<Dim ; i++){
  //   pod+=pow((pot[i]-q[i]),2);
  // }

//find distance between querry and the current node reached in the dimension this level is sorted by
  double pod = pow((nod->point[dim]-q[dim]),2);



  //recursively search for nearest in not target only if potential distance is lesser than or equal to current radius
  if (pod<=rad){
    findnn(notarget, currentBest,q,(dim+1)%Dim);
  }


}
