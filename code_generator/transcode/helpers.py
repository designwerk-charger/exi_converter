
def get_target_dict(ns_view):
    for attr in ('_target_dict', 'target_dict'):
        if hasattr(ns_view, attr):
            return getattr(ns_view, attr)
    raise AttributeError("No known target dict found in NamespaceView")