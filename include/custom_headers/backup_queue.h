#ifndef __BACKUP_QUEUE_H__
#define __BACKUP_QUEUE_H__

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
#include <filesystem>

#include <external_headers/json.hpp>
#include <external_headers/CppConsoleTable.hpp>

#include <custom_headers/database.h>
#include <custom_headers/backup_entity.h>
#include <custom_headers/backup_handler.h>

class CBackupQueue
{

private:
    std::vector<CBackupEntity> backup_entities;

    CDatabase *database_ptr;

    void set_database_instance(CDatabase &database_instance);

    CDatabase *get_database_instance();

    void set_backup_entities(std::vector<CBackupEntity> backup_entities);

    std::vector<CBackupEntity> get_backup_entities();

    void initialize_backup_entities();
    
    void save_backup_entities(std::vector<CBackupEntity> backup_entities);

    void add_backup_entity();

    void delete_backup_entity(int index);

    void modify_backup_entity(CBackupEntity backup_entity);

    std::vector<int> query_backup_entity(std::string entity_path);

    void backup_queue_menu();

    void backup_queue_menu_clear();
    
    void handle_backup_queue();

public:
    CBackupQueue(CDatabase &database_instance, bool service);
};

#endif