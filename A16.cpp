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

struct OverlayUniformBlock {
	alignas(4) float visible;
};

struct GlobalUniformBlock {
	alignas(16) glm::vec3 DlightDir;
	alignas(16) glm::vec3 DlightColor;
	alignas(16) glm::vec3 AmbLightColor;
	alignas(16) glm::vec3 eyePos;
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

/* A16 -- OK */
/* Add the C++ datastructure for the required vertex format */
struct VertexVColor {
	glm::vec3 pos;
	glm::vec3 norm;
	glm::vec3 color;
};

class A16;
void GameLogic(A16 *A, float Ar, glm::mat4 &View,glm::mat4 &Prj, glm::mat4 &World);


// MAIN ! 
class A16 : public BaseProject {
	protected:

	// Current aspect ratio (used by the callback that resized the window
	float Ar;

	// Descriptor Layouts ["classes" of what will be passed to the shaders]
	DescriptorSetLayout DSLGubo, DSLMesh, DSLOverlay,DSLCabinet;
	/* A16 -- OK */
	/* Add the variable that will contain the required Descriptor Set Layout */
	DescriptorSetLayout DSLVColor;

	// Vertex formats
	VertexDescriptor VMesh;
	VertexDescriptor VOverlay;
	/* A16 -- OK */
	/* Add the variable that will contain the required Vertex format definition */
	VertexDescriptor VVColor;

	// Pipelines [Shader couples]
	Pipeline PMesh;
	Pipeline POverlay;
	/* A16 -- OK */
	/* Add the variable that will contain the new pipeline */
	Pipeline PVColor;
	// Models, textures and Descriptors (values assigned to the uniforms)
	// Please note that Model objects depends on the corresponding vertex structure
	Model<VertexMesh> MBody, MHandle, MWheel,MCabinet;
	/* A16 -- OK */
	/* Add the variable that will contain the model for the room */
	Model<VertexVColor> MRoom;
	
	Model<VertexOverlay> MKey, MSplash;
	
	DescriptorSet DSGubo, DSBody, DSHandle, DSWheel1, DSWheel2, DSWheel3, DSKey, DSSplash,DSCabinet;
	/* A16 -- OK */
	/* Add the variable that will contain the Descriptor Set for the room */	
	DescriptorSet DSRoom;
	Texture THandle, TWheel, TKey, TSplash,T1,T2,T3;

	// C++ storage for uniform variables
	MeshUniformBlock uboBody, uboHandle, uboWheel1, uboWheel2, uboWheel3,uboCabinet;
	/* A16 -- OK */
	/* Add the variable that will contain the Uniform Block in slot 0, set 1 of the room */
	MeshUniformBlock uboRoom;

	GlobalUniformBlock gubo;
	OverlayUniformBlock uboKey, uboSplash;

	// Other application parameters
    int currScene = 0;
    int gameState=0;
    glm::mat4 View = glm::mat4(1);
    glm::mat4 Prj = glm::mat4(1);
    glm::mat4 World = glm::mat4(1);
    glm::vec3 Pos = glm::vec3(0,0,15);
    glm::vec3 cameraPos;
    float alpha;
    float beta;
    float rho;

    float CamYaw = glm::radians(0.0f);
    float CamPitch = glm::radians(0.0f);
    float CamRoll = glm::radians(0.0f);
	float CamRadius;
	float HandleRot = 0.0;
	float Wheel1Rot = 0.0;
	float Wheel2Rot = 0.0;
	float Wheel3Rot = 0.0;
	float RoomRot = 0.0;

