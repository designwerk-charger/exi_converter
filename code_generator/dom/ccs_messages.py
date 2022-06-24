
ISO15118_2_MSGS_SPECIAL = ["PaymentDetails", "MeteringReceipt", "ChargingStatus", "CertificateInstallation",
                           "CertificateUpdate", "ServiceDetail"]
ISO15118_2_MSGS = ["SessionSetup", "ServiceDiscovery", "PaymentServiceSelection", "Authorization",
                   "ChargeParameterDiscovery", "CableCheck", "PreCharge", "PowerDelivery", "CurrentDemand",
                   "WeldingDetection", "SessionStop"] + ISO15118_2_MSGS_SPECIAL

DIN_MSGS_SPECIAL = ["ServiceDetail", "PaymentDetails"]

DIN_SPEC_MSGS = ["SessionSetup", "ServiceDiscovery", "ServicePaymentSelection",
                 "ContractAuthentication", "ChargeParameterDiscovery", "CableCheck",
                 "PreCharge", "PowerDelivery", "CurrentDemand", "WeldingDetection",
                 "SessionStop"]


MESSAGE_TYPES = {"iso15118_2": ISO15118_2_MSGS,
                 "din_spec": DIN_SPEC_MSGS + DIN_MSGS_SPECIAL,
                 "app_protocol": ["supportedAppProtocol"]  # this is unfortunatelly not defined as type
                 }

ADDITIONAL_TYPES = {"iso15118_2": ["MessageHeaderType"],
                    "din_spec": ["MessageHeaderType"],
                    "app_protocol": []
                    }
