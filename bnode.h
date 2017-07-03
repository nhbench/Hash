/***********************************************************************
 * Component:
 *    Week 10, Binary Node w/ red lack info
 *    Brother Helfrich, CS 235
 * Author:
 *    Br. Helfrich
 * Summary:
 *    Create a binary node for use in binary trees
 ************************************************************************/

#ifndef BNODE_H
#define BNODE_H

#include <iostream>  // for OFSTREAM
#include <cassert>

// debug stuff
#ifndef debug
#ifdef DEBUG
#define debug(x) x
#else
#define debug(x)
#endif // DEBUG
#endif // !debug

/*****************************************************************
 * BINARY NODE
 * A single node in a binary tree.  Note that the node does not know
 * anything about the properties of the tree so no validation can be done.
 *****************************************************************/
template <class T>
class BinaryNode
{
public:
   // constructor: set the pointers to NULL
   BinaryNode()            : pLeft(NULL), pRight(NULL), pParent(NULL),
      data(T()), isRed(true)  {}
   BinaryNode(const T & t) : pLeft(NULL), pRight(NULL), pParent(NULL),
      data(t), isRed(true) {}

   // size
   int size() const
   {
      return 1 +
         (pLeft == NULL ? 0 : pLeft->size()) +
         (pRight == NULL ? 0 : pRight->size());
   }
   
   // add a node the left/right
   void addLeft (BinaryNode <T> * pNode);
   void addRight(BinaryNode <T> * pNode);

   // create a node and add it to the left/right
   void addLeft (const T & t) throw (const char *);
   void addRight(const T & t) throw (const char *);

   // is the passed node our right child?
   bool isRightChild(BinaryNode <T> * pNode) const { return pRight == pNode; }
   bool isLeftChild( BinaryNode <T> * pNode) const { return pLeft  == pNode; }
   
   // since no validation is done, everything is public
   BinaryNode <T> * pLeft;
   BinaryNode <T> * pRight;
   BinaryNode <T> * pParent;

   // balance the tree
   void balance();

#ifdef DEBUG
   // verify
   void verifyBTree() const;

   // how deep is the black nodes?
   int findDepth() const;

   // make sure all four red-black rules are followed
   void verifyRedBlack(int depth) const;
#endif // DEBUG   
   
   // the data of unknown type: cannot validate so is public
   T data;
   bool isRed;
};

#ifdef DEBUG
/****************************************************
 * BINARY NODE :: FIND DEPTH
 * Find the depth of the black nodes. This is useful for
 * verifying that a given red-black tree is valid
 ****************************************************/
template <class T>
int BinaryNode <T> :: findDepth() const
{
   // if there are no children, the depth is ourselves
   if (pRight == NULL && pLeft == NULL)
      return (isRed ? 0 : 1);

   // if there is a right child, go that way
   if (pRight != NULL)
      return (isRed ? 0 : 1) + pRight->findDepth();
   else
      return (isRed ? 0 : 1) + pLeft->findDepth();
}

/****************************************************
 * BINARY NODE :: VERIFY RED BLACK
 * Do all four red-black rules work here?
 ***************************************************/
template <class T>
void BinaryNode <T> :: verifyRedBlack(int depth) const
{
   depth -= (isRed == false) ? 1 : 0;

   // Rule a) Every node is either red or black
   assert(isRed == true || isRed == false); // this feels silly

   // Rule b) The root is black
   if (pParent == NULL)
      assert(isRed == false);

   // Rule c) Red nodes have black children
   if (isRed == true)
   {
      if (pLeft != NULL)
         assert(pLeft->isRed == false);
      if (pRight != NULL)
         assert(pRight->isRed == false);
   }

   // Rule d) Every path from a leaf to the root has the same # of black nodes
   if (pLeft == NULL && pRight && NULL)
      assert(depth == 0);
   if (pLeft != NULL)
      pLeft->verifyRedBlack(depth);
   if (pRight != NULL)
      pRight->verifyRedBlack(depth);
}


/******************************************************
 * VERIFY B TREE
 * Verify that the tree is correctly formed
 ******************************************************/
template <class T>
void BinaryNode <T> :: verifyBTree() const
{
   // check parent
   if (pParent)
      assert(pParent->pLeft == this || pParent->pRight == this);

   // check left
   if (pLeft)
   {
      assert(pLeft->data <= data);
      assert(pLeft->pParent == this);
      pLeft->verifyBTree();
   }

   // check right
   if (pRight)
   {
      assert(pRight->data >= data);
      assert(pRight->pParent == this);
      pRight->verifyBTree();
   }
}
#endif // DEBUG


