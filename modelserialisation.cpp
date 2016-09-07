#include <QAbstractItemModel>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QSaveFile>
#include <QFile>
#include <QVersionNumber>
#include <QDataStream>
#include <QDateTime>
#include <QBitArray>
#include <QUrl>
#include <QLocale>
#include <QRect>
#include <QRectF>
#include <QSize>
#include <QSizeF>
#include <QLine>
#include <QLineF>
#include <QEasingCurve>
#include <QUuid>
#include <QRegularExpression>
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFont>
#include <QPixmap>
#include <QBrush>
#include <QColor>
#include <QPalette>
#include <QIcon>
#include <QImage>
#include <QPolygon>
#include <QRegion>
#include <QBitmap>
#include <QCursor>
#include <QKeySequence>
#include <QPen>
#include <QTextLength>
#include <QTextFormat>
#include <QMatrix>
#include <QTransform>
#include <QMatrix4x4>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QQuaternion>
#include <QPolygonF>
#include <QSizePolicy>
#include "modelserialisation.h"
namespace ModelSerialisation {

    template <class T>
    QString variantToString(const QVariant& val){
        QString result;
        QByteArray data;
        QDataStream outStream(&data, QIODevice::WriteOnly);
        outStream << val.value<T>();
        for (const char* i = data.constBegin(); i != data.constEnd();++i){
            const QString tempString = QString::number(*reinterpret_cast<const unsigned char*>(i), 16);
            Q_ASSERT(tempString.size() == 2 || tempString.size() == 1);
            if (tempString.size() == 1)
                result += '0';
            result += tempString;
        }
        Q_ASSERT(result.size() % 2 == 0);
        return result;
    }

    template <class T>
    QVariant stringToVariant(const QString& val)
    {
        QByteArray data;
        for (int i = 0; i < val.size(); i += 2) {
            const unsigned char unsignedData = static_cast<unsigned char>(val.midRef(i, 2).toInt(nullptr, 16));
            data.append(*reinterpret_cast<const char*>(&unsignedData));
        }
        QDataStream inStream(data);
        T result;
        inStream >> result;
        return QVariant::fromValue(result);
    }


