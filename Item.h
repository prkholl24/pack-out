#ifndef ITEM_H
#define ITEM_H

#include <QObject>
#include <QString>

namespace Item {
class PackableItem;
}

class PackableItem : public QObject
{
    Q_OBJECT

    public:
        PackableItem();
        PackableItem(QString& toolTip, double weight, QObject* parent = nullptr);
        double getWeight() const;
        void setWeight(double newWeight);

    private:
        QString toolTip;
        double weight;
        QObject parent;

    signals:
        /**
         * @brief Sends the weight of this object to whatever class needs it. Doesn't have a method body because it is a signal.
         * @param The weight of this object.
         */
        void weightChanged(double newWeight);
        void addToBackpack();
        void removeFromBackpack();

    private slots:
};

#endif // ITEM_H
