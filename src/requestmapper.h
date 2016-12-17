 #ifndef REQUESTMAPPER_H
#define REQUESTMAPPER_H

#include "httprequesthandler.h"
#include "httpsessionstore.h"
#include "staticfilecontroller.h"
#include "templatecache.h"

class ConciseDB;
class LoginController;
class StaffTemplates;
class StudentTemplates;

class RequestMapper : public HttpRequestHandler {
  Q_OBJECT

public:

  RequestMapper(const ConciseDB& DB,
                QObject         *parent = 0);

  void service(HttpRequest & request,
               HttpResponse& response);

  static HttpSessionStore *session_store;
  static HttpSessionStore *reset_session_store;
  static StaticFileController *static_file_controller;
  static TemplateCache *template_cache;

signals:

public slots:

private:

  const ConciseDB& m_db;
  LoginController *m_login_controller;
  StaffTemplates *m_staff_templates;
  StudentTemplates *m_student_templates;

  bool route(HttpRequest & request,
             HttpResponse& response,
             QByteArray    path,
             QByteArray    domain);
};

#endif // REQUESTMAPPER_H
