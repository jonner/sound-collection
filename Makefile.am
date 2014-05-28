noinst_LIBRARIES = libcore.a libui.a

libcore_a_SOURCES = \
                    src/GRefPtr.cpp \
                    src/GRefPtr.h \
                    src/equipment-resource.c \
                    src/equipment-resource.h \
                    src/identification-resource.c \
                    src/identification-resource.h \
                    src/location.cc \
                    src/location.h \
                    src/location-resource.c \
                    src/location-resource.h \
                    src/recording.cc \
                    src/recording.h \
                    src/recording-resource.c \
                    src/recording-resource.h \
                    src/species-resource.c \
                    src/species-resource.h \
                    src/task.cc \
                    src/task.h \
                    src/util.cc \
                    src/util.h \
                    $(NULL)

libcore_a_CFLAGS = \
                   $(CORE_CFLAGS) \
                   $(NULL)

libcore_a_CXXFLAGS = $(libcore_a_CFLAGS)

libui_a_SOURCES = \
                  src/application.cc \
                  src/application.h \
                  src/header-label.cc \
                  src/header-label.h \
                  src/import-dialog.h \
                  src/import-dialog.cc \
                  src/location-form.cc \
                  src/location-form.h \
                  src/location-window.cc \
                  src/location-window.h \
                  src/main-window.cc \
                  src/main-window.h \
                  src/recording-form.cc \
                  src/recording-form.h \
                  src/recording-tree-model.cc \
                  src/recording-tree-model.h \
                  src/recording-tree-view.cc \
                  src/recording-tree-view.h \
                  src/recording-window.cc \
                  src/recording-window.h \
                  src/resource-edit-form.h \
                  src/resource-edit-window.cc \
                  src/resource-edit-window.h \
                  src/simple-audio-player.cc \
                  src/simple-audio-player.h \
                  $(NULL)

libui_a_CFLAGS = \
                 $(CORE_CFLAGS) \
                 $(NULL)

libui_a_CXXFLAGS = $(libui_a_CFLAGS)

bin_PROGRAMS = sound-collection

sound_collection_SOURCES = \
                           src/main.cc \
                           $(NULL)

sound_collection_CFLAGS = \
                            $(CORE_CFLAGS) \
                            $(NULL)

sound_collection_CXXFLAGS = $(sound_collection_CFLAGS)

sound_collection_LDADD = \
                         libui.a \
                         libcore.a \
                         $(CORE_LIBS) \
                         $(NULL)

noinst_PROGRAMS = test-audio-player test-recording-window test-location-window

test_CFLAGS = \
              $(CORE_CFLAGS) \
              -I$(top_srcdir)/src \
              $(NULL)

test_LIBS = \
            libui.a \
            libcore.a \
            $(CORE_LIBS) \
            $(NULL)

test_audio_player_CXXFLAGS = $(test_CFLAGS)
test_audio_player_LDADD = $(test_LIBS)
test_audio_player_SOURCES = \
                            test/test-audio-player.cc \
                            $(NULL)

test_recording_window_CXXFLAGS = $(test_CFLAGS)
test_recording_window_LDADD = $(test_LIBS)
test_recording_window_SOURCES = \
                                test/test-recording-window.cc \
                                $(NULL)

test_location_window_CXXFLAGS = $(test_CFLAGS)
test_location_window_LDADD = $(test_LIBS)
test_location_window_SOURCES = \
                                test/test-location-window.cc \
                                $(NULL)
