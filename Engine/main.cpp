/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 11:19:17 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/27 12:13:30 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Core/Application.h"
#include "Core/Reflection/ReflectionRegistry.h"
#include <iostream>

int main() {
	// Initialize Reflection System (Register basic types)
	Engine::Reflection::RegisterBasicTypes();

	// Initialize the application
	Engine::Application app;

	try {
		app.Run();
	} catch (const std::exception &e) {
		std::cerr << "Unhandled Exception: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
