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

//This is the data structure used for basic objects with only 1 texture
struct OBJUniformBlock {
	alignas(4) float amb; //how the object is affected by the ambient light
	alignas(4) float gamma; // gamma affects the reflection of the object (higher gamma --> smaller "light circle" and more mirror-like object)
	alignas(16) glm::vec3 sColor; // This is the color of the light that gets reflected by the object (i.e. the color of the "light circle")
	alignas(16) glm::mat4 mvpMat; //world view proj matrix
	alignas(16) glm::mat4 mMat; //world matrix
	alignas(16) glm::mat4 nMat; // inverse, transpose of the world matrix. Used in the shader to compute the normals
};

struct OverlayUniformBlock {
	alignas(4) float visible; //A boolean used to make the popup appear/disappear
};

struct PongUniformBlock {
	alignas(8) glm::vec2 pPos; //Position of the object
};

//Gubo is passed to (almost) every shader as it contains the information of the lights
struct GlobalUniformBlock {
	//Direct light: Actually turned off
	alignas(16) glm::vec3 DlightDir;
	alignas(16) glm::vec3 DlightColor;
	//Ambient light
	alignas(16) glm::vec3 AmbLightColor;
	//Position of the viewer
	alignas(16) glm::vec3 eyePos;
	//Point light 1
	alignas(16) glm::vec3 PLightPos1;
	alignas(16) glm::vec4 PLightColor1;
	//Point light 2
	alignas(16) glm::vec3 PLightPos2;
	alignas(16) glm::vec4 PLightColor2;
	//Spot light
	alignas(16) glm::vec3 SLightPos;
	alignas(16) glm::vec3 SLightDir;
	alignas(16) glm::vec4 SLightColor;
	//Point light lantern 1
	alignas(16) glm::vec3 PLightPosLantern1;
	alignas(16) glm::vec4 PLightColorLantern1;
	//Point light lantern 2
	alignas(16) glm::vec3 PLightPosLantern2;
	alignas(16) glm::vec4 PLightColorLantern2;
};

// The vertices data structures
//For almost every object of the scene
struct VertexOBJ {
	glm::vec3 pos;
	glm::vec3 norm;
	glm::vec2 UV;
};

//For pong objects and popup
struct VertexOverlay {
	glm::vec2 pos;
	glm::vec2 UV;
};

//This is a structure that contains all the information of an object
struct OBJStruct{
    DescriptorSet *DS;
    Texture *T;
    int s; //size of uniform block
    Model<VertexOBJ> *M;
    Pipeline *P;
    int currScene;
};

//This is a structure that contains all the information of an object of the pong game
struct PongStruct{
    DescriptorSet *DS;
    Texture *T;
    int s; //size of uniform block
    Model<VertexOverlay> *M;
    Pipeline *P;
    int currScene;
};

class SpaceArcade;
void GameLogic(SpaceArcade* A, float Ar, glm::mat4& View, glm::mat4& Prj, glm::mat4& World,glm::mat3 &CamDir);
void ballBounce();

class SpaceArcade : public BaseProject {
protected:
	// Current aspect ratio (used by the callback that resized the window
	float Ar;

	// Descriptor Layouts ["classes" of what will be passed to the shaders]
	DescriptorSetLayout DSLGubo, DSLOBJ, DSLAdvanced;
	// Vertex formats
	VertexDescriptor VOBJ, VOverlay, VSimple;

	// Pipelines [Shader couples]
	Pipeline POBJ, POverlay, PSkyBox, PRoom, PPong, PEmi, PFloor;
    std::vector<Pipeline*> pipelines = {&POBJ, &POverlay, &PSkyBox, &PRoom, &PPong, &PEmi, &PFloor};

    // Models, textures and Descriptors (values assigned to the uniforms)
	// Please note that Model objects depends on the corresponding vertex structure
	Model<VertexOBJ> MDefender, MAsteroids, MRoom, MDecoration, MCeiling,
                     MFloor, MDanceDance, MBattleZone, MNudge, MSnackMachine, MCeilingLamp, 
					 MPoolLamp, MPoolTable, MDoor, MSkyBox, MBanner, MWorldFloor,
					 MLantern;

	Model<VertexOverlay> MPongR, MPongL, MPongBall, MPongNet, MPopup;

    std::vector<Model<VertexOBJ>*> objModels = {
        &MDefender, &MAsteroids, &MRoom, &MDecoration, &MCeiling,
        &MFloor, &MDanceDance, &MBattleZone, &MNudge, &MSnackMachine,
        &MCeilingLamp, &MPoolLamp, &MPoolTable, &MDoor,
        &MSkyBox, &MBanner, &MWorldFloor, &MLantern
    };

    std::vector<Model<VertexOverlay>*> overlayModels = {
        &MPongR, &MPongL, &MPongBall, &MPongNet, &MPopup
    };

	DescriptorSet DSGubo, DSCabinet1, DSCabinet2, DSAsteroids, DSCeilingLamp1,
                  DSCeilingLamp2, DSPoolLamp, DSPoolTable, DSSnackMachine,
                  DSDanceDance, DSBattleZone, DSNudge, DSRoom, DSDecoration,
                  DSCeiling, DSFloor, DSskyBox, DSDoor, DSSkyBox, DSBanner,
                  DSWorldFloor, DSPongR, DSPongL, DSPongBall, DSPongNet, DSPopup,
				  DSJLantern1,DSJLantern2;

    std::vector<DescriptorSet*> descriptorSets = {
        &DSGubo, &DSCabinet1, &DSCabinet2, &DSAsteroids, &DSCeilingLamp1,
        &DSCeilingLamp2, &DSPoolLamp, &DSPoolTable, &DSSnackMachine,
        &DSDanceDance, &DSBattleZone, &DSNudge, &DSRoom, &DSDecoration,
        &DSCeiling, &DSFloor, &DSskyBox, &DSDoor, &DSSkyBox, &DSBanner,
        &DSWorldFloor, &DSPongR, &DSPongL, &DSPongBall, &DSPongNet, &DSPopup,
        &DSJLantern1, &DSJLantern2
    };

    Texture TDefender, TRoom, TDecoration, TCeiling, TFloor, TAsteroids,
            TWhite, TPoolLamp, TPoolLampEmi, TskyBox, TDanceDance, 
			TBattleZone, TNudge, TSnackMachine, TPoolTable, TDoor,
			TBanner, TWorldFloor, TPopup, TLanternColor, TLanternEmi,
			TPongCabinetEmi, TDanceDanceEmi;

	std::vector<Texture*> textures = {
		&TDefender, &TRoom, &TDecoration, &TCeiling, &TFloor, &TWhite,
		&TPoolLamp, &TPoolLampEmi, &TAsteroids, &TskyBox,
		&TDanceDance, &TBattleZone, &TNudge, &TSnackMachine, &TPoolTable,
		&TDoor, &TBanner, &TPopup, &TWorldFloor, &TPongCabinetEmi , &TLanternEmi,
		&TLanternColor, &TDanceDanceEmi
	};

