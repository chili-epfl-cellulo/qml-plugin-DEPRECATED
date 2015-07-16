#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDateTime>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QDebug>
#include <QDir>

QFile logFile;

void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg){
    Q_UNUSED(context);

    QString txt = QString("%1 ").arg(QDateTime::currentMSecsSinceEpoch());

    switch (type)
    {
    case QtDebugMsg:
        txt += QString(" %1").arg(msg);
        break;
    case QtWarningMsg:
        txt += QString("[WW] %1").arg(msg);
        break;
    case QtCriticalMsg:
        txt += QString("[EE] %1").arg(msg);
        break;
    case QtFatalMsg:
        txt += QString("[EE!] %1").arg(msg);
        abort();
        break;
    }

    logFile.open(QIODevice::WriteOnly | QIODevice::Append);

    QTextStream textStream(&logFile);
    textStream << txt << endl;

    logFile.close();
}

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QString path = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QDir dir(path);
    dir.mkdir(path);
    path += "/cellulo_localization_analysis.log";
    logFile.setFileName(path);
    qWarning() << QString("Logging everything to %1").arg(path);
    qInstallMessageHandler(customMessageHandler);

    QQmlApplicationEngine engine;

    engine.load(QUrl(QStringLiteral("qrc:///src/main.qml")));

    return app.exec();
}
