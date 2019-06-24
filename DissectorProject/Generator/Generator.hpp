#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <string>

//json library
using json = nlohmann::json;
using namespace std;

class Generator
{
  public:

    void generate_protocol(json protocol, std::ofstream& out);

    void generate_fields(json protocol, std::ofstream& out);

    void set_generated_fields(json protocol, std::ofstream& out);

    void generate_dissector(json protocol, std::ofstream& out);

    void set_dependencies(json protocol, std::ofstream& out);

    void generate_remaining_fields(json protocol, std::ofstream& out);

    void get_number_of_patterns(json protocol, std::ofstream& out);
};
