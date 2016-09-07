## Qt Model Serialisation
This code implements a pseudo-general method to serialised QAbstractItemModel based models to xml.

Example Usage

```C++
#include <QApplication>
#include "modelserialisation.h"
#include <QStandardItemModel>
#include <QTreeView>
#include <QHBoxLayout>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QPixmap bluePix(48,48);
    bluePix.fill(Qt::blue);
    QStandardItemModel baseModel;
    baseModel.insertRows(0, 3);
    baseModel.insertColumns(0, 2);
    baseModel.setHeaderData(0, Qt::Horizontal, "Col1");
    baseModel.setHeaderData(1, Qt::Horizontal, "Col2");
    baseModel.setData(baseModel.index(0, 0), "0.0");
    baseModel.setData(baseModel.index(0, 0), QIcon(bluePix), Qt::DecorationRole);
    baseModel.setData(baseModel.index(0, 1), "0.1");
    baseModel.setData(baseModel.index(1, 0), "1.0");
    baseModel.setData(baseModel.index(1, 1), "1.1");
    baseModel.setData(baseModel.index(2, 0), "2.0");
    baseModel.setData(baseModel.index(2, 1), "2.1");
    baseModel.insertRows(0, 2, baseModel.index(0, 0));
    baseModel.insertColumns(0, 2, baseModel.index(0, 0));
    baseModel.setData(baseModel.index(0, 0, baseModel.index(0, 0)), "0.0.0");
    baseModel.setData(baseModel.index(0, 1, baseModel.index(0, 0)), "0.0.1");
    baseModel.setData(baseModel.index(1, 0, baseModel.index(0, 0)), "0.1.0");
    baseModel.setData(baseModel.index(1, 1, baseModel.index(0, 0)), "0.1.1");
    QStandardItemModel loadedModel;

    QWidget mainWidget;
    QTreeView* savedView=new QTreeView(&mainWidget);
    savedView->setModel(&baseModel);
    QTreeView* loadedView = new QTreeView(&mainWidget);
    loadedView->setModel(&loadedModel);
    QHBoxLayout* mainLay = new QHBoxLayout(&mainWidget);
    mainLay->addWidget(savedView);
    mainLay->addWidget(loadedView);

    if (!ModelSerialisation::saveModel(&baseModel, "TestSave.xml"))
        Q_ASSERT(false);
    if (!ModelSerialisation::loadModel(&loadedModel, "TestSave.xml"))
        Q_ASSERT(false);
    mainWidget.show();
    return a.exec();
}
```