	// Here you set the main application parameters
	void setWindowParameters() {
		// window size, titile and initial background
		windowWidth = 800;
		windowHeight = 600;
		windowTitle = "A16";
    	windowResizable = GLFW_TRUE;
		initialBackgroundColor = {0.0f, 0.005f, 0.01f, 1.0f};
		
		// Descriptor pool sizes
		/* A16 -- OK */
		/* Update the requirements for the size of the pool */
		uniformBlocksInPool = 10;
		texturesInPool = 10;
		setsInPool = 10;
		
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
        DSLCabinet.init(this,{
                                {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},
                                {1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT},
                                {2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT},
                                {3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
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
		// Pipelines [Shader couples]
		// The second parameter is the pointer to the vertex definition
		// Third and fourth parameters are respectively the vertex and fragment shaders
		// The last array, is a vector of pointer to the layouts of the sets that will
		// be used in this pipeline. The first element will be set 0, and so on..
		PMesh.init(this, &VMesh, "shaders/MeshVert.spv", "shaders/MeshFrag.spv", {&DSLGubo, &DSLMesh,&DSLCabinet});
		POverlay.init(this, &VOverlay, "shaders/OverlayVert.spv", "shaders/OverlayFrag.spv", {&DSLOverlay});
		POverlay.setAdvancedFeatures(VK_COMPARE_OP_LESS_OR_EQUAL, VK_POLYGON_MODE_FILL,
 								    VK_CULL_MODE_NONE, false);
		/* A16 -- OK */
		/* Create the new pipeline, using shaders "VColorVert.spv" and "VColorFrag.spv" */
		PVColor.init(this, &VVColor, "shaders/VColorVert.spv", "shaders/VColorFrag.spv", { &DSLGubo, &DSLVColor});
		// Models, textures and Descriptors (values assigned to the uniforms)

		// Create models
		// The second parameter is the pointer to the vertex definition for this model
		// The third parameter is the file name
		// The last is a constant specifying the file type: currently only OBJ or GLTF
		MCabinet.init(this,  &VMesh, "Models/Cabinet.obj", OBJ);
		/* A16 -- OK*/
		/* load the mesh for the room, contained in OBJ file "Room.obj" */
		MRoom.init(this, &VVColor, "Models/Room.obj", OBJ);

		
		// Creates a mesh with direct enumeration of vertices and indices
		
		// Creates a mesh with direct enumeration of vertices and indices

		
		// Create the textures
		// The second parameter is the file name
        T1.init(this, "textures/DefenderTextures/Material.001_baseColor.png");
        T2.init(this, "textures/DefenderTextures/Material.001_metallicRoughness.png");
        T3.init(this, "textures/DefenderTextures/Material.001_normal.png");
		
		
		// Init local variables
		CamRadius = 3.0f;
		CamPitch = glm::radians(15.f);
		CamYaw = glm::radians(30.f);
        alpha = 0.0f;
        beta = 0.0f;
        rho= 0.0f;
	}
	
	// Here you create your pipelines and Descriptor Sets!
	void pipelinesAndDescriptorSetsInit() {
		// This creates a new pipeline (with the current surface), using its shaders
		PMesh.create();
		POverlay.create();
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
		/* A16 -- OK */
		/* Define the data set for the room */
		DSRoom.init(this, &DSLVColor, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr}
			});
		DSGubo.init(this, &DSLGubo, {
					{0, UNIFORM, sizeof(GlobalUniformBlock), nullptr}
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
		// Cleanup datasets
		DSCabinet.cleanup();
		/* A16 -- OK */
		/* cleanup the dataset for the room */
		DSRoom.cleanup();
		DSGubo.cleanup();
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
		
		// Cleanup models
		MCabinet.cleanup();
		/* A16 -- OK */
		/* Cleanup the mesh for the room */
		MRoom.cleanup();
		// Cleanup descriptor set layouts
		DSLMesh.cleanup();
		DSLOverlay.cleanup();
		/* A16 -- OK */
		/* Cleanup the new Descriptor Set Layout */
		DSLVColor.cleanup();
		DSLGubo.cleanup();
		
		// Destroies the pipelines
		PMesh.destroy();		
		POverlay.destroy();
		/* A16 -- OK */
		/* Destroy the new pipeline */
		PVColor.destroy();
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
					
		// record the drawing command in the command buffer
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MBody.indices.size()), 1, 0, 0, 0);
		// the second parameter is the number of indexes to be drawn. For a Model object,
		// this can be retrieved with the .indices.size() method.

		MCabinet.bind(commandBuffer);
		DSCabinet.bind(commandBuffer, PMesh, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MCabinet.indices.size()), 1, 0, 0, 0);
		/* A16 -- OK */
		/* Insert the commands to draw the room */
		PVColor.bind(commandBuffer);
		MRoom.bind(commandBuffer);
		DSGubo.bind(commandBuffer, PVColor, 0, currentImage);
		DSRoom.bind(commandBuffer, PVColor, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MRoom.indices.size()), 1, 0, 0, 0);
		POverlay.bind(commandBuffer);
	}

