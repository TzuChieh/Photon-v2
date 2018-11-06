#pragma once

#include <jni.h>

#include <unordered_map>
#include <string>
#include <iostream>

namespace ph
{

template<typename T>
class JObject
{
protected:
	static jfieldID getFieldId(const std::string& fieldName);

	// These static methods call JNI methods like FindClass(), GetMethodID(), GetFieldID() which are expensive operations 
	// that are guaranteed to generate the same result over the life of the JVM. Since these operations are time consuming, 
	// it is wise to call these static methods only once.

	static void cacheJavaClass(const std::string& className, JNIEnv* env);
	static void cacheJavaFieldId(const std::string& fieldName, const std::string& fieldSignature, JNIEnv* env);

public:
	// notice that we can't cache JNIEnv across multiple threads, hence it must be a parameter
	JObject(jobject m_jobject, JNIEnv* m_env);
	virtual ~JObject() = 0;

protected:
	jobject m_jobject;
	JNIEnv* m_env;

private:
	static jclass jclass_javaClassRef;
	static std::unordered_map<std::string, jfieldID> map_fieldName_jfieldID;
};

// Implementations:

template<typename T>
jclass JObject<T>::jclass_javaClassRef = nullptr;

template<typename T>
std::unordered_map<std::string, jfieldID> JObject<T>::map_fieldName_jfieldID;

template<typename T>
JObject<T>::JObject(jobject m_jobject, JNIEnv* m_env) : 
	m_jobject(m_jobject), m_env(m_env)
{}

template<typename T>
JObject<T>::~JObject() = default;

// FIXME: possible race condition on caching variables

template<typename T>
void JObject<T>::cacheJavaClass(const std::string& className, JNIEnv* env)
{
	if(jclass_javaClassRef)
	{
		return;
	}

	// cache class

	jclass localClassRef = env->FindClass(className.c_str());
	if(!localClassRef)
	{
		std::cerr << "error at FindClass()" << std::endl;
		return;
	}

	jclass_javaClassRef = static_cast<jclass>(env->NewGlobalRef(localClassRef));
	if(!jclass_javaClassRef)
	{
		std::cerr << "error at NewGlobalRef()" << std::endl;
		return;
	}

	// TODO: delete global references when the native library is garbage collected
}

template<typename T>
void JObject<T>::cacheJavaFieldId(const std::string& fieldName, const std::string& fieldSignature, JNIEnv* env)
{
	jfieldID fieldId = env->GetFieldID(jclass_javaClassRef, fieldName.c_str(), fieldSignature.c_str());
	if(!fieldId)
	{
		std::cerr << "error at GetFieldID()" << std::endl;
		return;
	}

	map_fieldName_jfieldID[fieldName] = fieldId;
}

template<typename T>
jfieldID JObject<T>::getFieldId(const std::string& fieldName)
{
	auto result = map_fieldName_jfieldID.find(fieldName);
	if(result != map_fieldName_jfieldID.end())
	{
		return result->second;
	}
	else
	{
		std::cerr << "field <" << fieldName << "> not found" << std::endl;
		return nullptr;
	}
}

}// end namespace ph