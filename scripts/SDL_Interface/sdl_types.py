CORE_CATEGORIES = {
    "camera",
    "film",
    "estimator",
    "sample-generator",
    "renderer",
    "option"
}

WORLD_CATEGORIES = {
    "geometry",
    "material",
    "motion",
    "light-source",
    "actor",
    "image",
    "frame-processor"
}

VALUE_TYPES = {
    "string",
    "integer",
    "real",
    "vector3",
    "quaternion",
    "real-array",
    "vector3-array"
}

CATEGORIES = CORE_CATEGORIES | WORLD_CATEGORIES
REFERENCE_TYPES = CATEGORIES


def is_core_category(category_name):
    return category_name in CORE_CATEGORIES


def is_world_category(category_name):
    return category_name in WORLD_CATEGORIES


def is_value(type_name):
    return type_name in VALUE_TYPES


def is_reference(type_name):
    return type_name in REFERENCE_TYPES


def is_struct(type_name):
    return not is_value(type_name) and not is_reference(type_name)
