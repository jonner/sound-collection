bin_PROGRAMS = sound-collection test-audio-player

sound_collection_SOURCES = \
                           src/application.cc \
                           src/application.h \
                           src/GRefPtr.cpp \
                           src/GRefPtr.h \
                           src/identification-resource.c \
                           src/identification-resource.h \
                           src/main.cc \
                           src/main-window.cc \
                           src/main-window.h \
                           src/recording-resource.c \
                           src/recording-resource.h \
                           src/recording-tree-model.cc \
                           src/recording-tree-model.h \
                           src/recording-tree-view.cc \
                           src/recording-tree-view.h \
                           src/species-resource.c \
                           src/species-resource.h \
                           $(NULL)

sound_collection_CFLAGS = \
                            $(CORE_CFLAGS) \
                            $(NULL)

sound_collection_CXXFLAGS = $(sound_collection_CFLAGS)

sound_collection_LDADD = \
                         $(CORE_LIBS) \
                         $(NULL)


test_audio_player_CXXFLAGS = \
                             $(CORE_CFLAGS) \
                             -I$(top_srcdir)/src \
                             $(NULL)

test_audio_player_LDADD = \
                          $(CORE_LIBS) \
                          $(NULL)

test_audio_player_SOURCES = \
                            src/simple-audio-player.cc \
                            src/simple-audio-player.h \
                            test/test-audio-player.cc \
                            $(NULL)

