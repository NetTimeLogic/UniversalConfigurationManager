//*****************************************************************************************
// Project:     UniversalConfigurationManager
//
// Author:      Sven Meier, NetTimeLogic GmbH
//
// License:     Copyright (c) 2017, NetTimeLogic GmbH, Switzerland, Sven Meier <contact@nettimelogic.com>
//              All rights reserved.
//
//              THIS PROGRAM IS FREE SOFTWARE: YOU CAN REDISTRIBUTE IT AND/OR MODIFY
//              IT UNDER THE TERMS OF THE GNU LESSER GENERAL PUBLIC LICENSE AS
//              PUBLISHED BY THE FREE SOFTWARE FOUNDATION, VERSION 3.
//
//              THIS PROGRAM IS DISTRIBUTED IN THE HOPE THAT IT WILL BE USEFUL, BUT
//              WITHOUT ANY WARRANTY; WITHOUT EVEN THE IMPLIED WARRANTY OF
//              MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE. SEE THE GNU
//              LESSER GENERAL LESSER PUBLIC LICENSE FOR MORE DETAILS.
//
//              YOU SHOULD HAVE RECEIVED A COPY OF THE GNU LESSER GENERAL PUBLIC LICENSE
//              ALONG WITH THIS PROGRAM. IF NOT, SEE <http://www.gnu.org/licenses/>.
//
//*****************************************************************************************

#ifndef UCM_COMMUNICATIONLIB_H
#define UCM_COMMUNICATIONLIB_H

#include <QSerialPortInfo>
#include <QSerialPort>
#include <QUdpSocket>
#include <QMutex>

#include <string>
using namespace std;

#define Ucm_CommunicationLib_ComType    1
#define Ucm_CommunicationLib_EthType    2

class Ucm_CommunicationLib
{
    private:
        bool is_open;
        QSerialPort com_port;
        QUdpSocket eth_port;
        QString port_name;
        QMutex* com_lock;
        int detect_baudrate(void);

    public:
        Ucm_CommunicationLib();
        ~Ucm_CommunicationLib();

        int port_type;

        int open_port(QString name);
        int close_port();
        int check_port();
        int write_reg(const unsigned int addr, unsigned int& data);
        int read_reg(const unsigned int addr, unsigned int& data);
};

#endif // Ucm_CommunicationLib_H
