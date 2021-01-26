#include <custom_headers/backup_queue.h>

// Zum Erzeugen einer Instanz wird eine initialisierte Datenbank benötigt
CBackupQueue::CBackupQueue(CDatabase &database_instance, bool service)
{
    // Prüfen ob Programm als Service oder mit UI / Menü gestartet werden soll
    if (service)
    {
        // Initialisieren der Backupplan Objekte und Start ohne Menü
        this->set_database_instance(database_instance);
        this->initialize_backup_entities();
        std::cout << "#################################################\n"
                     "#                                               #\n"
                     "#    ConsoleBackup - Jonas Mock - Testat C++    #\n"
                     "#    Es folgen die Servicelogs                  #\n"
                     "#                                               #\n"
                     "#################################################\n\n";
        this->handle_backup_queue();
    }
    else
    {
        // Initialisieren der Backupplan Objekte und starten des Menüs
        this->set_database_instance(database_instance);
        this->initialize_backup_entities();
        this->backup_queue_menu();
    }
}

// Setter Adresse der Datenbankinstanz
void CBackupQueue::set_database_instance(CDatabase &database_instance)
{
    this->database_ptr = &database_instance;
};

// Getter Pointer zur Datenbankinstanz
CDatabase *CBackupQueue::get_database_instance()
{
    return this->database_ptr;
};

// Setter des Vectors welcher Backupplan Objekte beinhaltet
void CBackupQueue::set_backup_entities(std::vector<CBackupEntity> backup_entities)
{
    this->backup_entities = backup_entities;
};

// Getter des Vectors welcher Backupplan Objekte beinhaltet
std::vector<CBackupEntity> CBackupQueue::get_backup_entities()
{
    return this->backup_entities;
};

// Auslesen der Datenbank. Anschließend aus der JSON Datenbank ein Vector aus Backupplan Objekten erzeugen
void CBackupQueue::initialize_backup_entities()
{

    // Auslesen der JSON Datenbank
    json database_json = this->get_database_instance()->fetch_database();
    std::vector<CBackupEntity> backup_entities;

    try
    {
        // Wert des Schlüssels "backup_entities" ist eine Liste der gespeicherten Backuppläne. 
        for (json::iterator it = database_json["backup_entities"].begin(); it != database_json["backup_entities"].end(); it++)
        {
            // Jeder Eintrag aus der "backup_entities" Liste wird in ein Objekt umgewandelt
            CBackupEntity new_backup_entity = CBackupEntity::backup_entity_from_json(it[0]);

            // Aktualisieren des "Letzte Modifikation" Zeitstempels 
            std::filesystem::file_time_type ftime = std::filesystem::last_write_time(new_backup_entity.get_path());
            int last_modification = CBackupHandler::convert_to_unix_timestamp(ftime);
            new_backup_entity.set_last_modification(last_modification);

            // Hinzufügen der Erzeugten Backupplan Instanz zum Backupplan Vector
            backup_entities.push_back(new_backup_entity);
        }
    }
    catch (const std::exception &e)
    {
        // Zurücksetzen der Datenbank, falls JSON Format nicht korrekt
        std::cout << "Datenbank moeglicherweise korrupt. Datenbank wird zurückgesetzt." << std::endl;
        this->get_database_instance()->reset_database();
    }

    this->set_backup_entities(backup_entities);
};

// Umwandlung der Backupplan Instanzen in JSON. Anschließend Speicherung in JSON Datei.
void CBackupQueue::save_backup_entities(std::vector<CBackupEntity> backup_entities)
{
    json backup_entities_json = json::parse("{\"backup_entities\":[]}");

    for (auto &it : backup_entities)
    {
        backup_entities_json["backup_entities"].push_back(CBackupEntity::backup_entity_to_json(it));
    }

    this->get_database_instance()->update_database(backup_entities_json);
    this->get_database_instance()->save_database();
};

