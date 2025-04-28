/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Resampling.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 11:47:22 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/28 15:36:29 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <algorithm>
#include <cstdint>
#include <vector>

/**
 * @file Resampling.h
 * @brief Abstract base class for image resampling algorithms.
 *
 * Provides a unified interface for implementing various image resampling
 * (scaling/interpolation) algorithms. Derived classes implement the Resample()
 * method for specific interpolation strategies (e.g., Nearest Neighbor, Bilinear, Bicubic, Lanczos).
 */

namespace Engine {
	namespace Renderer {
		namespace Textures {

			/**
			 * @brief Abstract base class for image resampling.
			 *
			 * Defines the interface for resampling (resizing) image data.
			 * Derived classes must implement the Resample() method.
			 */
			class Resampling {
			public:
				/**
				 * @brief Virtual destructor.
				 */
				virtual ~Resampling() = default;

				/**
				 * @brief Resample image data to a new resolution.
				 *
				 * @param inputData   Source image pixel data (row-major, tightly packed).
				 * @param inputWidth  Source image width in pixels.
				 * @param inputHeight Source image height in pixels.
				 * @param outputWidth Desired output image width in pixels.
				 * @param outputHeight Desired output image height in pixels.
				 * @param channels    Number of color channels per pixel (e.g., 3 = RGB, 4 = RGBA).
				 * @return std::vector<uint8_t> Resampled image data (row-major, tightly packed).
				 */
				virtual std::vector<uint8_t> Resample(
					const std::vector<uint8_t> &inputData,
					int inputWidth,
					int inputHeight,
					int outputWidth,
					int outputHeight,
					int channels) const = 0;

			protected:
				/**
				 * @brief Safely fetch a pixel from image data, clamping coordinates to valid range.
				 *
				 * @param data     Image pixel data (row-major, tightly packed).
				 * @param width    Image width in pixels.
				 * @param height   Image height in pixels.
				 * @param channels Number of color channels per pixel.
				 * @param x        X coordinate (will be clamped to [0, width-1]).
				 * @param y        Y coordinate (will be clamped to [0, height-1]).
				 * @param pixel    Output buffer to receive the pixel data (must be at least 'channels' in size).
				 */
				inline void GetPixelClamped(
					const std::vector<uint8_t> &data,
					int width,
					int height,
					int channels,
					int x,
					int y,
					uint8_t *pixel) const {
					x				   = std::max(0, std::min(x, width - 1));
					y				   = std::max(0, std::min(y, height - 1));
					const size_t index = (static_cast<size_t>(y) * width + x) * channels;
					for (int c = 0; c < channels; ++c) {
						pixel[c] = data[index + c];
					}
				}

				/**
				 * @brief Linearly interpolate between two uint8_t values.
				 *
				 * @param a First value.
				 * @param b Second value.
				 * @param t Interpolation factor in [0, 1].
				 * @return Interpolated value.
				 */
				inline uint8_t Lerp(uint8_t a, uint8_t b, float t) const {
					return static_cast<uint8_t>(static_cast<float>(a) * (1.0f - t) + static_cast<float>(b) * t);
				}
			};

		} // namespace Textures
	} // namespace Renderer
} // namespace Engine

// Include concrete resampling algorithm headers after base class definition.
#include "Bicubic.h"
#include "Bilinear.h"
#include "Lanczos.h"
#include "NearestNeighbor.h"
