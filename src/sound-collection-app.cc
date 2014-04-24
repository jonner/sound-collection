#include <gom/gom.h>
#include <gio/gio.h>
#include <gtkmm.h>
#include "sound-collection-app.h"
#include "recording-resource.h"
#include "GRefPtr.h"

struct SoundCollectionApp::Priv
{
    Priv()
        : status(0)
    {
    }
    
    int status;
    Glib::RefPtr<Gio::File> db;
    WTF::GRefPtr<GomAdapter> adapter;
    WTF::GRefPtr<GomRepository> repository;
    Gtk::ApplicationWindow window;
};

Glib::RefPtr<SoundCollectionApp> SoundCollectionApp::create()
{
    return Glib::RefPtr<SoundCollectionApp>(new SoundCollectionApp());
}

SoundCollectionApp::SoundCollectionApp()
    : Gtk::Application("org.quotidian.SoundCollection", Gio::APPLICATION_HANDLES_COMMAND_LINE)
    , m_priv(new Priv())
{
}

int SoundCollectionApp::on_command_line(const Glib::RefPtr<Gio::ApplicationCommandLine>& command_line)
{
    int argc = 0;
    char** argv = command_line->get_arguments(argc);
    g_debug("Got commandline: %i elements", argc);
    
        g_debug("argc = %i", argc);
    if (argc > 1)
        m_priv->db = command_line->create_file_for_arg(argv[1]);
    else
        m_priv->db = command_line->create_file_for_arg("sound-collection.sqlite");

    std::string uri = m_priv->db->get_uri();
    g_debug("Opening db %s...", uri.c_str());

    m_priv->adapter = gom_adapter_new();
    gom_adapter_open_async(m_priv->adapter.get(), uri.c_str(), SoundCollectionApp::adapter_open_ready, this);

    g_strfreev(argv);
    
    add_window(m_priv->window);
    return 0;
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
    m_priv->window.show();
}
