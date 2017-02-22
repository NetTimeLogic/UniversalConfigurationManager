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

#include "Ucm_CommunicationLib.h"

#include <string>
#include <iostream>
using namespace std;

Ucm_CommunicationLib::Ucm_CommunicationLib()
{
    com_lock = new QMutex(QMutex::Recursive);
    com_lock->lock();
    is_open = false;
    com_lock->unlock();
}

Ucm_CommunicationLib::~Ucm_CommunicationLib()
{
    com_lock->lock();
    if (is_open == true)
    {
        close_port();
    }
    com_lock->unlock();
}

Ucm_CommunicationLib::open_port(QString port_name)
{
    int baud_rate;
    int data_length;
    QByteArray read_data;
    QByteArray write_data;
    unsigned char checksum;
    QString temp_string;

    cout << "INFO: " << "Opening Port:" << endl;

    com_lock->lock();
    if (is_open == true)
    {
        cout << "port is already open" << endl;
        com_lock->unlock();
        return -1;
    }

    com_port.setPortName(port_name);
    baud_rate = QSerialPort::Baud115200;
    com_port.setBaudRate(baud_rate);

    if (false == com_port.open(QIODevice::ReadWrite))
    {
        cout << "ERROR: " << "opening port failed" << endl;
        com_lock->unlock();
        return -1;
    }
    else
    {
        cout << "INFO: " << "opening port: " << port_name.toLatin1().constData() << " successful" << endl;
    }

    write_data.append("$CC");
    checksum = 0;
    for (int i = 1; i < write_data.size(); i++)
    {
        checksum = checksum ^ write_data[i];
    }
    write_data.append('*');
    write_data.append(QString("%1").arg(checksum, 2, 16, QLatin1Char('0')));
    write_data.append(0x0D);
    write_data.append(0x0A);

    temp_string = write_data.constData();
    temp_string.chop(2);
    cout << "VERBOSE: " << "sent command: " << temp_string.toLatin1().constData() << endl;

    data_length = com_port.write(write_data);

    if (data_length == -1)
    {
        com_port.close();
        cout << "ERROR: " << "write failed" << endl;
        com_lock->unlock();
        return -1;
    }
    else if (data_length != write_data.size())
    {
        com_port.close();
        cout << "ERROR: " << "write failed to send all data" << endl;
        com_lock->unlock();
        return -1;
    }
    else if (false == com_port.waitForBytesWritten(500))
    {
        com_port.close();
        cout << "ERROR: " << "write timed out" << endl;
        com_lock->unlock();
        return -1;
    }

    // check response
    read_data = com_port.readAll();
    com_port.waitForReadyRead(50);
    for (int i=0; i < 32; i++)
    {
        if (read_data.endsWith(0x0A))
        {
            break;
        }

        if(0 != com_port.bytesAvailable())
        {
            read_data.append(com_port.readAll());
        }
        else
        {
            com_port.waitForReadyRead(1);
        }
    }

    if (com_port.error() == QSerialPort::ReadError)
    {
        com_port.close();
        cout << "ERROR: " << "read failed" << endl;
        com_lock->unlock();
        return -1;
    }
    else if (com_port.error() == QSerialPort::TimeoutError && read_data.isEmpty())
    {
        com_port.close();
        cout << "ERROR: " << "no response received" << endl;
        com_lock->unlock();
        return -1;
    }

    temp_string = read_data.constData();
    temp_string.chop(2);
    cout << "VERBOSE: " << "received command: " << temp_string.toLatin1().constData() << endl;

    if (false == read_data.startsWith("$CR"))
    {
        com_port.close();
        cout << "WARNING " << "no correct response received (not our device?)" << endl;
        com_lock->unlock();
        return -1;
    }

    cout << "INFO: " << "connected correctly to target on: " << port_name.toLatin1().constData() << endl;
    cout << endl;

    is_open = true;

    com_lock->unlock();
    return 0;
}

