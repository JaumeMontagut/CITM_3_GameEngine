#include "ResourceBone.h"
#include "Assimp/include/mesh.h"
#include "Application.h"
#include "ModuleResourceManager.h"
#include "ResourceModel.h"
#include "ModuleFileSystem.h"

RESOURCE_DEFINITION(Resource, ResourceBone);

ResourceBone::ResourceBone()
{
}

const char *ResourceBone::GetTypeString()
{
	return "bone";
}

bool ResourceBone::SaveFileData()
{
	bool ret = false;

	uint ranges[] = {
		num_weights
	};

	uint ranges_bytes = sizeof(ranges);
	uint name_bytes = sizeof(char) * NODE_NAME_SIZE;
	uint weights_bytes = sizeof(VertexWeigth) * num_weights;
	uint matrix_bytes = sizeof(float4x4);

	uint size = ranges_bytes
		+ name_bytes
		+ weights_bytes
		+ matrix_bytes;

	//Save
	char * data = new char[size];
	char * cursor = data;

	SaveVariable(ranges, &cursor, ranges_bytes);
	SaveVariable(name, &cursor, name_bytes);
	SaveVariable(weights, &cursor, weights_bytes);
	SaveVariable(&offset_matrix, &cursor, matrix_bytes);

	//SaveFile
	uint path_size = 250u;
	char * path = new char[path_size];
	App->file_system->CreatePath(path, path_size, RESOURCES_BONE_FOLDER, uid, BONE_EXTENSION);
	ret = App->file_system->SaveFile((const void *)data, size, &path);
	RELEASE_ARRAY(path);

	return ret;
}

bool ResourceBone::LoadFileData()
{
	Timer load_timer;

	//Create path
	char * data = nullptr;
	uint path_size = 250u;
	char * path = new char[path_size];
	App->file_system->CreatePath(path, path_size, RESOURCES_BONE_FOLDER, uid, BONE_EXTENSION);

	bool ret = App->file_system->LoadFile(path, &data);
	if (ret)
	{
		char * cursor = data;

		uint ranges[1];
		LoadVariable(ranges, &cursor, sizeof(ranges));
		num_weights = ranges[0];
		name = new char[NODE_NAME_SIZE];
		LoadVariable(name, &cursor, sizeof(char) * NODE_NAME_SIZE);
		weights = new VertexWeigth[num_weights];
		LoadVariable(weights, &cursor, sizeof(VertexWeigth) * num_weights);
		LoadVariable(&offset_matrix, &cursor, sizeof(float4x4));

		LOG("Success loading bone from: %s in: %i ms.", path, load_timer.Read());
	}

	RELEASE_ARRAY(path);

	return ret;
}

bool ResourceBone::ReleaseData()
{
	RELEASE_ARRAY(name);
	num_weights = 0u;
	RELEASE_ARRAY(weights);
	offset_matrix = float4x4::identity;

	return true;
}

char * ResourceBone::GetName()
{
	return name;
}
