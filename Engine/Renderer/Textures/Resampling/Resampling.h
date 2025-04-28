/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Resampling.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 11:47:22 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/28 11:47:23 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <cstdint>
#include <vector>

namespace Engine::Renderer::Textures {

	/**
	 * @brief Abstract base class for texture resampling algorithms.
	 */
	class Resampling {
	public:
		virtual ~Resampling() = default;

		/**
		 * @brief Resamples the input texture data to the target dimensions.
		 *
		 * @param inputData The raw pixel data of the input texture.
		 * @param inputWidth The width of the input texture.
		 * @param inputHeight The height of the input texture.
		 * @param outputWidth The desired width of the output texture.
		 * @param outputHeight The desired height of the output texture.
		 * @param channels The number of color channels per pixel (e.g., 3 for RGB, 4 for RGBA).
		 * @return A vector containing the raw pixel data of the resampled texture.
		 */
		virtual std::vector<uint8_t> Resample(
			const std::vector<uint8_t> &inputData,
			int inputWidth,
			int inputHeight,
			int outputWidth,
			int outputHeight,
			int channels) const = 0;

	protected:
		// Helper to get pixel data safely with boundary clamping
		inline void GetPixelClamped(const std::vector<uint8_t> &data, int width, int height, int channels, int x, int y, uint8_t *pixel) const {
			x				   = std::max(0, std::min(x, width - 1));
			y				   = std::max(0, std::min(y, height - 1));
			const size_t index = (static_cast<size_t>(y) * width + x) * channels;
			for (int c = 0; c < channels; ++c) {
				pixel[c] = data[index + c];
			}
		}

		// Helper for linear interpolation
		inline uint8_t Lerp(uint8_t a, uint8_t b, float t) const {
			return static_cast<uint8_t>(static_cast<float>(a) * (1.0f - t) + static_cast<float>(b) * t);
		}
	};

} // namespace Engine::Renderer::Textures
