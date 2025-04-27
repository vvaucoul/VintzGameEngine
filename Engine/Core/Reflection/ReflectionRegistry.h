/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ReflectionRegistry.h                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/27 11:41:40 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/27 11:43:19 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "TypeInfo.h"
#include <glm/glm.hpp> // Add include for glm types
#include <memory>	   // For unique_ptr
#include <string>
#include <unordered_map>

namespace Engine::Reflection {

	// Singleton registry for all known types
	class ReflectionRegistry {
	public:
		// Non-copyable/movable singleton
		ReflectionRegistry(const ReflectionRegistry &)			  = delete;
		ReflectionRegistry &operator=(const ReflectionRegistry &) = delete;
		ReflectionRegistry(ReflectionRegistry &&)				  = delete;
		ReflectionRegistry &operator=(ReflectionRegistry &&)	  = delete;

		static ReflectionRegistry &GetInstance() {
			static ReflectionRegistry instance;
			return instance;
		}

		// Register a new type
		Type *RegisterType(std::string name, size_t size) {
			if (types.find(name) == types.end()) {
				auto type	= std::make_unique<Type>(name, size);
				Type *ptr	= type.get();
				types[name] = std::move(type);
				// std::cout << "Registered type: " << name << std::endl; // Debug
				return ptr;
			}
			return types[name].get(); // Return existing if already registered
		}

		// Find a type by name
		const Type *FindType(const std::string &name) const {
			auto it = types.find(name);
			if (it != types.end()) {
				return it->second.get();
			}
			return nullptr;
		}

		// Find a type using template deduction (convenience)
		template <typename T>
		const Type *FindType() const {
			// Need a way to get the registered name for type T
			// This will be handled by the registration macros later
			return FindType(GetTypeName<T>()); // Assumes GetTypeName<T>() exists
		}

		// Get all registered types
		const std::unordered_map<std::string, std::unique_ptr<Type>> &GetAllTypes() const {
			return types;
		}

		// Placeholder for getting type name - will be specialized via macros
		template <typename T>
		static const char *GetTypeName();

	private:
		ReflectionRegistry() = default; // Private constructor
		std::unordered_map<std::string, std::unique_ptr<Type>> types;
	};

	// Global access function
	inline ReflectionRegistry &Registry() {
		return ReflectionRegistry::GetInstance();
	}

	// --- Template specializations for primitive types ---
	// We need to register basic types manually or via a helper function once.

	template <>
	inline const char *ReflectionRegistry::GetTypeName<int>() { return "int"; }
	template <>
	inline const char *ReflectionRegistry::GetTypeName<float>() { return "float"; }
	template <>
	inline const char *ReflectionRegistry::GetTypeName<double>() { return "double"; }
	template <>
	inline const char *ReflectionRegistry::GetTypeName<bool>() { return "bool"; }
	template <>
	inline const char *ReflectionRegistry::GetTypeName<std::string>() { return "std::string"; }
	template <>
	inline const char *ReflectionRegistry::GetTypeName<glm::vec3>() { return "glm::vec3"; }
	// template<> inline const char* ReflectionRegistry::GetTypeName<glm::quat>() { return "glm::quat"; }
	// template<> inline const char* ReflectionRegistry::GetTypeName<glm::mat4>() { return "glm::mat4"; }

	// Helper function to register basic types (call once at startup)
	inline void RegisterBasicTypes() {
		Registry().RegisterType(ReflectionRegistry::GetTypeName<int>(), sizeof(int));
		Registry().RegisterType(ReflectionRegistry::GetTypeName<float>(), sizeof(float));
		Registry().RegisterType(ReflectionRegistry::GetTypeName<double>(), sizeof(double));
		Registry().RegisterType(ReflectionRegistry::GetTypeName<bool>(), sizeof(bool));
		Registry().RegisterType(ReflectionRegistry::GetTypeName<std::string>(), sizeof(std::string));
		Registry().RegisterType(ReflectionRegistry::GetTypeName<glm::vec3>(), sizeof(glm::vec3));
		// Registry().RegisterType(ReflectionRegistry::GetTypeName<glm::quat>(), sizeof(glm::quat));
		// Registry().RegisterType(ReflectionRegistry::GetTypeName<glm::mat4>(), sizeof(glm::mat4));
	}

} // namespace Engine::Reflection
