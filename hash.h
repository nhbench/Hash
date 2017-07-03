/***********************************************************************
* Program:
*    Week 12, Hash
*    Brother Helfrich, CS 235
* Author:
*    Nathan Bench, Jed Bilman, Jeremy Chandler, Justin Chandler
* Summary: 
*    Implementation of the Hash class.
************************************************************************/

#ifndef HASH_H
#define HASH_H

#include "bst.h"

template <class T>
class Hash
{
   public:
      Hash(): tree(NULL), numElements(0), numBuckets(0) {}
      Hash(int numBuckets) throw (const char *);
      Hash(const Hash <T> & rhs) throw (const char *)
      {
         *this = rhs;
      }
      ~Hash()
      {
         clear();
      }
      
      Hash <T> & operator = (const Hash <T> & rhs) throw (const char *);
      
      int size() const       { return numElements;       }
      int capacity() const   { return numBuckets;        }
      bool empty() const     { return numElements == 0;  }
      void clear()
      {
         for (int i = 0; i < numBuckets; i++)
            tree[i].clear();
         numBuckets = numElements = 0;
      }
      
      bool find(const T & t);
      void insert(const T & t);
      virtual int hash(const T & t) const = 0; 
      
   private:
      int numElements, numBuckets;
      BST <T> * tree; // an array of bsts
};

template <class T>
Hash <T> :: Hash(int numBuckets) throw (const char *)
{
   this->numBuckets = numBuckets;
   numElements = 0;
   try
   {
      tree = new BST <T> [numBuckets];
      // do we need to construct the individual BSTs  now?
   }
   catch(std::bad_alloc)
   {
      throw "ERROR: Unable to allocate memory for the hash";
   }
}

template <class T>
Hash <T> & Hash <T> :: operator = (const Hash & rhs) throw (const char *)
{
   numBuckets = rhs.numBuckets;
   numElements = rhs.numElements;
   try
   {
      tree = new BST <T>[numBuckets];
      for (int i = 0; i < numBuckets; i++)
      {
         // do I need a separate try? -- No, because the assignment operator has a try/catch statement
         tree[i] = rhs.tree[i];
      }
   }
   catch(std::bad_alloc)
   {
      throw "ERROR: Unable to allocate memory for the hash";
   }
}

template <class T>
bool Hash <T> :: find(const T & t)
{
   int i = hash(t);
   BSTIterator <T> it = tree[i].find(t);
   if (it == NULL)
      return false;
   else
      return true;
}

template <class T>
void Hash <T> :: insert(const T & t) // do I need to throw a const char *?
{
   int i = hash(t);
   tree[i].insert(t);
   numElements++;
}

#endif