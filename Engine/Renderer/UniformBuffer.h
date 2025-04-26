/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UniformBuffer.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 12:31:40 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/26 12:31:41 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

namespace Engine {

	class UniformBuffer {
	public:
		// size: taille du buffer (en octets), binding: point d'attache du UBO
		UniformBuffer(unsigned int size, unsigned int binding);
		~UniformBuffer();

		void Bind() const;
		void Unbind() const;

		// Met à jour une portion du buffer (offset en octets)
		void SetData(unsigned int offset, unsigned int size, const void *data) const;

	private:
		unsigned int m_RendererID;
	};

} // namespace Engine