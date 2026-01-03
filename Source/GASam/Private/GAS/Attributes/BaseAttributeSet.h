// Samuel Lapointe - 2026

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "BaseAttributeSet.generated.h"

// [INFO] This macro is provided as an example in AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * [INFO] A base attribute set for your project isn't strictly necessary, and for this minimal project it only provides
 * the macros above to all child classes due to the inclusion of this header file. 
 */
UCLASS()
class GASAM_API UBaseAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
};
