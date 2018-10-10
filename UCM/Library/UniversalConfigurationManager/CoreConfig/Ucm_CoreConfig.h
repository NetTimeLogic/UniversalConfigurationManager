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

#ifndef UCM_CORECONFIG_H
#define UCM_CORECONFIG_H

#include <string>
using namespace std;

#define Ucm_CoreConfig_ConfSlaveCoreType                1
#define Ucm_CoreConfig_ClkClockCoreType                 2
#define Ucm_CoreConfig_ClkSignalGeneratorCoreType       3
#define Ucm_CoreConfig_ClkSignalTimestamperCoreType     4
#define Ucm_CoreConfig_IrigSlaveCoreType                5
#define Ucm_CoreConfig_IrigMasterCoreType               6
#define Ucm_CoreConfig_PpsSlaveCoreType                 7
#define Ucm_CoreConfig_PpsMasterCoreType                8
#define Ucm_CoreConfig_PtpOrdinaryClockCoreType         9
#define Ucm_CoreConfig_PtpTransparentClockCoreType      10
#define Ucm_CoreConfig_PtpHybridClockCoreType           11
#define Ucm_CoreConfig_RedHsrPrpCoreType                12
#define Ucm_CoreConfig_RtcSlaveCoreType                 13
#define Ucm_CoreConfig_RtcMasterCoreType                14
#define Ucm_CoreConfig_TodSlaveCoreType                 15
#define Ucm_CoreConfig_TodMasterCoreType                16
#define Ucm_CoreConfig_TapSlaveCoreType                 17
#define Ucm_CoreConfig_DcfSlaveCoreType                 18
#define Ucm_CoreConfig_DcfMasterCoreType                19
#define Ucm_CoreConfig_RedTsnCoreType                   20

class Ucm_CoreConfig
{
    public:
        unsigned int core_type;
        unsigned int core_instance_nr;
        unsigned int address_range_low;
        unsigned int address_range_high;
        unsigned int interrupt_mask;


        Ucm_CoreConfig();
        ~Ucm_CoreConfig();

};

#endif // Ucm_CoreConfig_H
