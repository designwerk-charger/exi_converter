
ISO15118_2_MSGS_SPECIAL = ["PaymentDetails", "MeteringReceipt", "ChargingStatus", "CertificateInstallation",
                           "CertificateUpdate", "ServiceDetail"]
ISO15118_2_MSGS = ["SessionSetup", "ServiceDiscovery", "PaymentServiceSelection", "Authorization",
                   "ChargeParameterDiscovery", "CableCheck", "PreCharge", "PowerDelivery", "CurrentDemand",
                   "WeldingDetection", "SessionStop"] + ISO15118_2_MSGS_SPECIAL
ISO15118_2_MAIN_TYPES = ["MessageHeaderType"]


MESSAGE_TYPES = {"iso15118_2": ISO15118_2_MSGS,
                 "app_protocol": ["supportedAppProtocol"]  # this is unfortunatelly not defined as type
                 }

ADDITIONAL_TYPES = {"iso15118_2": ISO15118_2_MAIN_TYPES,
                    "app_protocol": ["AppProtocolType", "responseCodeType", "idType"]
                    }
