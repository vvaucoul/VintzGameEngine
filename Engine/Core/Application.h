/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Application.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 11:18:59 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/26 13:35:59 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

namespace Engine {

	class Application {
	public:
		Application();
		~Application();

		// Starts the engine
		void Run();

	private:
		void Init();
		void MainLoop();
		void Shutdown();
	};

} // namespace Engine