    void GameLogic() {
        // Parameters
        // Camera FOV-y, Near Plane and Far Plane
        const float FOVy = glm::radians(45.0f);
        const float nearPlane = 0.1f;
        const float farPlane = 100.f;
        // Camera target height and distance
        const float camHeight = 1.0f;
        const float camDist = 1;
        // Camera Pitch limits
        const float minPitch = glm::radians(-60.0f);
        const float maxPitch = glm::radians(60.0f);
        // Rotation and motion speed
        const float ROT_SPEED = glm::radians(120.0f);
        float MOVE_SPEED = 2.0f* 2.0f;

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
        } else {
            MOVE_SPEED = 2.0f;
        }

        // Rotation

        alpha = alpha - ROT_SPEED * deltaT * r.y;
        beta  = beta  - ROT_SPEED * deltaT * r.x;
        beta  =  beta < glm::radians(-90.0f) ? glm::radians(-90.0f) :
                 (beta > glm::radians( 90.0f) ? glm::radians( 90.0f) : beta);



        // Position
        glm::vec3 ux = glm::rotate(glm::mat4(1.0f), alpha, glm::vec3(0,1,0)) * glm::vec4(1,0,0,1);
        glm::vec3 uz = glm::rotate(glm::mat4(1.0f), alpha, glm::vec3(0,1,0)) * glm::vec4(0,0,-1,1);
        Pos = Pos + MOVE_SPEED * m.x * ux * deltaT;
        Pos = Pos + MOVE_SPEED * m.y * glm::vec3(0,1,0) * deltaT;
        Pos.y = 0.0f;
        Pos = Pos + MOVE_SPEED * m.z * uz * deltaT;
        cameraPos = Pos;

        // Projection
        Prj = glm::perspective(FOVy, Ar, nearPlane, farPlane);
        Prj[1][1] *= -1;

        View =  //glm::rotate(glm::mat4(1.0),-rho,glm::vec3(0,0,1)) *
                glm::rotate(glm::mat4(1.0),-beta,glm::vec3(1,0,0)) *
                glm::rotate(glm::mat4(1.0),-alpha,glm::vec3(0,1,0)) *
                glm::translate(glm::mat4(1.0),-cameraPos);

    }
	// Here is where you update the uniforms.
	// Very likely this will be where you will be writing the logic of your application.
	void updateUniformBuffer(uint32_t currentImage) {
		// Standard procedure to quit when the ESC key is pressed
		if(glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		GameLogic();


		gubo.DlightDir = glm::normalize(glm::vec3(1, 2, 3));
		gubo.DlightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		gubo.AmbLightColor = glm::vec3(0.1f);
		gubo.eyePos = Pos;

		// Writes value to the GPU
		DSGubo.map(currentImage, &gubo, sizeof(gubo), 0);
		// the .map() method of a DataSet object, requires the current image of the swap chain as first parameter
		// the second parameter is the pointer to the C++ data structure to transfer to the GPU
		// the third parameter is its size
		// the fourth parameter is the location inside the descriptor set of this uniform block

		glm::mat4 World = glm::translate(glm::mat4(1),glm::vec3(2,0,2));

        World = glm::scale(glm::mat4(1),glm::vec3(0.01f));
			
		uboCabinet.amb = 1.0f; uboCabinet.gamma = 180.0f; uboCabinet.sColor = glm::vec3(1.0f);
		uboCabinet.mvpMat = Prj * View * World;
		uboCabinet.mMat = World;
		uboCabinet.nMat = glm::inverse(glm::transpose(World));
		
		
		DSCabinet.map(currentImage, &uboCabinet, sizeof(uboCabinet), 0);

		/* A16 -- OK*/
		/* fill the uniform block for the room. Identical to the one of the body of the slot machine */
		World = glm::scale(glm::mat4(1),glm::vec3(1,2.5f,1));
		uboRoom.amb = 1.0f; uboRoom.gamma = 180.0f; uboRoom.sColor = glm::vec3(1.0f);
		uboRoom.mvpMat = Prj * View * World;
		uboRoom.mMat = World;
		uboRoom.nMat = glm::inverse(glm::transpose(World));
		/* map the uniform data block to the GPU */
		DSRoom.map(currentImage, &uboRoom, sizeof(uboRoom), 0);

	}	
};


// This is the main: probably you do not need to touch this!
int main() {
    A16 app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
