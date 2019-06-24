protocol = Proto("ICMPules" ,"ICMPules Protocol")

type = ProtoField.int8("ICMPules.type", "type", base.DEC)
code = ProtoField.int8("ICMPules.code", "code", base.DEC)
checksum = ProtoField.uint8("ICMPules.checksum", "checksum", base.HEX)
identifier = ProtoField.uint16("ICMPules.identifier", "identifier", base.DEC_HEX)
sequence_number = ProtoField.uint16("ICMPules.sequence_number", "sequence_number", base.DEC_HEX)
timestamp = ProtoField.absolute_time("ICMPules.timestamp", "timestamp", base.MDT)
data = ProtoField.bytes("ICMPules.data", "data", base.NONE)

protocol.fields = {type, code, checksum, identifier, sequence_number, timestamp, data}

function protocol.dissector(buffer, pinfo, tree)
	length = buffer:len()
	if length == 0 then return end

	pinfo.cols.protocol = protocol.name
	local subtree = tree:add(protocol, buffer(), "ICMPules Protocol")
	subtree:add(type, buffer(0,1))
	subtree:add(code, buffer(1,1))
	subtree:add(checksum, buffer(2,2))
	subtree:add(identifier, buffer(4,2))
	subtree:add(sequence_number, buffer(6,2))
	subtree:add(timestamp, buffer(8,8))
	subtree:add(data, buffer(16,length - 16))
end

local proto = DissectorTable.get("ip.proto")
proto:add_for_decode_as(protocol)