    std::vector<OBJStruct*> Objects = {
        new OBJStruct{ &DSCabinet2, &TDefender, sizeof(OBJUniformBlock), &MDefender, &POBJ, 0},
        new OBJStruct{ &DSAsteroids, &TAsteroids, sizeof(OBJUniformBlock), &MAsteroids, &POBJ, 0},
        new OBJStruct{ &DSBattleZone, &TBattleZone, sizeof(OBJUniformBlock), &MBattleZone, &POBJ, 0},
        new OBJStruct{ &DSNudge, &TNudge, sizeof(OBJUniformBlock), &MNudge, &POBJ, 0},
        new OBJStruct{ &DSDoor, &TDoor, sizeof(OBJUniformBlock), &MDoor, &POBJ, 0},
        new OBJStruct{ &DSDecoration, &TDecoration, sizeof(OBJUniformBlock), &MDecoration, &POBJ, 0},
        new OBJStruct{ &DSCeiling, &TCeiling, sizeof(OBJUniformBlock), &MCeiling, &POBJ, 0},
        new OBJStruct{ &DSPoolTable, &TPoolTable, sizeof(OBJUniformBlock), &MPoolTable, &POBJ, 0},
        new OBJStruct{ &DSSnackMachine, &TSnackMachine, sizeof(OBJUniformBlock), &MSnackMachine, &POBJ, 0},
        new OBJStruct{ &DSWorldFloor, &TWorldFloor, sizeof(OBJUniformBlock), &MWorldFloor, &POBJ, 0},
        new OBJStruct{ &DSSkyBox, &TskyBox, sizeof(OBJUniformBlock), &MSkyBox, &PSkyBox, 0},
        new OBJStruct{ &DSRoom, &TRoom, sizeof(OBJUniformBlock), &MRoom, &PRoom, 0},
        new OBJStruct{ &DSFloor, &TFloor, sizeof(OBJUniformBlock), &MFloor, &PFloor, 0}
    };

    std::vector<PongStruct*> PongObjects = {
        new PongStruct{ &DSPongR, &TWhite, sizeof(PongUniformBlock), &MPongR, &PPong, 1},
        new PongStruct{ &DSPongL, &TWhite, sizeof(PongUniformBlock), &MPongL, &PPong, 1},
        new PongStruct{ &DSPongBall, &TWhite, sizeof(PongUniformBlock), &MPongBall, &PPong, 1},
        new PongStruct{ &DSPongNet, &TWhite, sizeof(OverlayUniformBlock), &MPongNet, &PPong, 1},
		new PongStruct{ &DSPopup, &TPopup, sizeof(OverlayUniformBlock), &MPopup, &POverlay, 0}
    };

    // C++ storage for uniform variables
	OBJUniformBlock uboCabinet1, uboCabinet2, uboAsteroids, uboRoom,
                    uboDecoration, uboCeiling, uboFloor, uboCeilingLamp1,
                    uboCeilingLamp2, uboPoolLamp, uboDanceDace, uboBattleZone,
                    uboNudge, uboSnackMachine, uboPoolTable, uboDoor, uboSkyBox,
					uboBanner, uboWorldFloor, uboJLantern1, uboJLantern2;

	OverlayUniformBlock uboPopup, uboPongNet;

	PongUniformBlock uboPong;

	GlobalUniformBlock gubo;

	// Other application parameters
	int currScene = 0;
	bool rangeVideogame; //when this bool is true, the popup of the game comes out
	glm::mat4 View = glm::mat4(1);
	glm::mat4 Prj = glm::mat4(1);
	glm::mat4 World = glm::mat4(1);
	glm::vec3 Pos = glm::vec3(12,0,-25);
	glm::vec3 cameraPos;

	float alpha = glm::radians(180.0f);
	float beta = glm::radians(0.0f);
	//Parameters of the pong bars
	float pongLength = 0.5f;
	float pongWidth = 0.05f;

	// COORD: (-1,-1) <-- top-left, (1,1) <-- bottom-right, (1,-1) <-- top-right, (-1,1) <-- bottom_left
	//xURR = x coordinate of upper-right vertex
	float xURR = 0.9f;
	float yURR = -pongLength / 2;
	glm::vec2 pongPosR = glm::vec2(xURR - pongWidth/2, 0.0f); //Position of the right pong bar

	//xULL = x coordinate of upper-left vertex
	float xULL = -0.9f;
	float yULL = -pongLength / 2;
	glm::vec2 pongPosL = glm::vec2(xULL + pongWidth / 2, 0.0f); //Position of the left pong bar

	glm::vec2 pongPosBall = glm::vec2(0.0f, 0.0f); //Pong ball position
	glm::vec2 pongVelBallInitial = glm::vec2(0.002f, 0.002f); //Pong ball initial velocity
	glm::vec2 pongVelBall = pongVelBallInitial; //Pong ball velocity
	float ballRadius = 0.03f;

	// Jump parameters
	bool isJumping = false;         // Flag to indicate if the player is currently jumping
	float jumpVelocity = 0.0f;      // Initial jump velocity
	float jumpHeight = 0.6f;        // Height the player can reach during the jump
	float gravity = 9.8f;           // Acceleration due to gravity
	float maxJumpTime = 1.3f;       // Maximum duration of the jump
	float jumpTime = 0.0f;          // Current time elapsed during the jump


