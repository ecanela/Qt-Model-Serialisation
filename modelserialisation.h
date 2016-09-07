#ifndef modelserialisation_h__
#define modelserialisation_h__
#include <QList>
class QAbstractItemModel;
class QString;
class QIODevice;
namespace ModelSerialisation{
    QList<int> modelDefaultRoles();
    bool saveModel(const QAbstractItemModel* const model, const QString& destination, const QList<int>& rolesToSave);
    bool saveModel(const QAbstractItemModel* const model, const QString& destination);
    bool loadModel(QAbstractItemModel* const model, const QString& source);
    bool saveModel(const QAbstractItemModel* const model, QIODevice* destination, const QList<int>& rolesToSave);
    bool saveModel(const QAbstractItemModel* const model, QIODevice* destination);
    bool loadModel(QAbstractItemModel* const model, QIODevice* source);
}
#endif // modelserialisation_h__