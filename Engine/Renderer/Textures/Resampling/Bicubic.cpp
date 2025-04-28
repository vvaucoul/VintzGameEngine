/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bicubic.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 11:47:11 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/28 15:34:47 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Bicubic.h"
#include <algorithm>
#include <cmath>
#include <vector>

namespace Engine::Renderer::Textures {

	namespace {
		inline float CubicKernel(float x) {
			x = std::abs(x);
			if (x < 1.0f)
				return 1.5f * x * x * x - 2.5f * x * x + 1.0f;
			else if (x < 2.0f)
				return -0.5f * x * x * x + 2.5f * x * x - 4.0f * x + 2.0f;
			return 0.0f;
		}
		inline uint8_t ClampToUint8(float v) {
			return static_cast<uint8_t>(std::max(0.0f, std::min(255.0f, v)));
		}
	}

	std::vector<uint8_t> Bicubic::Resample(
		const std::vector<uint8_t> &inputData,
		int inputWidth,
		int inputHeight,
		int outputWidth,
		int outputHeight,
		int channels) const {
		std::vector<uint8_t> outputData(static_cast<size_t>(outputWidth) * outputHeight * channels);
		const float scaleX = static_cast<float>(inputWidth) / outputWidth;
		const float scaleY = static_cast<float>(inputHeight) / outputHeight;
		std::vector<uint8_t> sample(channels);

		for (int y = 0; y < outputHeight; ++y) {
			for (int x = 0; x < outputWidth; ++x) {
				float srcXf			  = (x + 0.5f) * scaleX - 0.5f;
				float srcYf			  = (y + 0.5f) * scaleY - 0.5f;
				int x_int			  = static_cast<int>(std::floor(srcXf));
				int y_int			  = static_cast<int>(std::floor(srcYf));
				float tx			  = srcXf - x_int;
				float ty			  = srcYf - y_int;
				const size_t dstIndex = (static_cast<size_t>(y) * outputWidth + x) * channels;
				std::vector<float> channelSum(channels, 0.0f);

				for (int j = -1; j <= 2; ++j) {
					float ky = CubicKernel(ty - j);
					int sy	 = y_int + j;
					std::vector<float> row(channels, 0.0f);
					for (int i = -1; i <= 2; ++i) {
						float kx = CubicKernel(tx - i);
						int sx	 = x_int + i;
						GetPixelClamped(inputData, inputWidth, inputHeight, channels, sx, sy, sample.data());
						for (int c = 0; c < channels; ++c)
							row[c] += static_cast<float>(sample[c]) * kx;
					}
					for (int c = 0; c < channels; ++c)
						channelSum[c] += row[c] * ky;
				}
				for (int c = 0; c < channels; ++c)
					outputData[dstIndex + c] = ClampToUint8(channelSum[c]);
			}
		}
		return outputData;
	}

} // namespace Engine::Renderer::Textures
