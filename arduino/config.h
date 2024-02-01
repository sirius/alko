// SPDX-FileCopyrightText: 2024 Sirius Bakke <sirius@bakke.co>
// SPDX-License-Identifier: Apache-2.0

#ifndef CONFIG_H
#define CONFIG_H

#define VERSION_STR "Pilsomat v3999"
#define LCD_ADDRESS 4
#define LCD_COLS 20
#define LCD_ROWS 4

#define NARCO_TIMER 1000 // 1 sec
#define PING_TIMER 30000 // 30 sec
#define MOTOR_START_TIMER 3000 // 3 sec
#define MOTOR_RUN_TIMER 13000 // 13 sec

#define SERVICE_COLOR 150,150,0
#define SERVICE_TEXT_2_STR ""
#define SERVICE_TEXT_3_STR "Status: deaktivert"
#define FORCE_SERVICE_TIME 8000 // 8 sec
#define FORCE_SERVICE_TEXT_STR "Tvungen deaktivering"
#define PREPARE_READY_COLOR 255,255,0
#define PREPARE_READY_TEXT_2_STR "Et lite \x3ryeblikk"
#define PREPARE_READY_TEXT_3_STR "Status: straks klar"
#define PREPARE_READY_TIMER 3000
#define READY_COLOR 0,50,255
#define READY_TEXT_2_STR "Dra kort; stripe opp"
#define READY_TEXT_3_STR "Status: klar"
#define READY_WAIT_COLOR 255,255,0
#define READY_WAIT_TEXT_2_STR "Kommunikasjonsbrudd"
#define READY_WAIT_TEXT_3_STR "Status: frakoblet"
#define DEPLOY_COLOR 0,255,0
#define DEPLOY_ERROR_TEXT_0_STR "ADVARSEL: En feil"
#define DEPLOY_ERROR_TEXT_1_STR "oppstod! Pr\x3ver en"
#define DEPLOY_ERROR_TEXT_2_STR "gang til."
#define DEPLOY_ERROR_TEXT_3_STR "  Vennligst vent."
#define DEPLOY_ERROR_TIMER 5000
#define PREPARE_AUTO_HOME_ERROR_COLOR 255,0,0
#define PREPARE_AUTO_HOME_ERROR_TEXT_0_STR "FEIL: Du m\x5 vare"
#define PREPARE_AUTO_HOME_ERROR_TEXT_1_STR "i oppstart- eller"
#define PREPARE_AUTO_HOME_ERROR_TEXT_2_STR "servicemodus for \x5"
#define PREPARE_AUTO_HOME_ERROR_TEXT_3_STR "nullstille maskinen."
#define PREPARE_AUTO_HOME_TEXT_0_STR "ADVARSEL: Starter"
#define PREPARE_AUTO_HOME_TEXT_1_STR "nullstilling av"
#define PREPARE_AUTO_HOME_TEXT_2_STR "maskinen! Du kan"
#define PREPARE_AUTO_HOME_TEXT_3_STR "fortsatt avbryte."
#define PREPARE_AUTO_HOME_TIMER 10000
#define AUTO_HOME_COLOR 255,0,0
#define AUTO_HOME_TEXT_0_STR "Nullstiller maskinen"
#define AUTO_HOME_TEXT_1_STR "Vennligst vent!"
#define AUTO_HOME_TEXT_2_STR "Status: nullstiller"
#define INFO_TIMER 4000

#endif // CONFIG_H