Ucm_CommunicationLib::close_port()
{
    cout << "INFO: " << "Closing Port:" << endl;

    com_lock->lock();
    if (is_open == false)
    {
        cout << "ERROR: " << "port is already closed" << endl;
        com_lock->unlock();
        return -1;
    }

    com_port.close();

    cout << "INFO: " << "closing port successful" << endl;
    cout << endl;

    is_open = false;

    com_lock->unlock();
    return 0;
}

Ucm_CommunicationLib::write_reg(const unsigned int addr, unsigned int& data)
{
    int data_length;
    QByteArray temp_data;
    QByteArray read_data;
    QByteArray write_data;
    unsigned char checksum;
    QString temp_string;

    cout << "INFO: " << "Write Register:" << endl;

    com_lock->lock();
    if (is_open == false)
    {
        cout << "ERROR: " << "port is not open" << endl;
        com_lock->unlock();
        return -1;
    }

    write_data.append("$WC,");
    write_data.append(QString("0x%1").arg(addr, 8, 16, QLatin1Char('0')));
    write_data.append(',');
    write_data.append(QString("0x%1").arg(data, 8, 16, QLatin1Char('0')));
    checksum = 0;
    for (int i = 1; i < write_data.size(); i++)
    {
        checksum = checksum ^ write_data[i];
    }
    write_data.append('*');
    write_data.append(QString("%1").arg(checksum, 2, 16, QLatin1Char('0')));
    write_data.append(0x0D);
    write_data.append(0x0A);

    temp_string = write_data.constData();
    temp_string.chop(2);
    cout << "VERBOSE: " << "sent command: " << temp_string.toLatin1().constData() << endl;

    data_length = com_port.write(write_data);

    if (data_length == -1)
    {
        cout << "ERROR: " << "write failed" << endl;
        com_lock->unlock();
        return -1;
    }
    else if (data_length != write_data.size())
    {
        cout << "ERROR: " << "write failed to send all data" << endl;
        com_lock->unlock();
        return -1;
    }
    else if (false == com_port.waitForBytesWritten(500))
    {
        cout << "ERROR: " << "write timed out" << endl;
        com_lock->unlock();
        return -1;
    }

    // check response
    read_data = com_port.readAll();
    com_port.waitForReadyRead(50);
    for (int i=0; i < 32; i++)
    {
        if (read_data.endsWith(0x0A))
        {
            break;
        }

        if(0 != com_port.bytesAvailable())
        {
            read_data.append(com_port.readAll());
        }
        else
        {
            com_port.waitForReadyRead(1);
        }
    }

    if (com_port.error() == QSerialPort::ReadError)
    {
        cout << "ERROR: " << "read failed" << endl;
        com_lock->unlock();
        return -1;
    }
    else if (com_port.error() == QSerialPort::TimeoutError && read_data.isEmpty())
    {
        cout << "ERROR: " << "no response received" << endl;
        com_lock->unlock();
        return -1;
    }

    temp_string = read_data.constData();
    temp_string.chop(2);
    cout << "VERBOSE: " << "received command: " << temp_string.toLatin1().constData() << endl;

    temp_data.clear();
    temp_data.append("$WR,0x");
    temp_string = QString("%1").arg(addr, 8, 16, QLatin1Char('0'));
    temp_string = temp_string.toUpper();
    temp_data.append(temp_string);

    if (false == read_data.startsWith(temp_data))
    {
        cout << "ERROR: " << "no correct response received" << endl;
        com_lock->unlock();
        return -1;
    }

    checksum = 0;
    for (int i = 1; i < read_data.size(); i++)
    {
        if (read_data[i] == '*')
        {
            break;
        }
        else
        {
            checksum = checksum ^ read_data[i];
        }
    }

    temp_data.clear();
    temp_data.append(QString("%1").arg(checksum, 2, 16, QLatin1Char('0')));
    temp_data = temp_data.toUpper();
    temp_data.append(0x0D);
    temp_data.append(0x0A);
    if (false == read_data.endsWith(temp_data))
    {
        cout << "ERROR: " << "checksum wrong" << endl;
        cout << "checksum: " << temp_data.constData() << endl;
    }

    cout << endl;
    com_lock->unlock();
    return 0;
}

