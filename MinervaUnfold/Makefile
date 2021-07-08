CXX = g++
CXXFLAGS = -g -Wall -fPIC
ROOTFLAGS = `root-config --cflags --glibs`
CFLAGS = -Wall -fPIC
OBJS = MnvUnfold.o MnvResponse.o
SRC = MnvUnfold.cxx MnvResponse.cxx
INCLUDE += -I$(UNFOLDUTILSROOT)/ -I$(PLOTUTILSROOT)/
LDFLAGS += -shared -Wl,-soname

TARGET = libUnfoldUtils.so

$(TARGET): $(OBJS) $(ROOTDICTCXX)
	$(CXX) $(LDFLAGS),$(TARGET) -o $(TARGET) $(OBJS) $(ROOTFLAGS)
	mv $(TARGET) ../${CMTCONFIG}

$(OBJS): %.o: %.cxx 
	$(CXX) $(INCLUDE) $(CXXFLAGS) $(ROOTFLAGS) -c $< -o $@ 

clean:
	rm $(UNFOLDUTILSROOT)/$(CMTCONFIG)/$(TARGET)
	rm *.o
