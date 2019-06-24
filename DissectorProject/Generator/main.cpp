#include "Generator.hpp"

int main(int argc, char *argv[])
{

  //check args
  if (argc != 3)
  {
    cout << "Usage: ./main File.json Output.lua" << endl;
    return 0;
  }

  string json_file = argv[1];
  string output_lua = argv[2];

  Generator gen;

  //open the json file
  ifstream in(json_file);
  json protocol;
  //decode json file into a json object
  in >> protocol;

  //set output file
  ofstream out(output_lua, ofstream::app);

  gen.generate_protocol(protocol, out);

  gen.generate_fields(protocol, out);

  gen.set_generated_fields(protocol, out);

  gen.generate_dissector(protocol, out);

  gen.set_dependencies(protocol, out);

  return 0;
}
