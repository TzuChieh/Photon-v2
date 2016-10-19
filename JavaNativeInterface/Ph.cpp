#include "core_Ph.h"

#include <iostream>

/*
* Class:     core_Ph
* Method:    printTestMessage
* Signature: ()V
*/
void Java_core_Ph_printTestMessage(JNIEnv *, jclass)
{
	std::cout << "hi from C++" << std::endl;
}

/*
* Class:     core_Ph
* Method:    genTestHdrFrame
* Signature: (Lcore/FloatArrayRef;Lcore/IntRef;Lcore/IntRef;)V
*/
void Java_core_Ph_genTestHdrFrame(JNIEnv *, jclass, jobject, jobject, jobject)
{

}