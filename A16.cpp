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
	alignas(16) glm::vec3 PLightPosPool;
	alignas(16) glm::vec4 PLightColorPool;
	alignas(16) glm::vec3 SLightPos;
	alignas(16) glm::vec3 SLightDir;
	alignas(16) glm::vec4 SLightColor;
};

struct SkyboxUniformBufferObject {
	alignas(16) glm::mat4 mvpMat;
	alignas(16) glm::mat4 mMat;
	alignas(16) glm::mat4 nMat;
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
void GameLogic(A16* A, float Ar, glm::mat4& View, glm::mat4& Prj, glm::mat4& World,glm::mat3 &CamDir);


// MAIN ! 
class A16 : public BaseProject {
protected:

	// Current aspect ratio (used by the callback that resized the window
	float Ar;

	// Descriptor Layouts ["classes" of what will be passed to the shaders]
	DescriptorSetLayout DSLGubo, DSLMesh, DSLOverlay, DSLSimple,DSLskyBox;
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
	Pipeline PskyBox;
	/* A16 -- OK */
	/* Add the variable that will contain the new pipeline */
	Pipeline PVColor;
	// Models, textures and Descriptors (values assigned to the uniforms)
	// Please note that Model objects depends on the corresponding vertex structure
	Model<VertexMesh> MCabinet1,MCabinet2,MAsteroids, MRoom, MDecoration, MCeiling, MFloor;
	/* A16 -- OK */
	/* Add the variable that will contain the model for the room */

	//Model<VertexVColor> MRoom;
	Model<VertexSimple> MPoolTable,MSnackMachine;
	Model<VertexMesh> MCeilingLamp1, MCeilingLamp2, MPoolLamp;
	Model<VertexOverlay> MKey;
	Model<VertexSimple> MskyBox;

	DescriptorSet DSGubo, DSCabinet, DSCabinet2, DSAsteroids, DSCeilingLamp1, DSCeilingLamp2, DSPoolLamp, DSPoolTable, DSSnackMachine;
	/* A16 -- OK */
	/* Add the variable that will contain the Descriptor Set for the room */
	//DescriptorSet DSRoom
	DescriptorSet DSRoom, DSDecoration, DSCeiling, DSFloor, DSskyBox;
	Texture TCabinet, TRoom, TDecoration, TCeiling, TFloor,TAsteroids;
	Texture TCeilingLamp1, TCeilingLamp2, TPoolLamp, TForniture, TskyBox;

	// C++ storage for uniform variables
	UniformBlockSimple uboPoolTable,uboSnackMachine;
	MeshUniformBlock uboCabinet1,uboCabinet2, uboAsteroids, uboRoom, uboDecoration, uboCeiling, uboFloor, uboCeilingLamp1, uboCeilingLamp2, uboPoolLamp;
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
	glm::mat3 CamDir = glm::mat3(1.0f);

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
		uniformBlocksInPool = 100;
		texturesInPool = 100;
		setsInPool = 100;

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

