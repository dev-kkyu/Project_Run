#pragma once
#include <string>

class CImage
{
public:
	//stbi_set_flip_vertically_on_load(true); //--- �̹����� �Ųٷ� �����ٸ� �߰�
	static unsigned char* LoadImg(std::string filename, int* x, int* y, int* comp, int req_comp);
	static void FreeImg(void* data);
};

