#include "Generator.hpp"

//register our new protocol
void Generator::generate_protocol(json protocol, ofstream& out)
{
  out << "protocol = Proto(" << protocol["protoabbrev"] << " ," << protocol["protoname"] << ")" << '\n';
  out << '\n';
}

//create fields variables for later registration, the algorithm takes the variable's field name,
//type and base as parameters to generate the code for the defining the field in lua
void Generator::generate_fields(json protocol, ofstream& out)
{
  json fields = protocol["fields"];
  json remaining_fields = protocol["remaining_data"];

  for (json::iterator it = fields.begin(); it != fields.end(); ++it)
  {
    json field = *it;
    string proto_abbrev = protocol["protoabbrev"];
    string field_name = field["name"];
    string field_type = field["type"];
    string field_base = field["base"];

    out << field_name << " = ProtoField." << field_type << "(\"" << proto_abbrev << "." <<
    field_name << "\", \"" << field_name << "\", base." << field_base << ")" << '\n';
  }

  for (json::iterator it = remaining_fields.begin(); it != remaining_fields.end(); ++it)
  {
    json field = *it;
    string proto_abbrev = protocol["protoabbrev"];
    string field_name = field["name"];
    string field_type = field["type"];
    string field_base = field["base"];

    out << field_name << " = ProtoField." << field_type << "(\"" << proto_abbrev << "." <<
    field_name << "\", \"" << field_name << "\", base." << field_base << ")" << '\n';
  }


  out << '\n';
}

//generate the lua code to register the fields in our protocol
void Generator::set_generated_fields(json protocol, ofstream& out)
{
  json fields = protocol["fields"];
  json remaining_fields = protocol["remaining_data"];
  int field_number = 1;
  int fields_size = fields.size();
  int remaining_fields_size = remaining_fields.size();

  out << "protocol.fields = {";
  for (json::iterator it = fields.begin(); it != fields.end(); ++it)
  {
    json field = *it;
    string field_name = field["name"];

    if (!remaining_fields.is_null())
    {
      out << field_name << ", ";
    }

    else
    {
      if (field_number < fields_size) out << field_name << ", ";
      else out << field_name;
      field_number++;
    }

  }

  field_number = 1;

  if (!remaining_fields.is_null())
  {
    for (json::iterator it = remaining_fields.begin(); it != remaining_fields.end(); ++it)
    {
      json remaining_field = *it;
      string remaining_field_name = remaining_field["name"];
      if (field_number < remaining_fields_size) out << remaining_field_name << ", ";
      else out << remaining_field_name;
      field_number++;
    }
  }

  out << "}" << '\n';
  out << '\n';

  //write the utility functions in case of "remaining data" field set, these two
  //functions need to be defined before adding the fields to the dissection's tree
  get_number_of_patterns(protocol, out);
  generate_remaining_fields(protocol, out);

}

//this method add the fields defined in the "field" field in the json file
//to the dissection's tree
void Generator::generate_dissector(json protocol, ofstream& out)
{
  json fields = protocol["fields"];
  json remaining_fields = protocol["remaining_data"];
  string add_endian;
  string endianness = protocol["endianness"];

  if (endianness == "big") add_endian = "add";
  else if (endianness == "little") add_endian = "add_le";
  else throw "Bad endian format";

  out << "function protocol.dissector(buffer, pinfo, tree)" << '\n';
  out << "\tlength = buffer:len()" << '\n';
  out << "\tif length == 0 then return end" << '\n';
  out << '\n';
  out << "\tpinfo.cols.protocol = protocol.name" << '\n';
  out << "\tlocal subtree = tree:add(protocol, buffer(), " << protocol["protoname"] << ")" << '\n';

  for (json::iterator it = fields.begin(); it != fields.end(); ++it)
  {
    json field = *it;
    string field_name = field["name"];
    string byte_start = field["size"]["byte"][0];
    string byte_end = field["size"]["byte"][1];
    string bit_start = field["size"]["bit"][0];
    string bit_end = field["size"]["bit"][1];

    out << "\tsubtree:" << add_endian << "(" << field_name << ", buffer(" << byte_start <<",";
    if (byte_end == "end") out << "length - " << byte_start;
    else out << byte_end;

    out << ")";

    if (bit_start.compare("0") != 0 || bit_end.compare("0") != 0)
    {
      out << ":bitfield(" << bit_start << "," << bit_end << ")";
    }

    out << ")" << '\n';
  }

  if (!remaining_fields.is_null())
  {
    out << "\tgenerate_remaining_fields(buffer, subtree)" << '\n';
  }

  out << "end" << '\n';
  out << '\n';

}

