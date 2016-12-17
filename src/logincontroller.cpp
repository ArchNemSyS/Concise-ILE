#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>

#include "logincontroller.h"
#include "requestmapper.h"
#include "template.h"
#include "concisedb.h"

extern "C" {
#include <sodium.h>
}

LoginController::LoginController(const ConciseDB& DB, QObject *parent) :
  HttpRequestHandler(parent),
  m_db(DB)
{
  // initialize sodium crypto library
  if (sodium_init() == -1) qFatal("Crypto Failure");

  // RESET TOKEN
  QByteArray salt;
  salt.resize(crypto_pwhash_SALTBYTES);

  // unsigned char salt[crypto_pwhash_SALTBYTES];
  randombytes_buf(salt.data(), crypto_pwhash_SALTBYTES);
  salt = salt.toPercentEncoding();

  QByteArray url("http://localhost:8080/reset/staff?");
  url.append("&token=" + salt);

  qDebug() << url;
  QSqlQuery query(m_db.get_database());
  query.prepare("UPDATE 'Teachers' SET reset_token=:token, token_timestamp=:time "
                " WHERE username=:user");
  query.bindValue(":token", salt);
  query.bindValue(":time",  QDateTime::currentMSecsSinceEpoch());
  query.bindValue(":user",  "ZZZ");

  qDebug() << query.lastQuery();

  if (!query.exec()) qDebug() << query.lastError().text();


    #define PASSWORD "Correct Horse Battery Staple"
  char hashed_password[crypto_pwhash_STRBYTES];

  if (crypto_pwhash_str
        (hashed_password, PASSWORD, strlen(PASSWORD),
        crypto_pwhash_OPSLIMIT_INTERACTIVE,
        crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0)
  {
    /* out of memory */
  } else {
    query.prepare("UPDATE 'Teachers' SET password=:password_hash "
                  " WHERE username=:user");
    query.bindValue(":password_hash", hashed_password);
    query.bindValue(":user",          "ZZZ");

    qDebug() << query.lastQuery();

    if (!query.exec()) qDebug() << query.lastError().text();
  }

  if (crypto_pwhash_str
        (hashed_password, PASSWORD, strlen(PASSWORD),
        crypto_pwhash_OPSLIMIT_INTERACTIVE,
        crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0)
  {
    /* out of memory */
  } else {
    query.prepare("UPDATE 'Students' SET password=:password_hash "
                  " WHERE username=:user");
    query.bindValue(":password_hash", hashed_password);
    query.bindValue(":user",          "00.user");

    qDebug() << query.lastQuery();

    if (!query.exec()) qDebug() << query.lastError().text();
  }
}

void LoginController::login_service(HttpRequest & request,
                                    HttpResponse& response,
                                    QByteArray    domain,
                                    QByteArray    path) {
  HttpSession session = RequestMapper::session_store->getSession(request,
                                                                 response,
                                                                 true);
  QByteArray language = request.getHeader("Accept-Language");
  QByteArray username = request.getParameter("username");
  QByteArray password = request.getParameter("password");
  bool login_failed   = false;

  qDebug() << "Login " << path;

  if (!session.contains(domain))
  {
    if (!username.isEmpty() && !password.isEmpty()) {
      // Authenticate user
      if (authenticate(username, password, domain))
      {
        login(session, domain, username);
        response.redirect(path);
        return;
      }

      // INVAILD user_type or authentication
      login_failed = true;
    }

    response.setHeader("Content-Type", "text/html; charset=UTF-8");
    Template t = RequestMapper::template_cache->getTemplate("/login", language);
    t.setVariable("action", path);
    t.setCondition("logged-in",  false);
    t.setCondition("retry",      login_failed);
    t.setCondition("logged-out", false);
    response.write(t.toUtf8(), true);
  }
  else response.redirect(domain);
}

void LoginController::logout_service(HttpRequest & request,
                                     HttpResponse& response,
                                     QByteArray    user_type
                                     )
{
  HttpSession session = RequestMapper::session_store->getSession(request,
                                                                 response,
                                                                 false);
  QByteArray language = request.getHeader("Accept-Language");

  response.setHeader("Content-Type", "text/html; charset=UTF-8");
  Template t = RequestMapper::template_cache->getTemplate("/login", language);

  if (session.contains(user_type)) {
    RequestMapper::session_store->removeSession(session);
    t.setCondition("logged-in",  false);
    t.setCondition("retry",      false);
    t.setCondition("logged-out", true);
  } else {
    t.setCondition("logged-in",  false);
    t.setCondition("retry",      false);
    t.setCondition("logged-out", false);
  }
  response.write(t.toUtf8(), true);
}

bool LoginController::authenticate(QByteArray username,
                                   QByteArray password,
                                   QByteArray domain) {
  qDebug() << "Authenticate user " << " | " << username << " | " << password <<
    " | " << domain;

  QSqlQuery query(m_db.get_database());

  if (domain == "student")   {
    query.prepare(
      ("SELECT username,password FROM 'Students' WHERE username=:user"));
  }

  if (domain == "staff")   {
    query.prepare(
      ("SELECT username,password FROM 'Teachers' WHERE username=:user"));
  }

  query.bindValue(":user", username.constData());
  query.exec();


  if (query.next() &&
      (crypto_pwhash_str_verify(query.value(1).toByteArray(),
                                password.data(),
                                password.length()) == 0)) return true;

  return false;
}

void LoginController::login(HttpSession session,
                            QByteArray  domain,
                            QByteArray  username) {
  session.set(domain,       username);
  session.set("login_time", QTime::currentTime());
}

void LoginController::token_service(HttpRequest & request,
                                    HttpResponse& response,
                                    QByteArray    domain)
{
  QByteArray language = request.getHeader("Accept-Language");
  QByteArray username = request.getParameter("username");
  QByteArray token    = request.getParameter("token");

  qDebug() << "staffResetService";


  if (token.isEmpty()) {
    response.redirect("/files/index.html");
    return;
  } else if (username.isEmpty()) {
    // QByteArray quote("'");
    // token = quote + token + quote;
    Template t = RequestMapper::template_cache->getTemplate("/reset",
                                                            language);

    t.setVariable("action", request.getPath());
    t.setVariable("token",  token.toPercentEncoding());
    t.setCondition("user_token", false);
    response.write(t.toUtf8(), true);
    return;
  } else {
    QSqlQuery query(m_db.get_database());

    if (domain == "student")   {
      query.prepare(
        (
          "SELECT username,reset_token,token_timestamp FROM 'Students' WHERE username=:user"));
    }

    if (domain == "staff")   {
      query.prepare(
        (
          "SELECT username,reset_token,token_timestamp FROM 'Teachers' WHERE username=:user"));
    }

    query.bindValue(":user", username.constData());
    qDebug() << query.lastQuery();

    if (!query.exec()) qDebug() << query.lastError().text();


    if (query.next() && (token == query.value(1).toByteArray()) &&
        ((QDateTime::currentMSecsSinceEpoch()
          - query.value(2).toLongLong()) < 3600000))
    {
      qDebug() << " Valid Token "  << username << " | " << token;

      // Valid Reset Token
      HttpSession reset_session =
        RequestMapper::reset_session_store->getSession(
          request,
          response,
          true);
      reset_session.set(domain,      username);
      reset_session.set("logintime", QTime::currentTime());
      response.redirect(request.getPath());
      return;
    }
    else
    {
      response.redirect("/files/index.html");
      return;
    }
  }
}

void LoginController::reset_service(HttpRequest & request,
                                    HttpResponse& response,
                                    QByteArray    domain)
{
  QByteArray language = request.getHeader("Accept-Language");
  QByteArray password = request.getParameter("password");

  if (!password.isEmpty()) {
    // Reset
    HttpSession reset_session = RequestMapper::reset_session_store->getSession(
      request,
      response,
      true);
    QByteArray username = reset_session.get(domain).toByteArray();

    qDebug() << "RESET " << username << " | " << password;
  } else {
    QByteArray seed;
    seed.resize(crypto_pwhash_SALTBYTES);

    // unsigned char salt[crypto_pwhash_SALTBYTES];
    randombytes_buf(seed.data(), crypto_pwhash_SALTBYTES);

    Template t = RequestMapper::template_cache->getTemplate("/reset", language);
    t.setVariable("action", "'/reset/staff'");
    t.setVariable("seed",   seed.toPercentEncoding());
    t.setCondition("user_token", true);
    response.write(t.toUtf8(), true);
  }
}