/******************************************************
 * BINARY NODE :: BALANCE
 * Balance the tree from a given location
 ******************************************************/
template <class T>
void BinaryNode <T> :: balance()
{
   debug(verifyBTree());

   // Case 1: if we are the root, then color ourselves black and call it a day.
   if (pParent == NULL)
   {
      isRed = false;
      debug(std::cerr << "Case 1\n");
      return;
   }

   // Case 2: if the parent is black, then there is nothing left to do
   if (pParent->isRed == false)
   {
      debug(std::cerr << "Case 2\n");
      return;
   }

   // we better have a grandparent.  Otherwise there is a red node at the root
   assert(pParent->pParent != NULL);

   // find my relatives
   BinaryNode <T> * pGranny  = pParent->pParent;
   BinaryNode <T> * pGreatG  = pGranny->pParent;
   BinaryNode <T> * pSibling =
      pParent->isRightChild(this   ) ? pParent->pLeft : pParent->pRight;
   BinaryNode <T> * pAunt    =
      pGranny->isRightChild(pParent) ? pGranny->pLeft : pGranny->pRight;

   // verify things are as they should be
   assert(pGranny != NULL);          // I should have a grandparent here
   assert(pGranny->isRed == false);  // if granny is red, we violate red-red!

   // Case 3: if the aunt is red, then just recolor
   if (pAunt != NULL && pAunt->isRed == true)
   {
      // verification
      debug(std::cerr << "Case 3, ");
      debug(pGranny->verifyBTree());

      // perform Case 3
      pGranny->isRed = true;         // grandparent becomes red
      pParent->isRed = false;        // parent becomes black
      pAunt->isRed  = false;         // aunt becomes black
      pGranny->balance();            // balance granny!
      return;
   }

   // Case 4: if the aunt is black or non-existant, then we need to rotate
   assert(pParent->isRed == true && pGranny->isRed == false &&
          (pAunt == NULL || pAunt->isRed == false));
   debug(pGranny->verifyBTree());

   // the new top of the sub-tree
   BinaryNode <T> * pHead = NULL;

   // Case 4a: We are mom's left and mom is granny's left
   if (pParent->pLeft == this && pGranny->pLeft == pParent)
   {
      // verify case 4a is as it should be
      debug(std::cerr << "Case 4a\n");
      debug(pGranny->verifyBTree());
      assert(pParent->pLeft == this);
      assert(pGranny->pRight == pAunt);
      assert(this->isRed == true);

      // perform the necessary rotation
      pGranny->addLeft(pSibling);
      pParent->addRight(pGranny);
      pHead = pParent;

      // set the colors
      pParent->isRed = false;
      pGranny->isRed = true;
   }

   // Case 4b: We are mom's right and mom is granny's left
   else if (pParent->pRight == this && pGranny->pLeft == pParent)
   {
      // verify case 4b is as it should be
      debug(std::cerr << "Case 4b\n");
      assert(pGranny->pRight == pAunt);
      assert(pParent->pLeft == pSibling);
      assert(pParent->isRed == true);

      // perform the necessary rotation
      pGranny->addLeft(this->pRight);
      pParent->addRight(this->pLeft);
      this->addLeft(pParent);
      this->addRight(pGranny);
      pHead = this;

      // set the colors
      this->isRed = false;
      pGranny->isRed = true;
   }

   // case 4c: We are mom's right and mom is granny's right
   else if (pParent->isRightChild(this) && pGranny->isRightChild(pParent))
   {
      // verify case 4c is as it should be
      debug(std::cerr << "Case 4c\n");
      assert(pParent->pRight == this);
      assert(pGranny->isRed == false);
      assert(pGranny->pLeft == pAunt);

      // perform the necessary rotation
      pParent->addLeft(pGranny);
      pGranny->addRight(pSibling);
      pHead = pParent;

      // set the colors
      pGranny->isRed = true;
      pParent->isRed = false;
   }

   // case 4d: we are mom's left and mom is granny's right
   else if (pParent->isLeftChild(this) && pGranny->isRightChild(pParent))
   {
      // verify case 4d is as it should be
      debug(std::cerr << "Case 4d\n");
      assert(pGranny->pLeft == pAunt);
      assert(pGranny->pRight == pParent);
      assert(pParent->pRight == pSibling);

      // perform the necessary rotation
      pGranny->addRight(this->pLeft);
      pParent->addLeft(this->pRight);
      this->addLeft(pGranny);
      this->addRight(pParent);
      pHead = this;

      // ste the colors
      this->isRed = false;
      pGranny->isRed = true;
      pParent->isRed = true;
   }

   // else we are really confused!
   else
   {
      assert(false); // !!
   }

   // fix up great granny if she is not null
   if (pGreatG == NULL)
      pHead->pParent = NULL;
   else if (pGreatG->pRight == pGranny)
      pGreatG->addRight(pHead);
   else if (pGreatG->pLeft == pGranny)
      pGreatG->addLeft(pHead);

   debug(pHead->verifyBTree());
}