//this method generates the code to define the upper layer protocol used and tcp/udp ports
void Generator::set_dependencies(json protocol, ofstream& out)
{
  int tcp = protocol["tcp_port"];
  int udp = protocol["udp_port"];

  if (tcp != 0 && udp !=0 ) throw "Two ports defined";

  if (tcp != 0)
  {
    out << "local tcp_port = DissectorTable.get(\"tcp.port\")" << '\n';
    out << "tcp_port:add(" << tcp << ", protocol)" << '\n';
  }

  if (udp != 0)
  {
    out << "local udp_port = DissectorTable.get(\"udp.port\")" << '\n';
    out << "udp_port:add(" << udp << ", protocol)" << '\n';
  }

  if (!protocol["dependencies"].is_null())
  {
    out << "local proto = DissectorTable.get(" << protocol["dependencies"] << ")" << '\n';
    out << "proto:add_for_decode_as(protocol)" << '\n';
  }
}

//this method is used just in case of a "remaining data" field defined, it writes
//a lua function that gets called later in the code in order to gain the information
//about how many pattern instances are present inside the packet, it's a utility function used
//by the next method to know how many times to add iteratively the "remaining_data" fields

void Generator::get_number_of_patterns(json protocol, ofstream& out)
{
  json fields = protocol["remaining_data"];

  if (fields.is_null()) return;

  int fields_size = fields.size();
  string start_str = fields[0]["size"]["byte"][0];
  string offset_str = fields[0]["size"]["byte"][1];
  int start = stoi(start_str);
  int offset = stoi(offset_str);
  int pattern_offset = offset;
  int temp, temp1, temp2;
  string temp1_str, temp2_str;

  out << "function number_of_patterns(buffer)" << '\n';
  out << "\t length = buffer:len()" <<'\n';
  out << "\t length = length - " << start <<'\n';
  out << "\t number = 0" << '\n';

  for (int i = 1; i < fields_size; i++)
  {
    temp1_str = fields[i]["size"]["byte"][0];
    temp1 = stoi(temp1_str);
    temp2_str = fields[i]["size"]["byte"][1];
    temp2 = stoi(temp2_str);
    temp = temp1 - (start + offset) + temp2;
    pattern_offset += temp;
    start = temp1;
    offset = temp2;
  }

    out << "\twhile length ~= 0 do" <<'\n';
    out << "\t\t length = length - " << pattern_offset << '\n';
    out << "\t\t number = number + 1" << '\n';
    out << "\t end" << '\n';
    out << "\t return number" << '\n';
    out << "end" << '\n';
    out << '\n';
}

//this method is conceptually the same as generate_dissector, the difference is that while
//method generate_dissector already knows how many fields to add to the dissection's
//tree, instead this method gets that information by calling the lua function number_of_patterns(buffer)
//and adds the fields iteratively
void Generator::generate_remaining_fields(json protocol, ofstream& out)
{
  json fields = protocol["remaining_data"];

  if (fields.is_null()) return;

  string add_endian;
  string endianness = protocol["endianness"];

  if (endianness == "big") add_endian = "add";
  else if (endianness == "little") add_endian = "add_le";
  else throw "Bad endian format";

  int size = fields.size();

  string start_str = fields[0]["size"]["byte"][0];
  string byte1_str = fields[0]["size"]["byte"][1];
  int buf_start = stoi(start_str);
  int offset = 0;
  string byte0_str;
  int byte0, byte1;
  string bit0_str, bit1_str;
  int bit0, bit1;
  byte1 = stoi(byte1_str);

  out << "function generate_remaining_fields(buffer, subtree)" << '\n';
  out << "\tnumber = number_of_patterns(buffer)" << '\n';
  out << "\tbuf_start = " << buf_start << '\n';
  out << "\tfor i=1, number do" << '\n';

  for (int i = 0; i < size; ++i)
  {
      string name = fields[i]["name"];
      json bitfield = fields[i]["size"]["bit"];
      bit0_str = bitfield[0];
      bit1_str = bitfield[1];
      bit0 = stoi(bit0_str);
      bit1 = stoi(bit1_str);

      out << "\t\tsubtree:" << add_endian << "(" << name << ", buffer(buf_start + " << offset << ", " << byte1 << ")";

      if (bit0 != 0 || bit1 != 0)
      {
        out << ":bitfield(" << bit0 << ", " << bit1 << ")";
      }

      out << ")" << '\n';

      if (i + 1 != size)
      {
        byte0_str = fields[i+1]["size"]["byte"][0];
        byte1_str = fields[i+1]["size"]["byte"][1];
        byte0 = stoi(byte0_str);
        byte1 = stoi(byte1_str);
        offset = byte0 - buf_start;
      }

      else
      {
        out << "\t\tbuf_start = buf_start + " << offset + byte1 << '\n';
        out << "\tend" << '\n';
        out << "end" << '\n';
        out << '\n';
      }
  }
}
