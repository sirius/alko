// SPDX-FileCopyrightText: 2024 Sirius Bakke <sirius@bakke.co>
// SPDX-License-Identifier: Apache-2.0

#ifndef ALKO_CONFIG_H
#define ALKO_CONFIG_H

#define ALKO_CONFIGURATION__SERVICE_NAME "Alko"
#define ALKO_CONFIGURATION__SERVICE_DESCRIPTION "Alko Daemon"
#define ALKO_CONFIGURATION__LOG_FILE "alko.log"
#define ALKO_CONFIGURATION__HTTP_PORT 12345
#define ALKO_CONFIGURATION__HTTP_AUTH "Basic b2IxOmhlbGxvIHRoZXJl"
#define ALKO_CONFIGURATION__CUSTOMER_SERVER_ADDR "127.0.0.1"
#define ALKO_CONFIGURATION__CUSTOMER_SERVER_PORT 8080
#define ALKO_CONFIGURATION__CUSTOMER_SERVER_REQUEST "GET /customer?checkonly=%2&id=%1\r\n"
#define ALKO_CONFIGURATION__CUSTOMER_DEBUG_ID "010101"

#endif // ALKO_CONFIG_H