	// Here you set the main application parameters
	void setWindowParameters() {
		// window size, titile and initial background
		windowWidth = 1920;
		windowHeight = 1080;
		windowTitle = "SpaceArcade";
		windowResizable = GLFW_FALSE;
		initialBackgroundColor = { 0.0f, 0.005f, 0.01f, 1.0f };

		// Descriptor pool sizes
		uniformBlocksInPool = 50;
		texturesInPool = 50;
		setsInPool = 50;

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

		//DSL of normal objects, with only 1 texture (main color)
        DSLOBJ.init(this, {
			{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},
			{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT} //main color texture
		});

		DSLGubo.init(this, {
			{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS}
		});
		
		//This is the DSL for emissive objects, as they have 2 textures
		DSLAdvanced.init(this, {
			{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},
			{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}, //main color texture
			{2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}  //emission color texture
		});

		// Vertex descriptors
		VOBJ.init(this, {
			{0, sizeof(VertexOBJ), VK_VERTEX_INPUT_RATE_VERTEX}
		}, {
			{0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexOBJ, pos),
					sizeof(glm::vec3), POSITION},
			{0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexOBJ, norm),
					sizeof(glm::vec3), NORMAL},
			{0, 2, VK_FORMAT_R32G32_SFLOAT, offsetof(VertexOBJ, UV),
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

		// Pipelines [Shader couples]
		// The second parameter is the pointer to the vertex definition
		// Third and fourth parameters are respectively the vertex and fragment shaders
		// The last array, is a vector of pointer to the layouts of the sets that will
		// be used in this pipeline. The first element will be set 0, and so on..
        POBJ.init(this, &VOBJ, "shaders/ObjectVert.spv", "shaders/ObjectFrag.spv", { &DSLGubo, &DSLOBJ });

        PSkyBox.init(this, &VOBJ, "shaders/SkyboxVert.spv", "shaders/SkyBoxFrag.spv", { &DSLOBJ });
        PSkyBox.setAdvancedFeatures(VK_COMPARE_OP_LESS_OR_EQUAL, VK_POLYGON_MODE_FILL,
			VK_CULL_MODE_NONE, false);

		PRoom.init(this, &VOBJ, "shaders/ObjectVert.spv", "shaders/RoomFrag.spv", { &DSLGubo, &DSLOBJ });

		POverlay.init(this, &VOverlay, "shaders/OverlayVert.spv", "shaders/OverlayFrag.spv", { &DSLOBJ });
		POverlay.setAdvancedFeatures(VK_COMPARE_OP_LESS_OR_EQUAL, VK_POLYGON_MODE_FILL,
			VK_CULL_MODE_NONE, false);

		PPong.init(this, &VOverlay, "shaders/PongVert.spv", "shaders/PongFrag.spv", { &DSLOBJ });
		PPong.setAdvancedFeatures(VK_COMPARE_OP_LESS_OR_EQUAL, VK_POLYGON_MODE_FILL,
			VK_CULL_MODE_NONE, false);

		PEmi.init(this, &VOBJ, "shaders/ObjectVert.spv", "shaders/EmiFrag.spv", { &DSLGubo, &DSLAdvanced});

		PFloor.init(this, &VOBJ, "shaders/ObjectVert.spv", "shaders/FloorFrag.spv", { &DSLGubo, &DSLOBJ });

		// Models, textures and Descriptors (values assigned to the uniforms)

		// Create models
		// The second parameter is the pointer to the vertex definition for this model
		// The third parameter is the file name
		// The last is a constant specifying the file type: currently only OBJ or GLTF
		MDefender.init(this, &VOBJ, "Assets/DefenderCabinet/DefenderCabinet.obj", OBJ);
		MAsteroids.init(this, &VOBJ, "Assets/AsteroidsCabinet/AsteroidsCabinet.obj", OBJ);
		MDanceDance.init(this, &VOBJ, "Assets/DanceDanceCabinet/DanceDanceCabinet.obj", OBJ);
		MBattleZone.init(this, &VOBJ, "Assets/BattleZoneCabinet/BattleZoneCabinet.obj", OBJ);
		MNudge.init(this, &VOBJ, "Assets/NudgeCabinet/NudgeCabinet.obj", OBJ);
		MRoom.init(this, &VOBJ, "Assets/Room/Room.obj", OBJ);
		MCeiling.init(this, &VOBJ, "Assets/Ceiling/Ceiling.obj", OBJ);
		MDecoration.init(this, &VOBJ, "Assets/Decoration/Decoration.obj", OBJ);
		MFloor.init(this, &VOBJ, "Assets/Floor/Floor.obj", OBJ);
		MCeilingLamp.init(this, &VOBJ, "Assets/CeilingLamp/CeilingLamp.obj", OBJ);
		MPoolLamp.init(this, &VOBJ,"Assets/PoolLamp/PoolLamp.obj", OBJ);
		MPoolTable.init(this, &VOBJ, "Assets/PoolTable/PoolTable.obj", OBJ);
		MSnackMachine.init(this, &VOBJ, "Assets/SnackMachine/SnackMachine.obj", OBJ);
		MSkyBox.init(this, &VOBJ, "Assets/SkyBox/SkyBox.obj", OBJ);
		MDoor.init(this, &VOBJ, "Assets/Door/Door.obj", OBJ);
		MBanner.init(this, &VOBJ, "Assets/Banner/Banner.obj", OBJ);
		MWorldFloor.init(this, &VOBJ, "Assets/WorldFloor/WorldFloor.obj", OBJ);
		MLantern.init(this, &VOBJ, "Assets/Lantern/Lantern.obj", OBJ);
		// Creates a mesh with direct enumeration of vertices and indices

		//Mesh of the popup alert
		MPopup.vertices = { {{-0.7f, 0.5f}, {0.0f,0.0f}}, {{-0.7f, 0.95f}, {0.0f,1.0f}},
							{{ 0.7f, 0.5f}, {1.0f,0.0f}}, {{ 0.7f, 0.95f}, {1.0f,1.0f}}};
		MPopup.indices = { 0, 1, 2, 1, 2, 3 };
		MPopup.initMesh(this, &VOverlay);

		//Mesh of pong right bar
		MPongR.vertices = { {{xURR, yURR}, {0.8f,0.0f}}, {{xURR - pongWidth, yURR}, {0.0f,1.0f}},
						  {{xURR, yURR + pongLength}, {1.0f,0.0f}}, {{xURR - pongWidth, yURR + pongLength}, {1.0f,1.0f}}};
		MPongR.indices = { 0, 1, 2, 1, 2, 3 };
		MPongR.initMesh(this, &VOverlay);

		//Mesh of pong left bar
		MPongL.vertices = { {{xULL, yULL}, {0.8f,0.0f}}, {{xULL + pongWidth, yULL}, {0.0f,1.0f}},
						  {{xULL, yULL + pongLength}, {1.0f,0.0f}}, {{xULL + pongWidth, yULL + pongLength}, {1.0f,1.0f}} };
		MPongL.indices = { 0, 1, 2, 1, 2, 3 };
		MPongL.initMesh(this, &VOverlay);

		//Algorithm to create the mesh of the pong ball
		int vertexNumber = 50; //number of approximation vertices
		float anglestep = 360.0f / vertexNumber;
		//x center component
		float xBall = 0.0f;
		//y center component
		float yBall = 0.0f;
		//ball center
		MPongBall.vertices.push_back({{xBall, Ar * yBall }, {0.0f, 0.0f}});
		//first vertex
		MPongBall.vertices.push_back({ {xBall + ballRadius, Ar * yBall}, {1.0f,0.0f}});

		for (int i = 1; i < vertexNumber; i++){
			//Push new vertex
			MPongBall.vertices.push_back({{xBall + ballRadius * cos(glm::radians(i * anglestep)), Ar * (yBall + ballRadius * sin(glm::radians(i * anglestep)))},
				{cos(glm::radians(i * anglestep)),sin(glm::radians(i * anglestep))}});
			//Create a triangle with the new vertex, the previous one and the center of the ball
			MPongBall.indices.push_back(0);
			MPongBall.indices.push_back(i);
			MPongBall.indices.push_back(i + 1);
		}
		//Create last triangle
		MPongBall.indices.push_back(0);
		MPongBall.indices.push_back(1);
		MPongBall.indices.push_back(vertexNumber);

		MPongBall.initMesh(this, &VOverlay);

		//Create mesh of the pong net
		int netNumber = 15; //Number of rectangles in the net
		//length of a rectangle of the net calculated as the total lenght of the screen divided by the number of rectangles * 2 (every other)
		float netRectLenght = 2.0f / (netNumber * 2.0f);
		float netRectWidht = 0.02f;

		for (int i = 0; i < netNumber; i++) {
			MPongNet.vertices.push_back({ {-netRectWidht/2, -1 + 2 * i * netRectLenght},{0.0f,0.0f} });
			MPongNet.vertices.push_back({ {+netRectWidht/2, -1 + 2 * i * netRectLenght},{0.0f,1.0f} });
			MPongNet.vertices.push_back({ {-netRectWidht/2, -1 + 2 * i * netRectLenght + netRectLenght},{1.0f,0.0f} });
			MPongNet.vertices.push_back({ {+netRectWidht/2, -1 + 2 * i * netRectLenght + netRectLenght},{1.0f,1.0f} });

			MPongNet.indices.push_back(4 * i);
			MPongNet.indices.push_back(4 * i + 1);
			MPongNet.indices.push_back(4 * i + 2);

			MPongNet.indices.push_back(4 * i + 1);
			MPongNet.indices.push_back(4 * i + 2);
			MPongNet.indices.push_back(4 * i + 3);
		}

		MPongNet.initMesh(this, &VOverlay);

		// Create the textures
		// The second parameter is the file name
		TDefender.init(this, "Assets/DefenderCabinet/Textures/DefenderTexture.png");
		TPongCabinetEmi.init(this, "Assets/DefenderCabinet/Textures/PongEmit.png");
		TRoom.init(this, "Assets/Room/Textures/ArcadeWalls.jpg");
		TDecoration.init(this, "Assets/Decoration/Textures/TextureDecoration.jpg");
		TCeiling.init(this, "Assets/Ceiling/Textures/Ceiling.png");
		TFloor.init(this, "Assets/Floor/Textures/WoodFloor.jpg");
		TPoolLamp.init(this, "Assets/PoolLamp/Textures/TexturesCity.png");
		TPoolLampEmi.init(this, "Assets/PoolLamp/Textures/TexturesCityEmission.png");
        TAsteroids.init(this,"Assets/AsteroidsCabinet/Textures/Material.001_baseColor.png");
        TDanceDance.init(this,"Assets/DanceDanceCabinet/Textures/lambert3_emissive.png");
		TDanceDanceEmi.init(this, "Assets/DanceDanceCabinet/Textures/lambert3_baseColor.png");
        TBattleZone.init(this,"Assets/BattleZoneCabinet/Textures/Material.001_baseColor.png");
        TNudge.init(this,"Assets/NudgeCabinet/Textures/Material.002_albedo.jpg");
        TSnackMachine.init(this,"Assets/SnackMachine/Textures/albedo.jpg");
        TPoolTable.init(this, "Assets/PoolTable/Textures/pooltablelow_POOL_TABLE_BaseColor.png");
        TDoor.init(this,"Assets/Door/Textures/DoorTexture.png");
		TBanner.init(this, "Assets/Banner/Textures/DarkGrey.png");
		TWorldFloor.init(this, "Assets/WorldFloor/Textures/MyGrid.png");
		TWhite.init(this, "Assets/OtherTextures/White.png");
		TLanternColor.init(this, "Assets/Lantern/Textures/LanternColor.png");
		TLanternEmi.init(this, "Assets/Lantern/Textures/LanternEmiV2.png");
		TPopup.init(this, "Assets/OtherTextures/PressP.png");

		//Skybox textures
		const char* T2fn[] = { "Assets/SkyBox/Textures/px.png", "Assets/SkyBox/Textures/nx.png",
							   "Assets/SkyBox/Textures/py.png", "Assets/SkyBox/Textures/ny.png",
							   "Assets/SkyBox/Textures/pz.png", "Assets/SkyBox/Textures/nz.png" };
		TskyBox.initCubic(this, T2fn);
	}

	// Here you create your pipelines and Descriptor Sets!
	void pipelinesAndDescriptorSetsInit() {

		// This creates a new pipeline (with the current surface), using its shaders
        for (size_t i = 0; i < pipelines.size(); i++) {
            pipelines[i]->create();
        }

		// Here you define the data set
		//Initialization of basic object with only 1 texture
        for (size_t i = 0; i < Objects.size(); i++) {
            Objects[i]->DS->init(this, &DSLOBJ, {
                {0, UNIFORM, Objects[i]->s, nullptr},
                {1, TEXTURE, 0, (Objects[i]->T)}
            });
        }

		//Initialization of pong objects
        for (size_t i = 0; i < PongObjects.size(); i++) {
            PongObjects[i]->DS->init(this, &DSLOBJ, {
                {0, UNIFORM, PongObjects[i]->s, nullptr},
                {1, TEXTURE, 0, (PongObjects[i]->T)}
            });
        }

		//Initialization of descriptors sets of emissive objects (2 textures)
        DSPoolLamp.init(this, &DSLAdvanced, {
            {0, UNIFORM, sizeof(OBJUniformBlock), nullptr},
            {1, TEXTURE, 0, &TPoolLamp},
            {2,TEXTURE,0,&TPoolLampEmi}
        });

        DSCeilingLamp1.init(this, &DSLAdvanced, {
            {0, UNIFORM, sizeof(OBJUniformBlock), nullptr},
            {1, TEXTURE, 0, &TPoolLamp},
            {2,TEXTURE,0,&TPoolLampEmi}
        });

        DSCeilingLamp2.init(this, &DSLAdvanced, {
            {0, UNIFORM, sizeof(OBJUniformBlock), nullptr},
            {1, TEXTURE, 0, &TPoolLamp},
            {2,TEXTURE,0,&TPoolLampEmi}
        });

		DSJLantern1.init(this, &DSLAdvanced, {
			{0, UNIFORM, sizeof(OBJUniformBlock), nullptr},
			{1, TEXTURE, 0, &TLanternColor},
			{2,TEXTURE,0,&TLanternEmi}
		});

		DSJLantern2.init(this, &DSLAdvanced, {
			{0, UNIFORM, sizeof(OBJUniformBlock), nullptr},
			{1, TEXTURE, 0, &TLanternColor},
			{2,TEXTURE,0,&TLanternEmi}
		});

		DSBanner.init(this, &DSLAdvanced, {
			{0, UNIFORM, sizeof(OBJUniformBlock), nullptr},
			{1, TEXTURE, 0, &TBanner},
			{2,TEXTURE,0,&TLanternEmi} //using the same emission as the japanese lanterns
		});

		DSCabinet1.init(this, &DSLAdvanced, {
			{0, UNIFORM, sizeof(OBJUniformBlock), nullptr},
			{1, TEXTURE, 0, &TDefender},
			{2,TEXTURE,0,&TPongCabinetEmi} 
		});

		DSDanceDance.init(this, &DSLAdvanced, {
			{0, UNIFORM, sizeof(OBJUniformBlock), nullptr},
			{1, TEXTURE, 0, &TDanceDance},
			{2,TEXTURE,0,&TDanceDanceEmi}
		});

        DSGubo.init(this, &DSLGubo, {
			{0, UNIFORM, sizeof(GlobalUniformBlock), nullptr}
        });
	}

	// Here you destroy your pipelines and Descriptor Sets!
	// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
	void pipelinesAndDescriptorSetsCleanup() {
		// Cleanup pipelines
        for (size_t i = 0; i < pipelines.size(); i++) {
			pipelines[i]->cleanup();
        }
		// Cleanup datasets
        for (size_t i = 0; i < descriptorSets.size(); i++) {
            descriptorSets[i]->cleanup();
        }
	}

	// Here you destroy all the Models, Texture and Desc. Set Layouts you created!
	// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
	// You also have to destroy the pipelines: since they need to be rebuilt, they have two different
	// methods: .cleanup() recreates them, while .destroy() delete them completely
	void localCleanup() {
		// Cleanup textures
        for (Texture* texture : textures) {
            texture->cleanup();
        }

		// Cleanup models
        for (size_t i = 0; i < objModels.size(); i++) {
            objModels[i]->cleanup();
        }

        for (size_t i = 0; i < overlayModels.size(); i++) {
            overlayModels[i]->cleanup();
        }

		// Cleanup descriptor set layouts
        DSLOBJ.cleanup();
		DSLGubo.cleanup();
		DSLAdvanced.cleanup();

		// Destroies the pipelines
        for (size_t i = 0; i < pipelines.size(); i++) {
            pipelines[i]->destroy();
        }
	}

	// Here it is the creation of the command buffer:
	// You send to the GPU all the objects you want to draw,
	// with their buffers and textures
	void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) {
		switch (currScene) {
		case 0: //The basic scene with the arcade

			// sets global uniforms (see below from parameters explanation)
			DSGubo.bind(commandBuffer, POBJ, 0, currentImage);

			// binds the pipeline
			// For a pipeline object, this command binds the corresponing pipeline to the command buffer passed in its parameter
            POBJ.bind(commandBuffer);

            for (size_t i = 0; i < Objects.size() -3; i++) {
				// binds the model
				// For a Model object, this command binds the corresponing index and vertex buffer to the command buffer passed in its parameter
                Objects[i]->M->bind(commandBuffer);

				// binds the data set
				// For a Dataset object, this command binds the corresponing dataset to the command buffer and pipeline passed in its first and second parameters.
                Objects[i]->DS->bind(commandBuffer,*(Objects[i]->P),1,currentImage); // The third parameter is the number of the set being bound

				// record the drawing command in the command buffer
				// the second parameter is the number of indexes to be drawn. For a Model object,
				// this can be retrieved with the .indices.size() method.
                vkCmdDrawIndexed(commandBuffer, 
					static_cast<uint32_t>(Objects[i]->M->indices.size()), 1, 0, 0, 0);
            }

            //Room (walls)
			PRoom.bind(commandBuffer);
			MRoom.bind(commandBuffer);
			DSRoom.bind(commandBuffer, PRoom, 1, currentImage);
			vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MRoom.indices.size()), 1, 0, 0, 0);

