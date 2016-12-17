#ifndef LOGINCONTROLLER_H
#define LOGINCONTROLLER_H


#include "httprequesthandler.h"
class ConciseDB;
class HttpSession;

class LoginController : public HttpRequestHandler {
  Q_OBJECT

public:

  LoginController(const ConciseDB& DB,
                  QObject         *parent = 0);

  void login_service(HttpRequest & request,
                     HttpResponse& response,
                     QByteArray    domain,
                     QByteArray    path);

  void logout_service(HttpRequest & request,
                      HttpResponse& response,
                      QByteArray    user_type);

  void token_service(HttpRequest & request,
                     HttpResponse& response,
                     QByteArray    domain);

  void reset_service(HttpRequest & request,
                     HttpResponse& response,
                     QByteArray    domain);

signals:

public slots:

private:

  bool authenticate(QByteArray username,
                    QByteArray password,
                    QByteArray domain);

  void login(HttpSession session,
             QByteArray  domain,
             QByteArray  username);

  const ConciseDB& m_db;
};

#endif // LOGINCONTROLLER_H
