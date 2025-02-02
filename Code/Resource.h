#ifndef __RESOURCE_H__
#define __RESOURCE_H__

#include "Globals.h"
#include <xstring>
#include <string>

#define INVALID_RESOURCE_UID 0


#define TO_STRING( x ) #x

class JSONFile;
typedef unsigned int uint;

//****************
// CLASS_DECLARATION
//
// This macro must be included in the declaration of any subclass of Component.
// It declares variables used in type checking.
//****************
#define RESOURCE_DECLARATION( classname )                                                      \
public:                                                                                     \
    static const uint type;                                                          \
    virtual bool IsClassType( const unsigned int classType ) const override;                 \
	virtual unsigned int GetType() override; \

//****************
// CLASS_DEFINITION
// 
// This macro must be included in the class definition to properly initialize 
// variables used in type checking. Take special care to ensure that the 
// proper parentclass is indicated or the run-time type information will be
// incorrect. Only works on single-inheritance RTTI.
//****************
#define RESOURCE_DEFINITION( parentclass, childclass )                                         \
const uint childclass::type = std::hash< std::string >()( TO_STRING( childclass ) ); \
bool childclass::IsClassType( const uint classType ) const {                         \
        if ( classType == childclass::type )                                                \
            return true;                                                                    \
        return parentclass::IsClassType( classType );                                       \
} \
unsigned int childclass::GetType()\
{\
         return childclass::type;\
}\

class Resource
{

public:
	static const uint type;
	
public:
	Resource();
	bool StartUsingResource();
	uint GetReferenceCount() const;
	bool StopUsingResource();
	void SaveModifiedDate(JSONFile * meta_file, const char * asset_path);
	UID GetUID() const;

	virtual bool IsClassType(const uint classType) const
	{
		return classType == type;
	}
	virtual uint GetType()
	{
		return type;
	}
	virtual const char * GetTypeString() = 0;
	
protected:
	//INFO: Saves the resource with custom format in the Resources folder
	virtual bool SaveFileData() = 0;
	virtual bool LoadFileData() = 0;
	virtual bool ReleaseData() = 0;

	virtual void CleanUp() {};
	
	//Helpers for LoadFileData() and SaveFileData()
	void SaveVariable(void* info, char ** data_cursor, size_t bytes);
	void LoadVariable(void* info, char ** data_cursor, size_t bytes);

protected:
	UID uid = 0u;
	uint reference_count = 0u;//How many GameObjects are currently using this resource

public:
	std::string asset_source;//The path to the asset that this resource was generated from
	
	friend class ModuleResourceManager;
	friend class ResourceModel;
};

#endif