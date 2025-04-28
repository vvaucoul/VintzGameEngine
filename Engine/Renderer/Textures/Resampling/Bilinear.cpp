/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bilinear.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 11:47:11 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/28 11:47:12 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Resampling.h"
#include <algorithm> // For std::max, std::min
#include <cmath>	 // For std::floor
#include <vector>

namespace Engine::Renderer::Textures {

	class Bilinear : public Resampling {
	public:
		std::vector<uint8_t> Resample(
			const std::vector<uint8_t> &inputData,
			int inputWidth,
			int inputHeight,
			int outputWidth,
			int outputHeight,
			int channels) const override {

			std::vector<uint8_t> outputData(static_cast<size_t>(outputWidth) * outputHeight * channels);
			const float scaleX = static_cast<float>(inputWidth) / outputWidth;
			const float scaleY = static_cast<float>(inputHeight) / outputHeight;

			std::vector<uint8_t> p00(channels), p10(channels), p01(channels), p11(channels);

			for (int y = 0; y < outputHeight; ++y) {
				for (int x = 0; x < outputWidth; ++x) {
					// Calculate corresponding floating-point coordinates in the input image
					float srcXf = (x + 0.5f) * scaleX - 0.5f;
					float srcYf = (y + 0.5f) * scaleY - 0.5f;

					// Get integer coordinates of the 4 neighboring pixels
					int x0 = static_cast<int>(std::floor(srcXf));
					int y0 = static_cast<int>(std::floor(srcYf));
					int x1 = x0 + 1;
					int y1 = y0 + 1;

					// Calculate interpolation weights
					float tx = srcXf - x0;
					float ty = srcYf - y0;

					// Get the 4 neighboring pixel values (clamped)
					GetPixelClamped(inputData, inputWidth, inputHeight, channels, x0, y0, p00.data());
					GetPixelClamped(inputData, inputWidth, inputHeight, channels, x1, y0, p10.data());
					GetPixelClamped(inputData, inputWidth, inputHeight, channels, x0, y1, p01.data());
					GetPixelClamped(inputData, inputWidth, inputHeight, channels, x1, y1, p11.data());

					const size_t dstIndex = (static_cast<size_t>(y) * outputWidth + x) * channels;

					// Interpolate for each channel
					for (int c = 0; c < channels; ++c) {
						// Interpolate horizontally
						uint8_t top	   = Lerp(p00[c], p10[c], tx);
						uint8_t bottom = Lerp(p01[c], p11[c], tx);
						// Interpolate vertically
						outputData[dstIndex + c] = Lerp(top, bottom, ty);
					}
				}
			}

			return outputData;
		}
	};

	// Optional: Factory function
	// std::unique_ptr<Resampling> CreateBilinearResampler() {
	//     return std::make_unique<Bilinear>();
	// }

} // namespace Engine::Renderer::Textures
