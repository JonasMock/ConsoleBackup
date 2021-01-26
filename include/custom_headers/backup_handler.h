#ifndef __BACKUP_HANDLER_H__
#define __BACKUP_HANDLER_H__

// Überprüfen welches OS verwendet wird.
// Student verwendet OSX und Dozent voraussichtlich Windows
#if defined(_WIN32)
#define PLATFORM_NAME "windows"
#elif defined(_WIN64)
#define PLATFORM_NAME "windows"
#elif defined(__CYGWIN__) && !defined(_WIN32)
#define PLATFORM_NAME "windows"
#elif defined(__APPLE__)
#define PLATFORM_NAME "osx"
#elif defined(__linux__)
#define PLATFORM_NAME "linux"
#endif

#include <iostream>
#include <fstream>
#include <filesystem>

#include <custom_headers/backup_entity.h>

class CBackupHandler
{

private:
    CBackupEntity *current_backup_entity;

    void set_current_backup_entity(CBackupEntity &current_backup_entity);

    CBackupEntity *get_current_backup_entity();

public:
    CBackupHandler(CBackupEntity &current_backup_entity);

    void handle_backup();
    
    template <typename TP>
    static std::time_t convert_to_unix_timestamp(TP tp);
};

#endif