TARGET	= robust_ocr

CXX = g++
CFLAGS =  -Wall -O3 -g -DNDEBUG -std=c++11
LDFLAGS = -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_features2d -ltesseract -llept


SRC_DIR = src
SOURCES =$(SRC_DIR)/main.cpp


BUILD_DIR = build
OBJECTS = $(SOURCES:%.cpp=$(BUILD_DIR)/%.o)
DEPS = $(SOURCES:%.cpp=$(BUILD_DIR)/%.d)


all: $(TARGET)



SUB_DIRS:= $(shell find $(SRC_DIR) -type d)

dirs:
	mkdir -p $(BUILD_DIR);
	for val in $(SUB_DIRS);do \
   		mkdir -p  $(BUILD_DIR)/$${val};\
	done;

$(TARGET):dirs	$(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS)  $(LIBS)  $(LIB_DIRS) -o $(addprefix $(BUILD_DIR)/,$@)
	cp build/$(TARGET)  .

$(BUILD_DIR)/%.o:%.cpp
		$(CXX) $(CFLAGS) -fmessage-length=0  -MMD -MP -MF"$(@:%.o=%.d)"  -c $< -o $@

#depency rules
-include $(DEPS)
$(BUILD_DIR)/%.d:./%.cpp
		@set -e; rm -f $@; \
		$(CXX) -M $(CFLAG) $< >; $@.$$$$; \
		sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ >; $@; \
		rm -f $@.$$$$

clean:
	rm -fr build

.PHONY: all clean
