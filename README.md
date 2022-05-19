
# Designwerk EXI Converter Library
A C++ Library with Python bindings (Pybind11) to encode and decode json strings to EXI.

## Converter Tool
Simple tool to interface with the EXI Converter Library.

### Binary EXI to JSON String

`./converter_tool decode 8098023c17ef4aa4d723b151e020310d20c4c8cd115cc4c8cc115ea200`

```
{
	"V2G_Message": {
		"Header": {
			"SessionID": "F05FBD2A935C8EC5"
		},
		"Body": {
			"SessionSetupRes": {
				"ResponseCode": "OK_NewSessionEstablished",
				"EVSEID": "CH123DW123",
				"EVSETimeStamp": 277130
			}
		}
	}
}
```