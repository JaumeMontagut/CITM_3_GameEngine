#include "ResourceAnimation.h"
#include "Assimp/include/scene.h"
#include "Globals.h"
#include "ModuleImportModel.h"
#include "ResourceModel.h"
#include "ModuleFileSystem.h"
#include "Application.h"

RESOURCE_DEFINITION(Resource, ResourceAnimation);

ResourceAnimation::~ResourceAnimation()
{
	CleanUp();
}

const char * ResourceAnimation::GetTypeString()
{
	return "animation";
}

void ResourceAnimation::CleanUp()
{
	RELEASE(channels);
	name = "";
	num_channels = 0;
	duration = 0;
	ticks_per_second = 0;
}

uint ResourceAnimation::GetNumChannels()
{
	return num_channels;
}

AnimationChannels * ResourceAnimation::GetChannels()
{
	return channels;
}

AnimationChannels * ResourceAnimation::GetChannelByName(const char* name)
{
	for (uint i = 0; i < num_channels; ++i)
	{
		if(strcmp(channels[i].name, name))
		{
			return &channels[i];
		}
	}
	return nullptr;
}

float ResourceAnimation::GetTicksPerSecond()
{
	return ticks_per_second;
}

double ResourceAnimation::GetDuration()
{
	return duration;
}

char * ResourceAnimation::GetName()
{
	return name;
}

bool ResourceAnimation::SaveFileData()
{
	bool ret = true;

	//Calculate the size of the data array
	uint size = 
		NODE_NAME_SIZE//name
		+ sizeof(duration)
		+ sizeof(ticks_per_second)
		+ sizeof(num_channels);

	for (int i = 0; i < num_channels; ++i)
	{
		size += NODE_NAME_SIZE;//name

		size += sizeof(channels[i].num_position_keys);
		size += sizeof(channels[i].num_rotation_keys);
		size += sizeof(channels[i].num_scale_keys);

		size += sizeof(KeyAnimation<float3>) * channels[i].num_position_keys;//TODO: Try if we can do sizeof(position_keys)
		size += sizeof(KeyAnimation<Quat>) * channels[i].num_rotation_keys;
		size += sizeof(KeyAnimation<float3>) * channels[i].num_scale_keys;
	}

	//Save the data array
	char* data = new char[size];
	char* cursor = data;
	SaveVariable(name, &cursor,sizeof(char) * NODE_NAME_SIZE);
	SaveVariable(&duration, &cursor,sizeof(duration));
	SaveVariable(&ticks_per_second, &cursor,sizeof(ticks_per_second));
	SaveVariable(&num_channels, &cursor,sizeof(num_channels));

	for (int i = 0; i < num_channels; ++i)
	{
		uint ranges[] = {
			channels[i].num_position_keys,
			channels[i].num_rotation_keys,
			channels[i].num_scale_keys
		};

		SaveVariable(channels[i].name, &cursor, NODE_NAME_SIZE);
		SaveVariable(ranges, &cursor, sizeof(ranges));
		SaveVariable(channels[i].position_keys, &cursor, sizeof(KeyAnimation<float3>) * channels[i].num_position_keys);
		SaveVariable(channels[i].rotation_keys, &cursor, sizeof(KeyAnimation<Quat>) * channels[i].num_rotation_keys);
		SaveVariable(channels[i].scale_keys, &cursor, sizeof(KeyAnimation<float3>) * channels[i].num_scale_keys);
	}

	//Save file
	uint path_size = 250u;
	char * path = new char[path_size];
	App->file_system->CreatePath(path, path_size, RESOURCES_ANIMATION_FOLDER, uid, ANIMATION_EXTENSION);
	ret = App->file_system->SaveFile((const void *)data, size, &path);
	RELEASE_ARRAY(path);

	return ret;
}

bool ResourceAnimation::LoadFileData()
{
	Timer load_timer;

	//Create path
	char * data = nullptr;
	uint path_size = 250u;
	char * path = new char[path_size];
	App->file_system->CreatePath(path, path_size, RESOURCES_ANIMATION_FOLDER, uid, ANIMATION_EXTENSION);

	bool ret = App->file_system->LoadFile(path, &data);
	if (ret)
	{
		char * cursor = data;

		name = new char[NODE_NAME_SIZE];
		LoadVariable(name, &cursor, NODE_NAME_SIZE);
		LoadVariable(&duration, &cursor, sizeof(duration));
		LoadVariable(&ticks_per_second, &cursor, sizeof(ticks_per_second));
		LoadVariable(&num_channels, &cursor, sizeof(num_channels));
		channels = new AnimationChannels[num_channels];

		for (int i = 0; i < num_channels; ++i)
		{
			channels[i].name = new char[NODE_NAME_SIZE];
			LoadVariable(channels[i].name, &cursor, NODE_NAME_SIZE);
			
			//INFO: The number of elements on the ranges array must be the same as in the ranges array of the ResourceAnimation::SaveFileData()
			uint ranges[3];
			LoadVariable(ranges, &cursor, sizeof(ranges));
			channels[i].num_position_keys = ranges[0];
			channels[i].num_rotation_keys = ranges[1];
			channels[i].num_scale_keys = ranges[2];

			channels[i].position_keys = new KeyAnimation<float3>[channels[i].num_position_keys];
			LoadVariable(channels[i].position_keys, &cursor, sizeof(KeyAnimation<float3>) * channels[i].num_position_keys);
			
			channels[i].rotation_keys = new KeyAnimation<Quat>[channels[i].num_rotation_keys];
			LoadVariable(channels[i].rotation_keys, &cursor, sizeof(KeyAnimation<Quat>) * channels[i].num_rotation_keys);
			
			channels[i].scale_keys = new KeyAnimation<float3>[channels[i].num_scale_keys];
			LoadVariable(channels[i].scale_keys, &cursor, sizeof(KeyAnimation<float3>) * channels[i].num_scale_keys);

			LOG("Success loading animation from: %s in : %i ms.", path, load_timer.Read());
		}
	}

	RELEASE_ARRAY(path);

	return ret;
}

