#include "requestmapper.h"
#include "logincontroller.h"
#include "studenttemplates.h"
#include "stafftemplates.h"

#include <QHostAddress>
#include <QTime>
#include <QHash>
#include <QMap>
#include <ctime>

HttpSessionStore *RequestMapper::session_store              = 0;
HttpSessionStore *RequestMapper::reset_session_store        = 0;
StaticFileController *RequestMapper::static_file_controller = 0;
TemplateCache *RequestMapper::template_cache                = 0;

RequestMapper::RequestMapper(const ConciseDB& DB,
                             QObject         *parent) : HttpRequestHandler(parent),
  m_db(DB)
{
  m_login_controller  = new LoginController(m_db, this);
  m_student_templates = new StudentTemplates(m_db, this);
  m_staff_templates   = new StaffTemplates(m_db, this);
}

void
RequestMapper::service(HttpRequest& request, HttpResponse& response)
{
  QByteArray path = request.getPath();

  qDebug("RequestMapper: path=%s", path.data());

  if (path.startsWith("/files")) {
    qDebug("/files Get Static File");
    static_file_controller->service(request, response);
    return;
  }


  else if (path.startsWith("/reset"))   {
    HttpSession reset_session = reset_session_store->getSession(request,
                                                                response,
                                                                false);
    QByteArray domain;

    if (path == "/reset/student")
    {
      domain = "student";
    }
    else if (path == "/reset/staff")
    {
      domain = "staff";
    }

    QByteArray username = reset_session.get(domain).toByteArray();

    if (username.isEmpty())
    {
      m_login_controller->token_service(request, response, domain);
      return;
    }
    else {
      m_login_controller->reset_service(request, response, domain);
      return;
    }

    return;
  }
  else if (path.startsWith("/student"))   {
    if (route(request, response, path, "student"))
    {
      m_student_templates->service(request, response);
      return;
    }
  }
  else if (path.startsWith("/staff"))   {
    if (route(request, response, path, "staff"))
    {
      m_staff_templates->service(request, response);
      return;
    }
  }
  else {
    // response.setStatus(404,"Not found");
    // response.write("The URL is wrong, no such document.");
    // Custom 404
    response.redirect("/files/index.html");
    return;
  }
}

bool RequestMapper::route(HttpRequest & request,
                          HttpResponse& response,
                          QByteArray    path,
                          QByteArray    domain)
{
  HttpSession session = session_store->getSession(request, response,
                                                  false);
  QByteArray username = session.get(domain).toByteArray();

  if (username.isEmpty())
  {
    m_login_controller->login_service(request, response, domain, path);
    return false;
  }
  else if (path.startsWith("/" + domain + "/files")) {
    static_file_controller->service(request, response);
    return false;
  }
  else if (path.startsWith("/" + domain + "/logout")) {
    m_login_controller->logout_service(request, response, domain);
    return false;
  }
  return true;
}
