/* Raw access to dd64.ini entries (port/src/config.c), for settings that are not numbers. */
#ifndef PORT_INI_H
#define PORT_INI_H

const char *Port_IniGet(const char *section, const char *key, const char *def);
int Port_IniGetInt(const char *section, const char *key, int def);
void Port_IniSet(const char *section, const char *key, const char *value);
void Port_IniSetInt(const char *section, const char *key, int value);

#endif
