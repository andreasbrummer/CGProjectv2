VULKAN_SDK_PATH = /home/user/VulkanSDK/x.x.x.x/x86_64
STB_INCLUDE_PATH = /home/andreas/Desktop/CGProjectv2/headers
TINYOBJ_INCLUDE_PATH = /home/andreas/Desktop/CGProjectv2/headers
TINYGLTF_INCLUDE_PATH = /home/andreas/Desktop/CGProjectv2/headers
CFLAGS = -std=c++17 -O2 -I$(STB_INCLUDE_PATH) -I$(TINYOBJ_INCLUDE_PATH) -I$(TINYGLTF_INCLUDE_PATH) -I./models
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

A16: A16.o
	g++ $(CFLAGS) -o A16 A16.o $(LDFLAGS)

A16.o: A16.cpp Starter.hpp  
	g++ $(CFLAGS) -c A16.cpp -o A16.o

.PHONY: test clean

test: A16
	./A16

clean:
	rm -f A16 A16.o

