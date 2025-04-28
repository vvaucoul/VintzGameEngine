/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lanczos.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 11:47:14 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/28 11:47:14 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Resampling.h"
#include <algorithm> // For std::max, std::min
#include <cmath>	 // For std::floor, std::abs, std::sin, M_PI
#include <vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace Engine::Renderer::Textures {

	class Lanczos : public Resampling {
	private:
		const int a; // Lanczos kernel size parameter (e.g., 2 or 3)

		// Lanczos kernel function
		inline float LanczosKernel(float x) const {
			if (x == 0.0f) {
				return 1.0f;
			}
			float abs_x = std::abs(x);
			if (abs_x < static_cast<float>(a)) {
				float pi_x		  = static_cast<float>(M_PI) * x;
				float pi_x_over_a = pi_x / static_cast<float>(a);
				// Check for potential division by zero (though pi_x should not be zero here if x != 0)
				if (pi_x == 0.0f) return 1.0f;											// Should not happen based on x != 0 check, but safety first
				return (std::sin(pi_x) * std::sin(pi_x_over_a)) / (pi_x * pi_x_over_a); // Simplified form sinc(x) * sinc(x/a)
			}
			return 0.0f;
		}

		// Clamp float result to uint8 range
		inline uint8_t ClampToUint8(float value) const {
			// Add 0.5f for rounding before casting
			return static_cast<uint8_t>(std::max(0.0f, std::min(255.0f, value + 0.5f)));
		}

	public:
		// Constructor to set the kernel size 'a' (default to 3 for Lanczos-3)
		explicit Lanczos(int kernelSizeA = 3) : a(kernelSizeA) {}

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

			const float kernelRadiusX = static_cast<float>(a) * std::max(1.0f, 1.0f / scaleX);
			const float kernelRadiusY = static_cast<float>(a) * std::max(1.0f, 1.0f / scaleY);

			std::vector<uint8_t> samplePixel(channels); // Temporary storage for a pixel

			for (int y = 0; y < outputHeight; ++y) {
				for (int x = 0; x < outputWidth; ++x) {
					// Calculate center of the kernel in source image coordinates
					float srcXf = (static_cast<float>(x) + 0.5f) * scaleX - 0.5f;
					float srcYf = (static_cast<float>(y) + 0.5f) * scaleY - 0.5f;

					// Determine the sampling bounds in the source image
					int xMin = static_cast<int>(std::ceil(srcXf - kernelRadiusX));
					int xMax = static_cast<int>(std::floor(srcXf + kernelRadiusX));
					int yMin = static_cast<int>(std::ceil(srcYf - kernelRadiusY));
					int yMax = static_cast<int>(std::floor(srcYf + kernelRadiusY));

					std::vector<float> channelSum(channels, 0.0f);
					float totalWeight = 0.0f;

					// Iterate over the kernel neighborhood in the source image
					for (int srcY = yMin; srcY <= yMax; ++srcY) {
						float weightY = LanczosKernel((srcYf - static_cast<float>(srcY)) * (1.0f / std::max(1.0f, 1.0f / scaleY))); // Apply kernel scale based on down/upsampling
						if (weightY == 0.0f) continue;

						for (int srcX = xMin; srcX <= xMax; ++srcX) {
							float weightX = LanczosKernel((srcXf - static_cast<float>(srcX)) * (1.0f / std::max(1.0f, 1.0f / scaleX))); // Apply kernel scale
							if (weightX == 0.0f) continue;

							float finalWeight = weightX * weightY;
							totalWeight += finalWeight;

							GetPixelClamped(inputData, inputWidth, inputHeight, channels, srcX, srcY, samplePixel.data());

							for (int c = 0; c < channels; ++c) {
								channelSum[c] += static_cast<float>(samplePixel[c]) * finalWeight;
							}
						}
					}

					const size_t dstIndex = (static_cast<size_t>(y) * outputWidth + x) * channels;

					// Normalize and clamp the results
					if (totalWeight > 1e-6) { // Avoid division by zero or near-zero
						for (int c = 0; c < channels; ++c) {
							outputData[dstIndex + c] = ClampToUint8(channelSum[c] / totalWeight);
						}
					} else {
						// Fallback: Use nearest neighbor if total weight is negligible (should rarely happen)
						int nearX = static_cast<int>(std::floor(srcXf));
						int nearY = static_cast<int>(std::floor(srcYf));
						GetPixelClamped(inputData, inputWidth, inputHeight, channels, nearX, nearY, samplePixel.data());
						for (int c = 0; c < channels; ++c) {
							outputData[dstIndex + c] = samplePixel[c];
						}
					}
				}
			}

			return outputData;
		}
	};

	// Optional: Factory function
	// std::unique_ptr<Resampling> CreateLanczosResampler(int a = 3) {
	//     return std::make_unique<Lanczos>(a);
	// }

} // namespace Engine::Renderer::Textures
