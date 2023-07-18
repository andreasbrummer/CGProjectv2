// This has been adapted from the Vulkan tutorial

#include "Starter.hpp"

// The uniform buffer objects data structures
// Remember to use the correct alignas(...) value
//        float : alignas(4)
//        vec2  : alignas(8)
//        vec3  : alignas(16)
//        vec4  : alignas(16)
//        mat3  : alignas(16)
//        mat4  : alignas(16)


struct MeshUniformBlock {
	alignas(4) float amb;
	alignas(4) float gamma;
	alignas(16) glm::vec3 sColor;
	alignas(16) glm::mat4 mvpMat;
	alignas(16) glm::mat4 mMat;
	alignas(16) glm::mat4 nMat;
};

struct UniformBlockSimple {
	alignas(16) glm::mat4 mvpMat;
};

struct OverlayUniformBlock {
	alignas(4) float visible;
};

struct GlobalUniformBlock {
	alignas(16) glm::vec3 DlightDir;
	alignas(16) glm::vec3 DlightColor;
	alignas(16) glm::vec3 AmbLightColor;
	alignas(16) glm::vec3 eyePos;
	alignas(16) glm::vec3 PLightPos;
	alignas(16) glm::vec4 PLightColor;
	alignas(16) glm::vec3 PLightPos2;
	alignas(16) glm::vec4 PLightColor2;
	alignas(16) glm::vec3 SLightPos;
	alignas(16) glm::vec3 SLightDir;
	alignas(16) glm::vec4 SLightColor;
};

// The vertices data structures
struct VertexMesh {
	glm::vec3 pos;
	glm::vec3 norm;
	glm::vec2 UV;
};

struct VertexOverlay {
	glm::vec2 pos;
	glm::vec2 UV;
};

struct VertexSimple {
	glm::vec3 pos;
	glm::vec2 UV;
};

/* A16 -- OK */
/* Add the C++ datastructure for the required vertex format */
struct VertexVColor {
	glm::vec3 pos;
	glm::vec3 norm;
	glm::vec3 color;
};

class A16;
void GameLogic(A16* A, float Ar, glm::mat4& View, glm::mat4& Prj, glm::mat4& World);


// MAIN ! 
class A16 : public BaseProject {
protected:

	// Current aspect ratio (used by the callback that resized the window
	float Ar;

	// Descriptor Layouts ["classes" of what will be passed to the shaders]
	DescriptorSetLayout DSLGubo, DSLMesh, DSLOverlay, DSLCabinet, DSLNeoGeoCabinet, DSLSimple;
	/* A16 -- OK */
	/* Add the variable that will contain the required Descriptor Set Layout */
	DescriptorSetLayout DSLVColor;

	// Vertex formats
	VertexDescriptor VMesh;
	VertexDescriptor VOverlay;
	VertexDescriptor VSimple;
	/* A16 -- OK */
	/* Add the variable that will contain the required Vertex format definition */
	VertexDescriptor VVColor;

	// Pipelines [Shader couples]
	Pipeline PMesh;
	Pipeline POverlay;
	Pipeline PSimple;
	/* A16 -- OK */
	/* Add the variable that will contain the new pipeline */
	Pipeline PVColor;
	// Models, textures and Descriptors (values assigned to the uniforms)
	// Please note that Model objects depends on the corresponding vertex structure
	Model<VertexMesh> MCabinet1, MRoom1, MCeiling, MFloor, MNeoGeoCabinet;
	/* A16 -- OK */
	/* Add the variable that will contain the model for the room */

	//Model<VertexVColor> MRoom;
	Model<VertexSimple> MPoolTable;
	Model<VertexMesh> MCeilingLamp1, MCeilingLamp2;
	Model<VertexOverlay> MKey;

	DescriptorSet DSGubo, DSCabinet, DSNeoGeoCabinet, DSCeilingLamp1, DSCeilingLamp2, DSPoolTable;
	/* A16 -- OK */
	/* Add the variable that will contain the Descriptor Set for the room */
	//DescriptorSet DSRoom
	DescriptorSet DSRoom1, DSCeiling, DSFloor;
	Texture T1, T2, T3, TRoom1, TCeiling, TFloor;
	Texture NeoGeoCabinetT1, NeoGeoCabinetT2;
	Texture TCeilingLamp1, TCeilingLamp2, TForniture;

