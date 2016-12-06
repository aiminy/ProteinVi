#ifndef _tmatrix_H
#define _tmatrix_H

#include "tvector.h"

// *******************************************************************
// Last Revised: September 1, 1997
//
// APCS matrix class
//
// extends tvector.h to two dimensional "safe" (range-checked) matrices
// examples are given at the end of this file
// *******************************************************************

template <class itemType>

class tmatrix
{
  public:

  // constructors/destructor
    tmatrix( );                                      // default size 0 x 0
    tmatrix( int rows, int cols );                   // size rows x cols
    tmatrix( int rows, int cols,
            const itemType & fillValue );           // all entries == fillValue
    tmatrix( const tmatrix & mat );                   // copy constructor
    ~tmatrix( );                                     // destructor

  // assignment
    const tmatrix & operator = ( const tmatrix & rhs );

  // accessors
    int numrows( ) const;                             // number of rows
    int numcols( ) const;                             // number of columns

  // indexing
    const tvector<itemType> & operator [ ] ( int k ) const;  // range-checked indexing
    tvector<itemType> & operator [ ] ( int k );              // range-checked indexing

  // modifiers
    void resize( int newRows, int newCols );   // resizes matrix to newRows x newCols
                                               // (can result in losing values)
  private:

    int myRows;                             // # of rows (capacity)
    int myCols;                             // # of cols (capacity)
    tvector<tvector<itemType> > myMatrix; // the matrix of items
};


// *******************************************************************
// Specifications for matrix functions
//
// To use this class, itemType must satisfy the same constraints
// as forvector class.
//
// Any violation of a function's precondition will result in an error  message
// followed by a call to abort.
//
// constructors/destructor
//
//  tmatrix( );
//     postcondition: matrix of size 0x0 is constructed, and therefore
//                    will need to be resized later
//
//  tmatrix( int rows, int cols );
//     precondition: 0 <= rows and 0 <= cols
//     postcondition: matrix of size rows x cols is constructed
//
//  tmatrix( int rows, int cols, const itemType & fillValue );
//     precondition: 0 <= rows and 0 <= cols
//     postcondition: matrix of size rows x cols is constructed
//                    all entries are set by assignment to fillValue after
//                    default construction
//
//  tmatrix( const tmatrix<itemType> & mat );
//     postcondition: matrix is a copy of mat
//
//  ~tmatrix( );
//     postcondition: matrix is destroyed
//
// assignment
//
//  const tmatrix & operator = ( const tmatrix & rhs );
//     postcondition: normal assignment via copying has been performed
//                    (if matrix and rhs were different sizes, matrix has 
//                    been resized to match the size of rhs)
//
// accessors
//
//  int numrows( ) const;
//     postcondition: returns number of rows
//
//  int numcols( ) const;
//     postcondition: returns number of columns
//
// indexing
//
//  const tvector<itemType> & operator [ ] ( int k ) const;
//     precondition: 0 <= k < number of rows
//     postcondition: returns k-th row
//
//  tvector<itemType> & operator [ ] ( int k );
//     precondition: 0 <= k < number of rows
//     postcondition: returns k-th row
//
// modifiers
//
//  void resize( int newRows, int newCols );
//     precondition: matrix size is rows X cols,
//                   0 <= newRows and 0 <= newCols
//     postcondition: matrix size is newRows X newCols;
//                    for each 0 <= j <= min(rows,newRows) and
//                    for each 0 <= k <= min(cols,newCols), matrix[j][k] is
//                    a copy of the original; other elements of matrix are
//                    initialized using the default constructor for itemType
//                    Note: if newRows < rows or newCols < cols,
//                          elements may be lost
//
//  Examples of use:
//
//     tmatrix<double> dmat( 100, 80 );       // 100 x 80 matrix of doubles
//     tmatrix<double> dzmat( 100, 80, 0.0 ); // initialized to 0.0
//     tmatrix<apstring> smat( 300, 1 );      // 300 strings
//     tmatrix<int> imat;                     // has room for 0 ints

#include "tmatrix.cpp"
#endif


