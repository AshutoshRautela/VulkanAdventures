#include "GraphicsPipeline.h"

namespace va {

	GraphicsPipeline::GraphicsPipeline(const VkDevice& vkDevice): _vkDevice(vkDevice) {

		this->_vShaderModule = VK_NULL_HANDLE;
		this->_fShaderModule= VK_NULL_HANDLE;

		this->_vertexInputState = {};
		this->_inputAssemblyState = {};

		this->_scissor = {};
		this->_vkViewport = {};
		this->_viewportState = {};

		this->_rasterizerState = {};
		this->_multisampleState = {};
		this->_colorBlendState = {};
	}

	void GraphicsPipeline::startGraphicsPipelineProcess(const std::string& vShaderPath, const std::string& fShaderPath, const VkSurfaceFormatKHR& swapchainSurfaceFormat, const VkExtent2D& swapchainExxtent) {
		this->readShaders(vShaderPath, fShaderPath);
		this->createRenderPass(swapchainSurfaceFormat);
		this->createShaderModules();
		this->initShaderPipelineStage();
		this->initVertexInputPipelineState();
		this->initInputAssemblyPipelineState();
		this->initViewportAndScissorsPipelineState(swapchainExxtent);
		this->initRasterizerPipelineState();
		this->initMultisampleState();
		this->initColorBlendState();
		this->createGraphicsPipelineLayout();
		this->createGraphicsPipeline();
		this->clearShaderModules();
	}

	void GraphicsPipeline::createRenderPass(const VkSurfaceFormatKHR& swapChainSurfaceFormat) {

		VkSubpassDependency vkSubpassDependency{};
		vkSubpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		vkSubpassDependency.dstSubpass = 0;
		vkSubpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		vkSubpassDependency.srcAccessMask = 0;
		vkSubpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		vkSubpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkAttachmentDescription vkAttachmentDescription{};
		vkAttachmentDescription.format = swapChainSurfaceFormat.format;
		vkAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
		vkAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		vkAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		vkAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		vkAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		vkAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		vkAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference vkAttachmentReference{};
		vkAttachmentReference.attachment = 0;
		vkAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription vkSubpassDescription{};
		vkSubpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		vkSubpassDescription.colorAttachmentCount = 1;
		vkSubpassDescription.pColorAttachments = &vkAttachmentReference;

		VkRenderPassCreateInfo vkRenderPassCreateInfo{};
		vkRenderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		vkRenderPassCreateInfo.attachmentCount = 1;
		vkRenderPassCreateInfo.pAttachments = &vkAttachmentDescription;
		vkRenderPassCreateInfo.subpassCount = 1;
		vkRenderPassCreateInfo.pSubpasses = &vkSubpassDescription;
		vkRenderPassCreateInfo.dependencyCount = 1;
		vkRenderPassCreateInfo.pDependencies = &vkSubpassDependency;

		if (vkCreateRenderPass(this->_vkDevice, &vkRenderPassCreateInfo, nullptr, &this->_vkRenderPass) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create Render Pass");
		}

#if _DEBUG
		LOGGER_INFO("Successfully Created Render Pass");
#endif
	}

	void GraphicsPipeline::readShaders(const std::string& vShaderPath, const std::string& fShaderPath) {
		this->_vSource.clear();
		this->_fSource.clear();

		// Reading Vertex Source
		std::ifstream fStream(vShaderPath, std::ios::ate | std::ios::binary);
		if (!fStream.is_open()) {
			throw std::runtime_error("Failed to read Vertex Shader: " + vShaderPath);
		}

		uint32_t fileSize = static_cast<uint32_t>(fStream.tellg());
		this->_vSource.resize(fileSize);

		fStream.seekg(0);
		fStream.read(this->_vSource.data(), fileSize);
		fStream.close();
		fileSize = 0;

		// Reading Fragment Source
		fStream.open(fShaderPath, std::ios::ate | std::ios::binary);
		if (!fStream.is_open()) {
			throw std::runtime_error("Failed to read Fragment Shader: " + fShaderPath);
		}

		fileSize = static_cast<uint32_t>(fStream.tellg());
		this->_fSource.resize(fileSize);

		fStream.seekg(0);
		fStream.read(this->_fSource.data(), fileSize);
		fStream.close();

#if _DEBUG || _RELEASE
		LOGGER_INFO("Read Vertex Shader and Fragment Shader");
#endif
	}

