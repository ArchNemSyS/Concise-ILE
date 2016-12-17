#ifndef STAFFTEMPLATES_H
#define STAFFTEMPLATES_H
#include "httprequesthandler.h"
class ConciseDB;
class HttpSession;

class StaffTemplates : public HttpRequestHandler {
public:

  StaffTemplates(const ConciseDB& DB,
                 QObject         *parent = 0);

  void service(HttpRequest & request,
               HttpResponse& response);

signals:

public slots:

private:

  const ConciseDB& m_db;

  QByteArray courses_query(QString teacher_code);
  QByteArray table_browser();
};

#endif // STAFFTEMPLATES_H
