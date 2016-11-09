#include "ph_core.h"
#include "Api/ApiDatabase.h"

PHint32 phStart()
{
	return PH_TRUE;
}

void phExit()
{
	ph::ApiDatabase::releaseAllData();
}