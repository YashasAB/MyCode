/**
 * @file list.cpp
 * Doubly Linked List (MP 3).
 */

template <class T>
List<T>::List() {
  // @TODO: graded in MP3.1
    head_ = NULL;
    tail_ = NULL;
    length_ = 0;
}

/**
 * Returns a ListIterator with a position at the beginning of
 * the List.
 */
template <typename T>
typename List<T>::ListIterator List<T>::begin() const {
  // @TODO: graded in MP3.1
  return List<T>::ListIterator(head_);
}

/**
 * Returns a ListIterator one past the end of the List.
 */
template <typename T>
typename List<T>::ListIterator List<T>::end() const {
  // @TODO: graded in MP3.1
  if(tail_!=NULL){
  return List<T>::ListIterator(tail_->next);
  }
  else{
    return List<T>::ListIterator(tail_);
  }
}


/**
 * Destroys all dynamically allocated memory associated with the current
 * List class.
 */
template <typename T>
void List<T>::_destroy() {
  /// @todo Graded in MP3.1

  ListNode * cur = head_;
  ListNode * temp ;
  while(cur!=NULL){
    temp = cur;
    cur=cur->next;
    delete temp;

  }
  temp=NULL;

  head_=NULL;
  tail_=NULL;
  length_=0;

}

/**
 * Inserts a new node at the front of the List.
 * This function **SHOULD** create a new ListNode.
 *
 * @param ndata The data to be inserted.
 */
template <typename T>
void List<T>::insertFront(T const & ndata) {
  /// @todo Graded in MP3.1
  ListNode * newNode = new ListNode(ndata);

  newNode -> prev = NULL;

  if (head_ != NULL) {
    newNode -> next = head_;
    head_ -> prev = newNode;
    head_=newNode;
    length_++;
  }
  if (head_ == NULL) {
    head_ = newNode;
    newNode->next=NULL;
    length_=1;
  }




}

/**
 * Inserts a new node at the back of the List.
 * This function **SHOULD** create a new ListNode.
 *
 * @param ndata The data to be inserted.
 */
template <typename T>
void List<T>::insertBack(const T & ndata) {
  /// @todo Graded in MP3.1
  ListNode * newNode = new ListNode(ndata);
  newNode -> next = NULL;


  if (tail_ != NULL) {
    newNode -> prev = tail_;
    tail_ -> next = newNode;
    tail_=newNode;
    length_++;
  }
  if (tail_ == NULL) {
    if(head_==NULL){
      head_=newNode;
      tail_=NULL;
      length_=1;
    }
    else{

    newNode->next=NULL;
    head_->next=newNode;
    newNode->prev=head_;
    tail_ = newNode;
    length_++;
    }
  }



}

/**
 * Helper function to split a sequence of linked memory at the node
 * splitPoint steps **after** start. In other words, it should disconnect
 * the sequence of linked memory after the given number of nodes, and
 * return a pointer to the starting node of the new sequence of linked
 * memory.
 *
 * This function **SHOULD NOT** create **ANY** new List or ListNode objects!
 *
 * This function is also called by the public split() function located in
 * List-given.hpp
 *
 * @param start The node to start from.
 * @param splitPoint The number of steps to walk before splitting.
 * @return The starting node of the sequence that was split off.
 */
template <typename T>
typename List<T>::ListNode * List<T>::split(ListNode * start, int splitPoint) {
  /// @todo Graded in MP3.1

  if(head_==NULL || start==NULL || splitPoint>length_){
    return NULL;
  }
  if(splitPoint<1 ){
    return start;
  }

  ListNode * curr = start;

  for (int i = 1; i <= splitPoint && curr!=NULL; i++) {
    if(curr->next!=NULL){
    curr = curr->next;
  }
  }
  if(curr!=NULL){

      curr->prev->next = NULL;

      tail_=curr->prev;
      curr->prev=NULL;
      return curr;
}
  return NULL;

}

/**
  * Modifies List using the rules for a TripleRotate.
  *
  * This function will to a wrapped rotation to the left on every three
  * elements in the list starting for the first three elements. If the
  * end of the list has a set of 1 or 2 elements, no rotation all be done
  * on the last 1 or 2 elements.
  *
  * You may NOT allocate ANY new ListNodes!
  */
template <typename T>
void List<T>::tripleRotate() {
  // @todo Graded in MP3.1
  if(head_==NULL){
    return ;
  }
  int l = this->List<T>::size();
  int count= l/3;
  ListNode * curr=head_;
  if(count==0 || l<3){
    return ;
  }
  for(int i=0; i<count ; i++){

  ListNode * n1=curr;
  ListNode * n2= curr->next;
  ListNode * n3= curr->next->next;
  if(n3->next==NULL){
    n1->next=NULL;
    if(n1->prev==NULL){
    n2->prev=NULL;
    head_=n2;
      }
    else {
      n1->prev->next=n2;
      n2->prev=n1->prev;
    }

  }
  if(n3->next!=NULL){
  n1->next=n3->next;
  if(n1->prev==NULL){
  n2->prev=NULL;
  head_=n2;
    }
  else{
    n2->prev=n1->prev;
  }
     }
  n1->prev=n3;
  n3->next=n1;
  n3->prev=n2;
  n2->next=n3;
  if(n1->next==NULL){
    tail_=n1;
    return ;
  }
  curr=n1->next;
  curr->prev=n1;
  }

}