	// C++ storage for uniform variables
	UniformBlockSimple uboPoolTable;
	MeshUniformBlock uboCabinet1, uboRoom1, uboCeiling, uboFloor, uboNeoGeoCabinet, uboCeilingLamp1, uboCeilingLamp2;
	/* A16 -- OK */
	/* Add the variable that will contain the Uniform Block in slot 0, set 1 of the room */
	//MeshUniformBlock uboRoom;

	GlobalUniformBlock gubo;

	// Other application parameters
	int currScene = 0;
	int gameState = 0;
	glm::mat4 View = glm::mat4(1);
	glm::mat4 Prj = glm::mat4(1);
	glm::mat4 World = glm::mat4(1);
	glm::vec3 Pos = glm::vec3(12, 0, -25);
	glm::vec3 cameraPos;
	float alpha;
	float beta;
	float RoomRot = 0.0;

	//PROVA
	// Jump parameters
	bool isJumping = false;         // Flag to indicate if the player is currently jumping
	float jumpVelocity = 0.0f;      // Initial jump velocity
	float jumpHeight = 2.0f;        // Height the player can reach during the jump
	float gravity = 9.8f;           // Acceleration due to gravity
	float maxJumpTime = 1.3f;       // Maximum duration of the jump
	float jumpTime = 0.0f;          // Current time elapsed during the jump


	// Here you set the main application parameters
	void setWindowParameters() {
		// window size, titile and initial background
		windowWidth = 800;
		windowHeight = 600;
		windowTitle = "A16";
		windowResizable = GLFW_TRUE;
		initialBackgroundColor = { 0.0f, 0.005f, 0.01f, 1.0f };

		// Descriptor pool sizes
		/* A16 -- OK */
		/* Update the requirements for the size of the pool */
		uniformBlocksInPool = 20;
		texturesInPool = 20;
		setsInPool = 20;

		Ar = (float)windowWidth / (float)windowHeight;
	}

	// What to do when the window changes size
	void onWindowResize(int w, int h) {
		Ar = (float)w / (float)h;
	}

