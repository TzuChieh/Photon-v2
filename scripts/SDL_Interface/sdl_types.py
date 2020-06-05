VALUE_TYPES = {
    "string",
    "integer",
    "real",
    "vector3",
    "quaternion",
    "real-array",
    "vector3-array"
}

REFERENCE_TYPES = {
    "camera",
    "film",
    "estimator",
    "sample-generator",
    "renderer",
    "option",
    "geometry",
    "material",
    "motion",
    "light-source",
    "actor",
    "image",
    "frame-processor"
}


def is_value(type_name):
    return type_name in VALUE_TYPES


def is_reference(type_name):
    return type_name in REFERENCE_TYPES


def is_struct(type_name):
    return not is_value(type_name) and not is_reference(type_name)
