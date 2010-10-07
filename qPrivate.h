#ifndef QPRIVATE_H
#define QPRIVATE_H

#include <QDialog>
#include <QVBoxLayout>
#include <QButtonGroup>
#include <QDialogButtonBox>
//#include "QIrExpander/qirexpander.h"
#include "qTypes.h"

class qPrivate : public QDialog
{
    Q_OBJECT
public:
    explicit qPrivate(QWidget *parent = 0);

signals:

public slots:

};

#endif // QPRIVATE_H
