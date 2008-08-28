/*
 *  UniMotion - Unified Motion detection for Apple portables.
 *
 *  Copyright (c) 2006 Lincoln Ramsay. All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "unimotion.h"
#include <QtCore/QtCore>
#include <QtGui/QtGui>
#include "ui_calibrate.h"

class MyWidget : public QWidget, private Ui::Calibrate
{
    Q_OBJECT
public:
    MyWidget();
    ~MyWidget();

public slots:
    void tick();

private:
    int type;
};

MyWidget::MyWidget()
{
    QString name;
    type = detect_sms();
    if ( type == unknown ) {
        printf("Could not detect an SMS\n");
        qApp->exit(1);
    }
    switch ( type ) {
        case powerbook:
            name = "powerbook";
            break;
        case ibook:
            name = "ibook";
            break;
        case highrespb:
            name = "highrespb";
            break;
        case macbookpro:
            name = "macbookpro";
            break;
        default:
            name = "???";
            break;
    }

    setupUi(this);
    leModel->setText( name );
}

MyWidget::~MyWidget()
{
}

void MyWidget::tick()
{
    int x, y, z;

    read_sms_raw(type, &x, &y, &z);
    leCurX->setText( QString("%1").arg(x) );
    leCurY->setText( QString("%1").arg(y) );
    leCurZ->setText( QString("%1").arg(z) );
}

int main( int argc, char **argv )
{
    QApplication a( argc, argv );
    MyWidget *w = new MyWidget();
    w->show();
    QTimer *t = new QTimer( w );
    t->setInterval( 100 );
    QObject::connect( t, SIGNAL(timeout()), w, SLOT(tick()) );
    t->start();
    return a.exec();
}

#include "main.moc"