// Dialog zum Hinzufügen eines neuen Backupplans
void CBackupQueue::add_backup_entity()
{
    std::cout << "Bitte treffen Sie eine Auswahl:\n1 = Neuen Backupplan hinzufügen \n2 = Zurück zum Hauptmenü" << std::endl;

    this->backup_queue_menu_clear();

    std::srand(time(NULL) * 9999);
    std::string entity_path;
    std::string entity_backup_path;
    int last_modification;
    CBackupEntity new_entity;

    std::cout << "Bitte geben Sie den Pfad zur Datei / dem Ordner an welcher gesichert werden soll:" << std::endl;
    std::cin >> entity_path;

    // Prüfen ob Datei / Ordner des eingegeben Pfads bereits in einem Backupplan vorhanden ist
    if (this->query_backup_entity(entity_path).size() != 0)
    {
        this->backup_queue_menu_clear();
        std::cout << "Der Ordner / die Datei: " << entity_path << " wird bereits gesichtert.\n"
                  << std::endl;

        return;
    }
    // Prüfen ob eingegebener Pfad existiert
    else if (!std::filesystem::exists(entity_path))
    {
        this->backup_queue_menu_clear();
        std::cout << "Der Ordner / die Datei: " << entity_path << " existiert nicht.\n"
                  << std::endl;
        return;
    }

    std::cout << "Bitte geben Sie den Pfad zum Speicherort der Backups an:" << std::endl;
    std::cin >> entity_backup_path;

    // Prüfen ob Backuppfad existiert und ob es ein Ordner ist
    if (!std::filesystem::exists(entity_backup_path) || !std::filesystem::is_directory(entity_backup_path))
    {
        this->backup_queue_menu_clear();
        std::cout << "Der Backupordner " << entity_path << " existiert nicht.\n"
                  << std::endl;
        return;
    }
    // Prüfen ob Backuppfad gleich Datei/Ordner Pfad
    else if (entity_backup_path == entity_path)
    {
        this->backup_queue_menu_clear();
        std::cout << "Der Backuppfad muss sich vom Datei / Ordnerpfad unterscheiden.\n"
                  << std::endl;
        return;
    }

    // Auslesen der Letzten Modifikation
    std::filesystem::file_time_type ftime = std::filesystem::last_write_time(entity_path);
    last_modification = CBackupHandler::convert_to_unix_timestamp(ftime);

    // Backupplan in JSON Format
    json json_entity = {{"id", 0},
                        {"path", entity_path},
                        {"backup_path", entity_backup_path},
                        {"last_modification", last_modification},
                        {"backup_mode", 0},
                        {"last_backup", 0}};

    // Neues Objekt aus JSON erzeugen
    new_entity = CBackupEntity().backup_entity_from_json(json_entity);

    // Neuen Backupplan zum Vector hinzufügen und sichern der Daten
    std::vector<CBackupEntity> backup_entities = this->get_backup_entities();
    backup_entities.push_back(new_entity);
    this->set_backup_entities(backup_entities);
    this->save_backup_entities(backup_entities);

    this->backup_queue_menu_clear();
    std::cout << "Neuer Backupplan wurde angelegt.\n"
              << std::endl;
};

// Löschen eines Backupplans
void CBackupQueue::delete_backup_entity(int index)
{
    std::vector<CBackupEntity> backup_entities = this->get_backup_entities();
    backup_entities.erase(backup_entities.begin() + index);
    this->set_backup_entities(backup_entities);
    this->save_backup_entities(backup_entities);
};

// Optional: Bearbeitung vorhandener Backuppläne
void CBackupQueue::modify_backup_entity(CBackupEntity backup_entity){

};

