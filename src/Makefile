
BASEDIR = ../
SOURCES = cbir_server.cpp featureloader.cpp embeded_web_server.cpp cbir_config.cpp
          
DLIB = $(BASEDIR)dlib-17.12/dlib/all/source.cpp
YAMLLIB = $(BASEDIR)yaml-cpp-read-only/src/*.cpp

ANNINCDIR = $(BASEDIR)ann_1.1.1/include
DLIBINCDIR = $(BASEDIR)dlib-17.12
YAMLINCDIR = $(BASEDIR)yaml-cpp-read-only/include

ANNLIBDIR = $(BASEDIR)ann_1.1.1/lib

LIB = -lANN -lpthread

LIBDIR = -L$(ANNLIBDIR)
INCDIR = -I$(ANNINCDIR) -I$(DLIBINCDIR) -I$(YAMLINCDIR) -I$(BASEDIR)include

ARGS = -Wall -ggdb -DDEBUG -DDLIB_NO_GUI_SUPPORT

cbir_server: $(SOURCES) $(DLIB) $(YAMLLIB)
	g++ -o cbir_server_test $(SOURCES) $(DLIB) $(YAMLLIB) $(LIBDIR) \
	 $(INCDIR) $(LIB) $(ARGS)
	mv cbir_server_test $(BASEDIR)		