	// Here you load and setup all your Vulkan Models and Texutures.
	// Here you also create your Descriptor set layouts and load the shaders for the pipelines
	void localInit() {
		// Descriptor Layouts [what will be passed to the shaders]
		DSLCabinet.init(this, {
								{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},
								{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT},
								{2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT},
								{3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
			});
		DSLNeoGeoCabinet.init(this, {
								{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},
								{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT},
								{2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
			});
		DSLMesh.init(this, {
			// this array contains the bindings:
			// first  element : the binding number
			// second element : the type of element (buffer or texture)
			//                  using the corresponding Vulkan constant
			// third  element : the pipeline stage where it will be used
			//                  using the corresponding Vulkan constant
			{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},
			{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
			});

		DSLOverlay.init(this, {
					{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},
					{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
			});
		/* A16 -- OK */
		/* Init the new Data Set Layout */
		DSLVColor.init(this, {
					{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS}
			});

		DSLGubo.init(this, {
					{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS}
			});

		DSLSimple.init(this, {
			{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},
			{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
			});

		// Vertex descriptors
		VMesh.init(this, {
			// this array contains the bindings
			// first  element : the binding number
			// second element : the stride of this binging
			// third  element : whether this parameter change per vertex or per instance
			//                  using the corresponding Vulkan constant
			{0, sizeof(VertexMesh), VK_VERTEX_INPUT_RATE_VERTEX}
			}, {
				// this array contains the location
				// first  element : the binding number
				// second element : the location number
				// third  element : the offset of this element in the memory record
				// fourth element : the data type of the element
				//                  using the corresponding Vulkan constant
				// fifth  elmenet : the size in byte of the element
				// sixth  element : a constant defining the element usage
				//                   POSITION - a vec3 with the position
				//                   NORMAL   - a vec3 with the normal vector
				//                   UV       - a vec2 with a UV coordinate
				//                   COLOR    - a vec4 with a RGBA color
				//                   TANGENT  - a vec4 with the tangent vector
				//                   OTHER    - anything else
				//
				// ***************** DOUBLE CHECK ********************
				//    That the Vertex data structure you use in the "offsetoff" and
				//	in the "sizeof" in the previous array, refers to the correct one,
				//	if you have more than one vertex format!
				// ***************************************************
				{0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexMesh, pos),
					   sizeof(glm::vec3), POSITION},
				{0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexMesh, norm),
					   sizeof(glm::vec3), NORMAL},
				{0, 2, VK_FORMAT_R32G32_SFLOAT, offsetof(VertexMesh, UV),
					   sizeof(glm::vec2), UV}
			});

		VOverlay.init(this, {
				  {0, sizeof(VertexOverlay), VK_VERTEX_INPUT_RATE_VERTEX}
			}, {
			  {0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(VertexOverlay, pos),
					 sizeof(glm::vec2), OTHER},
			  {0, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(VertexOverlay, UV),
					 sizeof(glm::vec2), UV}
			});
		/* A16 -- OK */
		/* Define the new Vertex Format */
		VVColor.init(this, {
				  {0, sizeof(VertexVColor), VK_VERTEX_INPUT_RATE_VERTEX}
			}, {
			  {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexVColor, pos),
					 sizeof(glm::vec3), POSITION},
			  {0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexVColor, norm),
					 sizeof(glm::vec3), NORMAL},
			  {0, 2, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexVColor, color),
					 sizeof(glm::vec3), COLOR},
			});
		
		//VOverlay MISSING!!!!!

		VSimple.init(this, {
				  {0, sizeof(VertexSimple), VK_VERTEX_INPUT_RATE_VERTEX}
			}, {
			  {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexSimple, pos),
					 sizeof(glm::vec3), POSITION},
			  {0, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(VertexSimple, UV),
					 sizeof(glm::vec2), UV}
			});


		// Pipelines [Shader couples]
		// The second parameter is the pointer to the vertex definition
		// Third and fourth parameters are respectively the vertex and fragment shaders
		// The last array, is a vector of pointer to the layouts of the sets that will
		// be used in this pipeline. The first element will be set 0, and so on..
		PMesh.init(this, &VMesh, "shaders/MeshVert.spv", "shaders/MeshFrag1.spv", { &DSLGubo,&DSLMesh,&DSLCabinet,&DSLNeoGeoCabinet });
		POverlay.init(this, &VOverlay, "shaders/OverlayVert.spv", "shaders/OverlayFrag.spv", { &DSLOverlay });
		POverlay.setAdvancedFeatures(VK_COMPARE_OP_LESS_OR_EQUAL, VK_POLYGON_MODE_FILL,
			VK_CULL_MODE_NONE, false);
		/* A16 -- OK */
		/* Create the new pipeline, using shaders "VColorVert.spv" and "VColorFrag.spv" */
		PVColor.init(this, &VVColor, "shaders/VColorVert.spv", "shaders/VColorFrag.spv", { &DSLGubo, &DSLVColor });

		PSimple.init(this, &VSimple, "shaders/shaderVertSimple.spv", "shaders/ShaderFragSimple.spv", { &DSLSimple });
		// Models, textures and Descriptors (values assigned to the uniforms)

		// Create models
		// The second parameter is the pointer to the vertex definition for this model
		// The third parameter is the file name
		// The last is a constant specifying the file type: currently only OBJ or GLTF
		MCabinet1.init(this, &VMesh, "Models/Cabinet.obj", OBJ);
		MNeoGeoCabinet.init(this, &VMesh, "Models/neoGeoCabinet.obj", OBJ);
		/* A16 -- OK*/
		/* load the mesh for the room, contained in OBJ file "Room.obj" */
		//MRoom.init(this, &VVColor, "Models/Room.obj", OBJ);
		MRoom1.init(this, &VMesh, "Models/RoomV5.obj", OBJ);

