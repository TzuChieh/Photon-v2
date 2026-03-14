#include <Engine/SDL/Introspect/field_set_op.h>
#include <Engine/SDL/Introspect/FieldSet/TSdlBruteForceFieldSet.h>
#include <Engine/SDL/Introspect/TSdlReal.h>
#include <Engine/SDL/SdlInputClauses.h>
#include <Engine/SDL/Introspect/SdlInputContext.h>

#include <gtest/gtest.h>

#include <string>

using namespace ph;

namespace
{
	struct TestOwner
	{
		real normalField = 1.0_r;
		real nativeField = 2.0_r;
	};
}

TEST(FieldSetOpTest, LoadFieldsFromSdlWithPreferNativeAccess)
{
	TestOwner owner;
	owner.normalField = 11.0_r;
	owner.nativeField = 22.0_r;

	TSdlBruteForceFieldSet<TSdlOwnedField<TestOwner>> fieldSet;
	
	TSdlReal<TestOwner> normalField("normal", &TestOwner::normalField);
	normalField.defaultTo(1.0_r);
	normalField.optional();
	fieldSet.addField(normalField);

	TSdlReal<TestOwner> nativeField("native", &TestOwner::nativeField);
	nativeField.defaultTo(2.0_r);
	nativeField.optional();
	nativeField.options(EFieldOption::PreferNativeAccess);
	fieldSet.addField(nativeField);

	SdlInputClauses clauses;
	SdlInputContext ctx;

	// Call with empty clauses
	field_set_op::load_fields_from_sdl(owner, fieldSet, clauses, ctx);

	// `normalField` should be reset to its default value (1.0)
	EXPECT_EQ(owner.normalField, 1.0_r);

	// `nativeField` should RETAIN its current value (22.0), NOT reset to default (2.0)
	EXPECT_EQ(owner.nativeField, 22.0_r);
}

TEST(FieldSetOpTest, ThrowOnMissingRequiredFieldWithDisabledFallback)
{
	TestOwner owner;
	TSdlBruteForceFieldSet<TSdlOwnedField<TestOwner>> fieldSet;

	TSdlReal<TestOwner> requiredField("required", &TestOwner::normalField);
	requiredField.defaultTo(1.0_r);
	requiredField.required();
	requiredField.options(EFieldOption::DisableFallback);
	fieldSet.addField(requiredField);

	SdlInputClauses clauses;// no input provided
	SdlInputContext ctx;

	// Should throw `SdlLoadError` as the field is required but fallback is disabled
	EXPECT_THROW(field_set_op::load_fields_from_sdl(owner, fieldSet, clauses, ctx), SdlLoadError);
}

TEST(FieldSetOpTest, NoThrowOnMissingRequiredFieldWithPreferNativeAccess)
{
	TestOwner owner;
	owner.nativeField = 22.0_r;

	TSdlBruteForceFieldSet<TSdlOwnedField<TestOwner>> fieldSet;

	TSdlReal<TestOwner> requiredNativeField("required-native", &TestOwner::nativeField);
	requiredNativeField.defaultTo(2.0_r);
	requiredNativeField.required();
	requiredNativeField.options({EFieldOption::PreferNativeAccess, EFieldOption::DisableFallback});
	fieldSet.addField(requiredNativeField);

	SdlInputClauses clauses;// no input provided
	SdlInputContext ctx;

	// Should NOT throw even if it is required and fallback is disabled
	EXPECT_NO_THROW(field_set_op::load_fields_from_sdl(owner, fieldSet, clauses, ctx));

	// Value should remain unchanged (22.0)
	EXPECT_EQ(owner.nativeField, 22.0_r);
}

