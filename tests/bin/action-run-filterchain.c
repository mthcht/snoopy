/*
 * SNOOPY LOGGER
 *
 * Copyright (c) 2015 Bostjan Skufca Jese <bostjan@a2o.si>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */



/*
 * Includes order: from local to global
 */
#include "action-common.h"

#include "snoopy.h"
#include "entrypoint/test-cli.h"
#include "filtering.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>






void snoopyTestCli_action_run_filterchain_showHelp ()
{
    char * helpContent =
        "Snoopy TEST SUITE CLI utility :: Action `run` :: Subsystem `filterchain`\n"
        "\n"
        "Usage:\n"
        "    snoopy-test run filterchain \"MESSAGE\" \"FILTER_CHAIN\"\n"
        "    snoopy-test run filterchain --help\n"
        "\n"
        "Description:\n"
        "    Runs MESSAGE through a specified FILTER_CHAIN, with filters acting on the data taken from the current process.\n"
        "    Filter chain specification format is described in the comments of snoopy.ini.\n"
        "\n"
        "Result:\n"
        "    Prints the result of a filter chain as a \"PASS\" or a \"DROP\" to stdout.\n"
        "    Sets the exit status to 0 or PASS or 1 for DROP.\n"
        "\n";
    printf("%s", helpContent);
}



int snoopyTestCli_action_run_filterchain (int argc, char **argv)
{
    char * message;
    char * messageCopy;
    const char * filterChain;
    int filterResult;


    /* Initialize Snoopy */
    snoopy_entrypoint_test_cli_init((char const *)g_argv[0], g_argv, NULL);


    /* Check if all arguments are present */
    if (argc < 1) {
        snoopyTestCli_action_run_filterchain_showHelp();
        fatalError("Missing argument: log message, --help");
    }
    message = argv[0];
    if (0 == strcmp(message, "--help")) {
        snoopyTestCli_action_run_filterchain_showHelp();
        return 0;
    }

    if (argc < 2) {
        snoopyTestCli_action_run_filterchain_showHelp();
        fatalError("Missing argument: filter chain specification");
    }
    filterChain = argv[1];


    // Copy the message into a separate buffer - filters may mangle it (for now)
    messageCopy = malloc(SNOOPY_LOG_MESSAGE_MAX_SIZE);
    messageCopy[0] = '\0';
    strncpy(messageCopy, message, SNOOPY_LOG_MESSAGE_MAX_SIZE-1);
    messageCopy[SNOOPY_LOG_MESSAGE_MAX_SIZE-1] = '\0'; // Just in case


    // Process the filter chain
    filterResult = snoopy_filtering_check_chain(messageCopy, filterChain);


    /* Housekeeping */
    snoopy_entrypoint_test_cli_exit();


    /* Display and return */
    if (SNOOPY_FILTER_PASS == filterResult) {
        if (0 != strncmp(message, messageCopy, SNOOPY_LOG_MESSAGE_MAX_SIZE)) {
            printf("Filter chain has modified the message:\n");
            printf("    Filter chain:     %s\n", filterChain);
            printf("    Original message: %s\n", message);
            printf("    Adjusted message: %s\n", messageCopy);
        }
        printf("PASS\n");
        return 0;
    } else {
        printf("DROP\n");
        return 1;
    }
}
