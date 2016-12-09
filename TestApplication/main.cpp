#include <ph_test.h>

#include <iostream>

int main(int argc, char* argv[])
{
	//printTestMessage();
	//testRun();

	/*const PHfloat32* pixelData;
	PHuint32 width;
	PHuint32 height;
	genTestHdrFrame(&pixelData, &width, &height);*/

	phStart();

	PHuint64 worldId;
	phCreateWorld(&worldId);

	PHuint64 rendererId;
	phCreateRenderer(&rendererId, PH_MT_IMPORTANCE_RENDERER_TYPE);

	PHuint64 sampleGeneratorId;
	phCreateSampleGenerator(&sampleGeneratorId, PH_STANDARD_SAMPLE_GENERATOR_TYPE, 32);

	PHuint64 cameraId;
	phCreateCamera(&cameraId, PH_DEFAULT_CAMERA_TYPE);

	PHuint64 filmId;
	phCreateFilm(&filmId, 900, 900);

	phSetRendererSampleGenerator(rendererId, sampleGeneratorId);
	phSetCameraFilm(cameraId, filmId);

	phLoadTestScene(worldId);
	phCookWorld(worldId);

	phRender(rendererId, worldId, cameraId);

	phExit();

	return 0;
}