// Backuppläne können eindeutig am Pfad identifiziert werden. Prüft ob bereits ein Plan für einen Pfad existiert
std::vector<int> CBackupQueue::query_backup_entity(std::string entity_path)
{

    std::vector<CBackupEntity> backup_entities = this->get_backup_entities();
    std::vector<int> result_indices;
    int index = 0;

    for (auto &it : backup_entities)
    {
        if (it.get_path() == entity_path)
        {
            result_indices.push_back(index);
        }
        index++;
    }

    return result_indices;
};


// UI Routine - Hauptmenü und Backupplanverwaltung
void CBackupQueue::backup_queue_menu()
{
    this->backup_queue_menu_clear();

    bool main_menu = true;

    // Hauptmenü
    while (main_menu)
    {
        bool backup_menu = true;
        std::cout << "#################################################\n"
                     "#                                               #\n"
                     "#    ConsoleBackup - Jonas Mock - Testat C++    #\n"
                     "#                                               #\n"
                     "#################################################\n\n";

        std::string eingabe;
        std::cout << "#################################################\n"
                     "#                 Hauptmenue                    #\n"
                     "#                                               #\n"
                     "#        1 - Backupplaene verwalten             #\n"
                     "#        2 - Programm beenden                   #\n"
                     "#                                               #\n"
                     "#################################################\n\nEingabe: ";
        std::cin >> eingabe;
        int eingabe_int;

        // Konvertierung der Eingabe zu int um switch case verwenden zu können
        try
        {
            eingabe_int = std::stoi(eingabe);
        }
        catch (const std::exception &e)
        {
            eingabe_int = 99;
        }

        // Logik des Hauptmenüs
        switch (eingabe_int)
        {
        case 1:
            // Verwaltung der Backuppläne
            while (backup_menu)
            {
                this->initialize_backup_entities();
                try
                {
                    std::vector<CBackupEntity> backup_entities = this->get_backup_entities();
                }
                catch (const std::exception &e)
                {
                    std::cerr << e.what() << '\n';
                }

                this->backup_queue_menu_clear();

                if (backup_entities.size() < 1)
                {
                    std::string eingabe;
                    std::cout << "#################################################\n"
                                 "#                                               #\n"
                                 "#        1 - Hauptmenue                         #\n"
                                 "#        2 - Backupplan hinuzügen               #\n"
                                 "#                                               #\n"
                                 "#################################################\n\nEingabe: ";
                    std::cin >> eingabe;
                    int eingabe_int;

                    try
                    {
                        eingabe_int = std::stoi(eingabe);
                    }
                    catch (const std::exception &e)
                    {
                        eingabe_int = 99;
                    }

                    switch (eingabe_int)
                    {
                    case 1:
                        backup_menu = false;
                        this->backup_queue_menu_clear();
                        break;
                    case 2:
                        this->add_backup_entity();
                        continue;
                        break;

                    default:
                        backup_menu = true;
                        this->backup_queue_menu_clear();
                        continue;
                        break;
                    }
                }
                else
                {

                    // Backuppläne werden in einer Tabelle dargestellt, hierfür wurde die externe CppConsoleTable.hpp verwendet
                    samilton::ConsoleTable table;

                    // Tabelle: Kopfzeile
                    table[0][0] = "Backupplan Nr.\n";
                    table[0][1] = "Datei / Ordner\n";
                    table[0][2] = "Backuppfad\n";
                    table[0][3] = "Letzte Modifikation\n";
                    table[0][4] = "Letztes Backup\n";

                    // Tabelle struct für verwendete Zeichen
                    samilton::ConsoleTable::TableChars chars;

                    // Tabelle: Verwendete Zeichen
                    chars.topDownSimple = '-';
                    chars.leftSeparation = '-';
                    chars.rightSeparation = '-';
                    chars.centreSeparation = '-';
                    chars.downLeft = '-';
                    chars.downRight = '-';
                    chars.topLeft = '-';
                    chars.topRight = '-';
                    chars.topSeparation = '-';
                    chars.downSeparation = '-';
                    chars.leftRightSimple = '|';

                    // Tabelle: Abstände
                    table.setTableChars(chars);
                    table.setIndent(2, 3);

                    int index = 1;

                    // Schleife über alle vorhandenen Backuppläne
                    for (auto &it : backup_entities)
                    {
                        std::string path = it.get_path();
                        std::string backup_path = it.get_backup_path();
                        std::time_t last_modification = it.get_last_modification();
                        std::time_t last_backup = it.get_last_backup();
                        std::string backup_time;

                        if (last_backup == 0)
                        {
                            backup_time = "Kein Backup vorhanden";
                        }
                        else
                        {
                            backup_time = std::asctime(std::localtime(&last_backup));
                        }

                        std::string liste[5] = {std::to_string(index), path, backup_path, std::asctime(std::localtime(&last_modification)), backup_time};

                        for (size_t i = 0; i < 5; i++)
                        {
                            table[index][i] = liste[i];
                        }

                        index++;
                    }
                    // Ausgabe der erzeugten Tabelle
                    std::cout << table << std::endl;

                    std::string eingabe;
                    std::cout << "#################################################\n"
                                 "#                                               #\n"
                                 "#        1 - Hauptmenue                         #\n"
                                 "#        2 - Backupplan hinuzfügen              #\n"
                                 "#        3 - Backupplan loeschen                #\n"
                                 "#        4 - Alle Backups starten               #\n"
                                 "#                                               #\n"
                                 "#     Hinweis: Backup wird nur durchgefuehrt    #\n"
                                 "#     falls Datei / Ordner veraendert wurde.    #\n"
                                 "#                                               #\n"
                                 "#################################################\n\nEingabe: ";
                    std::cin >> eingabe;
                    int eingabe_int;
                    int eingabe_index_int;
                    std::string eingabe_index;

                    try
                    {
                        eingabe_int = std::stoi(eingabe);
                    }
                    catch (const std::exception &e)
                    {
                        eingabe_int = 99;
                    }

                    switch (eingabe_int)
                    {
                    case 1:
                        backup_menu = false;
                        this->backup_queue_menu_clear();
                        break;
                    case 2:
                        this->add_backup_entity();
                        continue;
                        break;
                    case 3:

                        std::cout << "\nWelcher Backupplan soll geloescht werden ? Nr. 1 - " << index - 1 << "\nEingabe: ";
                        std::cin >> eingabe_index;
                        try
                        {
                            eingabe_index_int = std::stoi(eingabe_index);
                            if (eingabe_index_int > 0 && eingabe_index_int < index)
                            {
                                this->delete_backup_entity(eingabe_index_int - 1);
                                continue;
                            }
                        }
                        catch (const std::exception &e)
                        {
                            continue;
                        }

                        break;
                    case 4:
                        this->handle_backup_queue();
                        continue;
                        break;
                    default:
                        continue;
                        break;
                    }
                }
                break;
            case 2:
                exit(0);
                break;
            default:
                this->backup_queue_menu_clear();
                break;
            }
        }
    }
};


// Löschen Inhalt des Konsolenfensters, je nach OS
void CBackupQueue::backup_queue_menu_clear()
{
    std::string system_name = PLATFORM_NAME;

    if (system_name == "windows")
    {
        std::system("cls");
        std ::cout << "\x1B[2J\x1B[H";
    }
    else if (system_name == "osx" || system_name == "linux")
    {
        std::system("clear");
    }
    else
    {
        std::system("clear");
    }
};


// Durchführung der Backups für alle Backuppläne
void CBackupQueue::handle_backup_queue()
{
    std::vector<CBackupEntity> backup_entities = this->get_backup_entities();

    for (auto &it : backup_entities)
    {
        // Übergeben der aktuellen Backupplan Instanz
        CBackupHandler backup_handler = CBackupHandler(it);

        backup_handler.handle_backup();
    }

    this->set_backup_entities(backup_entities);
    this->save_backup_entities(backup_entities);
};