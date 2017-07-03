/***********************************************************************
 * Component:
 *    Week 09, Binary Search Tree (BST)
 *    Brother Helfrich, CS 235
 * Author:
 *    Br. Helfrich
 * Summary:
 *    Create a binary search tree
 ************************************************************************/

#ifndef BST_H
#define BST_H

#include "bnode.h"    // for BinaryNode
#include "stack.h"    // for Stack

// forward declaration for the BST iterator
template <class T>
class BSTIterator; 

/*****************************************************************
 * BINARY SEARCH TREE
 * Create a Binary Search Tree
 *****************************************************************/
template <class T>
class BST
{
public:
   // constructors, destructor, assignment operator
   BST() : root(NULL)                {                                    }
   BST(const BST & rhs)               throw (const char *);
  ~BST()                             { clear();                           }
   BST & operator = (const BST & rhs) throw (const char *);

   // standard container interfaces
   bool empty() const                { return root == NULL;               }
   int  size() const                 { return empty() ? 0 : root->size(); }
   void clear()
   {
      if (root)
         deleteBinaryTree(root);
      assert(empty());
   }

   // BST specific interfaces
   void insert(const T & t) throw (const char * );
   void remove(BSTIterator <T> & it);
   BSTIterator <T> find(const T & t);

   // for debug purposes so we can validate the tree
   BinaryNode <T> * getRoot() { return root; }
   
   // iterators
   BSTIterator <T> begin();
   BSTIterator <T> end()             { return BSTIterator <T> (NULL); }
   BSTIterator <T> rbegin();
   BSTIterator <T> rend()            { return BSTIterator <T> (NULL); }
   
private:
   // delete a single node from the tree
   void deleteNode (BinaryNode <T> * & pDelete, bool toRight);

   BinaryNode <T> * root;
};

/**********************************************************
 * BINARY SEARCH TREE ITERATOR
 * Forward and reverse iterator through a BST
 *********************************************************/
template <class T>
class BSTIterator
{
public:
   // constructors and assignment
   BSTIterator(BinaryNode <T> * p = NULL)    { nodes.push(p);     }
   BSTIterator(Stack <BinaryNode <T> *> & s) { nodes = s;         }
   BSTIterator(const BSTIterator <T> & rhs)  { nodes = rhs.nodes; }
   BSTIterator <T> & operator = (const BSTIterator <T> & rhs)
   {
      // need an assignment operator for the Stack class.
      nodes = rhs.nodes;
   }

   // compare
   bool operator == (const BSTIterator <T> & rhs) const
   {
      // only need to compare the leaf node 
      return rhs.nodes.const_top() == nodes.const_top();
   }
   bool operator != (const BSTIterator <T> & rhs) const
   {
      // only need to compare the leaf node 
      return rhs.nodes.const_top() != nodes.const_top();
   }

   // de-reference. Cannot change because it will invalidate the BST
   const T & operator * () const 
   {
      return nodes.const_top()->data;
   }

   // iterators
   BSTIterator <T> & operator ++ ();
   BSTIterator <T>   operator ++ (int postfix)
   {
      BSTIterator <T> itReturn = *this;
      ++(*this);
      return itReturn;
   }
   BSTIterator <T> & operator -- ();
   BSTIterator <T>   operator -- (int postfix)
   {
      BSTIterator <T> itReturn = *this;
      --(*this);
      return itReturn;
   }

   // must give friend status to remove so it can call getNode() from it
   friend void BST <T> :: remove(BSTIterator <T> & it);

private:
   
   // get the node pointer
   BinaryNode <T> * getNode() { return nodes.top(); }
   
   // the stack of nodes
   Stack < BinaryNode <T> * > nodes;
};


/*****************************************************
 * BST :: BEGIN
 * Return the first node (left-most) in a binary search tree
 ****************************************************/
template <class T>
BSTIterator <T> BST <T> :: begin()
{
   Stack < BinaryNode <T> * > nodes;

   nodes.push(NULL);
   nodes.push(root);
   while (nodes.top() != NULL && nodes.top()->pLeft)
      nodes.push(nodes.top()->pLeft);

   return nodes;   
}

