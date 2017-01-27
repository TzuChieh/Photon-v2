#include <ph_test.h>

#include <iostream>

int main(int argc, char* argv[])
{
	phStart();

	PHuint64 descriptionId;
	phCreateDescription(&descriptionId);
	phLoadDescription(descriptionId, "../SceneResource/testScene.p2");
	phUpdateDescription(descriptionId);

	PHuint64 rendererId;
	phCreateRenderer(&rendererId, 4);

	phRender(rendererId, descriptionId);

	phExit();

	return 0;
}