Ucm_CommunicationLib::read_reg(const unsigned int addr, unsigned int& data)
{
    int data_length;
    QByteArray temp_data;
    QByteArray read_data;
    QByteArray write_data;
    unsigned char checksum;
    QString temp_string;

    cout << "INFO: " << "Read Register:" << endl;

    com_lock->lock();
    if (is_open == false)
    {
        cout << "ERROR: " << "port is not open" << endl;
        com_lock->unlock();
        return -1;
    }

    write_data.append("$RC,");
    write_data.append(QString("0x%1").arg(addr, 8, 16, QLatin1Char('0')));
    checksum = 0;
    for (int i = 1; i < write_data.size(); i++)
    {
        checksum = checksum ^ write_data[i];
    }
    write_data.append('*');
    write_data.append(QString("%1").arg(checksum, 2, 16, QLatin1Char('0')));
    write_data.append(0x0D);
    write_data.append(0x0A);

    temp_string = write_data.constData();
    temp_string.chop(2);
    cout << "VERBOSE: " << "sent command: " << temp_string.toLatin1().constData() << endl;

    data_length = com_port.write(write_data);

    if (data_length == -1)
    {
        cout << "ERROR: " << "write failed" << endl;
        com_lock->unlock();
        return -1;
    }
    else if (data_length != write_data.size())
    {
        cout << "ERROR: " << "write failed to send all data" << endl;
        com_lock->unlock();
        return -1;
    }
    else if (false == com_port.waitForBytesWritten(500))
    {
        cout << "ERROR: " << "write timed out" << endl;
        com_lock->unlock();
        return -1;
    }

    // check response
    read_data = com_port.readAll();
    com_port.waitForReadyRead(50);
    for (int i=0; i < 32; i++)
    {
        if (read_data.endsWith(0x0A))
        {
            break;
        }

        if(0 != com_port.bytesAvailable())
        {
            read_data.append(com_port.readAll());
        }
        else
        {
            com_port.waitForReadyRead(1);
        }
    }

    if (com_port.error() == QSerialPort::ReadError)
    {
        cout << "ERROR: " << "read failed" << endl;
        com_lock->unlock();
        return -1;
    }
    else if (com_port.error() == QSerialPort::TimeoutError && read_data.isEmpty())
    {
        cout << "ERROR: " << "no response received" << endl;
        com_lock->unlock();
        return -1;
    }

    temp_string = read_data.constData();
    temp_string.chop(2);
    cout << "VERBOSE: " << "received command: " << temp_string.toLatin1().constData() << endl;

    temp_data.clear();
    temp_data.append("$RR,0x");
    temp_string = QString("%1").arg(addr, 8, 16, QLatin1Char('0'));
    temp_string = temp_string.toUpper();
    temp_data.append(temp_string);
    if (false == read_data.startsWith(temp_data))
    {
        cout << "ERROR: " << "no correct response received" << endl;
        com_lock->unlock();
        return -1;
    }


    checksum = 0;
    for (int i = 1; i < read_data.size(); i++)
    {
        if (read_data[i] == '*')
        {
            break;
        }
        else
        {
            checksum = checksum ^ read_data[i];
        }
    }

    temp_data.clear();
    temp_data.append(QString("%1").arg(checksum, 2, 16, QLatin1Char('0')));
    temp_data = temp_data.toUpper();
    temp_data.append(0x0D);
    temp_data.append(0x0A);
    if (false == read_data.endsWith(temp_data))
    {
        cout << "ERROR: " << "checksum wrong" << endl;
        cout << "checksum: " << temp_data.constData() << endl;
    }

    temp_data.clear();
    temp_data = read_data.mid(17,8);

    data = temp_data.toUInt(nullptr, 16);

    cout << endl;
    com_lock->unlock();
    return 0;
}
