/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   NearestNeighbor.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 11:47:16 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/28 15:33:50 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "NearestNeighbor.h"
#include <algorithm>
#include <cmath>
#include <vector>

namespace Engine::Renderer::Textures {

	std::vector<uint8_t> NearestNeighbor::Resample(
		const std::vector<uint8_t> &inputData,
		int inputWidth,
		int inputHeight,
		int outputWidth,
		int outputHeight,
		int channels) const {
		std::vector<uint8_t> outputData(static_cast<size_t>(outputWidth) * outputHeight * channels);
		const float scaleX = static_cast<float>(inputWidth) / outputWidth;
		const float scaleY = static_cast<float>(inputHeight) / outputHeight;

		for (int y = 0; y < outputHeight; ++y) {
			for (int x = 0; x < outputWidth; ++x) {
				int srcX			  = static_cast<int>(std::floor(x * scaleX));
				int srcY			  = static_cast<int>(std::floor(y * scaleY));
				srcX				  = std::max(0, std::min(srcX, inputWidth - 1));
				srcY				  = std::max(0, std::min(srcY, inputHeight - 1));
				const size_t srcIndex = (static_cast<size_t>(srcY) * inputWidth + srcX) * channels;
				const size_t dstIndex = (static_cast<size_t>(y) * outputWidth + x) * channels;
				for (int c = 0; c < channels; ++c)
					outputData[dstIndex + c] = inputData[srcIndex + c];
			}
		}
		return outputData;
	}

} // namespace Engine::Renderer::Textures
