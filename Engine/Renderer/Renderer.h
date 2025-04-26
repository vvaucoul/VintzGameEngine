/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Renderer.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 11:19:20 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/26 12:01:14 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

namespace Engine {

	class Renderer {
	public:
		static void Clear();
		static void SetClearColor(float r, float g, float b, float a);
		static void SetViewport(int x, int y, int width, int height);
	};

} // namespace Engine
