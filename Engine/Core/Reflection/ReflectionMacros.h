/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ReflectionMacros.h                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/27 11:41:43 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/27 12:09:28 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "ReflectionRegistry.h"
#include <cstddef>	// For offsetof
#include <iostream> // For potential error logging
#include <string>
#include <type_traits> // For remove_reference_t

namespace Engine::Reflection {

// Macro to place in the class definition (header)
#undef DECLARE_REFLECTABLE
#define DECLARE_REFLECTABLE(ClassName)                                           \
public:                                                                          \
	static ::Engine::Reflection::Type *StaticType();                             \
	virtual ::Engine::Reflection::Type *GetType() const { return StaticType(); } \
	/* Make TypeInitializer public for static instance definition */             \
	struct TypeInitializer;                                                      \
	friend struct TypeInitializer;                                               \
                                                                                 \
private:                                                                         \
	static ::Engine::Reflection::Type *s_StaticType;

// Macro to place in the cpp file to define the type and its fields
#undef BEGIN_DEFINE_TYPE
#define BEGIN_DEFINE_TYPE(ClassName)                                                                                \
	/* Initialization of the static type pointer */                                                                 \
	::Engine::Reflection::Type *ClassName::s_StaticType = nullptr;                                                  \
	/* Definition of the nested public TypeInitializer */                                                           \
	struct ClassName::TypeInitializer {                                                                             \
		TypeInitializer() {                                                                                         \
			using T	  = ClassName;                                                                                  \
			auto &reg = ::Engine::Reflection::Registry();                                                           \
			/* Ensure basic types are registered before registering class */                                        \
			::Engine::Reflection::RegisterBasicTypes();                                                             \
			/* Use the specialized GetTypeName (defined externally) to get the name */                              \
			s_StaticType = reg.RegisterType(::Engine::Reflection::ReflectionRegistry::GetTypeName<T>(), sizeof(T)); \
			if (s_StaticType) {

#undef DEFINE_FIELD
// Pass ClassName explicitly to DEFINE_FIELD
#define DEFINE_FIELD(ClassName, FieldName)                                                                                            \
	{                                                                                                                                 \
		auto &reg = ::Engine::Reflection::Registry();                                                                                 \
		/* Use ClassName directly here */                                                                                             \
		using FieldType	  = std::remove_reference_t<decltype(std::declval<ClassName>().FieldName)>; /* Use remove_reference_t */      \
		const auto *fType = reg.FindType<FieldType>();                                                                                \
		if (fType) {                                                                                                                  \
			/* Calculate offset using C-style pointer arithmetic relative to nullptr */                                               \
			size_t fieldOffset = (size_t)&(((ClassName *)nullptr)->FieldName);                                                        \
			s_StaticType->AddField({#FieldName, fType, fieldOffset});                                                                 \
		} else {                                                                                                                      \
			/* Optional: Log an error if field type wasn't found/registered */                                                        \
			std::cerr << "Reflection Error: Could not find registered type for field '" << #FieldName << "' in class '" << #ClassName \
					  << "' (Type: " << typeid(FieldType).name() << ")." << std::endl;                                                \
		}                                                                                                                             \
	}

#undef END_DEFINE_TYPE
#define END_DEFINE_TYPE(ClassName)                                        \
	} /* End of if (s_StaticType) */                                      \
	} /* End of TypeInitializer constructor */                            \
	}                                                                     \
	; /* End of TypeInitializer struct definition */                      \
	/* Instantiate the TypeInitializer to register the type at startup */ \
	/* This needs to be defined at global/namespace scope */              \
	static ClassName::TypeInitializer s_##ClassName##_TypeInit;           \
	/* Implementation of StaticType */                                    \
	::Engine::Reflection::Type *ClassName::StaticType() { return s_StaticType; }

} // namespace Engine::Reflection
