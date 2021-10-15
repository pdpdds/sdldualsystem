#ifndef MATRIX_H_
#define MATRIX_H_

//Includes
//#include 	"SDL/SDL.h"				//For SDL access

template <class T>
class Matrix {
	
public:
	//Attributes
	
	//Constructors - Destructor
	Matrix(unsigned param_rows, unsigned param_columns);
	virtual ~Matrix();
	
	//Methods
	T Get(unsigned i, unsigned j);
	void Set(unsigned i, unsigned j, T value);
	//T& operator () (unsigned i, unsigned j);
    //const T& operator () (unsigned i, unsigned j) const;
	
private:

	//Attributes
	unsigned short rows, columns;
	T* values;
	
	//Methods
};

#endif /*MATRIX_H_*/

template <class T>
Matrix<T>::Matrix(unsigned param_rows, unsigned param_columns) {
	
	if(param_rows <= 0 || param_columns <= 0) {
		//Base attributes
		rows = 1;
		columns = 1;
	
		//Values of the matrix
		values = new T[(rows * columns)];
		
	} else {
		//Base attributes
		rows = param_rows;
		columns = param_columns;
	
		//Values of the matrix
		values = new T[(rows * columns)];
	}
}

template <class T>
Matrix<T>::~Matrix() {
	if (values != 0) {
        delete [] values;
    }
}

//Getters and setters
template <class T>
T Matrix<T>::Get(unsigned i, unsigned j) {
	
	return values[(i * columns) + j];
	
}

template <class T>
void Matrix<T>::Set(unsigned i, unsigned j, T value) {
	
	values[(i * columns) + j] = value;
	
}

//// Retorna una referencia al elemento [i,j] de la matriz.
//// M(i,j) = M[i][j].
//// Permite val = M(i,j);
//template <class T>
//const T& Matrix<T>::operator() (unsigned i, unsigned j) const {
//    return values[(i * columns) + j ];
//}
//
///// Retorna una referencia al elemento [i,j] de la matriz.
///// M(i,j) = M[i][j].
///// Permite M(i,j) = val;
//template <class T>
//T& Matrix<T>::operator() (unsigned i, unsigned j) {
//    return values[(i * columns) + j ] ;
//}

