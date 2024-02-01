// SPDX-FileCopyrightText: 2024 Sirius Bakke <sirius@bakke.co>
// SPDX-License-Identifier: Apache-2.0

#include <QStringList>
#include <QTextStream>
#include <QDateTime>
#include <QFile>
#include <QBuffer>
#include <QUrl>
#include "httphandler.h"
#include "alkomanager.h"
#include "alkoserialrequest.h"
#include "../common/alkoserialcommands.h"
#include "../common/alkostates.h"
#include "config.h"

static void generatePrettyHtmlLog(QTextStream& os);
static void generatePrettyHtmlLogObject(QString& logLine, QTextStream& os);
static void htmlParser(const QMap<QString, QString>& context, const QString& fileName, QTextStream& os);

HttpHandler::HttpHandler(QTcpSocket *socket, AlkoManager *manager, QObject *parent) :
    QObject(parent), m_socket(socket), m_manager(manager)
{
    connect(m_socket, &QTcpSocket::disconnected, this, &HttpHandler::deleteLater);
    connect(m_socket, &QTcpSocket::readyRead, this, &HttpHandler::readyRead);
    m_socketOutputStream.setAutoDetectUnicode(true);
}

void HttpHandler::readyRead()
{
    m_socketOutputStream.setDevice(m_socket);

    if (m_socket->canReadLine()) {

        QString method, resource;
        bool authenticated = false;
        QMap<QString, QString> headers;
        QMultiMap<QString, QString> tokens;
        QMap<QString, QString> context;

        QString line = m_socket->readLine();

        if (line.indexOf("GET") == 0) {
            method = "GET";
            resource = line.mid(5, line.length() - 16).trimmed();
        } else if (line.indexOf("POST") == 0) {
            method = "POST";
            resource = line.mid(6, line.length() - 16).trimmed();
        } else {
            m_socket->close();
            if (m_socket->state() == QTcpSocket::UnconnectedState) {
                delete m_socket;
            }
            return;
        }

        if (resource.isEmpty())
            resource = "index.html";

        while (m_socket->canReadLine()) {
            // Read headers
            line = m_socket->readLine();
            int index = line.indexOf(QRegExp(":"));
            headers[line.left(index).toLower()] = line.mid(index + 1).trimmed();
        }

        if (!headers.contains("host")) {
            error_400();
            return;
        }

        // Check authentication
        if (headers.value("authorization") == ALKO_CONFIGURATION__HTTP_AUTH)
            authenticated = true;

        if (!authenticated) {
            m_socketOutputStream << "HTTP/1.1 401 Unauthorized\r\n"
                  "Content-Type: text/html; charset=\"utf-8\"\r\n"
                  "Connection: close\r\n"
                  "WWW-Authenticate: Basic realm=\"Manager\"\r\n"
                  "\r\n"
                  "<html>\n"
                  "<head>\n"
                  "<link rel=\"shortcut icon\" href=\"data:image/x-icon;,\" type=\"image/x-icon\">\n"
                  "<title>401 Authorization Required</title>"
                  "</head>\n"
                  "<body>\n"
                  "<h1>Authorization Required</h1>\n"
                  "<p>This server could not verify that you "
                  "are authorized to access the document "
                  "requested.  Either you supplied the wrong"
                  "credentials (e.g., bad password), or your "
                  "browser doesn't understand how to supply "
                  "the credentials required.</p>\n"
                  "</body>\n"
                  "</html>\n";

            m_socketOutputStream.flush();

            m_socket->close();
            if (m_socket->state() == QTcpSocket::UnconnectedState) {
                delete m_socket;
            }
        }

        if (!QFile::exists(QString(":/html/%1").arg(resource))) {
            error_404();
            return;
        }

        if (resource.right(3) == "css") {
            m_socketOutputStream << "HTTP/1.0 200 Ok\r\n"
                                    "Content-Type: text/css; charset=\"utf-8\"\r\n"
                                    "Connection: close\r\n"
                                    "\r\n";
            QFile htmlFile(QString(":/html/%1").arg(resource));
            htmlFile.open(QIODevice::ReadOnly | QIODevice::Text);
            m_socketOutputStream << htmlFile.readAll();
            m_socketOutputStream.flush();
            m_socket->close();
            if (m_socket->state() == QTcpSocket::UnconnectedState) {
                delete m_socket;
            }
            return;
        }

        m_socketOutputStream << "HTTP/1.0 200 Ok\r\n"
              "Content-Type: text/html; charset=\"utf-8\"\r\n"
              "Connection: close\r\n"
              "\r\n";


        context.insert("DATETIME", QDateTime::currentDateTime().toString());
        context.insert("RESOURCE", resource);
        context.insert("HOST", headers.value("host"));

        if (method == "POST") {
            if (m_socket->size()) {
                // Get tokens

                while (m_socket->bytesAvailable()) {
                    line = m_socket->readLine();
                    if (!line.isEmpty()) {
                        int index = -1;
                        foreach (QString token, line.split('&')) {
                            if ((index = token.indexOf(QRegExp("="))))
                                tokens.insert(token.left(index),
                                              QUrl::fromPercentEncoding(token.mid(index + 1).replace('+', ' ').trimmed().toLocal8Bit()));
                        }
                    }
                }
            }

            QString action = tokens.value("AlkoAction");

            if (!action.isEmpty()) {

                context.insert("ACTION", action);
                resource = "process.html";

                if (action == "Start") {
                    AlkoSerialRequest request;
                    request.setData(QByteArray().append(Alko::Cmd::SET_STATE).append(Alko::State::PREPARE_READY));
                    m_manager->get(request);
                } else if (action == "Stop") {
                    AlkoSerialRequest request;
                    request.setData(QByteArray().append(Alko::Cmd::SET_STATE).append(Alko::State::SERVICE));
                    m_manager->get(request);
                } else if (action == "Check") {
                    AlkoSerialRequest request;
                    request.setData(QByteArray().append(Alko::Cmd::CHECK_AVAIL));
                    m_manager->get(request);
                } else if (action == "State") {
                    AlkoSerialRequest request;
                    request.setData(QByteArray().append(Alko::Cmd::GET_STATE));
                    m_manager->get(request);
                } else if (action == "Fetch") {
                    AlkoSerialRequest request;
                    request.setData(QByteArray().append(Alko::Cmd::FETCH));
                    m_manager->get(request);
                } else if (action == "AutoHome") {
                    AlkoSerialRequest request;
                    request.setData(QByteArray().append(Alko::Cmd::HOME_BEGIN));
                    m_manager->get(request);
                } else if (action == "CancelAutoHome") {
                    AlkoSerialRequest request;
                    request.setData(QByteArray().append(Alko::Cmd::HOME_ABORT));
                    m_manager->get(request);
                }
            }

        } else {
            // GET
            if (resource == "status.html") {
                // GET STATE and STATUS
                // Update context

                // TODO: Implement status view
                context.insert("ALKO_COLUMN_9", "btn-danger");
                context.insert("ALKO_COLUMN_8", "btn-success");
                context.insert("ALKO_COLUMN_7", "btn-success");
                context.insert("ALKO_COLUMN_6", "btn-success");
                context.insert("ALKO_COLUMN_5", "btn-success");
                context.insert("ALKO_COLUMN_4", "btn-danger");
                context.insert("ALKO_COLUMN_3", "btn-warning");
                context.insert("ALKO_COLUMN_2", "btn-danger");
                context.insert("ALKO_COLUMN_1", "btn-danger");
                context.insert("ALKO_STATE_BTN", "btn-success");
                context.insert("ALKO_STATE_TEXT", "READY");
            } else if (resource == "check_status.html") {
                AlkoSerialRequest request1, request2;
                request1.setData(QByteArray().append(Alko::Cmd::GET_CONFIG));
                request2.setData(QByteArray().append(Alko::Cmd::GET_STATE));
                m_manager->get(request1);
                m_manager->get(request2);
            }
        }

        htmlParser(context, resource, m_socketOutputStream);

        m_socket->close();
        if (m_socket->state() == QTcpSocket::UnconnectedState) {
            delete m_socket;
        }

    }
}

