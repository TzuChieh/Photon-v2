from abc import ABC, abstractmethod


class AbstractData(ABC):
    def __init__(self):
        super().__init__()

    @abstractmethod
    def get_type(self):
        pass

    @abstractmethod
    def generate_data(self):
        pass


class String(AbstractData):
    def __init__(self, string=""):
        super().__init__()
        self.__string = string

    def get_type(self):
        return "string"

    def generate_data(self):
        return self.__string


class Enum(AbstractData):
    def __init__(self, stringID=""):
        super().__init__()
        self.__stringID = stringID

    def get_type(self):
        return "enum"

    def generate_data(self):
        return self.__stringID


class Integer(AbstractData):
    def __init__(self, integer=0):
        super().__init__()
        self.__integer = integer

    def get_type(self):
        return "integer"

    def generate_data(self):
        return str(self.__integer)


class Real(AbstractData):
    def __init__(self, real=0):
        super().__init__()
        self.__real = real

    def get_type(self):
        return "real"

    def generate_data(self):
        return str(self.__real)


class Vector2(AbstractData):
    def __init__(self, vector=(0, 0)):
        super().__init__()
        self.__vector = vector

    def get_type(self):
        return "vector2"

    def generate_data(self):
        return "\"%f %f\"" % (self.__vector[0], self.__vector[1])


class Vector3(AbstractData):
    def __init__(self, vector=(0, 0, 0)):
        super().__init__()
        self.__vector = vector

    def get_type(self):
        return "vector3"

    def generate_data(self):
        return "\"%f %f %f\"" % (self.__vector[0], self.__vector[1], self.__vector[2])


class Quaternion(AbstractData):
    def __init__(self, quaternion=(0, 0, 0, 0)):
        super().__init__()
        self.__quaternion = quaternion

    def get_type(self):
        return "quaternion"

    def generate_data(self):
        return "\"%f %f %f %f\"" % (self.__quaternion[0], self.__quaternion[1], self.__quaternion[2], self.__quaternion[3])


class RealArray(AbstractData):
    def __init__(self, array=None):
        super().__init__()
        if array is None:
            self.__array = []
        else:
            self.__array = array

    def get_type(self):
        return "real-array"

    def generate_data(self):
        fragments = ["{"]
        for real in self.__array:
            fragments.append("%f " % real)
        fragments.append("}")
        return "".join(fragments)

    def set_array(self, array):
        self.__array = array
        return self

    def add(self, real):
        self.__array.append(real)
        return self


class Vector3Array(AbstractData):
    def __init__(self, array=None):
        super().__init__()
        if array is None:
            self.__array = []
        else:
            self.__array = array

    def get_type(self):
        return "vector3-array"

    def generate_data(self):
        fragments = ["{"]
        for vector3 in self.__array:
            fragments.append("\"%f %f %f\" " % (vector3[0], vector3[1], vector3[2]))
        fragments.append("}")
        return "".join(fragments)

    def set_array(self, array):
        self.__array = array
        return self

    def add(self, vector3):
        self.__array.append(vector3)
        return self


class Spectrum(AbstractData):
    def __init__(self, values=(0, 0, 0)):
        super().__init__()
        self.__values = values

    def get_type(self):
        return "spectrum"

    def generate_data(self):
        fragments = ["\""]
        for value in self.__values:
            fragments.append(str(value) + " ")
        fragments.append("\"")
        return "".join(fragments)


class Path(AbstractData):
    def __init__(self, string=""):
        super().__init__()
        self.__string = string

    def get_type(self):
        return "path"

    def generate_data(self):
        return self.__string


class Reference(AbstractData):
    def __init__(self, ref_type="", ref_name=""):
        super().__init__()
        self.__ref_type = ref_type
        self.__ref_name = ref_name

    def get_type(self):
        return self.__ref_type

    def generate_data(self):
        return "@\"%s\"" % self.__ref_name


class RawData(AbstractData):
    def __init__(self, type_string="", data_string=""):
        super().__init__()
        self.__type_string = type_string
        self.__data_string = data_string

    def get_type(self):
        return self.__type_string

    def generate_data(self):
        return self.__data_string


class AbstractCommand(ABC):
    def __init__(self):
        super().__init__()

    @abstractmethod
    def generate(self):
        pass


class AbstractInputCommand(AbstractCommand):
    def __init__(self):
        super().__init__()
        self.__inputs = []

    @abstractmethod
    def get_full_type(self):
        pass

    @abstractmethod
    def generate(self):
        pass

    def set_input(self, name, data: AbstractData):
        self.__inputs.append((name, data))

    def _generate_input_fragments(self, out_fragments):
        for name, data in self.__inputs:
            out_fragments.append("[")
            out_fragments.append(data.get_type() + " ")
            out_fragments.append(name + " ")
            out_fragments.append(data.generate_data())
            out_fragments.append("]")


class RawCommand(AbstractCommand):
    """
    Take any string and use it as a command.
    """

    def __init__(self):
        super().__init__()
        self.__command_string = ""

    def append_string(self, string):
        self.__command_string += string

    def generate(self):
        return self.__command_string


class DirectiveCommand(AbstractCommand):
    def __init__(self):
        super().__init__()
        self.__directive_string = ""

    def append_directive(self, string):
        self.__directive_string += string

    def generate(self):
        fragments = ["#", self.__directive_string, ";\n"]
        return "".join(fragments)


class CreatorCommand(AbstractInputCommand):
    def __init__(self):
        super().__init__()
        self.__data_name = ""

    @abstractmethod
    def get_full_type(self):
        pass

    def generate(self):
        # TODO: some part can be pre-generated
        fragments = [
            self.get_full_type(), " ",
            "@\"" + self.__data_name + "\"", " = "]
        self._generate_input_fragments(fragments)
        fragments.append(";\n")

        return "".join(fragments)

    def set_data_name(self, data_name):
        self.__data_name = data_name


class ExplicitExecutorCommand(AbstractInputCommand):
    def __init__(self):
        super().__init__()
        self.__target_name = ""

    @abstractmethod
    def get_full_type(self):
        pass

    @abstractmethod
    def get_name(self):
        pass

    def generate(self):
        # TODO: some part can be pre-generated
        fragments = [
            self.get_full_type(), ".",
            self.get_name(), "(",
            "@\"" + self.__target_name + "\")", " = "]
        self._generate_input_fragments(fragments)
        fragments.append(";\n")

        return "".join(fragments)

    def set_target_name(self, data_name):
        self.__target_name = data_name


class ImplicitExecutorCommand(AbstractInputCommand):
    def __init__(self):
        super().__init__()
        self.__target_name = ""

    def get_full_type(self):
        return None

    @abstractmethod
    def get_name(self):
        pass

    def generate(self):
        # TODO: some part can be pre-generated
        fragments = [
            self.get_name(), "(",
            "@\"" + self.__target_name + "\")", " = "]
        self._generate_input_fragments(fragments)
        fragments.append(";\n")

        return "".join(fragments)

    def set_target_name(self, data_name):
        self.__target_name = data_name
