#include "stafftemplates.h"
#include "requestmapper.h"
#include "template.h"
#include "concisedb.h"

#include <QSqlQuery>
#include <ctime>

StaffTemplates::StaffTemplates(const ConciseDB& DB, QObject *parent)
  : HttpRequestHandler(parent), m_db(DB) {
  // m_query = new QSqlQuery(DB.getDatabase());
}

void StaffTemplates::service(HttpRequest& request, HttpResponse& response) {
  HttpSession session = RequestMapper::session_store->getSession(request,
                                                                 response,
                                                                 true);
  QByteArray language = request.getHeader("Accept-Language");
  QByteArray path     = request.getPath();

  response.setHeader("Content-Type", "text/html; charset=UTF-8");
  Template t =
    RequestMapper::template_cache->getTemplate("/MainWindow", language);
  t.setVariable("domain", "staff");

  if (path == "/staff/courses") {
    t.setVariable("nav-bar", "<a>Courses</a>");
    t.setVariable("main",    courses_query(session.get("username").toString()));
  }
  else if (path == "/staff/database") {
    t.setVariable("nav-bar", "<a>Database</a>");
    t.setVariable("main",    table_browser());
  }
  else {
    // staff logged-in 404
    // response.setStatus(404, "Not found");
    // response.write("Staff Login - URL is wrong, no such document.");
    response.redirect("/staff/courses");
    return;
  }

  response.write(t.toUtf8(), true);
}

QByteArray StaffTemplates::courses_query(QString teacher_code)
{
  QSqlQuery query(m_db.get_database());

  query.prepare(
    "SELECT Courses.course_ID,Courses.faculty,Courses.year,Courses.subject,TeachingGroups.class_code "
    "from Courses INNER JOIN TeachingGroups ON TeachingGroups.course_ID = Courses.course_ID "
    "WHERE teacher =:code"
    "ORDER BY Courses.subject, Courses.year DESC");
  query.bindValue(":code", teacher_code.constData());
  query.exec();

  QByteArray courses;
  QByteArray class_code;
  int course_ID = 0;
  QByteArray subject;

  while (query.next()) {
    class_code = query.value(4).toByteArray();
    int cID = query.value(0).toInt();

    if (course_ID != cID) {
      course_ID = cID;

      if (query.at() != 0) courses += ("</dl>");

      if (query.value(3).toByteArray() != subject) {
        subject  = query.value(3).toByteArray();
        courses += ("<br>");
      }

      courses +=  "<dl class=\"border-around link\">"
                  "<a  href=" + query.value(0).toByteArray() + ">"
                                                               "<dt>" +
                 query.value(2).toByteArray() + "-" + subject + "</dt></a>"
                                                                "<a href="
                 + class_code + ">"
                                "<dd>"
                 + class_code + "</dd></a>";
    }
    else courses +=  "<a href=" + class_code + ">"
                                               "<dd>" + class_code + "</dd></a>";
  }
  courses += ("</dl>");


  return courses;
}

QByteArray StaffTemplates::table_browser()
{
  QStringList db_tables = m_db.get_database().tables();

  db_tables.removeOne("sqlite_sequence");
  QByteArray output;

  for (int i = 0; i < db_tables.length(); i++) {
    output += "<dl class=\"border-around\"><a href=" + db_tables.at(i) + "><dt>" +
              db_tables.at(i) + "</dt></a></dl>";
  }

  return output;
}
