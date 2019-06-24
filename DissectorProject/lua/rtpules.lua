protocol = Proto("RTPules" ,"RTPules Protocol")

version = ProtoField.int8("RTPules.version", "version", base.DEC)
padding = ProtoField.bool("RTPules.padding", "padding", base.NONE)
extension = ProtoField.bool("RTPules.extension", "extension", base.NONE)
CSRC_count = ProtoField.int8("RTPules.CSRC_count", "CSRC_count", base.DEC)
marker = ProtoField.bool("RTPules.marker", "marker", base.NONE)
payload_type = ProtoField.int8("RTPules.payload_type", "payload_type", base.DEC)
sequence_number = ProtoField.uint16("RTPules.sequence_number", "sequence_number", base.DEC_HEX)
timestamp = ProtoField.int32("RTPules.timestamp", "timestamp", base.DEC)
SSRC = ProtoField.uint32("RTPules.SSRC", "SSRC", base.HEX_DEC)
data = ProtoField.bytes("RTPules.data", "data", base.NONE)

protocol.fields = {version, padding, extension, CSRC_count, marker, payload_type, sequence_number, timestamp, SSRC, data}

function protocol.dissector(buffer, pinfo, tree)
	length = buffer:len()
	if length == 0 then return end

	pinfo.cols.protocol = protocol.name
	local subtree = tree:add(protocol, buffer(), "RTPules Protocol")
	subtree:add(version, buffer(0,1):bitfield(0,2))
	subtree:add(padding, buffer(0,1):bitfield(2,1))
	subtree:add(extension, buffer(0,1):bitfield(3,1))
	subtree:add(CSRC_count, buffer(0,1):bitfield(4,4))
	subtree:add(marker, buffer(1,1):bitfield(0,1))
	subtree:add(payload_type, buffer(1,1):bitfield(1,7))
	subtree:add(sequence_number, buffer(2,2))
	subtree:add(timestamp, buffer(4,4))
	subtree:add(SSRC, buffer(8,4))
	subtree:add(data, buffer(12,length - 12))
end

local udp_port = DissectorTable.get("udp.port")
udp_port:add(6040, protocol)
