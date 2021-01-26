#include <custom_headers/backup_entity.h>

// Konstruktor
CBackupEntity::CBackupEntity(){
    //std::cout << "Hello" << std::endl;
};

// Destruktor
CBackupEntity::~CBackupEntity(){
    //std::cout << "ByeBye" << std::endl;
};

// Setter ID
void CBackupEntity::set_id(int id)
{
    this->id = id;
};

// Getter ID
int CBackupEntity::get_id()
{
    return this->id;
};

// Setter Pfad zur Datei / zum Ordner welcher gesichert werden soll
void CBackupEntity::set_path(std::string path)
{
    this->path = path;
};

// Getter Datei / zum Ordner welcher gesichert werden soll
std::string CBackupEntity::get_path()
{
    return this->path;
};

// Setter Backuppfad
void CBackupEntity::set_backup_path(std::string backup_path)
{
    this->backup_path = backup_path;
};

// Getter Backuppfad
std::string CBackupEntity::get_backup_path()
{
    return this->backup_path;
};

// Setter Zeitstempel der letzten Modifiktion der Datei/Ordner
void CBackupEntity::set_last_modification(int unix_timestamp)
{
    this->last_modification = unix_timestamp;
};

// Getter Zeitstempel der letzten Modifiktion der Datei/Ordner
int CBackupEntity::get_last_modification()
{
    return this->last_modification;
};

// Optional: Setter Modus des Backups z.B via ssh, lokal
void CBackupEntity::set_backup_mode(int backup_mode)
{
    this->backup_mode = backup_mode;
};

// Optional: Getter Modus des Backups z.B via ssh, lokal
int CBackupEntity::get_backup_mode()
{
    return this->backup_mode;
};

// Setter Zeitstempel des letzten Backups
void CBackupEntity::set_last_backup(int last_backup)
{
    this->last_backup = last_backup;
};

// Getter Zeitstempel des letzten Backups
int CBackupEntity::get_last_backup()
{
    return this->last_backup;
};

// Umwandeln eines Backupsplans in JSON
json CBackupEntity::backup_entity_to_json(CBackupEntity entity)
{
    json json_entity = {{"id", entity.get_id()},
                        {"path", entity.get_path()},
                        {"backup_path", entity.get_backup_path()},
                        {"last_modification", entity.get_last_modification()},
                        {"backup_mode", entity.get_backup_mode()},
                        {"last_backup", entity.get_last_backup()}};

    return json_entity;
}

// Erzeugen einer Backupplaninstanz aus JSON
CBackupEntity CBackupEntity::backup_entity_from_json(json entity)
{
    CBackupEntity new_entity = CBackupEntity();
    new_entity.set_id(entity["id"]);
    new_entity.set_path(entity["path"]);
    new_entity.set_backup_path(entity["backup_path"]);
    new_entity.set_last_modification(entity["last_modification"]);
    new_entity.set_backup_mode(entity["backup_mode"]);
    new_entity.set_last_backup(entity["last_backup"]);

    return new_entity;
}