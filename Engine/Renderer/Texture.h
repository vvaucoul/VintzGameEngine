/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Texture.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 12:00:30 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/27 11:12:56 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

namespace Engine {

	class Texture {
	public:
		Texture(const std::string &path);
		~Texture();

		void Bind(unsigned int slot = 0) const;
		void Unbind() const;

		unsigned int GetID() const { return m_RendererID; }

	private:
		unsigned int m_RendererID;
		std::string m_FilePath;
		int m_Width, m_Height, m_Channels;
	};

} // namespace Engine