/*****************************************************
 * BST :: RBEGIN
 * Return the last node (right-most) in a binary search tree
 ****************************************************/
template <class T>
BSTIterator <T> BST <T> :: rbegin()
{
   Stack < BinaryNode <T> * > nodes;

   nodes.push(NULL);
   nodes.push(root);
   while (nodes.top() != NULL && nodes.top()->pRight)
      nodes.push(nodes.top()->pRight);

   return nodes;
}  

#ifdef DEBUG
/**************************************************
 * BST :: VERIFY
 * Verify that the tree under the root is well-formed
 *************************************************/
template <class T>
void BST <T> :: verify() const
{
   // verify that every node is where it should be
   root->verifyBTree();

   // find the depth of the black nodes
   int depth = root->findDepth();

   // verify the red-black tree under here
   root->verifyRedBlack(depth);
}
#endif // DEBUG

/*********************************************
 * BST :: COPY CONSTRUCTOR
 * Copy one tree to another
 ********************************************/
template <class T>
BST <T> :: BST ( const BST <T> & rhs) throw (const char *) : root(NULL)
{
   *this = rhs;
}

/**********************************************
 * BST :: COPY
 * Make a copy of a binary search tree
 **********************************************/
template <class T>
BST <T> & BST <T> :: operator = (const BST <T> & rhs) throw (const char *)
{
   clear();
   if (rhs.root != NULL)
   {
      try
      {
         this->root = new BinaryNode <T> (rhs.root->data);
         this->root->isRed = false;
         copyBinaryTree(rhs.root, this->root);
      }
      catch (...)
      {
         throw "ERROR: Unable to allocate a node";
      }
   }
   return *this;
}

/*****************************************************
 * BST :: INSERT
 * Insert a node at a given location in the tree
 ****************************************************/
template <class T>
void BST <T> :: insert(const T & t) throw (const char *)
{
   debug(std::cerr << "===== insert(" << t << ") =====\n");
   // if we are at a trivial state (empty tree), then create a new root
   if (root == NULL)
   {
      root = new BinaryNode <T> (t);
      root->balance();
      debug(verify());
      return;
   }

   debug(std::cerr << "root:" << root->data << ' ');

   // otherwise, go a searching for the correct spot
   BinaryNode <T> * node = root;
   bool done = false;
   debug(verify());
   while (!done)
   {
      // if the center node is larger, go left
      if (node->data > t)
      {
         // if there is a node to the left, follow it
         if (node->pLeft)
         {
            debug(std::cerr << "left->");
            node = node->pLeft;
         }
         // if we are at the leaf, then create a new node
         else
         {
            node->addLeft(t);
            node = NULL;
            done = true;
         }

      }

      // if the center node is smaller, go right
      else
      {
         // if there is a node to the right, follow it
         if (node->pRight)
         {
            node = node->pRight;
            debug(std::cerr << "right->");
         }
         // if we are at the left, then create a new node.
         else
         {
            node->addRight(t);
            node = NULL;
            done = true;
         }
      }
   }

   // the root might have shifted due to rotation
   while (root->pParent != NULL)
      root = root->pParent;
   assert(root->pParent == NULL);
   debug(verify());
}

/*************************************************
 * BST :: REMOVE
 * Remove a given node as specified by the iterator
 ************************************************/
template <class T>
void BST <T> :: remove(BSTIterator <T> & it)
{
   // find the node
   BinaryNode <T> * pNode = it.getNode();

   // do nothing if there is nothing to do
   if (pNode == NULL)
      return;

   // if there is only one child (right) or no children (how sad!)
   if (pNode->pLeft == NULL)
      deleteNode(pNode, true /* goRight */);

   // if there is only one child (left)
   else if (pNode->pRight == NULL)
      deleteNode(pNode, false /* goRight */);

   // otherwise, swap places with the in-order successor
   else
   {
      // find the in-order successor
      BinaryNode <T> * pNodeIOS = pNode->pRight;
      while (pNodeIOS->pLeft != NULL)
         pNodeIOS = pNodeIOS->pLeft;

      // copy its data
      pNode->data = pNodeIOS->data;

      // if there are any children under the in-order successor, fix them
      assert(pNodeIOS->pLeft == NULL); // there cannot be a left child or
                                       // I would not be the IOS
      deleteNode(pNodeIOS, true /*goRight*/);

      // prepare for deletion
      pNode = pNodeIOS;
   }

   delete pNode;
}

