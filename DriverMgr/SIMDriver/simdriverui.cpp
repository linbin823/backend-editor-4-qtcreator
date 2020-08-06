#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
#include "simdriverui.h"
#include "ui_simdriverui.h"

SimDriverUI::SimDriverUI(SimDriver *driver, QWidget *parent) :
    QWidget(parent),_pDriver(driver),
    ui(new Ui::SimDriverUI)
{
    ui->setupUi(this);
    setLayout( ui->gridLayout );
//    setWindowFlag( Qt::WindowStaysOnTopHint );
    setWindowTitle(tr("711组播协议驱动设置"));
    _acceptedDataN = _receivedDataN = 0;

    //init state
    int state = _pDriver->state();
    updateState(state);

    ui->leHostIP->setText( _pDriver->udpdatabaseAddress() );
    ui->leLocalPort->setText( QString::number( _pDriver->udpdatabasePort() ) );
    QList<QNetworkInterface> ifaces = QNetworkInterface::allInterfaces();
    QNetworkInterface currentInterface = _pDriver->networkInterface();
    foreach(QNetworkInterface iface, ifaces){
        if(iface.flags().testFlag(QNetworkInterface::CanMulticast)){
            ui->cbInterface->addItem(QString("%1  %2")
                                     .arg(iface.name())
                                     .arg(iface.hardwareAddress() ),
                                     QVariant::fromValue(iface));
        }
        if(currentInterface.name() == iface.name() ){
            ui->cbInterface->setCurrentIndex( ui->cbInterface->count()-1 );
        }
    }
}

SimDriverUI::~SimDriverUI()
{
    delete ui;
}

void SimDriverUI::on_pbStart_clicked()
{

    int ret = -1;
    QHostAddress haddr = QHostAddress ( ui->leHostIP->text() );
    if(haddr.isNull()){
        ui->lbLastError->setText("IP地址错误");
    }
    bool ok;
    qint16 hport = ui->leHostPort->text().toInt(&ok);
    if(!ok || hport ==0){
        ui->lbLastError->setText("端口错误");
    }
    qint16 lport = ui->leLocalPort->text().toInt(&ok);
    if(!ok || lport ==0){
        ui->lbLastError->setText("端口错误");
    }

    ret = _pDriver->setUdpDatabaseAddress(haddr,hport,lport);
    if(ret!=0) return;
    qint16 stationid = ui->leStationID->text().toInt(&ok);
    ret = _pDriver->setSMDERIMStation(stationid);
    if(ret!=0) return;

    //to avoid create object in this ui thread

    QMetaObject::invokeMethod(_pDriver,"start");
    //emit start();
}

void SimDriverUI::on_pbStop_clicked()
{
    QMetaObject::invokeMethod(_pDriver,"stop");
}

void SimDriverUI::updateState(int state){
    ui->lbState->setText( _pDriver->enumStatesString( _pDriver->state() ));
    if(state == UdpDatabaseProtocolDriver::STAT_RUN){
        ui->pbStart->setEnabled(false);
        ui->pbStop->setEnabled(true);
    }else{
        ui->pbStart->setEnabled(true);
        ui->pbStop->setEnabled(false);
    }
}

void SimDriverUI::updateReceivedDataN(){
    _receivedDataN++;
    ui->lbReceiveDataN->setText( QString::number( _receivedDataN ) );
}

void SimDriverUI::updateAcceptedDataN(){
    _acceptedDataN++;
    ui->lbAcceptDataN->setText( QString::number( _acceptedDataN ) );
}


void SimDriverUI::on_pbClose_clicked()
{
    this->hide();
}
