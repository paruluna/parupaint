#include "parupaintConnection.h"
#include <QStringBuilder>
#include <QJsonDocument>
#include <QDebug>

ParupaintConnection::ParupaintConnection(QWsSocket * s) : socket(s), id(0)
{
}

qint64 ParupaintConnection::send(const QString id, const QJsonObject &obj)
{
	return this->send(id, QJsonDocument(obj).toJson(QJsonDocument::Compact));
}

qint64 ParupaintConnection::send(const QString id, const QString msg)
{
	if(!this->socket) return 0;
	return socket->sendTextMessage(id % " " % msg);
}
void ParupaintConnection::setId(sid id)
{
	this->id = id;
}
sid ParupaintConnection::getId() const
{
	return this->id;
}

QWsSocket * ParupaintConnection::getSocket()
{
	return this->socket;
}