    QVariant loadVariant(int type, const QString& val){
        switch (type) {
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
        case QMetaType::QChar: return stringToVariant<QChar>(val);
        case QMetaType::QString: return val;
        case QMetaType::QStringList: return stringToVariant<QStringList>(val);
        case QMetaType::QByteArray: return stringToVariant<QByteArray>(val);
        case QMetaType::QBitArray: return stringToVariant<QBitArray>(val);
        case QMetaType::QDate: return QDate::fromString(val, Qt::ISODate);
        case QMetaType::QTime: return QTime::fromString(val, Qt::ISODate);
        case QMetaType::QDateTime: return QDateTime::fromString(val, Qt::ISODate);
        case QMetaType::QUrl: return stringToVariant<QUrl>(val);
        case QMetaType::QLocale: return stringToVariant<QLocale>(val);
        case QMetaType::QRect: return stringToVariant<QRect>(val);
        case QMetaType::QRectF: return stringToVariant<QRectF>(val);
        case QMetaType::QSize: return stringToVariant<QSize>(val);
        case QMetaType::QSizeF: return stringToVariant<QSizeF>(val);
        case QMetaType::QLine: return stringToVariant<QLine>(val);
        case QMetaType::QLineF: return stringToVariant<QLineF>(val);
        case QMetaType::QPoint: return stringToVariant<QPoint>(val);
        case QMetaType::QPointF: return stringToVariant<QPointF>(val);
        case QMetaType::QRegExp: return stringToVariant<QRegExp>(val);
        case QMetaType::QEasingCurve: return stringToVariant<QEasingCurve>(val);
        case QMetaType::QUuid: return stringToVariant<QUuid>(val);
        case QMetaType::QRegularExpression: return stringToVariant<QRegularExpression>(val);
        case QMetaType::QVariantMap: return stringToVariant<QVariantMap>(val);
        case QMetaType::QVariantList: return stringToVariant<QVariantList>(val);
        case QMetaType::QVariantHash: return stringToVariant<QVariantHash>(val);
        case QMetaType::QByteArrayList: return stringToVariant<QByteArrayList>(val);
        case QMetaType::QFont: return stringToVariant<QFont>(val);
        case QMetaType::QPixmap: return stringToVariant<QPixmap>(val);
        case QMetaType::QBrush: return stringToVariant<QBrush>(val);
        case QMetaType::QColor: return stringToVariant<QColor>(val);
        case QMetaType::QPalette: return stringToVariant<QPalette>(val);
        case QMetaType::QIcon: return stringToVariant<QIcon>(val);
        case QMetaType::QImage: return stringToVariant<QImage>(val);
        case QMetaType::QPolygon: return stringToVariant<QPolygon>(val);
        case QMetaType::QRegion: return stringToVariant<QRegion>(val);
        case QMetaType::QBitmap: return stringToVariant<QBitmap>(val);
        case QMetaType::QCursor: return stringToVariant<QCursor>(val);
        case QMetaType::QKeySequence: return stringToVariant<QKeySequence>(val);
        case QMetaType::QPen: return stringToVariant<QPen>(val);
        case QMetaType::QTextLength: return stringToVariant<QTextLength>(val);
        case QMetaType::QTextFormat: return stringToVariant<QTextFormat>(val);
        case QMetaType::QMatrix: return stringToVariant<QMatrix>(val);
        case QMetaType::QTransform: return stringToVariant<QTransform>(val);
        case QMetaType::QMatrix4x4: return stringToVariant<QMatrix4x4>(val);
        case QMetaType::QVector2D: return stringToVariant<QVector2D>(val);
        case QMetaType::QVector3D: return stringToVariant<QVector3D>(val);
        case QMetaType::QVector4D: return stringToVariant<QVector4D>(val);
        case QMetaType::QQuaternion: return stringToVariant<QQuaternion>(val);
        case QMetaType::QPolygonF: return stringToVariant<QPolygonF>(val);
        case QMetaType::QSizePolicy: return stringToVariant<QSizePolicy>(val);
        default:
            Q_ASSERT_X(false, "ModelSerialisation::loadVariant", "Unhandled type of variant");
            return QVariant();
        }
    }
    QString saveVariant(const QVariant& val){
        switch(val.userType()){
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
        case QMetaType::Double:  return QString::number(val.toDouble(),'f',15);
        case QMetaType::Float: return QString::number(val.toDouble(), 'f', 7);
        case QMetaType::QChar: return variantToString<QChar>(val);
        case QMetaType::QString: return val.toString();
        case QMetaType::QStringList: return variantToString<QStringList>(val);
        case QMetaType::QByteArray: return variantToString<QByteArray>(val);
        case QMetaType::QBitArray: return variantToString<QBitArray>(val);
        case QMetaType::QDate: return val.toDate().toString(Qt::ISODate);
        case QMetaType::QTime: return val.toTime().toString(Qt::ISODate);
        case QMetaType::QDateTime: return val.toDateTime().toString(Qt::ISODate);
        case QMetaType::QUrl: return variantToString<QUrl>(val);
        case QMetaType::QLocale: return variantToString<QLocale>(val);
        case QMetaType::QRect: return variantToString<QRect>(val);
        case QMetaType::QRectF: return variantToString<QRectF>(val);
        case QMetaType::QSize: return variantToString<QSize>(val);
        case QMetaType::QSizeF: return variantToString<QSizeF>(val);
        case QMetaType::QLine: return variantToString<QLine>(val);
        case QMetaType::QLineF: return variantToString<QLineF>(val);
        case QMetaType::QPoint: return variantToString<QPoint>(val);
        case QMetaType::QPointF: return variantToString<QPointF>(val);
        case QMetaType::QRegExp: return variantToString<QRegExp>(val);
        case QMetaType::QEasingCurve: return variantToString<QEasingCurve>(val);
        case QMetaType::QUuid: return variantToString<QUuid>(val);
        case QMetaType::QRegularExpression: return variantToString<QRegularExpression>(val);
        case QMetaType::QVariantMap: return variantToString<QVariantMap>(val);
        case QMetaType::QVariantList: return variantToString<QVariantList>(val);
        case QMetaType::QVariantHash: return variantToString<QVariantHash>(val);
        case QMetaType::QByteArrayList: return variantToString<QByteArrayList>(val);
        case QMetaType::QFont: return variantToString<QFont>(val);
        case QMetaType::QPixmap: return variantToString<QPixmap>(val);
        case QMetaType::QBrush: return variantToString<QBrush>(val);
        case QMetaType::QColor: return variantToString<QColor>(val);
        case QMetaType::QPalette: return variantToString<QPalette>(val);
        case QMetaType::QIcon: return variantToString<QIcon>(val);
        case QMetaType::QImage: return variantToString<QImage>(val);
        case QMetaType::QPolygon: return variantToString<QPolygon>(val);
        case QMetaType::QRegion: return variantToString<QRegion>(val);
        case QMetaType::QBitmap: return variantToString<QBitmap>(val);
        case QMetaType::QCursor: return variantToString<QCursor>(val);
        case QMetaType::QKeySequence: return variantToString<QKeySequence>(val);
        case QMetaType::QPen: return variantToString<QPen>(val);
        case QMetaType::QTextLength: return variantToString<QTextLength>(val);
        case QMetaType::QTextFormat: return variantToString<QTextFormat>(val);
        case QMetaType::QMatrix: return variantToString<QMatrix>(val);
        case QMetaType::QTransform: return variantToString<QTransform>(val);
        case QMetaType::QMatrix4x4: return variantToString<QMatrix4x4>(val);
        case QMetaType::QVector2D: return variantToString<QVector2D>(val);
        case QMetaType::QVector3D: return variantToString<QVector3D>(val);
        case QMetaType::QVector4D: return variantToString<QVector4D>(val);
        case QMetaType::QQuaternion: return variantToString<QQuaternion>(val);
        case QMetaType::QPolygonF: return variantToString<QPolygonF>(val);
        case QMetaType::QSizePolicy: return variantToString<QSizePolicy>(val);
        default:
            Q_ASSERT_X(false, "ModelSerialisation::saveVariant", "Unhandled type of variant");
            return QString();
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
                for (int singleRole : rolesToSave) {
                    const QVariant roleData = model->index(i, j, parent).data(singleRole);
                    if (roleData.isNull())
                        continue; // Skip empty roles
                    const QString roleString = saveVariant(roleData);
                    if (roleString.isEmpty())
                        continue; // Skip unhandled types
                    destination.writeStartElement(QStringLiteral("DataPoint"));
                    destination.writeAttribute(QStringLiteral("Role"), QString::number(singleRole));
                    destination.writeAttribute(QStringLiteral("Type"), QString::number(roleData.userType()));
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
    bool readElement(QXmlStreamReader& source, QAbstractItemModel* const model, const QModelIndex& parent = QModelIndex()){
        if (source.name() != QStringLiteral("Element"))
            return false;
        int rowCount, colCount;
        const auto tableSizeAttribute = source.attributes();
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
        bool cellStarted =false;
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
                    const auto dataPointTattributes = source.attributes();
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
            else if (source.isEndElement()){
                if (source.name() == QStringLiteral("Cell")) {
                    cellStarted = false;
                    rowIndex = -1;
                    colIndex = -1;
                }
                else if (source.name() == QStringLiteral("Element")) {
                    if(!cellStarted)
                        return true;
                }
            }
            
        }
        return false;
    }

    bool saveModel(const QAbstractItemModel* const model, const QString& destination)
    {
        return saveModel(model, destination, {
            Qt::DecorationRole
            , Qt::EditRole
            , Qt::ToolTipRole
            , Qt::StatusTipRole
            , Qt::WhatsThisRole
            , Qt::SizeHintRole
            , Qt::FontRole
            , Qt::TextAlignmentRole
            , Qt::BackgroundRole
            , Qt::ForegroundRole
            , Qt::CheckStateRole
            , Qt::InitialSortOrderRole
            , Qt::AccessibleTextRole
            , Qt::AccessibleDescriptionRole
            , Qt::UserRole
        });
    }
    bool saveModel(const QAbstractItemModel* const model, const QString& destination, const QList<int>& rolesToSave)
    {
        const QVersionNumber modelSerialisationVersion(1, 0, 0); // Use this to implement versioning of the serialised values
        QSaveFile destinationFile(destination);
        if (!destinationFile.open(QIODevice::WriteOnly))
            return false;
        QXmlStreamWriter writer(&destinationFile);
        writer.writeStartDocument();
        writer.writeStartElement(QStringLiteral("ItemModel"));
        writer.writeEmptyElement(QStringLiteral("Version"));
        writer.writeAttribute(QStringLiteral("VersionNumber"), modelSerialisationVersion.toString());
        writeElement(writer, model, rolesToSave);
        writer.writeStartElement(QStringLiteral("HeaderData"));
        // Header data is saved only for the number of rows and columns in the root table
        writer.writeStartElement(QStringLiteral("Horizontal"));
        for (int i = 0; i < model->columnCount(); ++i) {
            for (int singleRole : rolesToSave) {
                const QVariant roleData = model->headerData(i, Qt::Horizontal, singleRole);
                if (roleData.isNull())
                    continue;
                const QString roleString = saveVariant(roleData);
                if (roleString.isEmpty())
                    continue; // Skip unhandled types
                writer.writeStartElement(QStringLiteral("HeaderDataPoint"));
                writer.writeAttribute(QStringLiteral("Section"), QString::number(i));
                writer.writeAttribute(QStringLiteral("Role"), QString::number(singleRole));
                writer.writeAttribute(QStringLiteral("Type"), QString::number(roleData.userType()));
                writer.writeCharacters(roleString);
                writer.writeEndElement(); // HeaderDataPoint
            }
        }
        writer.writeEndElement(); // Horizontal
        writer.writeStartElement(QStringLiteral("Vertical"));
        for (int i = 0; i < model->rowCount(); ++i) {
            for (int singleRole : rolesToSave) {
                const QVariant roleData = model->headerData(i, Qt::Vertical, singleRole);
                if (roleData.isNull())
                    continue;
                const QString roleString = saveVariant(roleData);
                if (roleString.isEmpty())
                    continue; // Skip unhandled types
                writer.writeStartElement(QStringLiteral("HeaderDataPoint"));
                writer.writeAttribute(QStringLiteral("Section"), QString::number(i));
                writer.writeAttribute(QStringLiteral("Role"), QString::number(singleRole));
                writer.writeAttribute(QStringLiteral("Type"), QString::number(roleData.userType()));
                writer.writeCharacters(roleString);
                writer.writeEndElement(); // HeaderDataPoint
            }
        }
        writer.writeEndElement(); // Vertical
        writer.writeEndElement(); // HeaderData
        writer.writeEndElement(); // ItemModel
        writer.writeEndDocument();
        return destinationFile.commit();
    }
    
    bool loadModel(QAbstractItemModel* const model, const QString& source){
        QFile sourceFile(source);
        if (!sourceFile.open(QIODevice::ReadOnly))
            return false;
        model->removeColumns(0, model->columnCount());
        model->removeRows(0, model->rowCount());
        QVersionNumber modelSerialisationVersion; // Use this to implement versioning of the serialised values
        QXmlStreamReader reader(&sourceFile);
        bool headerDataStarted = false;
        bool hHeaderDataStarted = false;
        bool vHeaderDataStarted = false;
        while (!reader.atEnd() && !reader.hasError()) {
            reader.readNext();
            if (reader.isStartElement()) {
                if (reader.name() == QStringLiteral("Version")) {
                    const auto versionAttributes = reader.attributes();
                    if (!versionAttributes.hasAttribute(QStringLiteral("VersionNumber")))
                        return false;
                    modelSerialisationVersion = QVersionNumber::fromString(versionAttributes.value(QStringLiteral("VersionNumber")).toString());
                }
                else if (reader.name() == QStringLiteral("Element")){
                    if(!readElement(reader, model)){
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
                    const auto headDataAttribute = reader.attributes();
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
            else if (reader.isEndElement()){
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

}