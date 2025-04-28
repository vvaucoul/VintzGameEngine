/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bilinear.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 11:47:11 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/28 15:33:50 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Bilinear.h"
#include <algorithm>
#include <cmath>
#include <vector>

namespace Engine::Renderer::Textures {

	std::vector<uint8_t> Bilinear::Resample(
		const std::vector<uint8_t> &inputData,
		int inputWidth,
		int inputHeight,
		int outputWidth,
		int outputHeight,
		int channels) const {
		std::vector<uint8_t> outputData(static_cast<size_t>(outputWidth) * outputHeight * channels);
		const float scaleX = static_cast<float>(inputWidth) / outputWidth;
		const float scaleY = static_cast<float>(inputHeight) / outputHeight;

		std::vector<uint8_t> p00(channels), p10(channels), p01(channels), p11(channels);

		for (int y = 0; y < outputHeight; ++y) {
			for (int x = 0; x < outputWidth; ++x) {
				float srcXf = (x + 0.5f) * scaleX - 0.5f;
				float srcYf = (y + 0.5f) * scaleY - 0.5f;

				int x0 = static_cast<int>(std::floor(srcXf));
				int y0 = static_cast<int>(std::floor(srcYf));
				int x1 = x0 + 1;
				int y1 = y0 + 1;

				float tx = srcXf - x0;
				float ty = srcYf - y0;

				GetPixelClamped(inputData, inputWidth, inputHeight, channels, x0, y0, p00.data());
				GetPixelClamped(inputData, inputWidth, inputHeight, channels, x1, y0, p10.data());
				GetPixelClamped(inputData, inputWidth, inputHeight, channels, x0, y1, p01.data());
				GetPixelClamped(inputData, inputWidth, inputHeight, channels, x1, y1, p11.data());

				const size_t dstIndex = (static_cast<size_t>(y) * outputWidth + x) * channels;

				for (int c = 0; c < channels; ++c) {
					uint8_t top				 = Lerp(p00[c], p10[c], tx);
					uint8_t bottom			 = Lerp(p01[c], p11[c], tx);
					outputData[dstIndex + c] = Lerp(top, bottom, ty);
				}
			}
		}
		return outputData;
	}

} // namespace Engine::Renderer::Textures
