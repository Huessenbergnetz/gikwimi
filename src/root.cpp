#include "root.h"

#include <QCoreApplication>
#include <Cutelyst/Application>

using namespace Cutelyst;

Root::Root(QObject *parent) : Controller(parent)
{
}

Root::~Root()
{
}

void Root::index(Context *c)
{
    QString body;
    body = QStringLiteral("Hello World!");
    c->response()->setBody(body);
    c->response()->setContentType(QLatin1String("text/html; charset=utf-8"));
}

void Root::defaultPage(Context *c)
{
    c->response()->setBody(QStringLiteral("Page not found!"));
    c->response()->setStatus(404);
}

