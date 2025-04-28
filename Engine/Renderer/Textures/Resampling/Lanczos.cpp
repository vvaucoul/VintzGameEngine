/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lanczos.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 11:47:14 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/28 15:33:50 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Lanczos.h"
#include <algorithm>
#include <cmath>
#include <vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace Engine::Renderer::Textures {

	Lanczos::Lanczos(int a) : m_A(a) {}

	namespace {
		inline float Kernel(float x, int a) {
			if (x == 0.0f) return 1.0f;
			float absx = std::abs(x);
			if (absx < static_cast<float>(a)) {
				float pix	= static_cast<float>(M_PI) * x;
				float pix_a = pix / static_cast<float>(a);
				return (std::sin(pix) * std::sin(pix_a)) / (pix * pix_a);
			}
			return 0.0f;
		}
		inline uint8_t ClampToUint8(float v) {
			return static_cast<uint8_t>(std::max(0.0f, std::min(255.0f, v + 0.5f)));
		}
	}

	std::vector<uint8_t> Lanczos::Resample(
		const std::vector<uint8_t> &inputData,
		int inputWidth,
		int inputHeight,
		int outputWidth,
		int outputHeight,
		int channels) const {
		std::vector<uint8_t> outputData(static_cast<size_t>(outputWidth) * outputHeight * channels);
		const float scaleX	= static_cast<float>(inputWidth) / outputWidth;
		const float scaleY	= static_cast<float>(inputHeight) / outputHeight;
		const float radiusX = static_cast<float>(m_A) * std::max(1.0f, 1.0f / scaleX);
		const float radiusY = static_cast<float>(m_A) * std::max(1.0f, 1.0f / scaleY);
		std::vector<uint8_t> sample(channels);

		for (int y = 0; y < outputHeight; ++y) {
			for (int x = 0; x < outputWidth; ++x) {
				float srcXf = (x + 0.5f) * scaleX - 0.5f;
				float srcYf = (y + 0.5f) * scaleY - 0.5f;
				int xMin	= static_cast<int>(std::ceil(srcXf - radiusX));
				int xMax	= static_cast<int>(std::floor(srcXf + radiusX));
				int yMin	= static_cast<int>(std::ceil(srcYf - radiusY));
				int yMax	= static_cast<int>(std::floor(srcYf + radiusY));
				std::vector<float> sum(channels, 0.0f);
				float totalWeight = 0.0f;

				for (int sy = yMin; sy <= yMax; ++sy) {
					float wy = Kernel((srcYf - static_cast<float>(sy)) * (1.0f / std::max(1.0f, 1.0f / scaleY)), m_A);
					if (wy == 0.0f) continue;
					for (int sx = xMin; sx <= xMax; ++sx) {
						float wx = Kernel((srcXf - static_cast<float>(sx)) * (1.0f / std::max(1.0f, 1.0f / scaleX)), m_A);
						if (wx == 0.0f) continue;
						float w = wx * wy;
						totalWeight += w;
						GetPixelClamped(inputData, inputWidth, inputHeight, channels, sx, sy, sample.data());
						for (int c = 0; c < channels; ++c)
							sum[c] += static_cast<float>(sample[c]) * w;
					}
				}
				const size_t dstIndex = (static_cast<size_t>(y) * outputWidth + x) * channels;
				if (totalWeight > 1e-6f) {
					for (int c = 0; c < channels; ++c)
						outputData[dstIndex + c] = ClampToUint8(sum[c] / totalWeight);
				} else {
					int nearX = static_cast<int>(std::floor(srcXf));
					int nearY = static_cast<int>(std::floor(srcYf));
					GetPixelClamped(inputData, inputWidth, inputHeight, channels, nearX, nearY, sample.data());
					for (int c = 0; c < channels; ++c)
						outputData[dstIndex + c] = sample[c];
				}
			}
		}
		return outputData;
	}

} // namespace Engine::Renderer::Textures
