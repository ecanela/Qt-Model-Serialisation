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
#ifndef modelserialisation_h__
#define modelserialisation_h__
#include <QList>
class QAbstractItemModel;
class QString;
class QIODevice;
namespace ModelSerialisation{
    /*!
    \brief A list of default roles in the model
    \details Returns a list containing all non-obsolete Qt::ItemDataRole values
    */
    QList<int> modelDefaultRoles();
    /*!
    \brief Save the model to file
    \arg \c model The model to save
    \arg \c destination The path to the file the model is to be saved to
    \arg \c rolesToSave The roles in the model data that should be saved
    */
    bool saveModel(const QAbstractItemModel* const model, const QString& destination, const QList<int>& rolesToSave);
    /*!
    \brief Save the model to file
    \arg \c model The model to save
    \arg \c destination The path to the file the model is to be saved to
    \details All non-obsolete Qt::ItemDataRole will be saved
    */
    bool saveModel(const QAbstractItemModel* const model, const QString& destination);
    /*!
    \brief Writes the model to a device
    \arg \c model The model to save
    \arg \c destination The device the model is to be written to
    \arg \c rolesToSave The roles in the model data that should be saved
    */
    bool saveModel(const QAbstractItemModel* const model, QIODevice* destination, const QList<int>& rolesToSave);
    /*!
    \brief Writes the model to a device
    \arg \c model The model to save
    \arg \c destination The device the model is to be written to
    \details All non-obsolete Qt::ItemDataRole will be saved
    */
    bool saveModel(const QAbstractItemModel* const model, QIODevice* destination);
    /*!
    \brief Reads a model from a device
    \arg \c model The model that will be loaded
    \arg \c source The device the model is to be read from
    */
    bool loadModel(QAbstractItemModel* const model, QIODevice* source);
    /*!
    \brief Reads a model from a device
    \arg \c model The model that will be loaded
    \arg \c source The path to the file the model is to be loaded from
    */
    bool loadModel(QAbstractItemModel* const model, const QString& source);
}
#endif // modelserialisation_h__