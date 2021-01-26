#include <custom_headers/database.h>

// Starte Datenbank Initialisierung sobal eine Instanz erzeugt wurde
CDatabase::CDatabase(std::string database_path)
{
    this->initialize_database(database_path);
};

// Setter Datenbankpfad
void CDatabase::set_database_path(std::string database_path)
{
    this->database_path = database_path;
};

// Getter Datenbankpfad
std::string CDatabase::get_database_path()
{
    return this->database_path;
};

// Falls möglich, auslesen der JSON Datei / Speichern des Inhalts in json Variable database
void CDatabase::initialize_database(std::string database_path)
{

    try
    {
        std::ifstream database_file(database_path);

        // Wenn JSON Datei voranden, Inhalt auslesen
        if (database_file)
        {
            this->set_database_path(database_path);
            this->database = json::parse(std::string((std::istreambuf_iterator<char>(database_file)), std::istreambuf_iterator<char>()));
            database_file.close();
        }
        else
        {
            database_file.close();
            throw std::runtime_error("Can't read database. Path \"" + database_path + "\" doesn't exist. Please provide a valid path.\n");
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
};

// Getter für JSON Datenbank
json CDatabase::fetch_database()
{
    return this->database;
};

// Setter für JSON Datenabk
void CDatabase::update_database(json database)
{
    this->database = database;
};

// Inhalt der json database Variable in die JSON Datei schreiben
void CDatabase::save_database()
{

    try
    {
        std::string database_path = this->get_database_path();
        std::ofstream database_file(database_path);

        // Falls Datei vorhanden, Inhalt in Datei schreiben
        if (database_file)
        {
            database_file << this->database.dump();
            database_file.close();
        }
        else
        {
            database_file.close();
            throw std::runtime_error("Can't save database. Path \"" + database_path + "\" doesn't exist. Please provide a valid path.\n");
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
};

// JSON Datei überschreiben und auf Usprung zurücksetzen
void CDatabase::reset_database()
{
    try
    {
        std::string database_path = this->get_database_path();
        std::ofstream database_file(database_path);

        // Falls Datei vorhanden, Inhalt in Datei schreiben
        if (database_file)
        {
            // JSON aufbau: {"backup_entities":[]}
            json backup_entities_json = json::parse("{\"backup_entities\":[]}");
            database_file << backup_entities_json.dump();
            database_file.close();
        }
        else
        {
            database_file.close();
            throw std::runtime_error("Can't save database. Path \"" + database_path + "\" doesn't exist. Please provide a valid path.\n");
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
};