#ifndef _APP_H
#define _APP_H

#include <gio/gio.h>
#include <glib-object.h>
#include <gom/gom.h>
#include <gtkmm.h>
#include <tr1/memory>

class SoundCollectionApp : public Gtk::Application {
 public:
  static Glib::RefPtr<SoundCollectionApp> create();

 private:
  SoundCollectionApp();
  static void adapter_open_ready_proxy(GObject* source_object,
                                 GAsyncResult* res,
                                 gpointer user_data);
  void adapter_open_ready(GomAdapter* adapter, GAsyncResult* res);
  static void repository_migrate_finished_proxy(GObject* source_object,
                                          GAsyncResult* res,
                                          gpointer user_data);
  void repository_migrate_finished(GomRepository* repository,
                                   GAsyncResult* res);

  virtual int on_command_line(
      const Glib::RefPtr<Gio::ApplicationCommandLine>& command_line);
  void show();
  struct Priv;
  std::tr1::shared_ptr<Priv> m_priv;
};

#endif  // _APP_H
