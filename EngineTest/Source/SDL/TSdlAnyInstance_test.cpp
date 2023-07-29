#include <SDL/TSdlAnyInstance.h>
#include <SDL/sdl_interface.h>
#include <SDL/sdl_traits.h>
#include <Actor/Actor.h>

#include <gtest/gtest.h>

using namespace ph;

namespace
{

struct DummyStruct
{
	int x = 0;

	PH_DEFINE_SDL_STRUCT(TSdlOwnerStruct<DummyStruct>)
	{
		StructType ztruct("dummy");
		return ztruct;
	}
};

struct DummyMethodStruct
{
	int p = 0;

	void operator () (Actor& res)
	{}

	PH_DEFINE_SDL_FUNCTION(TSdlMethod<DummyMethodStruct, Actor>)
	{
		return FunctionType("dummy");
	}
};

}// end anonymous namespace

static_assert(CDerived<Actor, ISdlResource> == true);
static_assert(CHasSdlClassDefinition<Actor> == true);
static_assert(CHasSdlStructDefinition<DummyStruct> == true);
static_assert(CHasSdlFunctionDefinition<DummyMethodStruct> == true);

TEST(TSdlAnyInstanceTest, ConstructEmptyInstance)
{
	{
		SdlConstInstance instance;
		EXPECT_FALSE(instance);
	}

	{
		SdlNonConstInstance instance;
		EXPECT_FALSE(instance);
	}

	{
		SdlConstInstance instance(nullptr);
		EXPECT_FALSE(instance);
	}

	{
		SdlNonConstInstance instance(nullptr);
		EXPECT_FALSE(instance);
	}
}

TEST(TSdlAnyInstanceTest, ClassInstance)
{
	{
		const Actor* constActor = nullptr;
		SdlConstInstance instance(constActor);
		EXPECT_FALSE(instance);
		EXPECT_FALSE(instance.get<Actor>());
		EXPECT_FALSE(instance.get<DummyStruct>());
	}

	{
		Actor* nonConstActor = nullptr;
		SdlNonConstInstance instance(nonConstActor);
		EXPECT_FALSE(instance);
		EXPECT_FALSE(instance.get<Actor>());
		EXPECT_FALSE(instance.get<DummyStruct>());
	}

	// Assign non-const actor to const instance
	{
		Actor* nonConstActor = nullptr;
		SdlConstInstance instance(nonConstActor);
		EXPECT_FALSE(instance);
		EXPECT_FALSE(instance.get<Actor>());
		EXPECT_FALSE(instance.get<DummyStruct>());
	}
}

TEST(TSdlAnyInstanceTest, StructInstance)
{
	{
		const DummyStruct* constStruct = nullptr;
		SdlConstInstance instance(constStruct);
		EXPECT_FALSE(instance);
		EXPECT_FALSE(instance.get<DummyStruct>());
		EXPECT_FALSE(instance.get<Actor>());
	}

	{
		DummyStruct* nonConstStruct = nullptr;
		SdlNonConstInstance instance(nonConstStruct);
		EXPECT_FALSE(instance);
		EXPECT_FALSE(instance.get<DummyStruct>());
		EXPECT_FALSE(instance.get<Actor>());
	}

	// Assign non-const struct to const instance
	{
		DummyStruct* nonConstStruct = nullptr;
		SdlConstInstance instance(nonConstStruct);
		EXPECT_FALSE(instance);
		EXPECT_FALSE(instance.get<DummyStruct>());
		EXPECT_FALSE(instance.get<Actor>());
	}

	// With non-null struct (as const instance)
	{
		DummyStruct ztruct;
		SdlConstInstance instance(&ztruct);
		EXPECT_TRUE(instance);
		EXPECT_TRUE(instance.get<DummyStruct>());
		EXPECT_FALSE(instance.get<Actor>());
	}

	// With non-null struct (as non-const instance)
	{
		DummyStruct ztruct;
		SdlNonConstInstance instance(&ztruct);
		EXPECT_TRUE(instance);
		EXPECT_TRUE(instance.get<DummyStruct>());
		EXPECT_FALSE(instance.get<Actor>());
	}

	// With non-null struct (value modification)
	{
		DummyStruct ztruct;
		SdlNonConstInstance instance(&ztruct);
		EXPECT_TRUE(instance);

		ztruct.x = 0;
		EXPECT_EQ(instance.get<DummyStruct>()->x, 0);

		ztruct.x = 3;
		EXPECT_EQ(instance.get<DummyStruct>()->x, 3);

		for(int i = -100; i <= 100; ++i)
		{
			ztruct.x = i;
			EXPECT_EQ(instance.get<DummyStruct>()->x, i);
		}
	}
}

TEST(TSdlAnyInstanceTest, FunctionParameterStructInstance)
{
	{
		const DummyMethodStruct* constStruct = nullptr;
		SdlConstInstance instance(constStruct);
		EXPECT_FALSE(instance);
		EXPECT_FALSE(instance.get<DummyMethodStruct>());
		EXPECT_FALSE(instance.get<Actor>());
	}

	{
		DummyMethodStruct* nonConstStruct = nullptr;
		SdlNonConstInstance instance(nonConstStruct);
		EXPECT_FALSE(instance);
		EXPECT_FALSE(instance.get<DummyMethodStruct>());
		EXPECT_FALSE(instance.get<Actor>());
	}

	// Assign non-const struct to const instance
	{
		DummyMethodStruct* nonConstStruct = nullptr;
		SdlConstInstance instance(nonConstStruct);
		EXPECT_FALSE(instance);
		EXPECT_FALSE(instance.get<DummyMethodStruct>());
		EXPECT_FALSE(instance.get<Actor>());
	}

	// With non-null struct (as const instance)
	{
		DummyMethodStruct ztruct;
		SdlConstInstance instance(&ztruct);
		EXPECT_TRUE(instance);
		EXPECT_TRUE(instance.get<DummyMethodStruct>());
		EXPECT_FALSE(instance.get<Actor>());
	}

	// With non-null struct (as non-const instance)
	{
		DummyMethodStruct ztruct;
		SdlNonConstInstance instance(&ztruct);
		EXPECT_TRUE(instance);
		EXPECT_TRUE(instance.get<DummyMethodStruct>());
		EXPECT_FALSE(instance.get<Actor>());
	}

	// With non-null struct (value modification)
	{
		DummyMethodStruct ztruct;
		SdlNonConstInstance instance(&ztruct);
		EXPECT_TRUE(instance);

		ztruct.p = 0;
		EXPECT_EQ(instance.get<DummyMethodStruct>()->p, 0);

		ztruct.p = 3;
		EXPECT_EQ(instance.get<DummyMethodStruct>()->p, 3);

		for(int i = -100; i <= 100; ++i)
		{
			ztruct.p = i;
			EXPECT_EQ(instance.get<DummyMethodStruct>()->p, i);
		}
	}
}
