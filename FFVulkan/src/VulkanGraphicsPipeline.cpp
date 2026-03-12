#include "FFVulkan/VulkanGraphicsPipeline.h"

#include "VulkanUtils.h"
#include "FFCore/Core/Types.h"

using namespace FFVk;

GraphicsPipeline::GraphicsPipeline(VkDevice device, GLFWwindow* window, VkRenderPass renderPass, VkShaderModule vs,
    VkShaderModule fs)
{
    _device = device;

	VkPipelineShaderStageCreateInfo shaderStageCreateInfo[2] = 
	{
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.stage = VK_SHADER_STAGE_VERTEX_BIT,
			.module = vs,
			.pName = "main",
		},
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
			.module = fs,
			.pName = "main"
		}
	};

	VkPipelineVertexInputStateCreateInfo vertexInputInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO
	};

	VkPipelineInputAssemblyStateCreateInfo pipelineIACreateInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		.primitiveRestartEnable = VK_FALSE
	};

	i32 WindowWidth, WindowHeight;
	glfwGetWindowSize(window, &WindowWidth, &WindowHeight);

	VkViewport VP = 
	{
		.x = 0.0f,
		.y = 0.0f,
		.width = (float)WindowWidth,
		.height = (float)WindowHeight,
		.minDepth = 0.0f,
		.maxDepth = 1.0f
	};

	VkRect2D scissor
	{
		.offset = 
	{
			.x = 0,
			.y = 0,
		},
		.extent = 
	{
			.width = (u32)WindowWidth,
			.height = (u32)WindowHeight
		}
	};

	VkPipelineViewportStateCreateInfo VPCreateInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		.viewportCount = 1,
		.pViewports = &VP,
		.scissorCount = 1,
		.pScissors = &scissor
	};

	VkPipelineRasterizationStateCreateInfo rastCreateInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		.polygonMode = VK_POLYGON_MODE_FILL,
		.cullMode = VK_CULL_MODE_NONE,
		.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
		.lineWidth = 1.0f
	};

	VkPipelineMultisampleStateCreateInfo pipelineMSCreateInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
		.sampleShadingEnable = VK_FALSE,
		.minSampleShading = 1.0f
	};

	VkPipelineColorBlendAttachmentState blendAttachState = 
	{
		.blendEnable = VK_FALSE,
		.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
	};

	VkPipelineColorBlendStateCreateInfo blendCreateInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		.logicOpEnable = VK_FALSE,
		.logicOp = VK_LOGIC_OP_COPY,
		.attachmentCount = 1,
		.pAttachments = &blendAttachState
	};

	VkPipelineLayoutCreateInfo layoutInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.setLayoutCount = 0,
		.pSetLayouts = nullptr
	};

	VK_CALL_AND_CHECK
	(
		vkCreatePipelineLayout,
		"Failed to create pipeline layout",
		_device, &layoutInfo, nullptr, &_pipelineLayout
	)
	
	VkGraphicsPipelineCreateInfo pipelineInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.stageCount = std::size(shaderStageCreateInfo),
		.pStages = &shaderStageCreateInfo[0],
		.pVertexInputState = &vertexInputInfo,
		.pInputAssemblyState = &pipelineIACreateInfo,
		.pViewportState = &VPCreateInfo,
		.pRasterizationState = &rastCreateInfo,
		.pMultisampleState = &pipelineMSCreateInfo,
		.pColorBlendState = &blendCreateInfo,
		.layout = _pipelineLayout,
		.renderPass = renderPass,
		.subpass = 0,
		.basePipelineHandle = VK_NULL_HANDLE,
		.basePipelineIndex = -1
	};

	VK_CALL_AND_CHECK
	(
		vkCreateGraphicsPipelines,
		"Failed to create graphics pipeline",
		_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &_pipeline
	)
}

GraphicsPipeline::~GraphicsPipeline()
{
	vkDestroyPipelineLayout(_device, _pipelineLayout, nullptr);
	vkDestroyPipeline(_device, _pipeline, nullptr);
}

void GraphicsPipeline::Bind(VkCommandBuffer commandBuffer)
{
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline);
}
