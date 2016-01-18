#if NOMPI==0
#ifndef CLASSCOMMBUFFER_HPP_
#define CLASSCOMMBUFFER_HPP_

// =================================================================================== //
// INCLUDES                                                                            //
// =================================================================================== //
#include "preprocessor_defines.dat"
#include <cstdint>
#include <typeinfo>
#include <algorithm>
#include "mpi.h"
#include "mpi_datatype_conversion.hpp"

// =================================================================================== //
// NAME SPACES                                                                         //
// =================================================================================== //
using namespace std;

// =================================================================================== //
// CLASS DEFINITION                                                                    //
// =================================================================================== //

class Class_Comm_Buffer {

	template<int dim> friend class Class_Para_Tree;

	// ------------------------------------------------------------------------------- //
	// MEMBERS ----------------------------------------------------------------------- //
	uint32_t commBufferSize;
	char* commBuffer;
	int pos;
	MPI_Comm comm;

	// ------------------------------------------------------------------------------- //
	// CONSTRUCTORS ------------------------------------------------------------------ //
public:
	Class_Comm_Buffer();
	Class_Comm_Buffer(MPI_Comm comm_);
	Class_Comm_Buffer(uint32_t size, char value, MPI_Comm comm_);
	Class_Comm_Buffer(const Class_Comm_Buffer& other);
	~Class_Comm_Buffer();

	// ------------------------------------------------------------------------------- //
	// METHODS ----------------------------------------------------------------------- //
	//TODO routines write and read to write and read POD types in buffer
	Class_Comm_Buffer& operator=(const Class_Comm_Buffer& rhs);

	template<class T>
	void write(T& val);
	template<class T>
	void read(T& val);
};

#include "Class_Comm_Buffer.tpp"

#endif /* CLASSCOMMBUFFER_HPP_ */
#endif /* NOMPI */
