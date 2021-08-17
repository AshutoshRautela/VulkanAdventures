#pragma once

#pragma warning(push, 0)
#define GLFW_INCLUDE_VULKAN
#include <GLFW\glfw3.h>
#pragma warning(pop)

#include <string>
#include <fstream>
#include <stdexcept>
#include <vector>

#include "Logger.h"

namespace va {
	class GraphicsPipeline
	{
	public:
		GraphicsPipeline(const VkDevice&);
		~GraphicsPipeline();

		// Deactivating the Copy Constructor and Assignment Operator
		GraphicsPipeline(const GraphicsPipeline&) = delete;
		const GraphicsPipeline& operator=(const GraphicsPipeline&) = delete;

		void readShaders(const std::string&, const std::string&);

		void startGraphicsPipelineProcess(const std::string&, const std::string&, const VkSurfaceFormatKHR&, const VkExtent2D&);

		inline const VkRenderPass& getRenderPass() const { return this->_vkRenderPass; }
		inline const VkPipeline& getGraphicsPipeline() const { return this->_vkPipeline; }

	private:
		std::vector<char> _vSource;
		std::vector<char> _fSource;

		const VkDevice& _vkDevice;

		VkShaderModule _vShaderModule;
		VkShaderModule _fShaderModule;

		VkPipelineShaderStageCreateInfo _shaderStages[2];
		VkPipelineVertexInputStateCreateInfo _vertexInputState;	
		VkPipelineInputAssemblyStateCreateInfo _inputAssemblyState;

		VkRect2D _scissor;
		VkViewport _vkViewport;
		VkPipelineViewportStateCreateInfo _viewportState;

		VkPipelineRasterizationStateCreateInfo _rasterizerState;
		VkPipelineMultisampleStateCreateInfo _multisampleState;
		VkPipelineColorBlendAttachmentState _vkPipelineColorBlendAttachmentState;
		VkPipelineColorBlendStateCreateInfo _colorBlendState;

		VkPipelineLayout _vkPipelineLayout;
		VkRenderPass _vkRenderPass;
		VkPipeline _vkPipeline;

		void createRenderPass(const VkSurfaceFormatKHR&);
		void createShaderModules();
		void initShaderPipelineStage();
		void initVertexInputPipelineState();
		void initInputAssemblyPipelineState();
		void initViewportAndScissorsPipelineState(const VkExtent2D&);
		void initRasterizerPipelineState();
		void initMultisampleState();
		void initColorBlendState();
		void createGraphicsPipelineLayout();

		void createGraphicsPipeline();

		void clearShaderModules();
	};
}