		DSLskyBox.init(this, {
			{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT},
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
		PMesh.init(this, &VMesh, "shaders/MeshVert.spv", "shaders/MeshFrag1.spv", { &DSLGubo,&DSLMesh});
		POverlay.init(this, &VOverlay, "shaders/OverlayVert.spv", "shaders/OverlayFrag.spv", { &DSLOverlay });
		POverlay.setAdvancedFeatures(VK_COMPARE_OP_LESS_OR_EQUAL, VK_POLYGON_MODE_FILL,
			VK_CULL_MODE_NONE, false);
		/* A16 -- OK */
		/* Create the new pipeline, using shaders "VColorVert.spv" and "VColorFrag.spv" */
		PVColor.init(this, &VVColor, "shaders/VColorVert.spv", "shaders/VColorFrag.spv", { &DSLGubo, &DSLVColor });

		PSimple.init(this, &VSimple, "shaders/ShaderVertSimple.spv", "shaders/ShaderFragSimple.spv", { &DSLSimple });
		PskyBox.init(this,&VSimple ,"shaders/SkyBoxVert.spv", "shaders/SkyBoxFrag.spv", { &DSLskyBox });
		PskyBox.setAdvancedFeatures(VK_COMPARE_OP_LESS_OR_EQUAL, VK_POLYGON_MODE_FILL,
			VK_CULL_MODE_BACK_BIT, false);


		// Models, textures and Descriptors (values assigned to the uniforms)

		// Create models
		// The second parameter is the pointer to the vertex definition for this model
		// The third parameter is the file name
		// The last is a constant specifying the file type: currently only OBJ or GLTF
		MCabinet1.init(this, &VMesh, "Models/Cabinet.obj", OBJ);
        MCabinet2.init(this,&VMesh,"Models/Cabinet.obj",OBJ );
        MAsteroids.init(this,&VMesh,"Models/Asteroids.obj",OBJ);
		/* A16 -- OK*/
		/* load the mesh for the room, contained in OBJ file "Room.obj" */
		//MRoom.init(this, &VVColor, "Models/Room.obj", OBJ);
		MRoom.init(this, &VMesh, "Models/RoomV5.obj", OBJ);
		MDecoration.init(this, &VMesh, "Models/Decoration.obj", OBJ);

		MCeiling.init(this, &VMesh, "Models/CeilingV3.obj", OBJ);

		MFloor.init(this, &VMesh, "Models/Floor.obj", OBJ);

		MCeilingLamp1.init(this, &VMesh, "Models/LampReverseN.obj",OBJ);
		MCeilingLamp2.init(this, &VMesh, "Models/LampReverseN.obj", OBJ);
		MPoolLamp.init(this, &VMesh, "Models/LampPoolFinal.obj", OBJ);

		MPoolTable.init(this, &VSimple, "Models/poolTable.mgcg", MGCG);
		MSnackMachine.init(this, &VSimple, "Models/SnackMachine.mgcg", MGCG);
		MskyBox.init(this,&VSimple, "Models/SkyBoxCube.obj",OBJ);

		// Creates a mesh with direct enumeration of vertices and indices

		// Create the textures
		// The second parameter is the file name
		TCabinet.init(this, "textures/DefenderTextures/Material.001_baseColor.png");
		TRoom.init(this, "textures/RoomTextures/ArcadeWalls.jpg");
		TDecoration.init(this, "textures/RoomTextures/Decoration.jpg");
		TCeiling.init(this, "textures/RoomTextures/CeilingV3.png");
		TFloor.init(this, "textures/RoomTextures/Floor.jpg");
		TCeilingLamp1.init(this, "textures/white.png");
		TCeilingLamp2.init(this, "textures/white.png");
		TPoolLamp.init(this, "textures/DarkGrey.png");
		TForniture.init(this, "textures/Textures_Forniture.png");
        TAsteroids.init(this,"textures/AsteroidsTextures/Material.001_baseColor.png");
   
		const char* T2fn[] = { "textures/sky/posx.jpg", "textures/sky/negx.jpg",
							  "textures/sky/posy.jpg",   "textures/sky/negy.jpg",
							  "textures/sky/posz.jpg", "textures/sky/negz.jpg" };
		TskyBox.initCubic(this, T2fn);
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
		PVColor.create();
		PskyBox.create();
		// Here you define the data set
		DSCabinet.init(this, &DSLSimple, {
			{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
			{1, TEXTURE, 0, &TCabinet}
		});
        DSCabinet2.init(this, &DSLSimple, {
                {0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
                {1, TEXTURE, 0, &TCabinet}
        });
        DSAsteroids.init(this, &DSLSimple, {
                {0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
                {1, TEXTURE, 0, &TAsteroids}
        });
		
		DSRoom.init(this, &DSLSimple, {
				{0,UNIFORM,sizeof(MeshUniformBlock), nullptr},
				{1, TEXTURE, 0, &TRoom},
		});

		DSDecoration.init(this, &DSLSimple, {
				{0,UNIFORM,sizeof(MeshUniformBlock), nullptr},
				{1, TEXTURE, 0, &TDecoration},
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

		DSPoolLamp.init(this, &DSLMesh, {
			{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
			{1, TEXTURE, 0, &TPoolLamp}
			});

		DSPoolTable.init(this, &DSLSimple, {
			{0, UNIFORM, sizeof(UniformBlockSimple), nullptr},
			{1, TEXTURE, 0, &TForniture}
		});

		DSSnackMachine.init(this, &DSLSimple, {
			{0, UNIFORM, sizeof(UniformBlockSimple), nullptr},
			{1, TEXTURE, 0, &TForniture}
		});

		DSskyBox.init(this, &DSLskyBox, {
					{0, UNIFORM, sizeof(SkyboxUniformBufferObject), nullptr},
					{1, TEXTURE, 0, &TskyBox}
			});

	}

	// Here you destroy your pipelines and Descriptor Sets!
	// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
	void pipelinesAndDescriptorSetsCleanup() {
		// Cleanup pipelines
		PMesh.cleanup();
		POverlay.cleanup();
		PVColor.cleanup();
		PSimple.cleanup();
		PskyBox.cleanup();
		// Cleanup datasets
		DSCabinet.cleanup();
        DSCabinet2.cleanup();
        DSAsteroids.cleanup();
		DSRoom.cleanup();
		DSDecoration.cleanup();
		DSCeiling.cleanup();
		DSFloor.cleanup();
		DSGubo.cleanup();


		DSCeilingLamp1.cleanup();
		DSCeilingLamp2.cleanup();
		DSPoolLamp.cleanup();
		DSPoolTable.cleanup();
		DSskyBox.cleanup();
	}

	// Here you destroy all the Models, Texture and Desc. Set Layouts you created!
	// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
	// You also have to destroy the pipelines: since they need to be rebuilt, they have two different
	// methods: .cleanup() recreates them, while .destroy() delete them completely
	void localCleanup() {
		// Cleanup textures
		TCabinet.cleanup();
		TRoom.cleanup();
		TDecoration.cleanup();
		TCeiling.cleanup();
		TFloor.cleanup();
		TCeilingLamp1.cleanup();
		TCeilingLamp2.cleanup();
		TPoolLamp.cleanup();
		TForniture.cleanup();
        TAsteroids.cleanup();
		TskyBox.cleanup();

		// Cleanup models
		MCabinet1.cleanup();
        MCabinet2.cleanup();
        MAsteroids.cleanup();
		MskyBox.cleanup();
		/* A16 -- OK */
		/* Cleanup the mesh for the room */
		//MRoom.cleanup();
		MRoom.cleanup();
		MDecoration.cleanup();
		MCeiling.cleanup();
		MFloor.cleanup();
		MCeilingLamp1.cleanup();
		MCeilingLamp2.cleanup();
		MPoolLamp.cleanup();
		MPoolTable.cleanup();
		// Cleanup descriptor set layouts
		DSLMesh.cleanup();
		DSLOverlay.cleanup();
		/* A16 -- OK */
		/* Cleanup the new Descriptor Set Layout */
		DSLVColor.cleanup();
		DSLGubo.cleanup();


		DSLSimple.cleanup();
		DSLskyBox.cleanup();
		// Destroies the pipelines
		PMesh.destroy();
		POverlay.destroy();
		/* A16 -- OK */
		/* Destroy the new pipeline */
		PVColor.destroy();
		PSimple.destroy();
		PskyBox.destroy();
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
        MCabinet2.bind(commandBuffer);
        DSCabinet2.bind(commandBuffer, PMesh, 1, currentImage);
        vkCmdDrawIndexed(commandBuffer,
                         static_cast<uint32_t>(MCabinet2.indices.size()), 1, 0, 0, 0);
        MAsteroids.bind(commandBuffer);
        DSAsteroids.bind(commandBuffer, PMesh, 1, currentImage);
        vkCmdDrawIndexed(commandBuffer,
                         static_cast<uint32_t>(MAsteroids.indices.size()), 1, 0, 0, 0);


		MRoom.bind(commandBuffer);
		// DSGubo.bind(commandBuffer, PMesh, 0, currentImage);
		DSRoom.bind(commandBuffer, PMesh, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MRoom.indices.size()), 1, 0, 0, 0);

		MDecoration.bind(commandBuffer);
		// DSGubo.bind(commandBuffer, PMesh, 0, currentImage);
		DSDecoration.bind(commandBuffer, PMesh, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MDecoration.indices.size()), 1, 0, 0, 0);

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

		MCeilingLamp1.bind(commandBuffer);
		DSCeilingLamp1.bind(commandBuffer, PMesh, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MCeilingLamp1.indices.size()), 1, 0, 0, 0);

		MCeilingLamp2.bind(commandBuffer);
		DSCeilingLamp2.bind(commandBuffer, PMesh, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MCeilingLamp2.indices.size()), 1, 0, 0, 0);

		MPoolLamp.bind(commandBuffer);
		DSPoolLamp.bind(commandBuffer, PMesh, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MPoolLamp.indices.size()), 1, 0, 0, 0);

		PVColor.bind(commandBuffer);


		POverlay.bind(commandBuffer);

		PSimple.bind(commandBuffer);
		DSPoolTable.bind(commandBuffer, PSimple, 0, currentImage);
		MPoolTable.bind(commandBuffer);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MPoolTable.indices.size()), 1, 0, 0, 0);

		DSSnackMachine.bind(commandBuffer, PSimple, 0, currentImage);
		MSnackMachine.bind(commandBuffer);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MSnackMachine.indices.size()), 1, 0, 0, 0);

