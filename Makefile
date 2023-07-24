VULKAN_SDK_PATH = /home/user/VulkanSDK/x.x.x.x/x86_64
STB_INCLUDE_PATH = /home/andreas/Desktop/CGProjectv2/headers
TINYOBJ_INCLUDE_PATH = /home/andreas/Desktop/CGProjectv2/headers
TINYGLTF_INCLUDE_PATH = /home/andreas/Desktop/CGProjectv2/headers
CFLAGS = -std=c++17 -O2 -I$(STB_INCLUDE_PATH) -I$(TINYOBJ_INCLUDE_PATH) -I$(TINYGLTF_INCLUDE_PATH) -I./models
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

SpaceArcade: SpaceArcade.o
	g++ $(CFLAGS) -o SpaceArcade SpaceArcade.o $(LDFLAGS)

SpaceArcade.o: SpaceArcade.cpp Starter.hpp  
	g++ $(CFLAGS) -c SpaceArcade.cpp -o SpaceArcade.o

.PHONY: test clean

test: SpaceArcade
	./SpaceArcade

clean:
	rm -f SpaceArcade SpaceArcade.o