/**
 * Reverses the current List.
 */
template <typename T>
void List<T>::reverse() {
  reverse(head_, tail_);

}

/**
 * Helper function to reverse a sequence of linked memory inside a List,
 * starting at startPoint and ending at endPoint. You are responsible for
 * updating startPoint and endPoint to point to the new starting and ending
 * points of the rearranged sequence of linked memory in question.
 *
 * @param startPoint A pointer reference to the first node in the sequence
 *  to be reversed.
 * @param endPoint A pointer reference to the last node in the sequence to
 *  be reversed.
 */
template <typename T>
void List<T>::reverse(ListNode *& startPoint, ListNode *& endPoint) {
  /// @todo Graded in MP3.2
  ListNode * start = startPoint;
  ListNode * end = endPoint;
  if(startPoint==NULL || endPoint==NULL  ){
    return ;
  }
  if(startPoint==tail_ || endPoint==head_){
    return ;
  }
  if(startPoint==endPoint){
    return ;
  }
  ListNode * curr = startPoint->next;
  ListNode * tprev = startPoint;

  ListNode * prevstart = NULL;
  ListNode * nexte = NULL;

  if(start->prev!=NULL){
   prevstart = start->prev;
  }

  if(end!=tail_){
    nexte = end->next;
  }



  //change all things in the middle
  while(curr!=end) {

    tprev->prev=curr;
    ListNode * temp=curr->next;
    curr->prev=temp;
    curr->next=tprev;
    tprev=curr;
    curr=temp;

   }

    curr->next=tprev;

  //end is right after start
  if(end==start->next){
    if(prevstart!=NULL){
      curr->prev=prevstart;
      prevstart->next=curr;
    }
    else{
    curr->prev=NULL;
    head_=curr;
    }

    tprev->prev=curr;
    if (nexte!=NULL){
      tprev->next=nexte;
      nexte->prev=tprev;
    }
    else{
      tprev->next=NULL;
      tail_=tprev;
    }

    if(nexte!=NULL){
    ListNode * lol = start;
    endPoint=start;
    nexte->prev=lol;
    lol->next=nexte;
    }
    if(nexte==NULL){
      endPoint=tail_;
    }

    if(prevstart!=NULL){
    ListNode * lol = prevstart->next;
    startPoint=end;
    prevstart->next=lol;
    }
    if(prevstart==NULL){
      startPoint=head_;
    }
    return ;

  }

  if(nexte!=NULL){
  start->next = nexte;
  nexte->prev=start;

   }
   // if start not head
   if(prevstart!=NULL){

   end->prev=prevstart;
   prevstart->next=end;
   }

  if(prevstart==NULL){

   curr->prev=NULL;
   head_=curr;

   }


  if(nexte==NULL){

   start->next = NULL;
   tail_=start;

 }

  //switch start and ending
  if(nexte!=NULL){
  ListNode * lol = nexte->prev;
  endPoint=start;
  nexte->prev=lol;
  }
  if(nexte==NULL){
    endPoint=tail_;
  }

  if(prevstart!=NULL){
  ListNode * lol = prevstart->next;
  startPoint=end;
  prevstart->next=lol;
  }
  if(prevstart==NULL){
    startPoint=head_;
  }
  return ;
}


/**
 * Reverses blocks of size n in the current List. You should use your
 * reverse( ListNode * &, ListNode * & ) helper function in this method!
 *
 * @param n The size of the blocks in the List to be reversed.
 */
template <typename T>
void List<T>::reverseNth(int n) {
  /// @todo Graded in MP3.2
  if(n==0 || head_==NULL){
    return ;
  }
  ListNode * start = head_;
  ListNode * curr = start;

  int i = n;
while(curr!=tail_){

  while(i>0){
    if(curr==tail_){
      reverse(start,curr);
      return ;
    }
    else{
      curr=curr->next;
      i--;
    }
    }

    reverse(start,curr->prev);

    start = curr;
    i=n;
}

 return ;

}


/**
 * Merges the given sorted list into the current sorted list.
 *
 * @param otherList List to be merged into the current list.
 */
