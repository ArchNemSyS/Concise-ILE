#include <QCoreApplication>
#include "concisedb.h"
#include "httplistener.h"
#include "requestmapper.h"

#include <QSettings>
#include <QFile>
#include <QDir>

#include <QDebug>
#include "qstdout.h"

QString searchConfigFile() {
  QString binDir  = QCoreApplication::applicationDirPath();
  QString appName = QCoreApplication::applicationName();
  QFile   file;

  file.setFileName(binDir + "/Concise.ini");

  if (!file.exists()) {
    file.setFileName(binDir + "/etc/Concise.ini");

    if (!file.exists()) {
      file.setFileName(binDir + "/../etc/Concise.ini");

      if (!file.exists()) {
        file.setFileName(binDir + "/../" + appName + "/etc/Concise.ini");

        if (!file.exists()) {
          file.setFileName(binDir + "/../../" + appName + "/etc/Concise.ini");

          if (!file.exists()) {
            file.setFileName(
              binDir + "/../../../../../" + appName + "/etc/Concise.ini");

            if (!file.exists()) {
              file.setFileName(QDir::rootPath() + "etc/Concise.ini");
            }
          }
        }
      }
    }
  }

  if (file.exists()) {
    QString configFileName = QDir(file.fileName()).canonicalPath();
    qDebug("using config file %s", qPrintable(configFileName));
    return configFileName;
  }
  else {
    qFatal("config file not found");
  }
}

int main(int argc, char *argv[])
{
  QCoreApplication app(argc, argv);
  QString configFileName = searchConfigFile();

  QSettings *databaseSettings = new QSettings(configFileName,
                                              QSettings::IniFormat,
                                              &app);

  databaseSettings->beginGroup("database");
  ConciseDB db(databaseSettings, &app);

  // Static file controller
  QSettings *fileSettings = new QSettings(configFileName,
                                          QSettings::IniFormat,
                                          &app);
  fileSettings->beginGroup("files");
  RequestMapper::static_file_controller = new StaticFileController(fileSettings,
                                                                   &app);

  // Session store
  QSettings *session_settings = new QSettings(configFileName,
                                              QSettings::IniFormat,
                                              &app);
  session_settings->beginGroup("sessions");
  RequestMapper::session_store = new HttpSessionStore(session_settings, &app);

  // Session store
  QSettings *reset_session_settings = new QSettings(configFileName,
                                                    QSettings::IniFormat,
                                                    &app);
  reset_session_settings->beginGroup("reset_sessions");
  RequestMapper::reset_session_store = new HttpSessionStore(
    reset_session_settings,
    &app);

  // Configure template cache
  QSettings *template_settings = new QSettings(configFileName,
                                               QSettings::IniFormat,
                                               &app);
  template_settings->beginGroup("templates");
  RequestMapper::template_cache = new TemplateCache(template_settings, &app);

  // HTTP server
  QSettings *listener_settings = new QSettings(configFileName,
                                               QSettings::IniFormat,
                                               &app);
  listener_settings->beginGroup("listener");
  new HttpListener(listener_settings, new RequestMapper(db, &app), &app);

  return app.exec();
}
