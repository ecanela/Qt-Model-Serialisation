#ifndef modelserialisation_h__
#define modelserialisation_h__
class QAbstractItemModel;
class QString;
namespace ModelSerialisation{
    bool saveModel(const QAbstractItemModel* const model, const QString& destination, const QList<int>& rolesToSave);
    bool saveModel(const QAbstractItemModel* const model, const QString& destination);
    bool loadModel(QAbstractItemModel* const model, const QString& source);
}
#endif // modelserialisation_h__