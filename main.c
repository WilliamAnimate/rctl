#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "rctl.h"

// TODO: scan /etc/runlevels instead
char *RUNLEVELS[] = { "default", "boot", "nonetwork", "sysinit", "shutdown" };

char* invoke_rc_service(char* argv[], const char* opstr) {
    const char* rc_service = "rc-service ";
    int size = strlen(argv[2]) + strlen(opstr) + strlen(rc_service) + 1;
    char* to_exec = malloc(size);
    strcpy(to_exec, rc_service);
    strcat(to_exec, argv[2]);
    strcat(to_exec, " ");
    strcat(to_exec, opstr);

    return to_exec;
}

char* invoke_rc_update(char* argv[], const char* opstr) {
    char* runlevel_opt = NULL;
    for (int i = 0; i < 5 /* TODO: runlevels length may change. */; i++) {
        DEBUGPRINT("trying %s == %s\n", argv[2], RUNLEVELS[i]);
        if (strcmp(argv[2], RUNLEVELS[i]) == 0) {
            DEBUGPRINT("runlevel %s of %i\n", RUNLEVELS[i], i);
            runlevel_opt = strdup(RUNLEVELS[i]);

            break;
        }
    }
    DEBUGPRINT("enable %s\n", argv[2]);
    DEBUGPRINT("%s\n", runlevel_opt);

    char* to_exec = NULL;
    const char* rc_update = "rc-update ";
    if (runlevel_opt == NULL) {
        int size = strlen(argv[2]) + strlen(rc_update) + strlen(opstr) + 1;
        char* to_exec_i = malloc(size);

        strcpy(to_exec_i, rc_update);
        strcat(to_exec_i, opstr);
        strcat(to_exec_i, argv[2]);

        to_exec = to_exec_i;
    } else {
        int size = strlen(runlevel_opt) + strlen(argv[3]) + strlen(opstr) + strlen(rc_update) + 2;
        char* to_exec_i = malloc(size);
        strcpy(to_exec_i, rc_update);
        strcat(to_exec_i, opstr);
        strcat(to_exec_i, argv[3]);
        strcat(to_exec_i, " ");
        strcat(to_exec_i, runlevel_opt);

        to_exec = to_exec_i;
    }

    free(runlevel_opt);
    return to_exec;
}

int main(int argc, char* argv[]) {
    /*
     * CLI draft
     *
     * rctl enable evremap .......... rc-update add evremap
     * rctl enable default evremap .. rc-update add evremap default
     * rctl enable boot evremap ..... rc-update add evremap boot
     * rctl enable RUNLEVEL evremap . rc-update add evremap RUNLEVEL
     *
     * rctl disable evremap ......... rc-update del evremap
     * rctl disable default evremap . rc-update del evremap default
     * rctl disable boot evremap .... rc-update del evremap boot
     * rctl disable RUNLEVEL evremap..rc-update del evremap RUNLEVEL
     * ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
     * also add --now versions of each to also run the assocated rc-service:
     * rctl disable evremap --now ... rc-update del evremap && rc-service evremap stop
     *
     * rctl start evremap ........... rc-service evremap start
     * rctl stop evremap ............ rc-service evremap stop
     */

#ifdef DEBUG
    for (int i = 0; i < argc; i++) {
        DEBUGPRINT("argv[%i]: %s\n", i, argv[i]);
    }
#endif

    // basic safety
    if (argc < 2) {
        /* sprintf(STDERR_FILENO, "Not enough options!"); */
        puts("not enough options\n");
        return 1;
    }
    bool run_supplementary_cmd = false;
    if (argc > 3) {
        DEBUGPRINT("%s\n", "suppkefljsd");
        if (strcmp(argv[3], "--now") == 0) {
            run_supplementary_cmd = true;
        }
    }

    char* exec = NULL;
    char* supplementary_exec = NULL;
    if (argc == 2) {
        if (strcmp(argv[1], "show") == 0) {
            exec = strdup("rc-update show -v"); // :skull:
        }
#ifdef SOMETHING_LIKE_SYSTEMD
        else if (strcmp(argv[1], "start-computer-hardware") == 0) {
            fprintf(stderr, "Unrecoverable failure: computer hardware is already online!\n");
            abort();
        }
        else if (strcmp(argv[1], "is-system-running") == 0) {
            fprintf(stderr, "\"Exactly. The predisposition of being able to call such a complex command does not imply the running system.\"\n\
    - https://suckless.org/sucks/systemd/\n");
            if (fopen("/usr/lib/systemd/systemd", "r") != NULL) {
                fprintf(stderr, "borked\n");
            } else if (fopen("/bin/openrcs", "r") == NULL) {
                fprintf(stderr, "degraded\n");
            }
#ifdef SOMETHING_LIKE_SYSTEMD_ALLOW_POSSIBLE_DATA_LOSS
            FILE *file = fopen("/proc/sysrq-trigger", "w");
            if (file == NULL) { perror("file"); }
            fprintf(file, "o"); // turn off computer without syncing
                                // SAFETY: memory safe, but you _will_ lose unsynced data
            fclose(file);
#endif

            abort();
        }
#endif
        else {
            puts("invalid args");
            return 1;
        }
    } else if (argc > 2) {
        if (strcmp(argv[1], "enable") == 0) {
            exec = invoke_rc_update(argv, "add ");
            if (run_supplementary_cmd) {
                DEBUGPRINT("%s\n", "hm");
                supplementary_exec = invoke_rc_service(argv, "start");
            }
        } else if (strcmp(argv[1], "disable") == 0) {
            exec = invoke_rc_update(argv, "del ");
            if (run_supplementary_cmd) {
                DEBUGPRINT("%s\n", "hm");
                supplementary_exec = invoke_rc_service(argv, "stop");
            }
        } else if (strcmp(argv[1], "start") == 0) {
            exec = invoke_rc_service(argv, "start");
        } else if (strcmp(argv[1], "stop") == 0) {
            exec = invoke_rc_service(argv, "stop");
        } else if (strcmp(argv[1], "status") == 0) {
            exec = invoke_rc_service(argv, "status");
        } else if (strcmp(argv[1], "show") == 0 && strcmp(argv[2], "crashed") == 0) {
            exec = strdup("rc-status -c");
        }
    }

    DEBUGPRINT("command to run: %s\n", exec);
    system(exec);
    if (supplementary_exec != NULL) {
        assert(run_supplementary_cmd);
        DEBUGPRINT("supplementary to run: %s\n", supplementary_exec);
        system(supplementary_exec);
    }

    free(exec);
    free(supplementary_exec);

    return 0;
}
