#include <iostream>
#include <filesystem>

#include <custom_headers/database.h>
#include <custom_headers/backup_queue.h>

using namespace std;

int main(int argc, char **argv)
{

    // Initialisieren der JSON Datei / Datenbank aus dem Projektordner
    string database_path = filesystem::current_path().c_str();
    database_path = database_path + "/../database/database.json";
    CDatabase new_database = CDatabase(database_path);

    // Starte ConsoleBackup ConsoleUI / mit Menü
    if (argc == 1)
    {
        CBackupQueue backup_queue = CBackupQueue(new_database, false);
    }
    // Starte ConsoleBackup ohne Menü, prüfe alle Backuppläne und schließe Programm
    else if (argc == 2)
    {
        string argument_string = argv[1];
        if (argument_string == "/service")
        {
            CBackupQueue backup_queue = CBackupQueue(new_database, true);
            cout << "\nAlle Backupplaene wurden ueberprueft." << endl;
        }
        else
        {
            cout << "Argument nicht gueltig. Keines oder /service angeben." << endl;
            exit(0);
        }
    }

    return 0;
}