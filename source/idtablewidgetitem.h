#ifndef IDTABLEWIDGETITEM_H
#define IDTABLEWIDGETITEM_H

#include <QTableWidgetItem>

class IdTableWidgetItem : public QTableWidgetItem
{
public:
    explicit IdTableWidgetItem(QString& text);

    virtual bool operator<(QTableWidgetItem const &other) const
    {
        IdTableWidgetItem const *item
            = dynamic_cast<IdTableWidgetItem const*>(&other);
        if(item)
        {
            return this->text().toInt() < item->text().toInt();
        }
        else
        {
            return QTableWidgetItem::operator<(other);
        }
    }


    
signals:
    
public slots:
    
};

#endif // IDTABLEWIDGETITEM_H
