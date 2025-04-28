/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Texture.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 12:00:38 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/28 11:47:47 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Texture.h"
#include "Resampling/Resampling.h" // Include base class
// Include concrete implementations needed for instantiation
#include "Resampling/Bicubic.cpp"
#include "Resampling/Bilinear.cpp"
#include "Resampling/Lanczos.cpp"
#include "Resampling/NearestNeighbor.cpp" // Note: Including .cpp is generally bad practice, link object files instead.

#include <glad/glad.h>
#include <iostream>
#include <memory> // For std::unique_ptr
#include <stb_image.h>
#include <vector>

namespace Engine {

	// Helper function to select resampler (could be moved elsewhere)
	std::unique_ptr<Renderer::Textures::Resampling> CreateResampler(ResamplingAlgorithm algorithm) {
		using namespace Engine::Renderer::Textures;
		switch (algorithm) {
			case ResamplingAlgorithm::NearestNeighbor:
				return std::make_unique<NearestNeighbor>();
			case ResamplingAlgorithm::Bilinear:
				return std::make_unique<Bilinear>();
			case ResamplingAlgorithm::Bicubic:
				return std::make_unique<Bicubic>();
			case ResamplingAlgorithm::Lanczos:
				return std::make_unique<Lanczos>(); // Uses default Lanczos-3
			case ResamplingAlgorithm::None:
			default:
				return nullptr;
		}
	}

	Texture::Texture(const std::string &path, int targetWidth, int targetHeight, ResamplingAlgorithm algorithm)
		: m_RendererID(0), m_FilePath(path), m_Width(0), m_Height(0), m_Channels(0) {
		stbi_set_flip_vertically_on_load(1);

		int originalWidth = 0, originalHeight = 0, originalChannels = 0;
		unsigned char *originalData = stbi_load(path.c_str(), &originalWidth, &originalHeight, &originalChannels, 0);

		if (!originalData) {
			std::cerr << "Failed to load texture: " << path << std::endl;
			return; // Exit constructor early
		}

		// Assign original channels
		m_Channels = originalChannels;

		// Determine final dimensions and if resampling is needed
		m_Width			   = (targetWidth > 0) ? targetWidth : originalWidth;
		m_Height		   = (targetHeight > 0) ? targetHeight : originalHeight;
		bool needsResample = (m_Width != originalWidth || m_Height != originalHeight) && algorithm != ResamplingAlgorithm::None;

		std::vector<uint8_t> resampledData;
		const unsigned char *finalDataPtr = originalData; // Point to original data by default

		if (needsResample) {
			std::cout << "[Texture] Resampling '" << path << "' from " << originalWidth << "x" << originalHeight
					  << " to " << m_Width << "x" << m_Height << " using algorithm " << static_cast<int>(algorithm) << std::endl;

			auto resampler = CreateResampler(algorithm);
			if (resampler) {
				// Convert original data to vector for Resample function
				std::vector<uint8_t> inputVec(originalData, originalData + static_cast<size_t>(originalWidth) * originalHeight * originalChannels);

				resampledData = resampler->Resample(inputVec, originalWidth, originalHeight, m_Width, m_Height, originalChannels);

				if (!resampledData.empty()) {
					finalDataPtr = resampledData.data(); // Point to resampled data
				} else {
					std::cerr << "Warning: Resampling failed for texture: " << path << ". Using original." << std::endl;
					// Fallback: use original dimensions if resampling failed
					m_Width		 = originalWidth;
					m_Height	 = originalHeight;
					finalDataPtr = originalData;
				}
			} else {
				std::cerr << "Warning: Invalid resampling algorithm selected for texture: " << path << ". Using original." << std::endl;
				// Fallback: use original dimensions if algorithm is invalid
				m_Width		 = originalWidth;
				m_Height	 = originalHeight;
				finalDataPtr = originalData;
			}
		} else {
			// No resampling needed, use original dimensions
			m_Width	 = originalWidth;
			m_Height = originalHeight;
		}

		// --- OpenGL Texture Creation ---
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);

		GLenum internalFormat = GL_RGB8; // Use sized internal formats
		GLenum dataFormat	  = GL_RGB;	 // Format of input data
		if (m_Channels == 4) {
			internalFormat = GL_RGBA8;
			dataFormat	   = GL_RGBA;
		} else if (m_Channels == 3) {
			internalFormat = GL_RGB8;
			dataFormat	   = GL_RGB;
		} else if (m_Channels == 1) {
			internalFormat = GL_R8; // Use GL_R8 for single channel internal format
			dataFormat	   = GL_RED;
		} else {
			std::cerr << "Warning: Unsupported number of channels (" << m_Channels << ") for texture: " << path << ". Defaulting to RGB." << std::endl;
			// Default to RGB, might look wrong
			internalFormat = GL_RGB8;
			dataFormat	   = GL_RGB;
		}

		// Use final dimensions (m_Width, m_Height) and final data pointer (finalDataPtr)
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, dataFormat, GL_UNSIGNED_BYTE, finalDataPtr);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// --- Cleanup ---
		stbi_image_free(originalData); // Always free the original data loaded by stb_image
		// resampledData vector goes out of scope automatically if it was used

		glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture
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
