#ifndef STUDENTTEMPLATES_H
#define STUDENTTEMPLATES_H
#include "httprequesthandler.h"
class ConciseDB;

class StudentTemplates : public HttpRequestHandler {
public:

  StudentTemplates(const ConciseDB& DB,
                   QObject         *parent = 0);
  void service(HttpRequest & request,
               HttpResponse& response);

signals:

public slots:

private:

  const ConciseDB& m_db;
  QByteArray courses_query(QString username);
};

#endif // STUDENTTEMPLATES_H
