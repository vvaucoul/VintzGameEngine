/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lanczos.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 15:32:31 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/28 15:35:36 by vvaucoul         ###   ########.fr       */
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
			 * @brief Lanczos resampling algorithm for high-quality image resizing.
			 *
			 * Implements the Lanczos filter for resampling images, which provides
			 * superior quality for downscaling and upscaling compared to simpler
			 * algorithms like nearest neighbor or bilinear interpolation.
			 *
			 * The parameter 'a' controls the size of the Lanczos window (typically 2 or 3).
			 */
			class Lanczos : public Resampling {
			public:
				/**
				 * @brief Construct a Lanczos resampler.
				 * @param a Window size parameter (default: 3). Higher values yield sharper results but are slower.
				 */
				explicit Lanczos(int a = 3);

				/**
				 * @brief Resample image data using the Lanczos algorithm.
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

			private:
				int m_A; ///< Lanczos window size parameter.
			};

		} // namespace Textures
	} // namespace Renderer
} // namespace Engine
