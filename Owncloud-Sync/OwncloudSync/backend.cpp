#include <QtQml>
#include <QtQml/QQmlContext>
#include "backend.h"
//#include "mytype.h"
#include "servicecontrol.h"


void BackendPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("OwncloudSync"));

    qmlRegisterType<ServiceControl>(uri, 1, 0, "ServiceController");
}

void BackendPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    QQmlExtensionPlugin::initializeEngine(engine, uri);
}