	void GraphicsPipeline::createShaderModules() {
		VkShaderModuleCreateInfo vkShaderModuleCreateInfo{};

		vkShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		vkShaderModuleCreateInfo.codeSize = static_cast<size_t>(this->_vSource.size());
		vkShaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(this->_vSource.data());

		if (vkCreateShaderModule(this->_vkDevice, &vkShaderModuleCreateInfo, nullptr, &this->_vShaderModule) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create vertex shader module");
		}

		vkShaderModuleCreateInfo = {};
		vkShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		vkShaderModuleCreateInfo.codeSize = static_cast<size_t>(this->_fSource.size());
		vkShaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(this->_fSource.data());

		if (vkCreateShaderModule(this->_vkDevice, &vkShaderModuleCreateInfo, nullptr, &this->_fShaderModule) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create fragment shader module");
		}

#if _DEBUG
		LOGGER_INFO("Created Vertex and Fragment Shader Modules");
#endif // _DEBUG
	}

	void GraphicsPipeline::initShaderPipelineStage() {
		this->_shaderStages[0] = {};
		this->_shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		this->_shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
		this->_shaderStages[0].module = this->_vShaderModule;
		this->_shaderStages[0].pName = "main";
		this->_shaderStages[0].pSpecializationInfo = nullptr;

		this->_shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		this->_shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		this->_shaderStages[1].module = this->_fShaderModule;
		this->_shaderStages[1].pName = "main";
		this->_shaderStages[1].pSpecializationInfo = nullptr;

		this->_shaderStages[0] = this->_shaderStages[0];
		this->_shaderStages[1] = this->_shaderStages[1];
	}

	void GraphicsPipeline::initVertexInputPipelineState() {
		this->_bindingDescription = Vertex::getBindingDescription();
		this->_attributeDescription = Vertex::getAttributeDescription();

		this->_vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

		this->_vertexInputState.vertexBindingDescriptionCount = 1;
		this->_vertexInputState.pVertexBindingDescriptions = &this->_bindingDescription;
		this->_vertexInputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(this->_attributeDescription.size());
		this->_vertexInputState.pVertexAttributeDescriptions = this->_attributeDescription.data();
	}

	void GraphicsPipeline::initInputAssemblyPipelineState() {
		this->_inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		this->_inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		this->_inputAssemblyState.primitiveRestartEnable = VK_FALSE;
	}

	void GraphicsPipeline::initViewportAndScissorsPipelineState(const VkExtent2D& swapchainExtent) {
		this->_scissor.offset = { 0 , 0 };
		this->_scissor.extent = swapchainExtent;
		
		this->_vkViewport.x = 0.0;
		this->_vkViewport.y = 0.0;
		this->_vkViewport.width = (float)swapchainExtent.width;
		this->_vkViewport.height = (float)swapchainExtent.height;
		this->_vkViewport.minDepth = 0.0;
		this->_vkViewport.maxDepth = 1.0;
		
		this->_viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		this->_viewportState.viewportCount = 1;
		this->_viewportState.pViewports = &this->_vkViewport;
		this->_viewportState.scissorCount = 1;
		this->_viewportState.pScissors = &this->_scissor;
	}

	void GraphicsPipeline::initRasterizerPipelineState() {
		this->_rasterizerState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		this->_rasterizerState.depthClampEnable = VK_FALSE;
		this->_rasterizerState.rasterizerDiscardEnable = VK_FALSE;
		this->_rasterizerState.polygonMode = VK_POLYGON_MODE_FILL;
		this->_rasterizerState.lineWidth = 1;
		this->_rasterizerState.cullMode = VK_CULL_MODE_BACK_BIT;
		this->_rasterizerState.frontFace = VK_FRONT_FACE_CLOCKWISE;
		this->_rasterizerState.depthBiasEnable = VK_FALSE;
		/*this->_rasterizerState.depthBiasConstantFactor = 0.0f;
		this->_rasterizerState.depthBiasClamp = 0.0f;
		this->_rasterizerState.depthBiasSlopeFactor = 0.0f;*/
	}

	void GraphicsPipeline::initMultisampleState() {
		this->_multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		this->_multisampleState.sampleShadingEnable = VK_FALSE;
		this->_multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		this->_multisampleState.minSampleShading = 1.0f;
		this->_multisampleState.pSampleMask = nullptr;
		this->_multisampleState.alphaToCoverageEnable = VK_FALSE;
		this->_multisampleState.alphaToOneEnable = VK_FALSE;
	}

	void GraphicsPipeline::initColorBlendState() {
		VkPipelineColorBlendAttachmentState vkPipelineColorBlendAttachmentState{};
		this->_vkPipelineColorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		this->_vkPipelineColorBlendAttachmentState.blendEnable = VK_FALSE;
	/*	vkPipelineColorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		vkPipelineColorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		vkPipelineColorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
		vkPipelineColorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		vkPipelineColorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		vkPipelineColorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;*/

		
		this->_colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		this->_colorBlendState.logicOpEnable = VK_FALSE;
		this->_colorBlendState.logicOp = VK_LOGIC_OP_COPY;
		this->_colorBlendState.attachmentCount = 1;
		this->_colorBlendState.pAttachments = &this->_vkPipelineColorBlendAttachmentState;
		this->_colorBlendState.blendConstants[0] = 0.0f;
		this->_colorBlendState.blendConstants[1] = 0.0f;
		this->_colorBlendState.blendConstants[2] = 0.0f;
		this->_colorBlendState.blendConstants[3] = 0.0f;
	}

