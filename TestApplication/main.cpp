#include <ph_test.h>

#include <iostream>

int main(int argc, char* argv[])
{
	//printTestMessage();
	//testRun();

	const PHfloat32* pixelData;
	PHuint32 width;
	PHuint32 height;


	
	genTestHdrFrame(&pixelData, &width, &height);


	return 0;
}