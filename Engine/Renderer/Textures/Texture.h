/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Texture.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 12:00:30 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/28 11:47:47 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <cstdint> // Added
#include <string>
#include <vector> // Added

// Forward declare Resampling classes if needed, or include headers in .cpp
// namespace Engine::Renderer::Textures { class Resampling; }

namespace Engine {

	/**
	 * @brief Enum for selecting the texture resampling algorithm.
	 */
	enum class ResamplingAlgorithm {
		None,			 // Default, no resampling
		NearestNeighbor, // Fastest, blocky results
		Bilinear,		 // Smooth, slightly blurry
		Bicubic,		 // Sharper than bilinear, potential ringing
		Lanczos			 // High quality, potentially slower, ringing artifacts
	};

	class Texture {
	public:
		/**
		 * @brief Constructs a Texture object, loading image data from a file.
		 *
		 * @param path Path to the image file.
		 * @param targetWidth Desired width after loading. If 0 or matches loaded width, no resampling occurs horizontally.
		 * @param targetHeight Desired height after loading. If 0 or matches loaded height, no resampling occurs vertically.
		 * @param algorithm The resampling algorithm to use if resizing is necessary.
		 */
		Texture(const std::string &path,
				int targetWidth				  = 0,
				int targetHeight			  = 0,
				ResamplingAlgorithm algorithm = ResamplingAlgorithm::Bilinear); // Default to Bilinear if resizing

		~Texture();

		void Bind(unsigned int slot = 0) const;
		void Unbind() const;

		unsigned int GetID() const { return m_RendererID; }
		int GetWidth() const { return m_Width; }	   // Returns final width
		int GetHeight() const { return m_Height; }	   // Returns final height
		int GetChannels() const { return m_Channels; } // Returns original channels

	private:
		unsigned int m_RendererID;
		std::string m_FilePath;
		int m_Width;	// Final width (potentially resampled)
		int m_Height;	// Final height (potentially resampled)
		int m_Channels; // Original number of channels from file
	};

} // namespace Engine