template <typename T>
void List<T>::mergeWith(List<T> & otherList) {
    // set up the current list
    head_ = merge(head_, otherList.head_);
    tail_ = head_;

    // make sure there is a node in the new list
    if (tail_ != NULL) {
        while (tail_->next != NULL)
            tail_ = tail_->next;
    }
    length_ = length_ + otherList.length_;

    // empty out the parameter list
    otherList.head_ = NULL;
    otherList.tail_ = NULL;
    otherList.length_ = 0;
}

/**
 * Helper function to merge two **sorted** and **independent** sequences of
 * linked memory. The result should be a single sequence that is itself
 * sorted.
 *
 * This function **SHOULD NOT** create **ANY** new List objects.
 *
 * @param first The starting node of the first sequence.
 * @param second The starting node of the second sequence.
 * @return The starting node of the resulting, sorted sequence.
 */
template <typename T>
typename List<T>::ListNode * List<T>::merge(ListNode * first, ListNode* second) {
  /// @todo Graded in MP3.2
  if(first==NULL && second==NULL){
    return first;
  }
  if(first==NULL){
    return second;
  }
  if(second==NULL){
    return first ;
  }
  if(first->data<second->data){
    head_=first;
  }
  else{
    head_=second;
  }
  ListNode * f = first;

  ListNode * s = second;

  ListNode * curr = head_;
  while(f!=NULL){
    if(f==head_){
      if(f->next==NULL){
        if(curr==f){
        curr->next=s;
        s->prev=curr;
        return head_;
      }
      else{
        curr->next=f;
        f->prev=curr;
        return head_;
      }
      }

      f=f->next;
    }
    if(s==head_){
      if(s->next==NULL){
        if(curr==s){
        curr->next=f;
        f->prev=curr;
        return head_;
      }
      else{
        curr->next=s;
        s->prev=curr;
        return head_;
      }
      }
      s=s->next;

    }



    if(s->data < f->data){
      ListNode * temp = curr;
      while(temp->next!=NULL ){
        if(temp->next->data< s->data || temp->next->data==s->data){
        temp=temp->next;
        if(temp==s || temp==f){
          curr=temp->prev;
          break;
        }
         }
        else{
          curr=temp;
          break;
        }

      curr=temp;
      }
      curr->next=s;
      s->prev=curr;
      if(s->next==NULL){
        curr=curr->next;
        curr->next=f;
        f->prev=curr;
        return head_;
      }
      s=s->next;

    }
    if(f->data<s->data){
      ListNode * temp = curr;
      while(temp->next!=NULL ){
        if(temp->next->data< f->data || temp->next->data==f->data){
        temp=temp->next;
        if(temp==s || temp==f){
          curr=temp->prev;
          break;
        }
      }
      else{
        curr=temp;
        break ;
      }
      curr=temp;
      }
      curr->next=f;
      f->prev=curr;
      if(f->next==NULL){
        curr=curr->next;
        curr->next=s;
        s->prev=curr;
        return head_;
      }
      f=f->next;

    }
    if(s->data==f->data){
      ListNode * temp = curr;
      while(temp->next->data<s->data){
        temp=temp->next;
        if(temp==s || temp==f){
          curr=temp->prev;
          break;
        }
      }
      curr=temp;
      curr->next=s;
      s->prev=curr;
      if(s->next==NULL){
        curr=curr->next;
        curr->next=f;
        f->prev=curr;
        return head_;
      }
      s=s->next;

      curr=curr->next;
      curr->next=f;
      f->prev=curr;

      if(f->next==NULL){
        curr=curr->next;
        curr->next=s;
        s->prev=curr;
        return head_;
      }
      f=f->next;
    }

    curr=curr->next;

  }
  if(f->next==NULL){
    if(curr==f){
    curr->next=s;
    s->prev=curr;
    return head_;
  }
  else{
    curr->next=f;
    f->prev=curr;
    return head_;
  }
  }
  if(s->next==NULL){
    if(curr==s){
    curr->next=f;
    f->prev=curr;
    return head_;
  }
  else{
    curr->next=s;
    s->prev=curr;
    return head_;
  }
  }

  return head_;
}

/**
 * Sorts a chain of linked memory given a start node and a size.
 * This is the recursive helper for the Mergesort algorithm (i.e., this is
 * the divide-and-conquer step).
 *
 * Called by the public sort function in List-given.hpp
 *
 * @param start Starting point of the chain.
 * @param chainLength Size of the chain to be sorted.
 * @return A pointer to the beginning of the now sorted chain.
 */
template <typename T>
typename List<T>::ListNode* List<T>::mergesort(ListNode * start, int chainLength) {
  /// @todo Graded in MP3.2
  if (chainLength == 1 || start == NULL) {
		return start;
	}

  else {
		ListNode* h = split(start, chainLength/2);

		start = mergesort(start, chainLength/2);

    h = mergesort(h,chainLength%2+chainLength/2);

		return merge(h,start);
  }

    start->prev = NULL;
    start->next = NULL;
    return start;
  }
