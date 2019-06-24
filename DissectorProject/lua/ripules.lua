protocol = Proto("RIPules" ,"RIPules Protocol")

command = ProtoField.int8("RIPules.command", "command", base.DEC)
version = ProtoField.int8("RIPules.version", "version", base.DEC)
address_family = ProtoField.int16("RIPules.address_family", "address_family", base.DEC)
ip = ProtoField.ipv4("RIPules.ip", "ip", base.DEC)
metric = ProtoField.int32("RIPules.metric", "metric", base.DEC)

protocol.fields = {command, version, address_family, ip, metric}

function number_of_patterns(buffer)
	 length = buffer:len()
	 length = length - 4
	 number = 0
	while length ~= 0 do
		 length = length - 20
		 number = number + 1
	 end
	 return number
end

function generate_remaining_fields(buffer, subtree)
	number = number_of_patterns(buffer)
	buf_start = 4
	for i=1, number do
		subtree:add(address_family, buffer(buf_start + 0, 2))
		subtree:add(ip, buffer(buf_start + 4, 4))
		subtree:add(metric, buffer(buf_start + 16, 4))
		buf_start = buf_start + 20
	end
end

function protocol.dissector(buffer, pinfo, tree)
	length = buffer:len()
	if length == 0 then return end

	pinfo.cols.protocol = protocol.name
	local subtree = tree:add(protocol, buffer(), "RIPules Protocol")
	subtree:add(command, buffer(0,1))
	subtree:add(version, buffer(1,1))
	generate_remaining_fields(buffer, subtree)
end

local udp_port = DissectorTable.get("udp.port")
udp_port:add(520, protocol)
local proto = DissectorTable.get("ip.proto")
proto:add_for_decode_as(protocol)
