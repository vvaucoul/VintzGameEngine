/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Texture.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 12:00:30 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/28 15:29:29 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <cstdint>
#include <string>
#include <vector>

/**
 * @file Texture.h
 * @brief Defines the Texture class for OpenGL texture management and image resampling.
 */

namespace Engine {

	/**
	 * @brief Supported texture resampling algorithms.
	 *
	 * Used to select the image resizing strategy when loading textures.
	 */
	enum class ResamplingAlgorithm {
		None,			 ///< No resampling; load image as-is.
		NearestNeighbor, ///< Fastest, blocky results.
		Bilinear,		 ///< Smooth, slightly blurry.
		Bicubic,		 ///< Sharper than bilinear, may introduce ringing.
		Lanczos			 ///< High quality, slower, may introduce ringing.
	};

	/**
	 * @brief Represents an OpenGL texture with optional image resampling.
	 *
	 * Loads image data from disk, optionally resamples to a target resolution,
	 * and uploads to the GPU as an OpenGL texture object.
	 */
	class Texture {
	public:
		/**
		 * @brief Construct a Texture from an image file.
		 *
		 * @param path Path to the image file.
		 * @param targetWidth Desired width after loading (0 = keep original).
		 * @param targetHeight Desired height after loading (0 = keep original).
		 * @param algorithm Resampling algorithm to use if resizing is needed.
		 *
		 * If targetWidth/targetHeight are zero or match the loaded image, no resampling occurs.
		 */
		Texture(const std::string &path,
				int targetWidth				  = 0,
				int targetHeight			  = 0,
				ResamplingAlgorithm algorithm = ResamplingAlgorithm::Bilinear);

		/**
		 * @brief Destroy the Texture and free GPU resources.
		 */
		~Texture();

		/**
		 * @brief Bind this texture to a given texture unit.
		 * @param slot Texture unit index (default: 0).
		 */
		void Bind(unsigned int slot = 0) const;

		/**
		 * @brief Unbind any texture from the active texture unit.
		 */
		void Unbind() const;

		/**
		 * @brief Get the OpenGL texture object ID.
		 * @return GLuint texture handle.
		 */
		unsigned int GetID() const { return m_RendererID; }

		/**
		 * @brief Get the width of the texture (after resampling, if any).
		 * @return Texture width in pixels.
		 */
		int GetWidth() const { return m_Width; }

		/**
		 * @brief Get the height of the texture (after resampling, if any).
		 * @return Texture height in pixels.
		 */
		int GetHeight() const { return m_Height; }

		/**
		 * @brief Get the number of color channels in the original image.
		 * @return Number of channels (e.g., 3 for RGB, 4 for RGBA).
		 */
		int GetChannels() const { return m_Channels; }

	private:
		unsigned int m_RendererID = 0; ///< OpenGL texture object handle.
		std::string m_FilePath;		   ///< Path to the source image file.
		int m_Width	   = 0;			   ///< Final texture width (after resampling).
		int m_Height   = 0;			   ///< Final texture height (after resampling).
		int m_Channels = 0;			   ///< Number of channels in the original image.
	};

} // namespace Engine
