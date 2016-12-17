#include "concisedb.h"
#include <QtSql>
#include <QFile>
#include <QSettings>
#include <QStringList>


#include <QDebug>


ConciseDB::ConciseDB(QSettings *settings, QObject *parent)
  : QObject(parent)
{
  m_settings      = settings;
  m_database_path = settings->value("filepath", "/Concise.db").toString();
  m_schema_path   = settings->value("schemafilepath", "/init.sql").toString();
  m_db            = NULL;

  connect_database(m_database_path);
}

ConciseDB::~ConciseDB()
{
  delete m_db;
  m_db = NULL;
}

bool ConciseDB::is_connected() const
{
  return m_db->isOpen();
}

bool ConciseDB::open_database(QString database_path)
{
  m_db  = new QSqlDatabase;
  *m_db = QSqlDatabase::addDatabase("QSQLITE");
  m_db->setDatabaseName(database_path);

  if (!m_db->open())
  {
    qDebug() << m_db->lastError().text();
    m_db = NULL;
    qFatal("unable to open Database");
    return false;
  }
  else
  {
    QSqlQuery Query(*m_db);
    Query.exec("PRAGMA foreign_keys = true");
    qDebug() << "Database Connected";
    return true;
  }
}

bool ConciseDB::connect_database(QString database_path)
{
  QFile file;

  file.setFileName(database_path);

  if (file.exists()) {
    qDebug() << "File Exists";
    open_database(database_path);
    return true;
  }
  else if (!init_database())
  {
    init_database();
    qDebug() << "File Not Found"; // -> OFFLINE
    return false;
  }
  return true;
}

bool ConciseDB::init_database()
{
  QFile file(m_schema_path);

  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return false;

  QString schema                = file.readAll().simplified();
  QStringList schema_statements = schema.split(";", QString::SkipEmptyParts);

  m_db  = new QSqlDatabase;
  *m_db = QSqlDatabase::addDatabase("QSQLITE");
  m_db->setDatabaseName(m_database_path);

  if (!m_db->open())
  {
    qDebug() << m_db->lastError().text();
    m_db = NULL;
    qFatal("unable to create Database");
    return false;
  }

  QSqlQuery Query(*m_db);
  foreach(QString sql_statement, schema_statements)
  {
    qDebug() << sql_statement << endl << endl;
    Query.exec(sql_statement);

    if (Query.lastError().isValid())
    {
      qDebug() << "init failed " << Query.lastError().text();
      return false;
    }
  }

  qDebug() << "Database init";
  return true;
}
