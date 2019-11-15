#include "DevIL/include/il.h"
#include "DevIL/include/ilu.h"
#include "DevIL/include/ilut.h"

#pragma comment (lib, "DevIL/lib/DevIL.lib")
#pragma comment (lib, "DevIL/lib/ILU.lib")
#pragma comment (lib, "DevIL/lib/ILUT.lib")

#include "ModuleTexture.h"
#include "ResourceTexture.h"
#include "Application.h"
#include "ModuleResourceManager.h"

#define checkImageWidth 512
#define checkImageHeight 512
static GLubyte checkImage[checkImageHeight][checkImageWidth][4];

ModuleTexture::ModuleTexture(const char * name) : Module(true, name)
{}

bool ModuleTexture::Init(JSONFile * module_file)
{
	//Initialize DevIL libraries
	LOG("Initializing DevIl libraries");
	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);
	CreateCheckerTexture();
	return true;
}

//Saves the texture as DDS in the Library folder (faster to use)
ResourceTexture* ModuleTexture::ImportTexture(const char * path)
{
	ResourceTexture* resource_texture = nullptr;

	//Import data from path first
	uint image_id = 0u;
	ilGenImages(1, &image_id);
	ilBindImage(image_id);
	//ilutRenderer(ILUT_OPENGL);

	if (ilLoadImage(path) == IL_TRUE)
	{
		resource_texture = App->resource_manager->CreateNewResource<ResourceTexture>();
		resource_texture->path = path;
		resource_texture->buffer_id = ilutGLBindTexImage();
		resource_texture->height = ilGetInteger(IL_IMAGE_HEIGHT);
		resource_texture->width = ilGetInteger(IL_IMAGE_WIDTH);
		resource_texture->size = ilGetInteger(IL_IMAGE_SIZE_OF_DATA);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glBindTexture(GL_TEXTURE_2D, resource_texture->buffer_id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, 0x8072, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glBindTexture(GL_TEXTURE_2D, 0);

		//Then save file
		resource_texture->SaveFileData();

		ilDeleteImages(1, &image_id);
	}
	else
	{
		auto error = ilGetError();
		LOG("Failed to load texture with path: %s. Error: %s", path, ilGetString(error));
	}
	//free(lump);

	return resource_texture;
}

void ModuleTexture::CreateCheckerTexture()
{
	ResourceTexture* new_texture = App->resource_manager->CreateNewResource<ResourceTexture>();

	int i, j, c;

	for (i = 0; i < checkImageHeight; i++) {
		for (j = 0; j < checkImageWidth; j++) {
			c = ((((i & 0x8) == 0) ^ ((j & 0x8)) == 0)) * 255;
			checkImage[i][j][0] = (GLubyte)c;
			checkImage[i][j][1] = (GLubyte)c;
			checkImage[i][j][2] = (GLubyte)c;
			checkImage[i][j][3] = (GLubyte)255;
		}
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	uint buffer = 0;
	
	glGenTextures(1, (uint*)&((*new_texture).buffer_id));
	glBindTexture(GL_TEXTURE_2D, ((*new_texture).buffer_id));

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
		GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, checkImageWidth,
		checkImageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE,
		checkImage);
	glBindTexture(GL_TEXTURE_2D, 0);
}
