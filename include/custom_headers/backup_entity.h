#ifndef __BACKUP_ENTITY_H__
#define __BACKUP_ENTITY_H__

#include <iostream>
#include <external_headers/json.hpp>

using json = nlohmann::json;

class CBackupEntity
{

private:
    int id;

    std::string path;

    std::string backup_path;

    int last_modification;

    int backup_mode;

    int last_backup;

public:
    CBackupEntity();

    ~CBackupEntity();

    void set_id(int id);

    int get_id();

    void set_path(std::string path);

    std::string get_path();

    void set_backup_path(std::string backup_path);

    std::string get_backup_path();

    void set_last_modification(int unix_timestamp);

    int get_last_modification();

    void set_backup_mode(int backup_mode);

    int get_backup_mode();

    void set_last_backup(int last_backup);

    int get_last_backup();

    static json backup_entity_to_json(CBackupEntity entity);
    
    static CBackupEntity backup_entity_from_json(json entity);
};

#endif