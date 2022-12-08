CC = g++
CXX_FLAGS = -std=c++11 -w 

BIN = UNIX-FILESYS

$(shell mkdir obj)

$(BIN): obj/main.o obj/BufferManager.o obj/DiskDriver.o obj/File.o obj/FileManager.o obj/FileSystem.o obj/INode.o obj/Kernel.o obj/OpenFileManager.o obj/Shell.o obj/User.o obj/Utility.o
	$(CC) -o $@ $^
obj/main.o: main.cpp
	$(CC) $(CXX_FLAGS) -c -o $@ $<
obj/BufferManager.o: BufferManager.cpp
	$(CC) $(CXX_FLAGS) -c -o $@ $<
obj/DiskDriver.o: DiskDriver.cpp
	$(CC) $(CXX_FLAGS) -c -o $@ $<
obj/File.o: File.cpp
	$(CC) $(CXX_FLAGS) -c -o $@ $<
obj/FileManager.o: FileManager.cpp
	$(CC) $(CXX_FLAGS) -c -o $@ $<
obj/FileSystem.o: FileSystem.cpp
	$(CC) $(CXX_FLAGS) -c -o $@ $<
obj/INode.o: INode.cpp
	$(CC) $(CXX_FLAGS) -c -o $@ $<
obj/Kernel.o: Kernel.cpp
	$(CC) $(CXX_FLAGS) -c -o $@ $<
obj/OpenFileManager.o: OpenFileManager.cpp
	$(CC) $(CXX_FLAGS) -c -o $@ $<
obj/Shell.o: Shell.cpp
	$(CC) $(CXX_FLAGS) -c -o $@ $<
obj/User.o: User.cpp
	$(CC) $(CXX_FLAGS) -c -o $@ $<
obj/Utility.o: Utility.cpp
	$(CC) $(CXX_FLAGS) -c -o $@ $<
	
.phony:
clean:
	rm -f *.o $(BIN)
	rm -f *.img
	rm -f obj
	