/****************************************************
 * DELETE NODE
 * Delete a single node (pDelete) from the tree indicated
 * by a node (pDelete). 
 *    pDelete     the node to be deleted
 *    toRight     should the right branch inherit our place?
 ****************************************************/
template <class T>
void BST <T> :: deleteNode(BinaryNode <T> * & pDelete, bool toRight)
{
   // shift everything up
   BinaryNode <T> * pNext = (toRight ? pDelete->pRight : pDelete->pLeft);

   // if we are not the parent, hook ourselves into the existing tree
   if (pDelete != root)
   {
      if (pDelete->pParent->pLeft == pDelete)
      {
         pDelete->pParent->pLeft = NULL;
         pDelete->pParent->addLeft(pNext);
      }
      else
      {
         pDelete->pParent->pRight = NULL;
         pDelete->pParent->addRight(pNext);
      }
   }

   // otherwise, the pNext is the new root
   else
   {
      root = pNext;
      pNext->pParent = NULL;
   }
}

/****************************************************
 * BST :: FIND
 * Return the node corresponding to a given value
 ****************************************************/
template <class T>
BSTIterator <T> BST <T> :: find(const T & t)
{
   Stack < BinaryNode <T> * > nodesFound;

   nodesFound.push(root);

   // non-recurive solution
   while (nodesFound.top() != NULL)
   {
      if (nodesFound.top()->data == t)
         return nodesFound;
      if (nodesFound.top()->data > t)
         nodesFound.push(nodesFound.top()->pLeft);
      else
         nodesFound.push(nodesFound.top()->pRight);
   }

   return BSTIterator <T> ();
}

/**************************************************
 * BST ITERATOR :: INCREMENT PREFIX
 * advance by one
 *************************************************/
template <class T>
BSTIterator <T> & BSTIterator <T> :: operator ++ ()
{
   // do nothing if we have nothing
   if (nodes.top() == NULL)
      return *this;
   
   // if there is a right node, take it
   if (nodes.top()->pRight != NULL)
   {
      nodes.push(nodes.top()->pRight);

      // there might be more left-most children
      while (nodes.top()->pLeft)
         nodes.push(nodes.top()->pLeft);
      return *this;
   }

   // there are no right children, the left are done
   assert(nodes.top()->pRight == NULL);
   BinaryNode <T> * pSave = nodes.top();
   nodes.pop();

   // if the parent is the NULL, we are done!
   if (NULL == nodes.top())
      return *this;

   // if we are the left-child, got to the parent.
   if (pSave == nodes.top()->pLeft)
      return *this;

   // we are the right-child, go up as long as we are the right child!
   while (nodes.top() != NULL && pSave == nodes.top()->pRight)
   {
      pSave = nodes.top();
      nodes.pop();
   }
      
   return *this;      
}

/**************************************************
 * BST ITERATOR :: DECREMENT PREFIX
 * advance by one
 *************************************************/
template <class T>
BSTIterator <T> & BSTIterator <T> :: operator -- ()
{
   // do nothing if we have nothing
   if (nodes.top() == NULL)
      return *this;

   // if there is a left node, take it
   if (nodes.top()->pLeft != NULL)
   {
      nodes.push(nodes.top()->pLeft);

      // there might be more right-most children
      while (nodes.top()->pRight)
         nodes.push(nodes.top()->pRight);
      return *this;
   }

   // there are no left children, the right are done
   assert(nodes.top()->pLeft == NULL);
   BinaryNode <T> * pSave = nodes.top();
   nodes.pop();

   // if the parent is the NULL, we are done!
   if (NULL == nodes.top())
      return *this;

   // if we are the right-child, got to the parent.
   if (pSave == nodes.top()->pRight)
      return *this;

   // we are the left-child, go up as long as we are the left child!
   while (nodes.top() != NULL && pSave == nodes.top()->pLeft)
   {
      pSave = nodes.top();
      nodes.pop();
   }

   return *this;
}

#endif // BST_H