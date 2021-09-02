#ifndef REGION_H
#define REGION_H

#include <QWidget>

namespace Ui {
class region;
}

class region : public QWidget
{
    Q_OBJECT

public:
    explicit region(QWidget *parent = nullptr);
    ~region();

private:
    Ui::region *ui;
};

#endif // REGION_H
