#include "studenttemplates.h"
#include "requestmapper.h"
#include "template.h"
#include "concisedb.h"

#include <QSqlQuery>

StudentTemplates::StudentTemplates(const ConciseDB& DB, QObject *parent)
  : HttpRequestHandler(parent), m_db(DB) {}

void StudentTemplates::service(HttpRequest& request, HttpResponse& response) {
  HttpSession session = RequestMapper::session_store->getSession(request,
                                                                 response,
                                                                 true);
  QByteArray language = request.getHeader("Accept-Language");
  QByteArray path     = request.getPath();

  response.setHeader("Content-Type", "text/html; charset=UTF-8");
  Template t =
    RequestMapper::template_cache->getTemplate("/MainWindow", language);
  t.setVariable("domain", "student");


  if (path == "/student/courses") {
    t.setVariable("nav-bar", "<a>Courses</a>");
    t.setVariable("main",    courses_query(session.get("username").toString()));
  }
  else {
    // student home
    response.redirect("/student/courses");
    return;
  }
  response.write(t.toUtf8(), true);
}

QByteArray StudentTemplates::courses_query(QString username)
{
  QSqlQuery query(m_db.get_database());

  query.prepare(
    "SELECT class_code "
    "from Enrolment "
    "WHERE username =:code");
  query.bindValue(":code", username.constData());
  query.exec();

  QByteArray courses;
  courses += "<dl>";

  while (query.next()) {
    courses += "<a href=" + query.value(0).toByteArray() + "><dd>" +
               query.value(0).toByteArray() + "</dd></a>";
  }
  courses += "</dl>";

  return courses;
}
