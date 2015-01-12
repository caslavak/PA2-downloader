PROGRAM=cechjak2
CXX=g++
CFLAGS=-std=c++11 -Wall -pedantic -Wno-long-long -O0 -ggdb -pthread
BUILD_DIR=build
OFILES=$(BUILD_DIR)/Balast.o $(BUILD_DIR)/Downloader.o $(BUILD_DIR)/Filemanager.o $(BUILD_DIR)/main.o $(BUILD_DIR)/Tag.o
DIR=mkdir -p $(BUILD_DIR);
.PHONY: all clean compile run doc

$(PROGRAM): $(OFILES)
	$(CXX) $(CFLAGS) -o $@ $(OFILES)

all: $(PROGRAM) doc

compile: $(PROGRAM)
  
clean:
	rm -Rf $(PROGRAM) $(BUILD_DIR)/ doc/;

run:
	./$(PROGRAM) -p -o download

doc:
	doxygen ./src/Doxyfile




#*****************************#


$(BUILD_DIR)/Balast.o: src/Balast.cpp src/Balast.h
	$(DIR)
	$(CXX) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/Downloader.o: src/Downloader.cpp src/Downloader.h src/Structures.h src/Exception.h src/Filemanager.h src/Balast.h src/Tag.h
	$(DIR)
	$(CXX) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/Filemanager.o: src/Filemanager.cpp src/Filemanager.h src/Exception.h src/Balast.h
	$(DIR)
	$(CXX) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/main.o: src/main.cpp src/Downloader.h src/Structures.h src/Exception.h src/Filemanager.h src/Balast.h src/Tag.h
	$(DIR)
	$(CXX) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/Tag.o: src/Tag.cpp src/Tag.h src/Downloader.h src/Structures.h src/Exception.h src/Filemanager.h src/Balast.h
	$(DIR)
	$(CXX) $(CFLAGS) -c -o $@ $<

