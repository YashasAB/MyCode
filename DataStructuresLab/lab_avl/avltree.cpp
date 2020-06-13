/**
 * @file avltree.cpp
 * Definitions of the binary tree functions you'll be writing for this lab.
 * You'll need to modify this file.
 */
using namespace std;

template <class K, class V>
V AVLTree<K, V>::find(const K& key) const
{
    return find(root, key);
}

template <class K, class V>
V AVLTree<K, V>::find(Node* subtree, const K& key) const
{
    if (subtree == NULL)
        return V();
    else if (key == subtree->key)
        return subtree->value;
    else {
        if (key < subtree->key)
            return find(subtree->left, key);
        else
            return find(subtree->right, key);
    }
}

template <class K, class V>
void AVLTree<K, V>::rotateLeft(Node*& t)
{
    functionCalls.push_back("rotateLeft"); // Stores the rotation name (don't remove this)
    // your code here
      if(t==NULL){
        return ;
      }
      Node * tr = NULL;
      Node * t2 =NULL;
    if(t->right!=NULL){
     tr = t->right;
      if(t->right->left!=NULL){
      t2 = t->right->left;
      }
    }

    Node * temproot = t;
    temproot->right=t2;
    t->height  = 1 + max(heightOrNeg1(t->left),heightOrNeg1(t->right));
    if(tr!=NULL){
      t= tr;
      t->height  = 1 + max(heightOrNeg1(t->left),heightOrNeg1(t->right));
      temproot->right=t2;
      tr->left = temproot;
    }
}

template <class K, class V>
void AVLTree<K, V>::rotateLeftRight(Node*& t)
{
    functionCalls.push_back("rotateLeftRight"); // Stores the rotation name (don't remove this)
    // Implemented for you:
    rotateLeft(t->left);
    rotateRight(t);
}

template <class K, class V>
void AVLTree<K, V>::rotateRight(Node*& t)
{
    functionCalls.push_back("rotateRight"); // Stores the rotation name (don't remove this)
    // your code here
    if(t==NULL){
      return ;
    }
    Node * tl = NULL;
    Node * t2 =NULL;
  if(t->left!=NULL){
    tl = t->left;
    if(t->left->right!=NULL){
    t2 = t->left->right;
    }
  }

  Node * temproot = t;
  temproot->left=t2;
  t->height  = 1 + max(heightOrNeg1(t->left),heightOrNeg1(t->right));
  if(tl!=NULL){
    t= tl;
    t->height  = 1 + max(heightOrNeg1(t->left),heightOrNeg1(t->right));
    temproot->left=t2;
    tl->right = temproot;
  }

}

template <class K, class V>
void AVLTree<K, V>::rotateRightLeft(Node*& t)
{
    functionCalls.push_back("rotateRightLeft"); // Stores the rotation name (don't remove this)
    // your code here
    rotateRight(t->right);
    rotateLeft(t);
}

template <class K, class V>
void AVLTree<K, V>::rebalance(Node*& subtree)
{
    // your code here
    if (subtree == NULL || (subtree->left == NULL && subtree->right == NULL)){
        return;
    }
    int balance = heightOrNeg1(subtree->right) - heightOrNeg1(subtree->left);

    if (balance <=-2){
        int tl_b = heightOrNeg1(subtree->left->right) - heightOrNeg1(subtree->left->left);
        if (tl_b<0){
            rotateRight(subtree);
        }
        else
        {
            rotateLeftRight(subtree);
        }

    }
    if (balance >=2){
        int tr_b = heightOrNeg1(subtree->right->right) - heightOrNeg1(subtree->right->left);
        if (tr_b>0){
            rotateLeft(subtree);
        }
        else
        {
            rotateRightLeft(subtree);
        }

    }
    subtree->height  = 1 + max(heightOrNeg1(subtree->left),heightOrNeg1(subtree->right));
}

template <class K, class V>
void AVLTree<K, V>::insert(const K & key, const V & value)
{
    insert(root, key, value);
}

template <class K, class V>
void AVLTree<K, V>::insert(Node*& subtree, const K& key, const V& value)
{
    // your code here
    if(subtree == NULL){
        subtree = new Node(key, value);
        subtree->height = 0;
        return;
         }

    if(key > subtree->key){
    insert(subtree->right, key, value);
         }
    else if(key < subtree->key){
    insert(subtree->left, key, value);
     }

    rebalance(subtree);

     return;
}

template <class K, class V>
void AVLTree<K, V>::remove(const K& key)
{
    remove(root, key);
}

template <class K, class V>
void AVLTree<K, V>::remove(Node*& subtree, const K& key)
{
    if (subtree == NULL)
        return;

    if (key < subtree->key) {
        // your code here
        remove(subtree->left, key);
        rebalance(subtree);
    } else if (key > subtree->key) {
        // your code here
        remove(subtree->right, key);
        rebalance(subtree);
    } else {
        if (subtree->left == NULL && subtree->right == NULL) {
            /* no-child remove */
            // your code here
            delete subtree;
            subtree = NULL;
        } else if (subtree->left != NULL && subtree->right != NULL) {
            /* two-child remove */
            // your code here
            Node* tl = subtree->left;
            while(tl->right != NULL){
            tl = tl->right;
            }
            swap(subtree, tl);
            remove(subtree->left, key);
            rebalance(tl);
        } else {
            /* one-child remove */
            // your code here
            Node * tr = NULL;
            Node * tl = NULL;
            if(subtree->right!=NULL){
            tr = subtree->right;
          }
            if (subtree->left!=NULL){
              tl = subtree->left;
            }
            if(tl!=NULL){
                delete subtree;
                 subtree = tl;
            }
            else{
              delete subtree;
              subtree = tr;
            }
            rebalance(subtree);
        }
        // your code here
    }
}