	void GraphicsPipeline::createGraphicsPipelineLayout() {
		/*VkDynamicState dynamicStates[] = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_LINE_WIDTH
		};

		VkPipelineDynamicStateCreateInfo vkPipelineDynamicStateCreateInfo{};
		vkPipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		vkPipelineDynamicStateCreateInfo.dynamicStateCount = 2;
		vkPipelineDynamicStateCreateInfo.pDynamicStates = dynamicStates;*/

		VkPipelineLayoutCreateInfo vkPipelineLayoutCreateInfo{};
		vkPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		vkPipelineLayoutCreateInfo.setLayoutCount = 0;
		vkPipelineLayoutCreateInfo.pSetLayouts = nullptr;
		vkPipelineLayoutCreateInfo.pushConstantRangeCount = 0;
		vkPipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

		if (vkCreatePipelineLayout(this->_vkDevice, &vkPipelineLayoutCreateInfo, nullptr, &this->_vkPipelineLayout) != VK_SUCCESS) {
#if DEBUG
			LOGGER_ERROR("Failed to create Graphics Pipeline");
#endif // DEBUG
			throw std::runtime_error("Failed to create Pipeline Layout");
		}

#if _DEBUG
		LOGGER_INFO("Successfully created Pipeline Layout");
#endif // _DEBUG
	}

	void GraphicsPipeline::createGraphicsPipeline() {
		VkGraphicsPipelineCreateInfo vkGraphicsPipelineCreateInfo{};
		vkGraphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		vkGraphicsPipelineCreateInfo.stageCount = 2;
		vkGraphicsPipelineCreateInfo.pStages = this->_shaderStages;
		vkGraphicsPipelineCreateInfo.pVertexInputState = &this->_vertexInputState;
		vkGraphicsPipelineCreateInfo.pInputAssemblyState = &this->_inputAssemblyState;
		vkGraphicsPipelineCreateInfo.pViewportState = &this->_viewportState;
		vkGraphicsPipelineCreateInfo.pRasterizationState = &this->_rasterizerState;
		vkGraphicsPipelineCreateInfo.pMultisampleState = &this->_multisampleState;
		vkGraphicsPipelineCreateInfo.pDepthStencilState = nullptr;
		vkGraphicsPipelineCreateInfo.pColorBlendState = &this->_colorBlendState;
		vkGraphicsPipelineCreateInfo.pDynamicState = nullptr;
		vkGraphicsPipelineCreateInfo.layout = this->_vkPipelineLayout;
		vkGraphicsPipelineCreateInfo.renderPass = this->_vkRenderPass;
		vkGraphicsPipelineCreateInfo.subpass = 0;
		vkGraphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
		vkGraphicsPipelineCreateInfo.basePipelineIndex = -1;

		if (vkCreateGraphicsPipelines(this->_vkDevice, VK_NULL_HANDLE, 1, &vkGraphicsPipelineCreateInfo, nullptr, &this->_vkPipeline) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create Graphics Pipeline");
		}

#if _DEBUG
		LOGGER_INFO("Successfully created Graphics Pipeline");
#endif
	}

	void GraphicsPipeline::clearShaderModules() {
		this->_fSource.clear();
		this->_vSource.clear();
		if (this->_vShaderModule != VK_NULL_HANDLE) {
			vkDestroyShaderModule(this->_vkDevice, this->_vShaderModule, nullptr);
			this->_vShaderModule = VK_NULL_HANDLE;
		}
		if (this->_fShaderModule != VK_NULL_HANDLE) {
			vkDestroyShaderModule(this->_vkDevice, this->_fShaderModule, nullptr);
			this->_fShaderModule = VK_NULL_HANDLE;
		}
	}

	void GraphicsPipeline::clearGraphicsPipeline() {
		vkDestroyPipeline(this->_vkDevice, this->_vkPipeline, nullptr);
	}

	void GraphicsPipeline::clearGraphicsPipelineLayout() {
		vkDestroyPipelineLayout(this->_vkDevice, this->_vkPipelineLayout, nullptr);
	}

	void GraphicsPipeline::clearRenderPass() {
		vkDestroyRenderPass(this->_vkDevice, this->_vkRenderPass, nullptr);
	}
	
	GraphicsPipeline::~GraphicsPipeline() {
		//this->clearShaderModules();
		this->clearGraphicsPipeline();
		this->clearGraphicsPipelineLayout();
		this->clearRenderPass();
	}
}