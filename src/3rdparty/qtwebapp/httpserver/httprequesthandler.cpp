/**
  @file
  @author Stefan Frings
*/

#include "httprequesthandler.h"

using namespace stefanfrings;

HttpRequestHandler::HttpRequestHandler(QObject * parent)
    : QObject{parent}
{}

HttpRequestHandler::~HttpRequestHandler()
{}

void HttpRequestHandler::service(HttpRequest& request, HttpResponse & response)
{
    Q_UNUSED(request)
    Q_UNUSED(response)
    qCritical("HttpRequestHandler: you need to override the service() function");
    //XDebug2("HttpRequestHandler: request=%s %s %s",request.getMethod().data(),request.getPath().data(),request.getVersion().data());
    response.setStatus(501,"not implemented");
    response.write("501 not implemented",true);
}