		PskyBox.bind(commandBuffer);
		MskyBox.bind(commandBuffer);
		DSskyBox.bind(commandBuffer, PskyBox,0,currentImage);

		// property .indices.size() of models, contains the number of triangles * 3 of the mesh.
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MskyBox.indices.size()), 1, 0, 0, 0);

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
		/*PROVA*/
		const float MOUSE_RES = 10.0f;
		static double old_xpos = 0, old_ypos = 0;
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		double m_dx = xpos - old_xpos;
		double m_dy = ypos - old_ypos;
		old_xpos = xpos; old_ypos = ypos;
		/*FINEPROVA*/

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

		glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
			CamDir = glm::mat3(glm::rotate(glm::mat4(1.0f),
				deltaT * (float)m_dx * ROT_SPEED / MOUSE_RES,
				glm::vec3(CamDir[1])) * glm::mat4(CamDir));
			CamDir = glm::mat3(glm::rotate(glm::mat4(1.0f),
				deltaT * (float)m_dy * ROT_SPEED / MOUSE_RES,
				glm::vec3(CamDir[0])) * glm::mat4(CamDir));
		}

		if (glfwGetKey(window, GLFW_KEY_LEFT)) {
			CamDir = glm::mat3(glm::rotate(glm::mat4(1.0f),
				deltaT * ROT_SPEED,
				glm::vec3(CamDir[1])) * glm::mat4(CamDir));
		}

		if (glfwGetKey(window, GLFW_KEY_RIGHT)) {
			CamDir = glm::mat3(glm::rotate(glm::mat4(1.0f),
				-deltaT * ROT_SPEED,
				glm::vec3(CamDir[1])) * glm::mat4(CamDir));
		}
		if (glfwGetKey(window, GLFW_KEY_UP)&& beta < glm::radians(90.0f)) { //HERE
			CamDir = glm::mat3(glm::rotate(glm::mat4(1.0f),
				deltaT * ROT_SPEED,
				glm::vec3(CamDir[0])) * glm::mat4(CamDir));
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN) && beta > glm::radians(-90.0f)) { //HERE
			CamDir = glm::mat3(glm::rotate(glm::mat4(1.0f),
				-deltaT * ROT_SPEED,
				glm::vec3(CamDir[0])) * glm::mat4(CamDir));
		}/*
		if (glfwGetKey(window, GLFW_KEY_Q)) {
			CamDir = glm::mat3(glm::rotate(glm::mat4(1.0f),
				deltaT * ROT_SPEED,
				glm::vec3(CamDir[2])) * glm::mat4(CamDir));
		}
		if (glfwGetKey(window, GLFW_KEY_E)) {
			CamDir = glm::mat3(glm::rotate(glm::mat4(1.0f),
				-deltaT * ROT_SPEED,
				glm::vec3(CamDir[2])) * glm::mat4(CamDir));
		}*/

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
		//gubo.PLightColor = glm::vec4(1.0f, 1.0f, 0.3f, 1.0f);
		gubo.PLightColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		gubo.PLightPos2 = glm::vec3(11.0f, 3.9f, -4.0f);
		gubo.PLightColor2 = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		//gubo.PLightColor2 = glm::vec4(1.0f, 1.0f, 0.3f, 1.0f);

		gubo.PLightPosPool = glm::vec3(10.0f, 3.0f, 1.0f);
		gubo.PLightColorPool = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		
		gubo.SLightDir = glm::vec3(0.0f, 1.0f, 0.0f);
		//gubo.SLightColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		gubo.SLightColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		gubo.SLightPos = glm::vec3(10.0f, 1.0f, 1.0f);

		// Writes value to the GPU
		DSGubo.map(currentImage, &gubo, sizeof(gubo), 0);
		// the .map() method of a DataSet object, requires the current image of the swap chain as first parameter
		// the second parameter is the pointer to the C++ data structure to transfer to the GPU
		// the third parameter is its size
		// the fourth parameter is the location inside the descriptor set of this uniform block

		World = glm::translate(glm::mat4(1.0), glm::vec3(0.1, 0.0f, -1.2f)) * glm::scale(glm::mat4(1), glm::vec3(0.018f));

		uboCabinet1.amb = 1.0f; uboCabinet1.gamma = 180.0f; uboCabinet1.sColor = glm::vec3(1.0f);
		uboCabinet1.mvpMat = Prj * View * World;
		uboCabinet1.mMat = World;
		uboCabinet1.nMat = glm::inverse(glm::transpose(World));
		DSCabinet.map(currentImage, &uboCabinet1, sizeof(uboCabinet1), 0);

        World = glm::translate(glm::mat4(1.0), glm::vec3(0.1, 0.0f, -9.2f)) * glm::scale(glm::mat4(1), glm::vec3(0.018f));

        uboCabinet2.amb = 1.0f; uboCabinet2.gamma = 180.0f; uboCabinet2.sColor = glm::vec3(1.0f);
        uboCabinet2.mvpMat = Prj * View * World;
        uboCabinet2.mMat = World;
        uboCabinet2.nMat = glm::inverse(glm::transpose(World));
        DSCabinet2.map(currentImage, &uboCabinet2, sizeof(uboCabinet2), 0);

        World = glm::translate(glm::mat4(1.0), glm::vec3(0.8, 0.75f, -7.2f)) * glm::scale(glm::mat4(1), glm::vec3(0.018f));

        uboAsteroids.amb = 1.0f; uboAsteroids.gamma = 180.0f; uboAsteroids.sColor = glm::vec3(1.0f);
        uboAsteroids.mvpMat = Prj * View * World;
        uboAsteroids.mMat = World;
        uboAsteroids.nMat = glm::inverse(glm::transpose(World));
        DSAsteroids.map(currentImage, &uboAsteroids, sizeof(uboAsteroids), 0);

		World = glm::rotate(glm::mat4(1.0), glm::radians(90.0f), glm::vec3(0, 1, 0)) *
			glm::translate(glm::mat4(1.0), glm::vec3(5.0, 0.0f, 0.6f)) * glm::scale(glm::mat4(1), glm::vec3(0.0065f));

		World = glm::mat4(1);
		uboRoom.amb = 1.0f; uboRoom.gamma = 180.0f; uboRoom.sColor = glm::vec3(1.0f);
		uboRoom.mvpMat = Prj * View * World;
		uboRoom.mMat = World;
		uboRoom.nMat = glm::inverse(glm::transpose(World));
		DSRoom.map(currentImage, &uboRoom, sizeof(uboRoom), 0);

		World = glm::mat4(1);
		uboDecoration.amb = 1.0f; uboDecoration.gamma = 180.0f; uboDecoration.sColor = glm::vec3(1.0f);
		uboDecoration.mvpMat = Prj * View * World;
		uboDecoration.mMat = World;
		uboDecoration.nMat = glm::inverse(glm::transpose(World));
		DSDecoration.map(currentImage, &uboDecoration, sizeof(uboDecoration), 0);

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


		World = translate(glm::mat4(1.0), glm::vec3(3.0f, 3.97f, -4.0f)) * 
			glm::scale(glm::mat4(1), glm::vec3(0.15f, 0.15f, 0.15f));
		uboCeilingLamp1.amb = 1.0f; uboCeilingLamp1.gamma = 180.0f; uboCeilingLamp1.sColor = glm::vec3(1.0f);
		uboCeilingLamp1.mvpMat = Prj * View * World;
		uboCeilingLamp1.mMat = World;
		uboCeilingLamp1.nMat = glm::inverse(glm::transpose(World));
		DSCeilingLamp1.map(currentImage, &uboCeilingLamp1, sizeof(uboCeilingLamp1), 0);

		World = translate(glm::mat4(1.0), glm::vec3(11.0f, 3.97f, -4.0f)) *
			glm::scale(glm::mat4(1), glm::vec3(0.15f, 0.15f, 0.15f));
		uboCeilingLamp2.amb = 1.0f; uboCeilingLamp2.gamma = 180.0f; uboCeilingLamp2.sColor = glm::vec3(1.0f);
		uboCeilingLamp2.mvpMat = Prj * View * World;
		uboCeilingLamp2.mMat = World;
		uboCeilingLamp2.nMat = glm::inverse(glm::transpose(World));
		DSCeilingLamp2.map(currentImage, &uboCeilingLamp2, sizeof(uboCeilingLamp2), 0);

		World = translate(glm::mat4(1.0), glm::vec3(10.0f, 1.6f, 1.0f)) *
			glm::scale(glm::mat4(1), glm::vec3(2.0f, 0.8f, 2.0f));
		uboPoolLamp.amb = 1.0f; uboPoolLamp.gamma = 180.0f; uboPoolLamp.sColor = glm::vec3(1.0f);
		uboPoolLamp.mvpMat = Prj * View * World;
		uboPoolLamp.mMat = World;
		uboPoolLamp.nMat = glm::inverse(glm::transpose(World));
		DSPoolLamp.map(currentImage, &uboPoolLamp, sizeof(uboPoolLamp), 0);

		World = rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 1, 0)) * translate(glm::mat4(1.0), glm::vec3(-1.0f, 0.0f, 10.0f)) *
			glm::scale(glm::mat4(1), glm::vec3(2.0f));
		uboPoolTable.mvpMat = Prj * View * World;
		DSPoolTable.map(currentImage, &uboPoolTable, sizeof(uboPoolTable), 0);

		World = rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0, 1, 0)) * translate(glm::mat4(1.0), glm::vec3(14.0f, 0.0f, -9.7f)) *
			glm::scale(glm::mat4(1), glm::vec3(2.0f));
		uboSnackMachine.mvpMat = Prj * View * World;
		DSSnackMachine.map(currentImage, &uboSnackMachine, sizeof(uboSnackMachine), 0);

		// update Skybox uniforms
		SkyboxUniformBufferObject sbubo{};
		sbubo.mMat = glm::mat4(1.0f);
		sbubo.nMat = glm::mat4(1.0f);
		sbubo.mvpMat = Prj * glm::transpose(glm::mat4(CamDir));
		DSskyBox.map(currentImage, &sbubo, sizeof(sbubo), 0);


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
