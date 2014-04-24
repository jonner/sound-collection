#include <gom/gom.h>
#include "sound-collection-app.h"
#include "recording-resource.h"
#include "GRefPtr.h"

struct SoundCollectionApp::Priv
{
    Priv()
        : status(0)
        , mainloop(g_main_loop_new(0, FALSE))
        , adapter(gom_adapter_new())
    {
    }
    
    int status;
    WTF::GRefPtr<GFile> db;
    WTF::GRefPtr<GMainLoop> mainloop;
    WTF::GRefPtr<GomAdapter> adapter;
    WTF::GRefPtr<GomRepository> repository;
};


SoundCollectionApp::SoundCollectionApp()
    : m_priv(new Priv())
{
}


static gchar* filename;
static GOptionEntry entries[] =
{
    { "db", 'd', G_OPTION_FLAG_FILENAME, G_OPTION_ARG_FILENAME, filename, "database", 0 },
};

int SoundCollectionApp::run(int argc, char** argv)
{
    GOptionContext* option_context = g_option_context_new("");
    GError* error = 0;
    if (!g_option_context_parse(option_context, &argc, &argv, &error))
    {
        g_warning(error->message);
        g_error_free(error);
    }
    
    g_debug("argc = %i", argc);
    if (argc > 1)
        m_priv->db = g_file_new_for_path(argv[1]);
    else
        m_priv->db = g_file_new_for_path("sound-collection.sqlite");
        
    char* uri = g_file_get_uri(m_priv->db.get());
    g_debug("Opening db %s...", uri);
    
    gom_adapter_open_async(m_priv->adapter.get(), uri, SoundCollectionApp::adapter_open_ready, this);
    g_free(uri);
        
    g_main_loop_run(m_priv->mainloop.get());
    return m_priv->status;
}

void SoundCollectionApp::quit()
{
    g_main_loop_quit(m_priv->mainloop.get());
}


void SoundCollectionApp::adapter_open_ready(GObject* source_object, GAsyncResult* res, gpointer user_data)
{
    g_debug("%s", G_STRFUNC);
    SoundCollectionApp *self = static_cast<SoundCollectionApp*>(user_data);
    GError* error = 0;
    if (!gom_adapter_open_finish(reinterpret_cast<GomAdapter*>(source_object), res, &error))
    {
        g_warning("failed to open adapter: %s", error->message);
        g_error_free(error);
        self->m_priv->status = -1;
        self->quit();
    }
    else
        g_debug("Opened adapter");
        
    self->m_priv->repository = gom_repository_new(self->m_priv->adapter.get());
    GList* types = g_list_prepend(0, GINT_TO_POINTER(SC_TYPE_RECORDING_RESOURCE));
    gom_repository_automatic_migrate_async(self->m_priv->repository.get(), 1, types,
        SoundCollectionApp::repository_migrate_finished, self);
}

void SoundCollectionApp::repository_migrate_finished(GObject* source_object, GAsyncResult* res, gpointer user_data)
{
    g_debug("%s", G_STRFUNC);
    SoundCollectionApp* self = static_cast<SoundCollectionApp*>(user_data);
    GError* error = 0;
    if (!gom_repository_automatic_migrate_finish(reinterpret_cast<GomRepository*>(source_object), res, &error))
    {
        g_warning("failed to migrate repository: %s", error->message);
        g_error_free(error);
        self->m_priv->status = -1;
        self->quit();
    }
    else
        g_debug("Repository migrated");
        
    self->show();
}

void SoundCollectionApp::show()
{
    //for now, just exit
    quit();
}
