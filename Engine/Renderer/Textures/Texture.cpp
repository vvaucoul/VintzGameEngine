/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Texture.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 12:00:38 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/28 15:36:40 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Texture.h"
#include "Resampling/Resampling.h"

#include <glad/glad.h>
#include <iostream>
#include <memory>
#include <stb_image.h>
#include <vector>

namespace Engine {

	namespace {
		// Factory for resampling algorithm selection
		std::unique_ptr<Renderer::Textures::Resampling> CreateResampler(ResamplingAlgorithm algorithm) {
			using namespace Renderer::Textures;
			switch (algorithm) {
				case ResamplingAlgorithm::NearestNeighbor: return std::unique_ptr<Renderer::Textures::Resampling>(new Renderer::Textures::NearestNeighbor());
				case ResamplingAlgorithm::Bilinear: return std::unique_ptr<Renderer::Textures::Resampling>(new Renderer::Textures::Bilinear());
				case ResamplingAlgorithm::Bicubic: return std::unique_ptr<Renderer::Textures::Resampling>(new Renderer::Textures::Bicubic());
				case ResamplingAlgorithm::Lanczos: return std::unique_ptr<Renderer::Textures::Resampling>(new Renderer::Textures::Lanczos());
				default: return nullptr;
			}
		}
	}

	Texture::Texture(const std::string &path, int targetWidth, int targetHeight, ResamplingAlgorithm algorithm)
		: m_RendererID(0), m_FilePath(path), m_Width(0), m_Height(0), m_Channels(0) {
		stbi_set_flip_vertically_on_load(1);

		int srcWidth = 0, srcHeight = 0, srcChannels = 0;
		unsigned char *srcData = stbi_load(path.c_str(), &srcWidth, &srcHeight, &srcChannels, 0);

		if (!srcData) {
			std::cerr << "[Texture] Failed to load: " << path << std::endl;
			return;
		}

		m_Channels = srcChannels;
		m_Width	   = (targetWidth > 0) ? targetWidth : srcWidth;
		m_Height   = (targetHeight > 0) ? targetHeight : srcHeight;

		const bool doResample = (m_Width != srcWidth || m_Height != srcHeight) && algorithm != ResamplingAlgorithm::None;
		std::vector<uint8_t> resampled;
		const unsigned char *uploadData = srcData;

		if (doResample) {
			auto resampler = CreateResampler(algorithm);
			if (resampler) {
				std::vector<uint8_t> input(srcData, srcData + size_t(srcWidth) * srcHeight * srcChannels);
				resampled = resampler->Resample(input, srcWidth, srcHeight, m_Width, m_Height, srcChannels);
				if (!resampled.empty()) {
					uploadData = resampled.data();
				} else {
					std::cerr << "[Texture] Resampling failed for: " << path << ", using original." << std::endl;
					m_Width	 = srcWidth;
					m_Height = srcHeight;
				}
			} else {
				std::cerr << "[Texture] Invalid resampling algorithm for: " << path << ", using original." << std::endl;
				m_Width	 = srcWidth;
				m_Height = srcHeight;
			}
		}

		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);

		GLenum internalFormat = GL_RGB8, dataFormat = GL_RGB;
		if (m_Channels == 4) {
			internalFormat = GL_RGBA8;
			dataFormat	   = GL_RGBA;
		} else if (m_Channels == 1) {
			internalFormat = GL_R8;
			dataFormat	   = GL_RED;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, dataFormat, GL_UNSIGNED_BYTE, uploadData);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		stbi_image_free(srcData);
		glBindTexture(GL_TEXTURE_2D, 0);
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