/******************************************************
 * BINARY NODE :: ADD LEFT
 * Add a node to the left of the current node
 ******************************************************/
template <class T>
void BinaryNode <T> :: addLeft (BinaryNode <T> * pNode)
{
   pLeft = pNode;
   if (pNode)
      pNode->pParent = this;
}

/******************************************************
 * BINARY NODE :: ADD RIGHT
 * Add a node to the right of the current node
 ******************************************************/
template <class T>
void BinaryNode <T> :: addRight (BinaryNode <T> * pNode)
{
   pRight = pNode;
   if (pNode)
      pNode->pParent = this;
}

/******************************************************
 * BINARY NODE :: ADD LEFT
 * Add a node to the left of the current node
 ******************************************************/
template <class T>
void BinaryNode <T> :: addLeft (const T & t) throw (const char *)
{
   assert(pLeft == NULL);
   assert(t <= data);
   try
   {
      // add the new node
      BinaryNode<T> * pNode = new BinaryNode <T> (t);
      addLeft(pNode);

      // paranoia and double-checks
      debug(verifyBTree());
      debug(std::cerr << "addLeft(" << t << ") ");

      pNode->balance();
   }
   catch (...)
   {
      throw "ERROR: Unable to allocate a node";
   }
}

/******************************************************
 * BINARY NODE :: ADD RIGHT
 * Add a node to the right of the current node
 ******************************************************/
template <class T>
void BinaryNode <T> :: addRight (const T & t) throw (const char *)
{
   assert(pRight == NULL);
   assert(t >= data);
   try
   {
      // add the new node
      BinaryNode<T> * pNode = new BinaryNode <T> (t);
      addRight(pNode);
      
      // paranoia and double-checks
      debug(verifyBTree());
      debug(std::cerr << "addRight(" << t << ") ");
      
      pNode->balance();
   }
   catch (...)
   {
      throw "ERROR: Unable to allocate a node";
   }
}


/*****************************************************
 * DELETE BINARY TREE
 * Delete all the nodes below pThis including pThis
 * using postfix traverse: LRV
 ****************************************************/
template <class T>
void deleteBinaryTree(BinaryNode <T> * & pThis)
{
   if (pThis == NULL)
      return;
   deleteBinaryTree(pThis->pLeft);
   deleteBinaryTree(pThis->pRight);
   delete pThis;
   pThis = NULL;
}

/**********************************************
 * COPY BINARY TREE
 * Copy pSrc->pRight to pDest->pRight and
 * pSrc->pLeft onto pDest->pLeft
 *********************************************/
template <class T>
void copyBinaryTree(const BinaryNode <T> * pSrc,
                    BinaryNode <T> * pDest) throw (const char *)
{
   BinaryNode <T> * p = NULL;
   assert(pSrc && pDest);
   assert(pDest->pLeft == NULL && pDest->pRight == NULL);

   try
   {
      if (pSrc->pRight)
      {
         p = new BinaryNode <T> (pSrc->pRight->data);
         p->isRed = pSrc->pRight->isRed;
         pDest->addRight(p);
         copyBinaryTree( pSrc->pRight, pDest->pRight);
      }
      if (pSrc->pLeft)
      {
         p = new BinaryNode <T> (pSrc->pLeft->data);
         p->isRed = pSrc->pLeft->isRed;
         pDest->addLeft(p);
         copyBinaryTree(pSrc->pLeft, pDest->pLeft);
      }
   }
   catch (...)
   {
      throw "ERROR: Unable to allocate a node";
   }
}

/**********************************************************
 * INSERTION
 * Display the contents of the binary tree using infix traverse: LVR
 **********************************************************/
template <class T>
std::ostream & operator << (std::ostream & out, BinaryNode <T> * rhs)
{
   if (rhs)
      out << rhs->pLeft << rhs->data << " " << rhs->pRight;
 
   return out;
}

#endif // BNODE_H