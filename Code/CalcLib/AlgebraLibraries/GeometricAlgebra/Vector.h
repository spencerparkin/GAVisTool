// Vector.h

/*
 * Copyright (C) 2012 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

//=========================================================================================
class GeometricAlgebra::Vector : public Utilities::List::Item
{
public:

	Vector( void );
	virtual ~Vector( void );

	virtual Item* MakeCopy( void ) const = 0;
	virtual double InnerProduct( const Vector& right ) const = 0;
	virtual const char* Name( void ) const = 0;
	virtual const char* LatexName( void ) const = 0;

	virtual Utilities::List::SortComparison SortCompare( const Item* compareWithItem ) const override;
};

//=========================================================================================
class GeometricAlgebra::Vector_e0 : public Vector
{
public:

	Vector_e0( void );
	virtual ~Vector_e0( void );

	virtual Item* MakeCopy( void ) const override;
	virtual double InnerProduct( const Vector& right ) const override;
	virtual const char* Name( void ) const override;
	virtual const char* LatexName( void ) const override;
};

//=========================================================================================
class GeometricAlgebra::Vector_e1 : public Vector
{
public:

	Vector_e1( void );
	virtual ~Vector_e1( void );

	virtual Item* MakeCopy( void ) const override;
	virtual double InnerProduct( const Vector& right ) const override;
	virtual const char* Name( void ) const override;
	virtual const char* LatexName( void ) const override;
};

//=========================================================================================
class GeometricAlgebra::Vector_e2 : public Vector
{
public:

	Vector_e2( void );
	virtual ~Vector_e2( void );

	virtual Item* MakeCopy( void ) const override;
	virtual double InnerProduct( const Vector& right ) const override;
	virtual const char* Name( void ) const override;
	virtual const char* LatexName( void ) const override;
};

//=========================================================================================
class GeometricAlgebra::Vector_no : public Vector
{
public:

	Vector_no( void );
	virtual ~Vector_no( void );

	virtual Item* MakeCopy( void ) const override;
	virtual double InnerProduct( const Vector& right ) const override;
	virtual const char* Name( void ) const override;
	virtual const char* LatexName( void ) const override;
};

//=========================================================================================
class GeometricAlgebra::Vector_ni : public Vector
{
public:

	Vector_ni( void );
	virtual ~Vector_ni( void );

	virtual Item* MakeCopy( void ) const override;
	virtual double InnerProduct( const Vector& right ) const override;
	virtual const char* Name( void ) const override;
	virtual const char* LatexName( void ) const override;
};

// Vector.h