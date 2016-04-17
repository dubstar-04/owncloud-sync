#include <QtQml>
#include <QtQml/QQmlContext>
#include "backend.h"
#include "mytype.h"


void BackendPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("OC_Sync"));

    qmlRegisterType<MyType>(uri, 1, 0, "MyType");
}

void BackendPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    QQmlExtensionPlugin::initializeEngine(engine, uri);
}

