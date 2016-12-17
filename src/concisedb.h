#ifndef CONCISEDB_H
#define CONCISEDB_H
#include <QObject>

// forwards
class QSqlDatabase;
class QSqlQuery;
class QSettings;

class ConciseDB : public QObject {
  Q_OBJECT

public:

  ConciseDB(QSettings *settings,
            QObject   *parent = NULL);
  ~ConciseDB();

  QString get_database_path() const {
    return m_database_path;
  }

  QSqlDatabase& get_database() const {
    return *m_db;
  }

  bool is_connected() const;

signals:

public slots:

private:

  QSettings *m_settings;
  QString m_database_path;
  QString m_schema_path;
  QSqlDatabase *m_db;

  bool open_database(QString qstrDatabaseFilePath);
  bool connect_database(QString qstrDatabaseFilePath);
  bool init_database();
};

#endif // CONCISEDB_H