bool ResourceAnimation::ReleaseData()
{
	if (num_channels > 0u)
	{
		for (int i = num_channels - 1; i < num_channels; ++i)
		{
			RELEASE_ARRAY(channels[i].scale_keys);
			RELEASE_ARRAY(channels[i].rotation_keys);
			RELEASE_ARRAY(channels[i].position_keys);
			channels[i].num_scale_keys = 0u;
			channels[i].num_rotation_keys = 0u;
			channels[i].num_position_keys = 0u;
			RELEASE_ARRAY(channels[i].name);
		}
		RELEASE_ARRAY(channels);
	}

	num_channels = 0u;
	ticks_per_second = 0.f;
	duration = 0.f;
	RELEASE_ARRAY(name);

	return true;
}

void ResourceAnimation::ImportAnimation(const aiAnimation& animation)
{
	const char * animation_name = animation.mName.C_Str();
	name = new char[NODE_NAME_SIZE];
	memset(name, NULL, NODE_NAME_SIZE);
	strcpy(name, animation_name);

	ticks_per_second = (animation.mTicksPerSecond != 0) ? animation.mTicksPerSecond : DEFAULT_TICK_PER_SECOND;
	duration = animation.mDuration/ ticks_per_second;

	num_channels = animation.mNumChannels;

	channels = new AnimationChannels[num_channels];
	for (uint i = 0u; i < num_channels; ++i)
	{
		channels[i].ImportAnimationNode((*animation.mChannels[i]));
	}

	SaveFileData();
}

void AnimationChannels::ImportAnimationNode(const aiNodeAnim & node_animation)
{
	const char * node_name = node_animation.mNodeName.C_Str();
	name = new char[NODE_NAME_SIZE];
	memset(name, NULL, NODE_NAME_SIZE);
	strcpy(name, node_name);//TODO: Make a function for this
	
	num_position_keys = node_animation.mNumPositionKeys;
	num_rotation_keys = node_animation.mNumRotationKeys;
	num_scale_keys = node_animation.mNumScalingKeys;

	position_keys = new KeyAnimation<float3>[num_position_keys]();
	rotation_keys = new KeyAnimation<Quat>[num_rotation_keys]();
	scale_keys = new KeyAnimation<float3>[num_scale_keys]();

	aiVectorKey* m_position_keys = node_animation.mPositionKeys;
	for (int i = 0; i < num_position_keys; ++i)
	{
		position_keys[i].time = m_position_keys[i].mTime;
		position_keys[i].value.Set(m_position_keys[i].mValue.x, m_position_keys[i].mValue.y, m_position_keys[i].mValue.z) ;
	}

	aiQuatKey* m_rotation_keys = node_animation.mRotationKeys;
	for (int i = 0; i < num_rotation_keys; ++i)
	{
		rotation_keys[i].time = m_rotation_keys[i].mTime;
		rotation_keys[i].value.Set(m_rotation_keys[i].mValue.x, m_rotation_keys[i].mValue.y, m_rotation_keys[i].mValue.z, m_rotation_keys[i].mValue.w);
	}

	aiVectorKey* m_scale_keys = node_animation.mScalingKeys;
	for (int i = 0; i < num_scale_keys; ++i)
	{
		scale_keys[i].time = m_scale_keys[i].mTime;
		scale_keys[i].value.Set(m_scale_keys[i].mValue.x, m_scale_keys[i].mValue.y, m_scale_keys[i].mValue.z);
	}
}

//Returns nullptr if there isn't a key with less time than the specified
bool AnimationChannels::GetKeyPosition(double time, float3& value)
{
	return GetKeyFloat3(time, value, position_keys, num_position_keys);
}

bool AnimationChannels::GetKeyFloat3(double time, math::float3 & value, KeyAnimation<float3>* keys, uint num_keys)
{
	KeyAnimation<float3>* prev_key = nullptr;
	if (num_keys > 0u
		&& keys[0].time <= time)
	{
		prev_key = &keys[0];
		for (uint i = 1u; i < num_keys; ++i)
		{
			if (keys[i].time <= time)
			{
				prev_key = &keys[i];
			}
			else
			{
				//next_key = &position_keys[i];
				//prev_key = &position_keys[i - 1];
				float t = (time - prev_key->time) / (keys[i].time - prev_key->time);
				value = prev_key->value.Lerp(keys[i].value, t);
				return true;
			}
		}
		value = prev_key->value;
		return true;
	}
	return false;
}

bool AnimationChannels::GetKeyRotation(double time, Quat& value)
{
	KeyAnimation<Quat>* prev_key = nullptr;
	if (num_rotation_keys > 0u
		&& rotation_keys[0].time <= time)
	{
		prev_key = &rotation_keys[0];
		for (uint i = 0; i < num_rotation_keys; ++i)
		{
			if (rotation_keys[i].time <= time)
			{
				prev_key = &rotation_keys[i];
			}
			else
			{
				float t = (time - prev_key->time) / (rotation_keys[i].time - prev_key->time);
				value = prev_key->value.Slerp(rotation_keys[i].value, t);
				return true;
			}
		}
		value = prev_key->value;
		return true;
	}
	
	return false;
}

bool AnimationChannels::GetKeyScale(double time, float3& value)
{
	return GetKeyFloat3(time, value, scale_keys, num_scale_keys);
}