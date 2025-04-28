/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Texture.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 12:00:38 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/28 11:04:06 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Texture.h"
#include <glad/glad.h>
#include <iostream>
#include <stb_image.h>

namespace Engine {

	Texture::Texture(const std::string &path)
		: m_RendererID(0), m_FilePath(path), m_Width(0), m_Height(0), m_Channels(0) {
		stbi_set_flip_vertically_on_load(1);
		unsigned char *data = stbi_load(path.c_str(), &m_Width, &m_Height, &m_Channels, 0);
		if (data) {
			glGenTextures(1, &m_RendererID);
			glBindTexture(GL_TEXTURE_2D, m_RendererID);

			GLenum internalFormat = GL_RGB; // Format stored on GPU
			GLenum dataFormat	  = GL_RGB; // Format of input data
			if (m_Channels == 4) {
				internalFormat = GL_RGBA;
				dataFormat	   = GL_RGBA;
			} else if (m_Channels == 3) {
				internalFormat = GL_RGB;
				dataFormat	   = GL_RGB;
			} else if (m_Channels == 1) {
				internalFormat = GL_RED; // Use GL_RED for single channel (grayscale)
				dataFormat	   = GL_RED;
			} else {
				std::cerr << "Warning: Unsupported number of channels (" << m_Channels << ") for texture: " << path << std::endl;
				// Default to RGB, might look wrong
			}

			// Use internalFormat for the 3rd argument (how GPU stores it)
			// Use dataFormat for the 7th argument (format of the 'data' pointer)
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// Use GL_CLAMP_TO_EDGE for billboards usually, unless repeating is desired
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			stbi_image_free(data);
		} else {
			std::cerr << "Failed to load texture: " << path << std::endl;
		}
	}

	Texture::~Texture() {
		glDeleteTextures(1, &m_RendererID);
	}

	void Texture::Bind(unsigned int slot) const {
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
	}

	void Texture::Unbind() const {
		glBindTexture(GL_TEXTURE_2D, 0);
	}

} // namespace Engine
