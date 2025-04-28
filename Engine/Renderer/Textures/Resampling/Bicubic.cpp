/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bicubic.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 11:47:11 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/28 11:47:11 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Resampling.h"
#include <algorithm> // For std::max, std::min
#include <cmath>	 // For std::floor, std::abs, std::pow
#include <vector>

namespace Engine::Renderer::Textures {

	class Bicubic : public Resampling {
	private:
		// Catmull-Rom cubic interpolation kernel (a = -0.5)
		inline float CubicKernel(float x) const {
			x = std::abs(x);
			if (x < 1.0f) {
				return 1.5f * x * x * x - 2.5f * x * x + 1.0f;
				// Alternative (more common?): return (1.5f * x - 2.5f) * x * x + 1.0f; // a = -0.5
			} else if (x < 2.0f) {
				return -0.5f * x * x * x + 2.5f * x * x - 4.0f * x + 2.0f;
				// Alternative: return ((-0.5f * x + 2.5f) * x - 4.0f) * x + 2.0f; // a = -0.5
			} else {
				return 0.0f;
			}
		}

		// Perform cubic interpolation on a 1D array of 4 points
		inline float CubicInterpolate(float p0, float p1, float p2, float p3, float t) const {
			// Using the kernel directly (alternative to polynomial form)
			// return p1 + 0.5f * t * (p2 - p0 + t * (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3 + t * (3.0f * (p1 - p2) + p3 - p0)));

			// Simpler approach using weighted sum with kernel
			return p0 * CubicKernel(t + 1.0f) +
				   p1 * CubicKernel(t) +
				   p2 * CubicKernel(t - 1.0f) +
				   p3 * CubicKernel(t - 2.0f);
		}

		// Clamp float result to uint8 range
		inline uint8_t ClampToUint8(float value) const {
			return static_cast<uint8_t>(std::max(0.0f, std::min(255.0f, value)));
		}

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

			std::vector<uint8_t> samplePoints(channels); // Temporary storage for a pixel

			for (int y = 0; y < outputHeight; ++y) {
				for (int x = 0; x < outputWidth; ++x) {
					float srcXf = (x + 0.5f) * scaleX - 0.5f;
					float srcYf = (y + 0.5f) * scaleY - 0.5f;

					int x_int = static_cast<int>(std::floor(srcXf));
					int y_int = static_cast<int>(std::floor(srcYf));

					float tx = srcXf - x_int;
					float ty = srcYf - y_int;

					const size_t dstIndex = (static_cast<size_t>(y) * outputWidth + x) * channels;

					std::vector<float> channelValues(channels, 0.0f);

					// Iterate over the 4x4 neighborhood
					for (int j = -1; j <= 2; ++j) { // Relative row index
						float kernelY = CubicKernel(ty - j);
						int currentY  = y_int + j;

						std::vector<float> rowInterpolated(channels, 0.0f);

						for (int i = -1; i <= 2; ++i) { // Relative column index
							float kernelX = CubicKernel(tx - i);
							int currentX  = x_int + i;

							GetPixelClamped(inputData, inputWidth, inputHeight, channels, currentX, currentY, samplePoints.data());

							// Accumulate weighted horizontal contribution for this row
							for (int c = 0; c < channels; ++c) {
								rowInterpolated[c] += static_cast<float>(samplePoints[c]) * kernelX;
							}
						}
						// Accumulate weighted vertical contribution using the horizontally interpolated row value
						for (int c = 0; c < channels; ++c) {
							channelValues[c] += rowInterpolated[c] * kernelY;
						}
					}

					// Clamp and store the final pixel value
					for (int c = 0; c < channels; ++c) {
						outputData[dstIndex + c] = ClampToUint8(channelValues[c]);
					}
				}
			}

			return outputData;
		}
	};

	// Optional: Factory function
	// std::unique_ptr<Resampling> CreateBicubicResampler() {
	//     return std::make_unique<Bicubic>();
	// }

} // namespace Engine::Renderer::Textures