static void htmlParser(const QMap<QString, QString> &context, const QString& fileName, QTextStream& os)
{
    QFile htmlFile(QString(":/html/%1").arg(fileName));
    htmlFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QString htmlLine;
    QRegExp rx("\\{\\$(\\S+)(?:\\s(\\S+))?\\}");

    while (htmlFile.bytesAvailable()) {
        htmlLine = htmlFile.readLine();
        int pos = 0;

        while ((pos = rx.indexIn(htmlLine, pos)) != -1) {
            if (rx.cap(1) == "PRINT_RAW_LOG") {
                htmlLine = "";
                QFile logFile(ALKO_CONFIGURATION__LOG_FILE);
                if (logFile.open(QIODevice::ReadOnly | QIODevice::Text))
                    os << "<pre>\n" << logFile.readAll() << "\n</pre>\n";
            } else if (rx.cap(1) == "PRINT_LOG") {
                htmlLine = "";
                generatePrettyHtmlLog(os);
            } else if (rx.cap(1) == "INCLUDE") {
                QString filePath = rx.cap(2);
                QFile includeFile(QString(":/html/%1").arg(filePath));
                if (includeFile.exists()) {
                    htmlParser(context, filePath, os);
                    htmlLine = "";
                }
            } else {
                htmlLine.replace(QString("{$%1}").arg(rx.cap(1)), context.value(rx.cap(1)));
            }
            pos += rx.matchedLength();
        }

        os << htmlLine;
    }

    htmlFile.close();
}


