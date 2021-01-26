#ifndef __DATABASE_H__
#define __DATABASE_H__

#include <iostream>
#include <filesystem>
#include <fstream>

#include <external_headers/json.hpp>

using json = nlohmann::json;

class CDatabase
{

private:
    std::string database_path;

    json database;

    void set_database_path(std::string database_path);

    std::string get_database_path();

    void initialize_database(std::string database_path);

public:
    CDatabase(std::string database_path);

    json fetch_database();

    void update_database(json database);

    void save_database();

    void reset_database();
};

#endif