            //Floor
            PFloor.bind(commandBuffer);
            MFloor.bind(commandBuffer);
            DSFloor.bind(commandBuffer, PFloor, 1, currentImage);
            vkCmdDrawIndexed(commandBuffer,
                             static_cast<uint32_t>(MFloor.indices.size()), 1, 0, 0, 0);

            //SkyBox
            PSkyBox.bind(commandBuffer);
            MSkyBox.bind(commandBuffer);
			DSSkyBox.bind(commandBuffer, PSkyBox, 0, currentImage);
			vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MSkyBox.indices.size()), 1, 0, 0, 0);

            //Popup
			POverlay.bind(commandBuffer);
			MPopup.bind(commandBuffer);
			DSPopup.bind(commandBuffer, POverlay, 0, currentImage);
			vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MPopup.indices.size()), 1, 0, 0, 0);

            //Emission objects
			PEmi.bind(commandBuffer);
			DSGubo.bind(commandBuffer, PEmi, 0, currentImage);
			MPoolLamp.bind(commandBuffer);
			DSPoolLamp.bind(commandBuffer, PEmi, 1, currentImage);
			vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MPoolLamp.indices.size()), 1, 0, 0, 0);

			MCeilingLamp.bind(commandBuffer);
			DSCeilingLamp1.bind(commandBuffer, PEmi, 1, currentImage);
			vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MCeilingLamp.indices.size()), 1, 0, 0, 0);

			DSCeilingLamp2.bind(commandBuffer, PEmi, 1, currentImage);
			vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MCeilingLamp.indices.size()), 1, 0, 0, 0);

			MLantern.bind(commandBuffer);
			DSJLantern1.bind(commandBuffer, PEmi, 1, currentImage);
			vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MLantern.indices.size()), 1, 0, 0, 0);

			DSJLantern2.bind(commandBuffer, PEmi, 1, currentImage);
			vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MLantern.indices.size()), 1, 0, 0, 0);

			MBanner.bind(commandBuffer);
			DSBanner.bind(commandBuffer, PEmi, 1, currentImage);
			vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MBanner.indices.size()), 1, 0, 0, 0);

			MDefender.bind(commandBuffer);
			DSCabinet1.bind(commandBuffer, PEmi, 1, currentImage);
			vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MDefender.indices.size()), 1, 0, 0, 0);

			MDanceDance.bind(commandBuffer);
			DSDanceDance.bind(commandBuffer, PEmi, 1, currentImage);
			vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MDanceDance.indices.size()), 1, 0, 0, 0);

			break;
		case 1:
			//Pong objects
			PPong.bind(commandBuffer);

            for (size_t i = 0; i < PongObjects.size() -2; i++) {
                PongObjects[i]->M->bind(commandBuffer);
                PongObjects[i]->DS->bind(commandBuffer, *(PongObjects[i]->P), 0, currentImage);
                vkCmdDrawIndexed(commandBuffer,
                                    static_cast<uint32_t>(PongObjects[i]->M->indices.size()), 1, 0, 0, 0);
            }

			POverlay.bind(commandBuffer);
			MPongNet.bind(commandBuffer);
			DSPongNet.bind(commandBuffer, POverlay, 0, currentImage);
			vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MPongNet.indices.size()), 1, 0, 0, 0);

			break;
		}

    }

	//function that changes the velocity of the ball when it hits on of the 2 "racket"
	void ballBounce() {
		//flip ball velocity when it hits on of the 2 "racket"
		pongVelBall.x *= -1.0f;
		
		//variation of ball velocity min and max
		float min = 0.0001f;
		float max = 0.0003f;
		//limit to the ball velocity x and y
		float velLimitX = 0.01f; 
		float velLimitY = 0.01f;
		
		//calculate a random value between min and max to add to the ball velocity
		std::srand(std::time(0));
		double randomValue = min + static_cast<double>(std::rand()) / (static_cast<double>(RAND_MAX / (max - min)));

		if (abs(pongVelBall.x) < velLimitX) { //Don't increase the velocity if it has reached the maximum value (in abs)
			if (pongVelBall.x > 0.0f) {
				pongVelBall.x += randomValue;
			}
			else {
				pongVelBall.x -= randomValue;
			}
		}
		
		std::srand(std::time(0));
		randomValue = min + static_cast<double>(std::rand()) / (static_cast<double>(RAND_MAX / (max - min)));

		if (abs(pongVelBall.y) < velLimitY) { //Don't increase the velocity if it has reached the maximum value (in abs)
			if (pongVelBall.y > 0.0f) {
				pongVelBall.y += randomValue;
			}
			else {
				pongVelBall.y -= randomValue;
			}
		}
	}

	void GameLogic() {	// Game Logic implementation
		// Parameters
		// Camera FOV-y, Near Plane and Far Plane
		const float FOVy = glm::radians(45.0f);
		const float nearPlane = 0.1f;
		const float farPlane = 100.f;
		// Camera target height and distance
		const float camHeight = 2.35f;
		// Rotation and motion speed
		const float ROT_SPEED = glm::radians(120.0f);
		float MOVE_SPEED;
		// Integration with the timers and the controllers
		float deltaT;
		glm::vec3 m = glm::vec3(0.0f), r = glm::vec3(0.0f);
		bool fire = false;
        glm::vec3 ux ;
        glm::vec3 uz ;
        getSixAxis(deltaT, m, r, fire);
		switch (currScene) {
			case 0:

				if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) { //Running
					MOVE_SPEED = 8.0f;
				}
				else { //Walking
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
					Pos+= jumpOffset;

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
                ux = glm::rotate(glm::mat4(1.0f), alpha, glm::vec3(0, 1, 0)) * glm::vec4(1, 0, 0, 1);
                uz = glm::rotate(glm::mat4(1.0f), alpha, glm::vec3(0, 1, 0)) * glm::vec4(0, 0, -1, 1);
				Pos = Pos + MOVE_SPEED * m.x * ux * deltaT;
				Pos = Pos + MOVE_SPEED * m.z * uz * deltaT;
				cameraPos = Pos + glm::vec3(0, camHeight, 0);

				Prj = glm::perspective(FOVy, Ar, nearPlane, farPlane);
				Prj[1][1] *= -1;

				View = glm::rotate(glm::mat4(1.0), -beta, glm::vec3(1, 0, 0)) *
					   glm::rotate(glm::mat4(1.0), -alpha, glm::vec3(0, 1, 0)) *
					   glm::translate(glm::mat4(1.0), -cameraPos);

				break;
			case 1:
				float pongVel = 0.005f; //velocity of the pong bars
				pongPosBall += pongVelBall; //update pong ball position

				if (pongPosBall.y + ballRadius <= -1.0f || pongPosBall.y + ballRadius >= 1.0f) { //if the ball touches the horizontal borders
					pongVelBall.y *= -1.0f; //flip y component of its velocity
				}

				if (pongPosBall.x - ballRadius <= pongPosL.x + pongWidth/2) { //if the ball passes the x of the left bar
					if (pongPosBall.y + ballRadius >= pongPosL.y - pongLength/2 && pongPosBall.y - ballRadius <= pongPosL.y + pongLength/2) { //and also the y of the ball is inside the range of the bar
						ballBounce(); //the ball has touched the left bar and needs to bounce back
					}
					else if (pongPosBall.x - ballRadius <= -1.0f) { //else, it has passed the bar and the game restarts
						//reset parameters
						pongPosBall = glm::vec2(0.0f, 0.0f);
						pongVelBall = pongVelBallInitial;
					}
					
				}
				//Same as left bar in previous case
				if (pongPosBall.x + ballRadius >= pongPosR.x - pongWidth / 2) {
					if (pongPosBall.y + ballRadius >= pongPosR.y - pongLength / 2 && pongPosBall.y - ballRadius <= pongPosR.y + pongLength / 2) {
						ballBounce();

					}
					else if (pongPosBall.x + ballRadius >= 1.0f) {
						pongPosBall = glm::vec2(0.0f, 0.0f);
						pongVelBall = pongVelBallInitial;
					}

				}

				//Move the bars
				
				//Player 1 (left bar)
				if (glfwGetKey(window, GLFW_KEY_W) && pongPosL.y > -1.0f + pongLength/2) {
					pongPosL += glm::vec2(0, -pongVel);
				}

				if (glfwGetKey(window, GLFW_KEY_S) && pongPosL.y < 1.0f - pongLength/2) {
					pongPosL += glm::vec2(0, pongVel);
				}

				//Player 2 (right bar)
				if (glfwGetKey(window, GLFW_KEY_UP) && pongPosR.y > -1.0f + pongLength/2) {
					pongPosR += glm::vec2(0, -pongVel);
				}

				if (glfwGetKey(window, GLFW_KEY_DOWN) && pongPosR.y < 1.0f - pongLength/2) {
					pongPosR += glm::vec2(0, pongVel);
				}

				break;
		}
	}

	// Here is where you update the uniforms.
	// Very likely this will be where you will be writing the logic of your application.
	void updateUniformBuffer(uint32_t currentImage) {
		static bool debounce = false;
		static int curDebounce = 0;
		//this makes the popup comes out only if you are in the proximities of the working cabinet
		bool rangeVideogame = Pos.x < 5.5f && Pos.x > 0.0f && Pos.z < 0.0f && Pos.z > -2.5f;

		if (glfwGetKey(window, GLFW_KEY_P) && currScene == 0 && rangeVideogame || glfwGetKey(window, GLFW_KEY_P) && currScene == 1) {
			if (!debounce) {
				debounce = true;
				curDebounce = GLFW_KEY_P;
				currScene = (currScene + 1) % 2;
				std::cout << "Scene : " << currScene << "\n";
				pongPosBall = glm::vec2(0.0f, 0.0f);
				pongVelBall = pongVelBallInitial;
				pongPosR = glm::vec2(xURR - pongWidth / 2, 0.0f);
				pongPosL = glm::vec2(xULL + pongWidth / 2, 0.0f);

				RebuildPipeline();
			}
		} else {
			if ((curDebounce == GLFW_KEY_P) && debounce) {
				debounce = false;
				curDebounce = 0;
			}
		}

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) && currScene == 0) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		
		GameLogic();
		
		//Set lights parameters

		//Direct light (actually turned off for better light effects)
		gubo.DlightDir = glm::normalize(glm::vec3(0.0f, 0.0f, 0.0f));
		gubo.DlightColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

		//Ambient light color
		gubo.AmbLightColor = glm::vec3(0.05f);

		//Position of the viewer
		gubo.eyePos = Pos;

		//First point light parameters (ceiling lamp)
		gubo.PLightPos1 = glm::vec3(3.0f, 3.9f, -4.0f);
		gubo.PLightColor1 = glm::vec4(1.0f, 1.0f, 0.3f, 1.0f);

		//Second point light parameters (ceiling lamp)
		gubo.PLightPos2 = glm::vec3(11.0f, 3.9f, -4.0f);
		gubo.PLightColor2 = glm::vec4(1.0f, 1.0f, 0.3f, 1.0f);

		//Spot light parameters
		gubo.SLightDir = glm::vec3(0.0f, 1.0f, 0.0f);
		gubo.SLightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		gubo.SLightPos = glm::vec3(10.0f, 3.0f, 1.0f);

		//First lantern parameters
		gubo.PLightPosLantern1 = glm::vec3(13.0f, 2.8f, -13.0f);
		gubo.PLightColorLantern1 = glm::vec4(1.0f, 0.2f, 0.0f, 1.0f);

		//Second lantern parameters
		gubo.PLightPosLantern2 = glm::vec3(8.3f, 2.8f, -13.0f);
		gubo.PLightColorLantern2 = glm::vec4(1.0f, 0.2f, 0.0f, 1.0f);

		switch (currScene) {
		case 0:
			DSGubo.map(currentImage, &gubo, sizeof(gubo), 0);
			// the .map() method of a DataSet object, requires the current image of the swap chain as first parameter
			// the second parameter is the pointer to the C++ data structure to transfer to the GPU
			// the third parameter is its size
			// the fourth parameter is the location inside the descriptor set of this uniform block

			World = glm::scale(glm::mat4(1), glm::vec3(150.0f));

			uboSkyBox.amb = 1.0f; uboSkyBox.gamma = 180.0f; uboSkyBox.sColor = glm::vec3(1.0f); //actually useless for the skybox
            uboSkyBox.mvpMat = Prj * View * World;
            uboSkyBox.mMat = World;
            uboSkyBox.nMat = glm::inverse(glm::transpose(World));
			DSSkyBox.map(currentImage, &uboSkyBox, sizeof(uboSkyBox), 0);

			World = glm::translate(glm::mat4(1.0), glm::vec3(0.1, 0.0f, -1.2f)) *
				glm::scale(glm::mat4(1), glm::vec3(0.018f));

			uboCabinet1.amb = 1.0f; uboCabinet1.gamma = 180.0f; uboCabinet1.sColor = glm::vec3(1.0f);
			uboCabinet1.mvpMat = Prj * View * World;
			uboCabinet1.mMat = World;
			uboCabinet1.nMat = glm::inverse(glm::transpose(World));
			DSCabinet1.map(currentImage, &uboCabinet1, sizeof(uboCabinet1), 0);

			World = glm::translate(glm::mat4(1.0), glm::vec3(0.1, 0.0f, -9.2f)) *
				glm::scale(glm::mat4(1), glm::vec3(0.018f));

			uboCabinet2.amb = 1.0f; uboCabinet2.gamma = 180.0f; uboCabinet2.sColor = glm::vec3(1.0f);
			uboCabinet2.mvpMat = Prj * View * World;
			uboCabinet2.mMat = World;
			uboCabinet2.nMat = glm::inverse(glm::transpose(World));
			DSCabinet2.map(currentImage, &uboCabinet2, sizeof(uboCabinet2), 0);

			World = glm::translate(glm::mat4(1.0), glm::vec3(0.8, 0.7f, -7.2f)) *
				glm::scale(glm::mat4(1), glm::vec3(0.018f));

			uboAsteroids.amb = 1.0f; uboAsteroids.gamma = 180.0f; uboAsteroids.sColor = glm::vec3(1.0f);
			uboAsteroids.mvpMat = Prj * View * World;
			uboAsteroids.mMat = World;
			uboAsteroids.nMat = glm::inverse(glm::transpose(World));
			DSAsteroids.map(currentImage, &uboAsteroids, sizeof(uboAsteroids), 0);

			World = glm::rotate(glm::mat4(1.0), glm::radians(-90.0f), glm::vec3(0, 1, 0)) *
				glm::translate(glm::mat4(1.0), glm::vec3(-3.2, 1.7f, -0.5f)) *
				glm::scale(glm::mat4(1), glm::vec3(0.018f));

			uboBattleZone.amb = 1.0f; uboBattleZone.gamma = 180.0f; uboBattleZone.sColor = glm::vec3(1.0f);
			uboBattleZone.mvpMat = Prj * View * World;
			uboBattleZone.mMat = World;
			uboBattleZone.nMat = glm::inverse(glm::transpose(World));
			DSBattleZone.map(currentImage, &uboBattleZone, sizeof(uboBattleZone), 0);

			World = glm::rotate(glm::mat4(1.0), glm::radians(90.0f), glm::vec3(0, 1, 0)) *
				glm::translate(glm::mat4(1.0), glm::vec3(5.3f, -0.1f, 0.7f)) *
				glm::scale(glm::mat4(1), glm::vec3(0.018f));

			uboNudge.amb = 1.0f; uboNudge.gamma = 180.0f; uboNudge.sColor = glm::vec3(1.0f);
			uboNudge.mvpMat = Prj * View * World;
			uboNudge.mMat = World;
			uboNudge.nMat = glm::inverse(glm::transpose(World));
			DSNudge.map(currentImage, &uboNudge, sizeof(uboNudge), 0);

			World = glm::rotate(glm::mat4(1.0), glm::radians(-0.1f), glm::vec3(0, 1, 0)) *
				glm::translate(glm::mat4(1.0), glm::vec3(5.5f, 0.0f, -8.5f)) *
				glm::scale(glm::mat4(1), glm::vec3(0.5f));

			uboDanceDace.amb = 1.0f; uboDanceDace.gamma = 180.0f; uboDanceDace.sColor = glm::vec3(1.0f);
			uboDanceDace.mvpMat = Prj * View * World;
			uboDanceDace.mMat = World;
			uboDanceDace.nMat = glm::inverse(glm::transpose(World));
			DSDanceDance.map(currentImage, &uboDanceDace, sizeof(uboDanceDace), 0);

			World = translate(glm::mat4(1.0), glm::vec3(14.8f, 0.0f, -9.4f)) *
				glm::scale(glm::mat4(1), glm::vec3(12.0f)) *
				glm::rotate(glm::mat4(1.0), glm::radians(-90.0f), glm::vec3(0, 1, 0));

			uboSnackMachine.amb = 1.0f; uboSnackMachine.gamma = 180.0f; uboSnackMachine.sColor = glm::vec3(1.0f);
			uboSnackMachine.mvpMat = Prj * View * World;
			uboSnackMachine.mMat = World;
			uboSnackMachine.nMat = glm::inverse(glm::transpose(World));
			DSSnackMachine.map(currentImage, &uboSnackMachine, sizeof(uboSnackMachine), 0);

			World = rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0, 1, 0)) *
				translate(glm::mat4(1.0), glm::vec3(10.0f, 0.0f, 1.0f)) *
				glm::scale(glm::mat4(1), glm::vec3(1.5f));

			uboPoolTable.amb = 1.0f; uboPoolTable.gamma = 180.0f; uboPoolTable.sColor = glm::vec3(1.0f);
			uboPoolTable.mvpMat = Prj * View * World;
			uboPoolTable.mMat = World;
			uboPoolTable.nMat = glm::inverse(glm::transpose(World));
			DSPoolTable.map(currentImage, &uboPoolTable, sizeof(uboPoolTable), 0);

			World = glm::scale(glm::mat4(1), glm::vec3(0.97f, 1.0f, 1.0f)) *
				translate(glm::mat4(1.0), glm::vec3(0.33f, 0.0f, 0.0f));

			uboDoor.amb = 1.0f; uboDoor.gamma = 180.0f; uboDoor.sColor = glm::vec3(1.0f);
			uboDoor.mvpMat = Prj * View * World;
			uboDoor.mMat = World;
			uboDoor.nMat = glm::inverse(glm::transpose(World));
			DSDoor.map(currentImage, &uboDoor, sizeof(uboDoor), 0);

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

			World = translate(glm::mat4(1.0), glm::vec3(3.0f, 3.9f, -4.0f)) *
				glm::scale(glm::mat4(1), glm::vec3(0.15f, 0.15f, 0.15f));

			uboCeilingLamp1.amb = 1.0f; uboCeilingLamp1.gamma = 180.0f; uboCeilingLamp1.sColor = glm::vec3(1.0f);
			uboCeilingLamp1.mvpMat = Prj * View * World;
			uboCeilingLamp1.mMat = World;
			uboCeilingLamp1.nMat = glm::inverse(glm::transpose(World));
			DSCeilingLamp1.map(currentImage, &uboCeilingLamp1, sizeof(uboCeilingLamp1), 0);

			World = translate(glm::mat4(1.0), glm::vec3(11.0f, 3.9f, -4.0f)) *
				glm::scale(glm::mat4(1), glm::vec3(0.15f, 0.15f, 0.15f));

			uboCeilingLamp2.amb = 1.0f; uboCeilingLamp2.gamma = 180.0f; uboCeilingLamp2.sColor = glm::vec3(1.0f);
			uboCeilingLamp2.mvpMat = Prj * View * World;
			uboCeilingLamp2.mMat = World;
			uboCeilingLamp2.nMat = glm::inverse(glm::transpose(World));
			DSCeilingLamp2.map(currentImage, &uboCeilingLamp2, sizeof(uboCeilingLamp2), 0);

			World = translate(glm::mat4(1.0), glm::vec3(10.0f, 3.0f, 1.0f)) *
				glm::scale(glm::mat4(1), glm::vec3(2.0f, 2.0f, 2.0f));

			uboPoolLamp.amb = 1.0f; uboPoolLamp.gamma = 180.0f; uboPoolLamp.sColor = glm::vec3(1.0f);
			uboPoolLamp.mvpMat = Prj * View * World;
			uboPoolLamp.mMat = World;
			uboPoolLamp.nMat = glm::inverse(glm::transpose(World));
			DSPoolLamp.map(currentImage, &uboPoolLamp, sizeof(uboPoolLamp), 0);

			uboPopup.visible = (rangeVideogame) ? 1.0f : 0.0f;
			DSPopup.map(currentImage, &uboPopup, sizeof(uboPopup), 0);
			
			World = rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0, 1, 0)) *
			translate(glm::mat4(1.0), glm::vec3(-2.3f, 2.8f, 10.5f)) *
			glm::scale(glm::mat4(1), glm::vec3(1.2f, 1.0f, 0.25f));

			uboBanner.amb = 1.0f; uboBanner.gamma = 180.0f; uboBanner.sColor = glm::vec3(1.0f);
			uboBanner.mvpMat = Prj * View * World;
			uboBanner.mMat = World;
			uboBanner.nMat = glm::inverse(glm::transpose(World));
			DSBanner.map(currentImage, &uboBanner, sizeof(uboBanner), 0);

			World = glm::scale(glm::mat4(1), glm::vec3(160.0f, 1.0f, 160.0f));

			uboWorldFloor.amb = 1.0f; uboWorldFloor.gamma = 180.0f; uboWorldFloor.sColor = glm::vec3(1.0f);
			uboWorldFloor.mvpMat = Prj * View * World;
			uboWorldFloor.mMat = World;
			uboWorldFloor.nMat = glm::inverse(glm::transpose(World));
			DSWorldFloor.map(currentImage, &uboWorldFloor, sizeof(uboWorldFloor), 0);

			World = rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0, 1, 0)) *
				translate(glm::mat4(1.0), glm::vec3(-13.0f, 2.8f, 11.0f)) *
				glm::scale(glm::mat4(1), glm::vec3(0.7f));

			uboJLantern1.amb = 1.0f; uboJLantern1.gamma = 180.0f; uboJLantern1.sColor = glm::vec3(1.0f);
			uboJLantern1.mvpMat = Prj * View * World;
			uboJLantern1.mMat = World;
			uboJLantern1.nMat = glm::inverse(glm::transpose(World));
			DSJLantern1.map(currentImage, &uboJLantern1, sizeof(uboJLantern1), 0);

			World = rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0, 1, 0)) *
				translate(glm::mat4(1.0), glm::vec3(-8.3f, 2.8f, 11.0f)) *
				glm::scale(glm::mat4(1), glm::vec3(0.7f));

			uboJLantern2.amb = 1.0f; uboJLantern2.gamma = 180.0f; uboJLantern2.sColor = glm::vec3(1.0f);
			uboJLantern2.mvpMat = Prj * View * World;
			uboJLantern2.mMat = World;
			uboJLantern2.nMat = glm::inverse(glm::transpose(World));
			DSJLantern2.map(currentImage, &uboJLantern2, sizeof(uboJLantern2), 0);

			break;
		case 1:
			uboPong.pPos = glm::vec2(0.0, pongPosR.y);
			DSPongR.map(currentImage, &uboPong, sizeof(uboPong), 0);

			uboPong.pPos = glm::vec2(0.0, pongPosL.y);
			DSPongL.map(currentImage, &uboPong, sizeof(uboPong), 0);

			uboPong.pPos = pongPosBall;
			DSPongBall.map(currentImage, &uboPong, sizeof(uboPong), 0);
			
			uboPongNet.visible = 1.0f;
			DSPongNet.map(currentImage, &uboPongNet, sizeof(uboPongNet), 0);

			break;
		}
		
	}
};

// This is the main: probably you do not need to touch this!
int main() {
	SpaceArcade app;

	try {
		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