		MCeiling.init(this, &VMesh, "Models/CeilingV3.obj", OBJ);

		MFloor.init(this, &VMesh, "Models/Floor.obj", OBJ);

		MCeilingLamp1.init(this, &VMesh, "Models/untitled11.obj",OBJ);
		MCeilingLamp2.init(this, &VMesh, "Models/untitled11.obj", OBJ);

		MPoolTable.init(this, &VSimple, "Models/poolTable.mgcg", MGCG);


		// Creates a mesh with direct enumeration of vertices and indices

		// Create the textures
		// The second parameter is the file name
		T1.init(this, "textures/DefenderTextures/Material.001_baseColor.png");
		T2.init(this, "textures/DefenderTextures/Material.001_metallicRoughness.png");
		T3.init(this, "textures/DefenderTextures/Material.001_normal.png");
		TRoom1.init(this, "textures/RoomTextures/ArcadeWalls.jpg");
		TCeiling.init(this, "textures/RoomTextures/CeilingV3.png");
		TFloor.init(this, "textures/RoomTextures/Floor.jpg");
		NeoGeoCabinetT1.init(this, "textures/NeoGeoCabinet/DM.jpg");
		NeoGeoCabinetT2.init(this, "textures/NeoGeoCabinet/NM.jpg");
		TCeilingLamp1.init(this, "textures/white.png");
		TCeilingLamp2.init(this, "textures/white.png");
		TForniture.init(this, "textures/Textures_Forniture.png");
		// Init local variables
		alpha = glm::radians(180.0f);
		beta = 0.0f;
	}

	// Here you create your pipelines and Descriptor Sets!
	void pipelinesAndDescriptorSetsInit() {
		// This creates a new pipeline (with the current surface), using its shaders
		PMesh.create();
		POverlay.create();
		PSimple.create();
		/* A16 -- OK */
		/* Create the new pipeline */
		PVColor.create();
		// Here you define the data set
		DSCabinet.init(this, &DSLCabinet, {
			{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
			{1, TEXTURE, 0, &T1},
			{2, TEXTURE, 0, &T2},
			{3, TEXTURE, 0, &T3}

		});

		DSNeoGeoCabinet.init(this, &DSLNeoGeoCabinet, {
			{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
			{1, TEXTURE, 0, &NeoGeoCabinetT1},
			{2, TEXTURE, 0, &NeoGeoCabinetT2}
		});
		/* A16 -- OK */
		/* Define the data set for the room */
		/*
		DSRoom.init(this, &DSLVColor, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr}
			});
		*/

		DSRoom1.init(this, &DSLMesh, {
				{0,UNIFORM,sizeof(MeshUniformBlock), nullptr},
				{1, TEXTURE, 0, &TRoom1}
		});
		DSCeiling.init(this, &DSLMesh, {
			{0,UNIFORM,sizeof(MeshUniformBlock), nullptr},
			{1, TEXTURE, 0, &TCeiling}
		});
		DSFloor.init(this, &DSLMesh, {
			{0,UNIFORM,sizeof(MeshUniformBlock), nullptr},
			{1, TEXTURE, 0, &TFloor}
		});

		DSGubo.init(this, &DSLGubo, {
			{0, UNIFORM, sizeof(GlobalUniformBlock), nullptr}
		});

		DSCeilingLamp1.init(this, &DSLMesh, {
			{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
			{1, TEXTURE, 0, &TCeilingLamp1}
		});
		DSCeilingLamp2.init(this, &DSLMesh, {
			{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
			{1, TEXTURE, 0, &TCeilingLamp2}
		});

		DSPoolTable.init(this, &DSLSimple, {
			{0, UNIFORM, sizeof(UniformBlockSimple), nullptr},
			{1, TEXTURE, 0, &TForniture}
		});

	}

	// Here you destroy your pipelines and Descriptor Sets!
	// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
	void pipelinesAndDescriptorSetsCleanup() {
		// Cleanup pipelines
		PMesh.cleanup();
		POverlay.cleanup();
		/* A16 -- OK */
		/* cleanup the new pipeline */
		PVColor.cleanup();
		PSimple.cleanup();
		// Cleanup datasets
		DSCabinet.cleanup();
		DSNeoGeoCabinet.cleanup();
		/* A16 -- OK */
		/* cleanup the dataset for the room */
		DSRoom1.cleanup();
		//DSRoom.cleanup();
		DSCeiling.cleanup();
		DSFloor.cleanup();
		DSGubo.cleanup();

		DSCeilingLamp1.cleanup();
		DSCeilingLamp2.cleanup();
		DSPoolTable.cleanup();
	}

	// Here you destroy all the Models, Texture and Desc. Set Layouts you created!
	// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
	// You also have to destroy the pipelines: since they need to be rebuilt, they have two different
	// methods: .cleanup() recreates them, while .destroy() delete them completely
	void localCleanup() {
		// Cleanup textures
		T1.cleanup();
		T2.cleanup();
		T3.cleanup();
		NeoGeoCabinetT1.cleanup();
		NeoGeoCabinetT2.cleanup();
		TRoom1.cleanup();
		TCeiling.cleanup();
		TFloor.cleanup();
		TCeilingLamp1.cleanup();
		TCeilingLamp2.cleanup();
		TForniture.cleanup();

		// Cleanup models
		MCabinet1.cleanup();
		MNeoGeoCabinet.cleanup();
		/* A16 -- OK */
		/* Cleanup the mesh for the room */
		//MRoom.cleanup();
		MRoom1.cleanup();
		MCeiling.cleanup();
		MFloor.cleanup();
		MCeilingLamp1.cleanup();
		MCeilingLamp2.cleanup();
		MPoolTable.cleanup();
		// Cleanup descriptor set layouts
		DSLMesh.cleanup();
		DSLOverlay.cleanup();
		/* A16 -- OK */
		/* Cleanup the new Descriptor Set Layout */
		DSLVColor.cleanup();
		DSLGubo.cleanup();

		DSLCabinet.cleanup();
		DSLNeoGeoCabinet.cleanup();

		DSLSimple.cleanup();
		// Destroies the pipelines
		PMesh.destroy();
		POverlay.destroy();
		/* A16 -- OK */
		/* Destroy the new pipeline */
		PVColor.destroy();
		PSimple.destroy();
	}

	// Here it is the creation of the command buffer:
	// You send to the GPU all the objects you want to draw,
	// with their buffers and textures

	void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) {
		// sets global uniforms (see below from parameters explanation)
		DSGubo.bind(commandBuffer, PMesh, 0, currentImage);

		// binds the pipeline
		PMesh.bind(commandBuffer);
		// For a pipeline object, this command binds the corresponing pipeline to the command buffer passed in its parameter

		// binds the model
		// For a Model object, this command binds the corresponing index and vertex buffer
		// to the command buffer passed in its parameter

		// binds the data set
		// For a Dataset object, this command binds the corresponing dataset
		// to the command buffer and pipeline passed in its first and second parameters.
		// The third parameter is the number of the set being bound
		// As described in the Vulkan tutorial, a different dataset is required for each image in the swap chain.
		// This is done automatically in file Starter.hpp, however the command here needs also the index
		// of the current image in the swap chain, passed in its last parameter

		// record the drawing command in the command bufferun
		// the second parameter is the number of indexes to be drawn. For a Model object,
		// this can be retrieved with the .indices.size() method.

		MCabinet1.bind(commandBuffer);
		DSCabinet.bind(commandBuffer, PMesh, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MCabinet1.indices.size()), 1, 0, 0, 0);

		MNeoGeoCabinet.bind(commandBuffer);
		DSNeoGeoCabinet.bind(commandBuffer, PMesh, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MNeoGeoCabinet.indices.size()), 1, 0, 0, 0);

		MRoom1.bind(commandBuffer);
		// DSGubo.bind(commandBuffer, PMesh, 0, currentImage);
		DSRoom1.bind(commandBuffer, PMesh, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MRoom1.indices.size()), 1, 0, 0, 0);

		MCeiling.bind(commandBuffer);
		// DSGubo.bind(commandBuffer, PMesh, 0, currentImage);
		DSCeiling.bind(commandBuffer, PMesh, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MCeiling.indices.size()), 1, 0, 0, 0);

		MFloor.bind(commandBuffer);
		// DSGubo.bind(commandBuffer, PMesh, 0, currentImage);
		DSFloor.bind(commandBuffer, PMesh, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MFloor.indices.size()), 1, 0, 0, 0);
		/* A16 -- OK */
		/* Insert the commands to draw the room */
		//PVColor.bind(commandBuffer);
		//MRoom.bind(commandBuffer);
		//DSGubo.bind(commandBuffer, PVColor, 0, currentImage);
		//vkCmdDrawIndexed(commandBuffer,
		//		static_cast<uint32_t>(MRoom.indices.size()), 1, 0, 0, 0);
		MCeilingLamp1.bind(commandBuffer);
		DSCeilingLamp1.bind(commandBuffer, PMesh, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MCeilingLamp1.indices.size()), 1, 0, 0, 0);
		MCeilingLamp2.bind(commandBuffer);
		DSCeilingLamp2.bind(commandBuffer, PMesh, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MCeilingLamp2.indices.size()), 1, 0, 0, 0);

		PVColor.bind(commandBuffer);


		POverlay.bind(commandBuffer);

		PSimple.bind(commandBuffer);
		DSPoolTable.bind(commandBuffer, PSimple, 0, currentImage);
		MPoolTable.bind(commandBuffer);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MPoolTable.indices.size()), 1, 0, 0, 0);

	}

	void GameLogic() {
		// Parameters
		// Camera FOV-y, Near Plane and Far Plane
		const float FOVy = glm::radians(45.0f);
		const float nearPlane = 0.1f;
		const float farPlane = 100.f;
		// Camera target height and distance
		const float camHeight = 2.1f;
		// Camera Pitch limits
		const float minPitch = glm::radians(-60.0f);
		const float maxPitch = glm::radians(60.0f);
		// Rotation and motion speed
		const float ROT_SPEED = glm::radians(120.0f);
		float MOVE_SPEED = 2.0f * 2.0f;



		// Integration with the timers and the controllers
		float deltaT;
		glm::vec3 m = glm::vec3(0.0f), r = glm::vec3(0.0f);
		bool fire = false;
		getSixAxis(deltaT, m, r, fire);
		static bool wasFire = false;
		bool handleFire = (wasFire && (!fire));
		wasFire = fire;

		// Game Logic implementation
		// Current Player Position - static variable make sure its value remain unchanged in subsequent calls to the procedure

		// World
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) {
			MOVE_SPEED = 8.0f;
		}
		else {
			MOVE_SPEED = 2.0f;
		}

		// Jumping
		if (glfwGetKey(window, GLFW_KEY_SPACE) && !isJumping) {
			// Start the jump
			isJumping = true;
			jumpVelocity = sqrt(2.0f * gravity * jumpHeight);  // Calculate the initial velocity based on the desired jump height
			jumpTime = 0.0f;
		}

		// Check if the player is currently jumping
		if (isJumping) {
			// Update the jump time
			jumpTime += deltaT;

			// Apply gravity to the jump velocity
			jumpVelocity -= gravity * deltaT;

			// Calculate the new position based on the jump velocity
			glm::vec3 jumpOffset = glm::vec3(0, jumpVelocity * deltaT, 0);
			Pos += jumpOffset;

			// Check if the maximum jump time is reached or if the player has landed
			if (jumpTime >= maxJumpTime || Pos.y <= 0.0f) {
				// End the jump
				isJumping = false;
				jumpVelocity = 0.0f;
				jumpTime = 0.0f;

				// Ensure the player is on the ground level
				Pos.y = 0.0f;
			}
		}


		// Rotation

		alpha = alpha - ROT_SPEED * deltaT * r.y;
		beta = beta - ROT_SPEED * deltaT * r.x;
		beta = beta < glm::radians(-90.0f) ? glm::radians(-90.0f) :
			(beta > glm::radians(90.0f) ? glm::radians(90.0f) : beta);



		// Position
		glm::vec3 ux = glm::rotate(glm::mat4(1.0f), alpha, glm::vec3(0, 1, 0)) * glm::vec4(1, 0, 0, 1);
		glm::vec3 uz = glm::rotate(glm::mat4(1.0f), alpha, glm::vec3(0, 1, 0)) * glm::vec4(0, 0, -1, 1);
		Pos = Pos + MOVE_SPEED * m.x * ux * deltaT;
		Pos = Pos + MOVE_SPEED * m.y * glm::vec3(0, 1, 0) * deltaT;
		//Pos.y = 0.0f; //DEBUG
		Pos = Pos + MOVE_SPEED * m.z * uz * deltaT;
		cameraPos = Pos + glm::vec3(0, camHeight, 0);

		// Projection
		Prj = glm::perspective(FOVy, Ar, nearPlane, farPlane);
		Prj[1][1] *= -1;

		View =  //glm::rotate(glm::mat4(1.0),-rho,glm::vec3(0,0,1)) *
			glm::rotate(glm::mat4(1.0), -beta, glm::vec3(1, 0, 0)) *
			glm::rotate(glm::mat4(1.0), -alpha, glm::vec3(0, 1, 0)) *
			glm::translate(glm::mat4(1.0), -cameraPos);

	}
	// Here is where you update the uniforms.
	// Very likely this will be where you will be writing the logic of your application.
	void updateUniformBuffer(uint32_t currentImage) {
		// Standard procedure to quit when the ESC key is pressed
		if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		GameLogic();

		gubo.DlightDir = glm::normalize(glm::vec3(1, 2, 3));
		gubo.DlightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		gubo.AmbLightColor = glm::vec3(0.05f); //0.05f
		gubo.eyePos = Pos;
		gubo.PLightPos = glm::vec3(3.0f, 3.9f, -4.0f);
		gubo.PLightColor = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
		//gubo.PLightColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		gubo.PLightPos2 = glm::vec3(11.0f, 3.9f, -4.0f);
		//gubo.PLightColor2 = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		gubo.PLightColor2 = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
		gubo.SLightDir = glm::vec3(0.0f, 1.0f, 0.0f);
		gubo.SLightColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		gubo.SLightPos = glm::vec3(11.0f, 1.0f, 1.0f);

		// Writes value to the GPU
		DSGubo.map(currentImage, &gubo, sizeof(gubo), 0);
		// the .map() method of a DataSet object, requires the current image of the swap chain as first parameter
		// the second parameter is the pointer to the C++ data structure to transfer to the GPU
		// the third parameter is its size
		// the fourth parameter is the location inside the descriptor set of this uniform block

		World = glm::translate(glm::mat4(1.0), glm::vec3(0.0, 0.0f, -1.2f)) * glm::scale(glm::mat4(1), glm::vec3(0.018f));

		uboCabinet1.amb = 1.0f; uboCabinet1.gamma = 180.0f; uboCabinet1.sColor = glm::vec3(1.0f);
		uboCabinet1.mvpMat = Prj * View * World;
		uboCabinet1.mMat = World;
		uboCabinet1.nMat = glm::inverse(glm::transpose(World));
		DSCabinet.map(currentImage, &uboCabinet1, sizeof(uboCabinet1), 0);

		World = glm::rotate(glm::mat4(1.0), glm::radians(90.0f), glm::vec3(0, 1, 0)) *
			glm::translate(glm::mat4(1.0), glm::vec3(5.0, 0.0f, 0.6f)) * glm::scale(glm::mat4(1), glm::vec3(0.0065f));

		uboNeoGeoCabinet.amb = 1.0f; uboNeoGeoCabinet.gamma = 180.0f; uboNeoGeoCabinet.sColor = glm::vec3(1.0f);
		uboNeoGeoCabinet.mvpMat = Prj * View * World;
		uboNeoGeoCabinet.mMat = World;
		uboNeoGeoCabinet.nMat = glm::inverse(glm::transpose(World));
		DSNeoGeoCabinet.map(currentImage, &uboNeoGeoCabinet, sizeof(uboNeoGeoCabinet), 0);

		/* A16 -- OK*/
		/* fill the uniform block for the room. Identical to the one of the body of the slot machine
		World = glm::scale(glm::mat4(1),glm::vec3(1,2.5f,1));
		uboRoom.amb = 1.0f; uboRoom.gamma = 180.0f; uboRoom.sColor = glm::vec3(1.0f);
		uboRoom.mvpMat = Prj * View * World;
		uboRoom.mMat = World;
		uboRoom.nMat = glm::inverse(glm::transpose(World));
		/* map the uniform data block to the GPU */
		//DSRoom.map(currentImage, &uboRoom, sizeof(uboRoom), 0);


		World = glm::mat4(1);
		uboRoom1.amb = 1.0f; uboRoom1.gamma = 180.0f; uboRoom1.sColor = glm::vec3(1.0f);
		uboRoom1.mvpMat = Prj * View * World;
		uboRoom1.mMat = World;
		uboRoom1.nMat = glm::inverse(glm::transpose(World));
		DSRoom1.map(currentImage, &uboRoom1, sizeof(uboRoom1), 0);

		World = glm::mat4(1);
		uboCeiling.amb = 1.0f; uboCeiling.gamma = 180.0f; uboCeiling.sColor = glm::vec3(1.0f);
		uboCeiling.mvpMat = Prj * View * World;
		uboCeiling.mMat = World;
		uboCeiling.nMat = glm::inverse(glm::transpose(World));
		DSCeiling.map(currentImage, &uboCeiling, sizeof(uboCeiling), 0);

		World = glm::mat4(1);
		uboFloor.amb = 1.0f; uboFloor.gamma = 180.0f; uboFloor.sColor = glm::vec3(1.0f);
		uboFloor.mvpMat = Prj * View * World;
		uboFloor.mMat = World;
		uboFloor.nMat = glm::inverse(glm::transpose(World));
		DSFloor.map(currentImage, &uboFloor, sizeof(uboFloor), 0);


		World = translate(glm::mat4(1.0), glm::vec3(3.0f, 3.97f, -4.0f)) * glm::rotate(glm::mat4(1.0), glm::radians(180.0f), glm::vec3(0, 0, 1)) * 
			glm::scale(glm::mat4(1), glm::vec3(0.15f, 0.3f, 0.15f));
		uboCeilingLamp1.amb = 1.0f; uboCeilingLamp1.gamma = 180.0f; uboCeilingLamp1.sColor = glm::vec3(1.0f);
		uboCeilingLamp1.mvpMat = Prj * View * World;
		uboCeilingLamp1.mMat = World;
		uboCeilingLamp1.nMat = glm::inverse(glm::transpose(World));
		DSCeilingLamp1.map(currentImage, &uboCeilingLamp1, sizeof(uboCeilingLamp1), 0);

		World = translate(glm::mat4(1.0), glm::vec3(11.0f, 3.97f, -4.0f)) * glm::rotate(glm::mat4(1.0), glm::radians(180.0f), glm::vec3(0, 0, 1)) *
			glm::scale(glm::mat4(1), glm::vec3(0.15f, 0.3f, 0.15f));
		uboCeilingLamp2.amb = 1.0f; uboCeilingLamp2.gamma = 180.0f; uboCeilingLamp2.sColor = glm::vec3(1.0f);
		uboCeilingLamp2.mvpMat = Prj * View * World;
		uboCeilingLamp2.mMat = World;
		uboCeilingLamp2.nMat = glm::inverse(glm::transpose(World));
		DSCeilingLamp2.map(currentImage, &uboCeilingLamp2, sizeof(uboCeilingLamp2), 0);

		World = rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 1, 0)) * translate(glm::mat4(1.0), glm::vec3(-1.0f, 0.0f, 10.0f)) *
			glm::scale(glm::mat4(1), glm::vec3(2.0f));
		uboPoolTable.mvpMat = Prj * View * World;
		DSPoolTable.map(currentImage, &uboPoolTable, sizeof(uboPoolTable), 0);

	}
};



// This is the main: probably you do not need to touch this!
int main() {
	A16 app;

	try {
		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
