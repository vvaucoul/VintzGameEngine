/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TypeInfo.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/27 11:41:38 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/27 11:41:38 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <any> // For holding arbitrary data like default values or attributes
#include <functional>
#include <string>
#include <vector>

namespace Engine::Reflection {

	class Type; // Forward declaration

	// Represents a member variable (field) of a reflected type
	struct Field {
		std::string name;
		const Type *type; // Type of the field
		size_t offset;	  // Offset within the struct/class in bytes
		// Add metadata if needed (e.g., std::any attributes;)

		// Helper to get the field's value from an object instance
		template <typename T>
		T &GetValue(void *instance) const {
			return *reinterpret_cast<T *>(static_cast<char *>(instance) + offset);
		}

		template <typename T>
		const T &GetValue(const void *instance) const {
			return *reinterpret_cast<const T *>(static_cast<const char *>(instance) + offset);
		}
	};

	// Represents a C++ type (class, struct, primitive)
	class Type {
	public:
		Type(std::string n, size_t s) : name(std::move(n)), size(s) {}

		const std::string &GetName() const { return name; }
		size_t GetSize() const { return size; }
		const std::vector<Field> &GetFields() const { return fields; }

		const Field *FindField(const std::string &fieldName) const {
			for (const auto &field : fields) {
				if (field.name == fieldName) {
					return &field;
				}
			}
			return nullptr;
		}

		// To be called by registration macros/functions
		void AddField(Field &&field) {
			fields.emplace_back(std::move(field));
		}

	private:
		std::string name;
		size_t size;
		std::vector<Field> fields;
		// Add methods, base classes, constructors etc. later if needed
	};

} // namespace Engine::Reflection
