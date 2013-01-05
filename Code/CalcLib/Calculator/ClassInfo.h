// ClassInfo.h

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

//=========================================================================================
class CalcLib::CalcLibClass
{
protected:
	virtual const char* VirtualClassName( void ) const = 0;

public:
	virtual bool IsTypeOf( const char* givenClassName ) const;
	template< typename ClassType >
	inline ClassType* Cast( void );
	template< typename ClassType >
	inline const ClassType* Cast( void ) const;
};

//=========================================================================================
class CalcLib::CalcLibClassInfo : public Utilities::List::Item
{
public:

	CalcLibClassInfo( const char* className );
	CalcLibClassInfo( const char* className, const char* inheritingClassName1 );
	CalcLibClassInfo( const char* className, const char* inheritingClassName1, const char* inheritingClassName2 );
	CalcLibClassInfo( const char* className, const char* inheritingClassName1, const char* inheritingClassName2, const char* inheritingClassName3 );

	~CalcLibClassInfo( void );

	static bool IsTypeOf( const char* childClassName, const char* parentClassName );

private:

	void AllocateInheritingClassesArray( int count );
	void DeleteInheritingClassesArray( void );

	void MapClass( const char* className );
	void MapClassInheritance( const char* inheritingClassName );

	static Utilities::Map< CalcLibClassInfo* >* classInfoMap;

	// This is just a list of class name from which a class inherits.
	char** inheritingClassNames;
	int inhertingClassesCount;
};

//=========================================================================================
template< typename ClassType >
inline ClassType* CalcLib::CalcLibClass::Cast( void )
{
	if( !IsTypeOf( ClassType::ClassName() ) )
		return 0;
	return( ( ClassType* )this );
}

//=========================================================================================
template< typename ClassType >
inline const ClassType* CalcLib::CalcLibClass::Cast( void ) const
{
	if( !IsTypeOf( ClassType::ClassName() ) )
		return 0;
	return( ( const ClassType* )this );
}

//=========================================================================================
#define DECLARE_CALCLIB_CLASS( className ) \
	protected: \
	virtual const char* VirtualClassName( void ) const override \
	{ \
		return #className; \
	} \
	public: \
	static const char* ClassName( void ) \
	{ \
		return #className; \
	} \
	virtual bool IsTypeOf( const char* givenClassName ) const \
	{ \
		const char* thisClassName = VirtualClassName(); \
		return CalcLib::CalcLibClassInfo::IsTypeOf( thisClassName, givenClassName ); \
	} \
	private: \
	static CalcLib::CalcLibClassInfo classInfo

//=========================================================================================
#define IMPLEMENT_CALCLIB_CLASS0( className ) \
	CalcLib::CalcLibClassInfo className::classInfo( #className )

//=========================================================================================
#define IMPLEMENT_CALCLIB_CLASS1( className, inheritingClassName1 ) \
	CalcLib::CalcLibClassInfo className::classInfo( #className, #inheritingClassName1 )

//=========================================================================================
#define IMPLEMENT_CALCLIB_CLASS2( className, inheritingClassName1, inheritingClassName2 ) \
	CalcLibClassInfo className::classInfo( #className, #inheritingClassName1, #inheritingClassName2 )

//=========================================================================================
#define IMPLEMENT_CALCLIB_CLASS3( className, inheritingClassName1, inheritingClassName2, inheritingClassName3 ) \
	CalcLibClassInfo className::classInfo( #className, #inheritingClassName1, #inheritingClassName2, #inheritingClassName3 )

// ClassInfo.h