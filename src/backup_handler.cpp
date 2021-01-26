#include <custom_headers/backup_handler.h>

// Benötigt die Instanz eines Backupplans
CBackupHandler::CBackupHandler(CBackupEntity &current_backup_entity)
{
    this->set_current_backup_entity(current_backup_entity);
};

// Setter Pointer zum aktuellen Backupplan
void CBackupHandler::set_current_backup_entity(CBackupEntity &current_backup_entity)
{
    this->current_backup_entity = &current_backup_entity;
};

// Getter Instanz aktueller Backupplan
CBackupEntity *CBackupHandler::get_current_backup_entity()
{
    return this->current_backup_entity;
}

// Backup wird durchgeführt, falls die Datei/ordner verändert wurde und noch kein Backup für diese Version existiert
void CBackupHandler::handle_backup()
{
    // Aktuelle Backupplaninstanz
    CBackupEntity *current_entity = this->get_current_backup_entity();

    // Auslesen letzte Modifikation
    std::filesystem::file_time_type ftime = std::filesystem::last_write_time(current_entity->get_path());
    int last_modification = CBackupHandler::convert_to_unix_timestamp(ftime);

    // Falls die letzte Modifikation neuer ist als das letzte Backup -> Backup durchführen
    if (last_modification > current_entity->get_last_backup())
    {
        try
        {
            // Prüfen ob ein Ordner oder eine Datei gesichert werden soll
            if (std::filesystem::is_directory(current_entity->get_path()))
            {
                std::string system_name = PLATFORM_NAME;
                std::string foldername;

                // Prüfen des Betriebssystems und extrahieren des Ordnernamens
                if (system_name == "windows")
                {
                    foldername = current_entity->get_path().erase(0, current_entity->get_path().find_last_of("\\"));
                }
                else
                {
                    foldername = current_entity->get_path().erase(0, current_entity->get_path().find_last_of("/"));
                }

                // Erzeugen eines neuen eindeutigen Ordnernamens
                foldername = foldername + "_Backup_" + std::to_string(last_modification);
                // Kopieren des Ordners und Inhalts zum Backuppfad
                std::filesystem::copy(current_entity->get_path(), current_entity->get_backup_path() + foldername, std::filesystem::copy_options::recursive);
                std::cout << "BACKUP von: \"" << current_entity->get_path() << "\" erfolgreich durchgefuehrt." << std::endl;

                // Aktualisieren der letzten Modifikation des Ordners und des Zeitstemples für das letzte Backup
                ftime = std::filesystem::last_write_time(current_entity->get_backup_path() + foldername);
                last_modification = CBackupHandler::convert_to_unix_timestamp(ftime);
                ftime = std::filesystem::last_write_time(current_entity->get_path());
                int last_modification_folder = CBackupHandler::convert_to_unix_timestamp(ftime);

                current_entity->set_last_backup(last_modification);
                current_entity->set_last_modification(last_modification_folder);
            }
            // Falls es sich um eine Datei handelt
            else
            {
                std::string system_name = PLATFORM_NAME;
                std::string filename;

                // Prüfen des Betriebssystems und extrahieren des Dateinamens und Erzeugen eines neuen eindeutigen Dateinamens
                if (system_name == "windows")
                {
                    filename = current_entity->get_path().erase(0, current_entity->get_path().find_last_of("\\") + 1);
                    filename = "\\Backup_" + std::to_string(last_modification) + "_" + filename;
                }
                else
                {
                    filename = current_entity->get_path().erase(0, current_entity->get_path().find_last_of("/") + 1);
                    filename = "/Backup_" + std::to_string(last_modification) + "_" + filename;
                }

                // Kopieren der Datei zum Backuppfad
                std::filesystem::copy(current_entity->get_path(), current_entity->get_backup_path() + filename, std::filesystem::copy_options::recursive);
                std::cout << "BACKUP von: \"" << current_entity->get_path() << "\" erfolgreich durchgefuehrt." << std::endl;

                // Aktualisieren der letzten Modifikation des Ordners und des Zeitstemples für das letzte Backup
                ftime = std::filesystem::last_write_time(current_entity->get_backup_path() + filename);
                last_modification = CBackupHandler::convert_to_unix_timestamp(ftime);
                ftime = std::filesystem::last_write_time(current_entity->get_path());
                int last_modification_file = CBackupHandler::convert_to_unix_timestamp(ftime);

                current_entity->set_last_backup(last_modification);
                current_entity->set_last_modification(last_modification_file);
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
        }
    }
    else
    {
        std::cout << "KEIN BACKUP fuer: \"" << current_entity->get_path() << "\" notwendig." << std::endl;
    }
};

// Umwandlung filesystem::last_write_time zu unix Zeitstempel
template <typename TP>
std::time_t CBackupHandler::convert_to_unix_timestamp(TP tp)
{

    auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(tp - TP::clock::now() + std::chrono::system_clock::now());
    return std::chrono::system_clock::to_time_t(sctp);
}