void generatePrettyHtmlLog(QTextStream& os)
{
    const int bufferSize = 2048;

    QByteArray buffer;
    QTextStream bufferStream(&buffer);
    bufferStream.setAutoDetectUnicode(true);

    QFile logFile(ALKO_CONFIGURATION__LOG_FILE);
    if (!logFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    if (logFile.size() > bufferSize)
        logFile.seek(logFile.size() - bufferSize);
    else
        buffer.resize(logFile.size());

    bufferStream << logFile.readAll();
    logFile.close();
    bufferStream.seek(0);

    int pos = -1;

    QString logLine;

    while ((pos = buffer.lastIndexOf('\n', pos - 1)) > 0) {
        bufferStream.seek(pos + 1);
        logLine = bufferStream.readLine();
        generatePrettyHtmlLogObject(logLine, os);
        bufferStream.seek(pos - 2);
    }

    if (buffer.size() < bufferSize) {
        bufferStream.seek(0);
        logLine = bufferStream.readLine();
        generatePrettyHtmlLogObject(logLine, os);
    }

}

void generatePrettyHtmlLogObject(QString& logLine, QTextStream& os)
{
    int pos = logLine.lastIndexOf('[');
    QString id = logLine.mid(pos + 7, 4);
    QString command = logLine.mid(pos + 15, 2);
    QString data;

    switch (command.toInt(0, 16)) {
    case Alko::Cmd::PING:
        if (logLine.lastIndexOf("RECV") != -1) // Ignore ping from dixie
            return;
        command = "PING";
        break;
    case Alko::Cmd::PONG:
        return;
        break;
    case Alko::Cmd::BUSY:
        command = "BUSY";
        break;
    case Alko::Cmd::READY:
        command = "READY";
        break;
    case Alko::Cmd::SERVICE:
        command = "SERVICE";
        break;
    case Alko::Cmd::GET_CONFIG:
        command = "GET_CONFIG";
        break;
    case Alko::Cmd::SET_CONFIG:
        command = "SET_CONFIG";
        break;
    case Alko::Cmd::GET_STATE:
        command = "GET_STATE";
    {
        bool ok;
        data = logLine.mid(pos + 17, 2);
        int state = data.toInt(&ok, 16);
        if (!ok) {
            data = "";
            break;
        }
        switch (state) {
        case Alko::State::SERVICE:
            data = "State: SERVICE";
            break;
        case Alko::State::PREPARE_READY:
            data = "State: PREPARE_READY";
            break;
        case Alko::State::READY:
            data = "State: READY";
            break;
        case Alko::State::DEPLOY:
            data = "State: DEPLOY";
            break;
        case Alko::State::DEPLOY_ERROR:
            data = "State: DEPLOY_ERROR";
            break;
        case Alko::State::PREPARE_AUTO_HOME:
            data = "State: PREPARE_AUTO_HOME";
            break;
        case Alko::State::AUTO_HOME:
            data = "State: AUTO_HOME";
            break;
        case Alko::State::INFO:
            data = "State: INFO";
            break;
        }
    }
        break;
    case Alko::Cmd::SET_STATE:
        command = "SET_STATE";
        {
            data = logLine.mid(pos + 17, 2);
            switch (data.toInt(0, 16)) {
            case Alko::State::SERVICE:
                data = "State: SERVICE";
                break;
            case Alko::State::READY:
            case Alko::State::PREPARE_READY:
                data = "State: PREPARE_READY";
                break;
            }
        }
        break;
    case Alko::Cmd::HOMING:
        command = "HOMING";
        break;
    case Alko::Cmd::HOME_ABORT:
        command = "HOME_ABORT";
        break;
    case Alko::Cmd::HOME_BEGIN:
        command = "HOME_BEGIN";
        break;
    case Alko::Cmd::CHECK_AVAIL:
        command = "CHECK_AVAIL";
        break;
    case Alko::Cmd::DRINK_AVAIL:
        command = "DRINK_AVAIL";
        break;
    case Alko::Cmd::EMPTY:
        command = "EMPTY";
        break;
    case Alko::Cmd::FETCH:
        command = "FETCH";
        break;
    case Alko::Cmd::SET_TEXT_0:
        command = "SET_TEXT_0";
        break;
    case Alko::Cmd::SET_TEXT_1:
        command = "SET_TEXT_1";
        break;
    case Alko::Cmd::SET_TEXT_2:
        command = "SET_TEXT_2";
        break;
    case Alko::Cmd::SET_TEXT_3:
        command = "SET_TEXT_3";
        break;
    case Alko::Cmd::UNKNOWN_CMD:
        command = "UNKNOWN_CMD";
        break;
    case Alko::Cmd::CHKSUM_INVALID:
        command = "CHKSUM_INVALID";
        break;
    default:
        command = "Other command: " + command;
        break;
    }

    if (logLine.lastIndexOf("customer-service:") != -1) {
        QString customerName = logLine.mid(58);
        os << "<p>" << logLine.left(23) << "<br />"
           << "<button type=\"button\" class=\"btn btn-xs btn-info\">"
           << "<span class=\"glyphicon glyphicon-user\"><span/> " << customerName
           << "</button><br /></p>\n";
        return;
    }

    os << "<p>" << logLine.left(23) << "<br />"
       << "<button type=\"button\" class=\"btn btn-xs btn-";
    if (logLine.lastIndexOf("SEND") != -1)
        os << "primary";
    else
        os << "success";
    os  << "\"><span class=\"glyphicon glyphicon-transfer\"><span/> "
        << command << ", id(" << id.toInt(0, 16) << ")";
    if (!data.isEmpty())
        os << " data(" << data << ")";
    os << "</button><br /></p>\n";
}

void HttpHandler::error_400()
{
    m_socketOutputStream << "HTTP/1.1 400 Bad Request\r\n"
                            "Content-Type: text/html; charset=\"utf-8\"\r\n"
                            "Connection: close\r\n"
                            "\r\n";

    QFile htmlFile(":/html/400.html");
    htmlFile.open(QIODevice::ReadOnly | QIODevice::Text);
    m_socketOutputStream << htmlFile.readAll();
    m_socketOutputStream.flush();

    m_socket->close();
    if (m_socket->state() == QTcpSocket::UnconnectedState) {
        delete m_socket;
    }
}

void HttpHandler::error_404()
{
    m_socketOutputStream << "HTTP/1.1 404 Not Found\r\n"
                            "Content-Type: text/html; charset=\"utf-8\"\r\n"
                            "Connection: close\r\n"
                            "\r\n";

    QFile htmlFile(":/html/404.html");
    htmlFile.open(QIODevice::ReadOnly | QIODevice::Text);
    m_socketOutputStream << htmlFile.readAll();
    m_socketOutputStream.flush();

    m_socket->close();
    if (m_socket->state() == QTcpSocket::UnconnectedState) {
        delete m_socket;
    }
}
