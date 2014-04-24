#ifndef _APP_H
#define _APP_H

#include <tr1/memory>
#include <glib-object.h>
#include <gio/gio.h>

class SoundCollectionApp
{
    public:
    SoundCollectionApp();
    
    int run(int argc, char** argv);
    void quit();

    static void adapter_open_ready(GObject* source_object, GAsyncResult* res, gpointer user_data);
    static void repository_migrate_finished(GObject* source_object, GAsyncResult* res, gpointer user_data);

    private:
    void show();
    struct Priv;
    std::tr1::shared_ptr<Priv> m_priv;
};

#endif // _APP_H
