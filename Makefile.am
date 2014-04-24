bin_PROGRAMS = sound-collection

sound_collection_SOURCES = \
                           src/main.cc \
                           src/GRefPtr.h \
                           src/GRefPtr.cpp \
                           src/recording-resource.c \
                           src/recording-resource.h \
                           src/sound-collection-app.cc \
                           src/sound-collection-app.h \
                           $(NULL)

sound_collection_CFLAGS = \
                            $(CORE_CFLAGS) \
                            $(NULL)

sound_collection_CXXFLAGS = $(sound_collection_CFLAGS)

sound_collection_LDADD = \
                         $(CORE_LIBS) \
                         $(NULL)
