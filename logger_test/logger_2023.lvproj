<?xml version='1.0' encoding='UTF-8'?>
<Project Type="Project" LVVersion="19008000">
	<Item Name="My Computer" Type="My Computer">
		<Property Name="server.app.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="server.control.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="server.tcp.enabled" Type="Bool">false</Property>
		<Property Name="server.tcp.port" Type="Int">0</Property>
		<Property Name="server.tcp.serviceName" Type="Str">My Computer/VI Server</Property>
		<Property Name="server.tcp.serviceName.default" Type="Str">My Computer/VI Server</Property>
		<Property Name="server.vi.callsEnabled" Type="Bool">true</Property>
		<Property Name="server.vi.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="specify.custom.address" Type="Bool">false</Property>
		<Item Name="assemble_command.vi" Type="VI" URL="../command/assemble_command.vi"/>
		<Item Name="basic.vi" Type="VI" URL="../panels/basic.vi"/>
		<Item Name="basic_configuration_cluster.ctl" Type="VI" URL="../typedefs/basic_configuration_cluster.ctl"/>
		<Item Name="command.ctl" Type="VI" URL="../typedefs/command.ctl"/>
		<Item Name="create_files.vi" Type="VI" URL="../file/create_files.vi"/>
		<Item Name="main.vi" Type="VI" URL="../main.vi"/>
		<Item Name="operation.ctl" Type="VI" URL="../typedefs/operation.ctl"/>
		<Item Name="Dependencies" Type="Dependencies">
			<Item Name="vi.lib" Type="Folder">
				<Item Name="VISA Configure Serial Port" Type="VI" URL="/&lt;vilib&gt;/Instr/_visa.llb/VISA Configure Serial Port"/>
				<Item Name="VISA Configure Serial Port (Instr).vi" Type="VI" URL="/&lt;vilib&gt;/Instr/_visa.llb/VISA Configure Serial Port (Instr).vi"/>
				<Item Name="VISA Configure Serial Port (Serial Instr).vi" Type="VI" URL="/&lt;vilib&gt;/Instr/_visa.llb/VISA Configure Serial Port (Serial Instr).vi"/>
				<Item Name="Write Delimited Spreadsheet (DBL).vi" Type="VI" URL="/&lt;vilib&gt;/Utility/file.llb/Write Delimited Spreadsheet (DBL).vi"/>
				<Item Name="Write Delimited Spreadsheet (I64).vi" Type="VI" URL="/&lt;vilib&gt;/Utility/file.llb/Write Delimited Spreadsheet (I64).vi"/>
				<Item Name="Write Delimited Spreadsheet (string).vi" Type="VI" URL="/&lt;vilib&gt;/Utility/file.llb/Write Delimited Spreadsheet (string).vi"/>
				<Item Name="Write Delimited Spreadsheet.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/file.llb/Write Delimited Spreadsheet.vi"/>
				<Item Name="Write Spreadsheet String.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/file.llb/Write Spreadsheet String.vi"/>
			</Item>
			<Item Name="command_codes.vi" Type="VI" URL="../command/command_codes.vi"/>
			<Item Name="converter_configuration_cluster.ctl" Type="VI" URL="../typedefs/converter_configuration_cluster.ctl"/>
			<Item Name="disassemble_command.vi" Type="VI" URL="../command/disassemble_command.vi"/>
			<Item Name="log_data_cluster.ctl" Type="VI" URL="../typedefs/log_data_cluster.ctl"/>
			<Item Name="serial_clear_reset_wait.vi" Type="VI" URL="../serial/serial_clear_reset_wait.vi"/>
			<Item Name="serial_command.vi" Type="VI" URL="../serial/serial_command.vi"/>
			<Item Name="test_configuration_cluster.ctl" Type="VI" URL="../typedefs/test_configuration_cluster.ctl"/>
		</Item>
		<Item Name="Build Specifications" Type="Build"/>
	</Item>
</Project>
