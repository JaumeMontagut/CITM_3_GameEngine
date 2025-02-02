#ifndef __MODULE_RANDOM_H__
#define __MODULE_RANDOM_H__

#include"Module.h"
#include <random>
#include "pcg-cpp-0.98\include\pcg_random.hpp"

class ModuleRandom : public Module
{
public:
	ModuleRandom(const char * name, bool start_enabled = true);
	bool Init(JSONFile * module_file) override;

	int RandomInt(int from, int to);
	float RandomFloat0to1();
	UID RandomUID();
	float RandomFloat(float from, float to);
private:
	pcg32 rng;
};

#endif
