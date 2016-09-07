/*
The MIT License (MIT)

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial
portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "modelserialisation.h"
#include <QAbstractItemModel>
#include <QBitArray>
#include <QDataStream>
#include <QDateTime>
#include <QFile>
#include <QSaveFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
namespace ModelSerialisation {

    QString variantToString(const QVariant& val)
    {
        QString result;
        QByteArray data;
        QDataStream outStream(&data, QIODevice::WriteOnly);
        outStream << val;
        data = qCompress(data);
        for (const char* i = data.constBegin(); i != data.constEnd(); ++i) {
            const QString tempString = QString::number(*reinterpret_cast<const unsigned char*>(i), 16);
            Q_ASSERT(tempString.size() == 2 || tempString.size() == 1);
            if (tempString.size() == 1)
                result += '0';
            result += tempString;
        }
        Q_ASSERT(result.size() % 2 == 0);
        return result;
    }

    QVariant stringToVariant(const QString& val)
    {
        QByteArray data;
        for (int i = 0; i < val.size(); i += 2) {
            const unsigned char unsignedData = static_cast<unsigned char>(val.midRef(i, 2).toInt(NULL, 16));
            data.append(*reinterpret_cast<const char*>(&unsignedData));
        }
        data = qUncompress(data);
        QDataStream inStream(data);
        QVariant result;
        inStream >> result;
        return result;
    }


    QVariant loadVariant(int type, const QString& val)
    {
        if (val.isEmpty())
            return QVariant();
        switch (type) {
        case QMetaType::UnknownType: return QVariant();
        case QMetaType::Bool: return val.toInt() == 1;
        case QMetaType::Long:
        case QMetaType::Int: return val.toInt();
        case QMetaType::ULong:
        case QMetaType::UInt: return val.toUInt();
        case QMetaType::LongLong: return val.toLongLong();
        case QMetaType::ULongLong: return val.toULongLong();
        case QMetaType::Double: return val.toDouble();
        case QMetaType::Short: return static_cast<short>(val.toInt());
        case QMetaType::SChar:
        case QMetaType::Char: return static_cast<char>(val.toInt());
        case QMetaType::UShort: return static_cast<unsigned short>(val.toUInt());
        case QMetaType::UChar: return static_cast<unsigned char>(val.toUInt());
        case QMetaType::Float: return val.toFloat();
        case QMetaType::QString: return val;
        case QMetaType::QDate: return QDate::fromString(val, Qt::ISODate);
        case QMetaType::QTime: return QTime::fromString(val, Qt::ISODate);
        case QMetaType::QDateTime: return QDateTime::fromString(val, Qt::ISODate);
        default:
            return stringToVariant(val);
        }
    }
    QString saveVariant(const QVariant& val)
    {
        if (val.isNull())
            return QString();
        switch (val.type()) {
        case QMetaType::UnknownType: return QString();
        case QMetaType::Bool: val.toBool() ? QStringLiteral("1") : QStringLiteral("0");
        case QMetaType::Long:
        case QMetaType::Short:
        case QMetaType::Char:
        case QMetaType::SChar:
        case QMetaType::Int: return QString::number(val.toInt());
        case QMetaType::ULong:
        case QMetaType::UShort:
        case QMetaType::UChar:
        case QMetaType::UInt: return QString::number(val.toUInt());
        case QMetaType::LongLong: return QString::number(val.toLongLong());
        case QMetaType::ULongLong:  return QString::number(val.toULongLong());
        case QMetaType::Double:  return QString::number(val.toDouble(), 'f', 15);
        case QMetaType::Float: return QString::number(val.toDouble(), 'f', 7);
        case QMetaType::QString: return val.toString();
        case QMetaType::QDate: return val.toDate().toString(Qt::ISODate);
        case QMetaType::QTime: return val.toTime().toString(Qt::ISODate);
        case QMetaType::QDateTime: return val.toDateTime().toString(Qt::ISODate);
        default:
            return variantToString(val);
        }
    }

    void writeElement(QXmlStreamWriter& destination, const QAbstractItemModel* const model, const QList<int>& rolesToSave, const QModelIndex& parent = QModelIndex())
    {
        if (model->rowCount(parent) + model->columnCount(parent) == 0)
            return;
        destination.writeStartElement(QStringLiteral("Element"));
        destination.writeAttribute(QStringLiteral("RowCount"), QString::number(model->rowCount(parent)));
        destination.writeAttribute(QStringLiteral("ColumnCount"), QString::number(model->columnCount(parent)));
        for (int i = 0; i < model->rowCount(parent); ++i) {
            for (int j = 0; j < model->columnCount(parent); ++j) {
                destination.writeStartElement(QStringLiteral("Cell"));
                destination.writeStartElement(QStringLiteral("Row"));
                destination.writeCharacters(QString::number(i));
                destination.writeEndElement(); // Row
                destination.writeStartElement(QStringLiteral("Column"));
                destination.writeCharacters(QString::number(j));
                destination.writeEndElement(); // Column
                foreach(int singleRole, rolesToSave)
                {
                    const QVariant roleData = model->index(i, j, parent).data(singleRole);
                    if (roleData.isNull())
                        continue; // Skip empty roles
                    const QString roleString = saveVariant(roleData);
                    if (roleString.isEmpty())
                        continue; // Skip unhandled types
                    destination.writeStartElement(QStringLiteral("DataPoint"));
                    destination.writeAttribute(QStringLiteral("Role"), QString::number(singleRole));
                    destination.writeAttribute(QStringLiteral("Type"), QString::number(roleData.type()));
                    destination.writeCharacters(roleString);
                    destination.writeEndElement(); // DataPoint
                }
                if (model->hasChildren(model->index(i, j, parent))) {
                    writeElement(destination, model, rolesToSave, model->index(i, j, parent));
                }
                destination.writeEndElement(); // Cell
            }
        }
        destination.writeEndElement(); // Element
    }
    bool readElement(QXmlStreamReader& source, QAbstractItemModel* const model, const QModelIndex& parent = QModelIndex())
    {
        if (source.name() != QStringLiteral("Element"))
            return false;
        int rowCount, colCount;
        const QXmlStreamAttributes tableSizeAttribute = source.attributes();
        if (!(
            tableSizeAttribute.hasAttribute(QStringLiteral("RowCount"))
            && tableSizeAttribute.hasAttribute(QStringLiteral("ColumnCount"))
            ))
            return false;
        rowCount = tableSizeAttribute.value(QStringLiteral("RowCount")).toInt();
        colCount = tableSizeAttribute.value(QStringLiteral("ColumnCount")).toInt();
        if (rowCount <= 0 || colCount <= 0)
            return false;
        if (model->rowCount(parent) < rowCount)
            model->insertRows(model->rowCount(parent), rowCount - model->rowCount(parent), parent);
        if (model->columnCount(parent) < colCount)
            model->insertColumns(model->columnCount(parent), colCount - model->columnCount(parent), parent);
        int rowIndex = -1;
        int colIndex = -1;
        bool cellStarted = false;
        while (!source.atEnd() && !source.hasError()) {
            source.readNext();
            if (source.isStartElement()) {
                if (source.name() == QStringLiteral("Cell")) {
                    cellStarted = true;
                }
                else if (source.name() == QStringLiteral("Row") && cellStarted) {
                    rowIndex = source.readElementText().toInt();
                }
                else if (source.name() == QStringLiteral("Column") && cellStarted) {
                    colIndex = source.readElementText().toInt();
                }
                else if (source.name() == QStringLiteral("DataPoint") && cellStarted) {
                    if (rowIndex < 0 || colIndex < 0)
                        return false;
                    const QXmlStreamAttributes dataPointTattributes = source.attributes();
                    if (!(
                        dataPointTattributes.hasAttribute(QStringLiteral("Role"))
                        && dataPointTattributes.hasAttribute(QStringLiteral("Type"))
                        ))
                        return false;
                    int dataRole = dataPointTattributes.value(QStringLiteral("Role")).toInt();
                    int dataType = dataPointTattributes.value(QStringLiteral("Type")).toInt();
                    const QVariant roleVariant = loadVariant(dataType, source.readElementText());
                    if (!roleVariant.isNull()) // skip unhandled types
                        model->setData(model->index(rowIndex, colIndex, parent), roleVariant, dataRole);
                }
                else if (source.name() == QStringLiteral("Element") && cellStarted) {
                    if (rowIndex < 0 || colIndex < 0)
                        return false;
                    readElement(source, model, model->index(rowIndex, colIndex, parent));
                }
            }
            else if (source.isEndElement()) {
                if (source.name() == QStringLiteral("Cell")) {
                    cellStarted = false;
                    rowIndex = -1;
                    colIndex = -1;
                }
                else if (source.name() == QStringLiteral("Element")) {
                    if (!cellStarted)
                        return true;
                }
            }

        }
        return false;
    }

    QList<int> modelDefaultRoles()
    {
        return QList<int>()
            << Qt::DecorationRole
            << Qt::EditRole
            << Qt::ToolTipRole
            << Qt::StatusTipRole
            << Qt::WhatsThisRole
            << Qt::SizeHintRole
            << Qt::FontRole
            << Qt::TextAlignmentRole
            << Qt::BackgroundRole
            << Qt::ForegroundRole
            << Qt::CheckStateRole
            << Qt::InitialSortOrderRole
            << Qt::AccessibleTextRole
            << Qt::AccessibleDescriptionRole
            << Qt::UserRole
            ;
    }

    bool saveModel(const QAbstractItemModel* const model, QIODevice* destination, const QList<int>& rolesToSave)
    {
        if (!destination->isWritable())
            return false;
        QXmlStreamWriter writer(destination);
        writer.writeStartDocument();
        writer.writeStartElement(QStringLiteral("ItemModel"));
        writer.writeStartElement(QStringLiteral("Version")); // Use these to implement versioning of the serialised values
        writer.writeTextElement(QStringLiteral("Major"), QString::number(1));
        writer.writeTextElement(QStringLiteral("Minor"), QString::number(0));
        writer.writeTextElement(QStringLiteral("Micro"), QString::number(0));
        writer.writeEndElement(); // Version
        writeElement(writer, model, rolesToSave);
        writer.writeStartElement(QStringLiteral("HeaderData"));
        // Header data is saved only for the number of rows and columns in the root table
        writer.writeStartElement(QStringLiteral("Horizontal"));
        for (int i = 0; i < model->columnCount(); ++i) {
            foreach(int singleRole, rolesToSave)
            {
                const QVariant roleData = model->headerData(i, Qt::Horizontal, singleRole);
                if (roleData.isNull())
                    continue;
                const QString roleString = saveVariant(roleData);
                if (roleString.isEmpty())
                    continue; // Skip unhandled types
                writer.writeStartElement(QStringLiteral("HeaderDataPoint"));
                writer.writeAttribute(QStringLiteral("Section"), QString::number(i));
                writer.writeAttribute(QStringLiteral("Role"), QString::number(singleRole));
                writer.writeAttribute(QStringLiteral("Type"), QString::number(roleData.type()));
                writer.writeCharacters(roleString);
                writer.writeEndElement(); // HeaderDataPoint
            }
        }
        writer.writeEndElement(); // Horizontal
        writer.writeStartElement(QStringLiteral("Vertical"));
        for (int i = 0; i < model->rowCount(); ++i) {
            foreach(int singleRole, rolesToSave)
            {
                const QVariant roleData = model->headerData(i, Qt::Vertical, singleRole);
                if (roleData.isNull())
                    continue;
                const QString roleString = saveVariant(roleData);
                if (roleString.isEmpty())
                    continue; // Skip unhandled types
                writer.writeStartElement(QStringLiteral("HeaderDataPoint"));
                writer.writeAttribute(QStringLiteral("Section"), QString::number(i));
                writer.writeAttribute(QStringLiteral("Role"), QString::number(singleRole));
                writer.writeAttribute(QStringLiteral("Type"), QString::number(roleData.type()));
                writer.writeCharacters(roleString);
                writer.writeEndElement(); // HeaderDataPoint
            }
        }
        writer.writeEndElement(); // Vertical
        writer.writeEndElement(); // HeaderData
        writer.writeEndElement(); // ItemModel
        writer.writeEndDocument();
        return true;
    }

    bool saveModel(const QAbstractItemModel* const model, const QString& destination, const QList<int>& rolesToSave)
    {
        QSaveFile destinationFile(destination);
        if (!destinationFile.open(QIODevice::WriteOnly))
            return false;
        if (!saveModel(model, &destinationFile, rolesToSave))
            return false;
        return destinationFile.commit();
    }

    bool saveModel(const QAbstractItemModel* const model, const QString& destination)
    {
        return saveModel(model, destination, modelDefaultRoles());
    }

    bool saveModel(const QAbstractItemModel* const model, QIODevice* destination)
    {
        return saveModel(model, destination, modelDefaultRoles());
    }

    bool loadModel(QAbstractItemModel* const model, QIODevice* source)
    {
        if (!source->isReadable())
            return false;
        model->removeColumns(0, model->columnCount());
        model->removeRows(0, model->rowCount());
        // Use these to implement versioning of the serialised values
        int majorVersion = -1;
        int minorVersion = -1;
        int microVersion = -1;
        QXmlStreamReader reader(source);
        bool headerDataStarted = false;
        bool hHeaderDataStarted = false;
        bool vHeaderDataStarted = false;
        bool versionStarted = false;
        while (!reader.atEnd() && !reader.hasError()) {
            reader.readNext();
            if (reader.isStartElement()) {
                if (reader.name() == QStringLiteral("Version")) {
                    versionStarted = true;
                }
                else if (versionStarted && reader.name() == QStringLiteral("Major")) {
                    majorVersion = reader.readElementText().toInt();
                }
                else if (versionStarted && reader.name() == QStringLiteral("Minor")) {
                    minorVersion = reader.readElementText().toInt();
                }
                else if (versionStarted && reader.name() == QStringLiteral("Micro")) {
                    microVersion = reader.readElementText().toInt();
                }
                else if (reader.name() == QStringLiteral("Element")) {
                    if (!readElement(reader, model)) {
                        model->removeColumns(0, model->columnCount());
                        model->removeRows(0, model->rowCount());
                        return false;
                    }
                }
                else if (reader.name() == QStringLiteral("HeaderData")) {
                    headerDataStarted = true;
                }
                else if (reader.name() == QStringLiteral("Vertical") && headerDataStarted) {
                    if (hHeaderDataStarted)
                        return false;
                    vHeaderDataStarted = true;
                }
                else if (reader.name() == QStringLiteral("Horizontal") && headerDataStarted) {
                    if (vHeaderDataStarted)
                        return false;
                    hHeaderDataStarted = true;
                }
                else if (reader.name() == QStringLiteral("HeaderDataPoint") && headerDataStarted) {
                    if (!(vHeaderDataStarted || hHeaderDataStarted))
                        return false;
                    const QXmlStreamAttributes headDataAttribute = reader.attributes();
                    if (!(
                        headDataAttribute.hasAttribute(QStringLiteral("Section"))
                        && headDataAttribute.hasAttribute(QStringLiteral("Role"))
                        && headDataAttribute.hasAttribute(QStringLiteral("Type"))
                        ))
                        return false;
                    int headerSection = headDataAttribute.value(QStringLiteral("Section")).toInt();
                    int headerRole = headDataAttribute.value(QStringLiteral("Role")).toInt();
                    int headerType = headDataAttribute.value(QStringLiteral("Type")).toInt();
                    const QVariant roleVariant = loadVariant(headerType, reader.readElementText());
                    if (!roleVariant.isNull()) // skip unhandled types
                        model->setHeaderData(headerSection, (vHeaderDataStarted ? Qt::Vertical : Qt::Horizontal), roleVariant, headerRole);
                }

            }
            else if (reader.isEndElement()) {
                if (reader.name() == QStringLiteral("Version")) {
                    versionStarted = false;
                }
                if (reader.name() == QStringLiteral("HeaderData")) {
                    headerDataStarted = false;
                }
                else if (reader.name() == QStringLiteral("Vertical") && headerDataStarted) {
                    vHeaderDataStarted = false;
                }
                else if (reader.name() == QStringLiteral("Horizontal") && headerDataStarted) {
                    hHeaderDataStarted = false;
                }
            }
        }
        if (reader.hasError()) {
            model->removeColumns(0, model->columnCount());
            model->removeRows(0, model->rowCount());
            return false;
        }
        return true;
    }

    bool loadModel(QAbstractItemModel* const model, const QString& source)
    {
        QFile sourceFile(source);
        if (!sourceFile.open(QIODevice::ReadOnly))
            return false;
        return loadModel(model, &sourceFile);
    }
}