/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   NearestNeighbor.h                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 15:32:33 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/28 15:35:48 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Resampling.h"
#include <cstdint>
#include <vector>

namespace Engine {
	namespace Renderer {
		namespace Textures {

			/**
			 * @brief Nearest neighbor resampling algorithm for image resizing.
			 *
			 * Provides fast, non-interpolated resizing of images by mapping each output pixel
			 * to the nearest input pixel. Supports any number of color channels (e.g., RGB, RGBA).
			 */
			class NearestNeighbor : public Resampling {
			public:
				/**
				 * @brief Resample image data using nearest neighbor interpolation.
				 *
				 * @param inputData   Source image pixel data (row-major, tightly packed).
				 * @param inputWidth  Source image width in pixels.
				 * @param inputHeight Source image height in pixels.
				 * @param outputWidth Desired output image width in pixels.
				 * @param outputHeight Desired output image height in pixels.
				 * @param channels    Number of color channels per pixel (e.g., 3 = RGB, 4 = RGBA).
				 * @return std::vector<uint8_t> Resampled image data (row-major, tightly packed).
				 */
				std::vector<uint8_t> Resample(
					const std::vector<uint8_t> &inputData,
					int inputWidth,
					int inputHeight,
					int outputWidth,
					int outputHeight,
					int channels) const override;
			};

		} // namespace Textures
	} // namespace Renderer
} // namespace Engine
