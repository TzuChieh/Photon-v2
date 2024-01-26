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

    def get_tag(self):
        return ''


class AbstractPacket(ABC):
    def __init__(self):
        super().__init__()

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
        return f"\"{self.__string}\""


class Enum(AbstractData):
    def __init__(self, string_id=""):
        super().__init__()
        self.__string_id = string_id

    def get_type(self):
        return "enum"

    def generate_data(self):
        return f"\"{self.__string_id}\""


class Bool(AbstractData):
    def __init__(self, value=False):
        """
        @param value A value that is convertible to bool.
        """
        super().__init__()
        self.__value = value

    def get_type(self):
        return "bool"

    def generate_data(self):
        # `__value` may not be bool already (e.g., an int), always convert to bool again
        return f"{bool(self.__value)}"


class Integer(AbstractData):
    def __init__(self, integer=0):
        super().__init__()
        self.__integer = integer

    def get_type(self):
        return "integer"

    def generate_data(self):
        return f"{self.__integer}"


class Real(AbstractData):
    def __init__(self, real=0):
        super().__init__()
        self.__real = real

    def get_type(self):
        return "real"

    def generate_data(self):
        return f"{self.__real}"


class Vector2(AbstractData):
    def __init__(self, vector=(0, 0)):
        super().__init__()
        self.__vector = vector

    def get_type(self):
        return "vector2"

    def generate_data(self):
        return f"\"{self.__vector[0]} {self.__vector[1]}\""


class Vector3(AbstractData):
    def __init__(self, vector=(0, 0, 0)):
        super().__init__()
        self.__vector = vector

    def get_type(self):
        return "vector3"

    def generate_data(self):
        return f"\"{self.__vector[0]} {self.__vector[1]} {self.__vector[2]}\""


class Quaternion(AbstractData):
    def __init__(self, quaternion=(0, 0, 0, 0)):
        super().__init__()
        self.__quaternion = quaternion

    def get_type(self):
        return "quaternion"

    def generate_data(self):
        return f"\"{self.__quaternion[0]} {self.__quaternion[1]} {self.__quaternion[2]} {self.__quaternion[3]}\""


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
            fragments.append(f"{real} ")
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
            fragments.append(f"\"{vector3[0]} {vector3[1]} {vector3[2]} \"")
        fragments.append("}")
        return "".join(fragments)

    def set_array(self, array):
        self.__array = array
        return self

    def add(self, vector3):
        self.__array.append(vector3)
        return self


class Spectrum(AbstractData):
    def __init__(self, values=(0, 0, 0), color_space=''):
        super().__init__()
        self.__values = values
        self.__color_space = color_space

    def get_type(self):
        return "spectrum"

    def get_tag(self):
        return self.__color_space

    def generate_data(self):
        fragments = ["\""]
        for value in self.__values:
            fragments.append(f"{value} ")
        fragments.append("\"")
        return "".join(fragments)


class Path(AbstractData):
    def __init__(self, string=""):
        super().__init__()
        self.__string = string

    def get_type(self):
        return "path"

    def generate_data(self):
        return f"\"{self.__string}\""


class ResourceIdentifier(AbstractData):
    def __init__(self, string=""):
        super().__init__()
        self.__string = string

    def get_type(self):
        return "PRI"

    def generate_data(self):
        return f"\"{self.__string}\""

    def set_bundled_path(self, path):
        self.__string = f":{path}"

    def set_external_path(self, path):
        self.__string = f"ext:{path}"


class Reference(AbstractData):
    def __init__(self, ref_type="", ref_name=""):
        super().__init__()
        self.__ref_type = ref_type
        self.__ref_name = ref_name

    def get_type(self):
        return self.__ref_type

    def generate_data(self):
        return f"@\"{self.__ref_name}\""


class StructArray(AbstractData):
    def __init__(self, packets=None):
        super().__init__()
        if packets is None:
            self.__packets = []
        else:
            self.__packets = packets

    def get_type(self):
        return "struct-array"

    def generate_data(self):
        fragments = ["{"]
        for packet in self.__packets:
            fragments.append(f"{packet.generate_data()} ")
        fragments.append("}")
        return "".join(fragments)

    def set_packets(self, packets):
        self.__packets = packets
        return self

    def add(self, packet: AbstractPacket):
        self.__packets.append(packet)
        return self


class RawData(AbstractData):
    """
    Make an input clause directly.
    """
    def __init__(self, type_string="", data_string="", tag_string=""):
        super().__init__()
        self.__type_string = type_string
        self.__data_string = data_string
        self.__tag_string = tag_string

    def get_type(self):
        return self.__type_string

    def get_tag(self):
        return self.__tag_string

    def generate_data(self):
        return self.__data_string


class CachedPacket(AbstractPacket):
    def __init__(self, packet_name=""):
        super().__init__()
        self.__packet_name = packet_name

    def generate_data(self):
        return f"$\"{self.__packet_name}\""


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
        self.__packet_inputs = []

    @abstractmethod
    def get_full_type(self):
        pass

    @abstractmethod
    def generate(self):
        pass

    def set_input(self, name, data: AbstractData):
        """
        Set `data` to the input named `name`.
        """
        self.__inputs.append((name, data))

    def add_inputs(self, data: AbstractPacket):
        """
        Add a packet of inputs which will be used for setting each individual input. Effectively the same
        as calling `set_input()` for each input in the packet. Packet inputs are placed after all inputs
        from `set_input()`.
        """
        self.__packet_inputs.append(data)

    def _generate_input_fragments(self, out_fragments):
        for name, data in self.__inputs:
            out_fragments.append(f"[{data.get_type()} {name}")

            if data.get_tag():
                out_fragments.append(f":{data.get_tag()}")

            out_fragments.append(f" {data.generate_data()}]")

        for data in self.__packet_inputs:
            out_fragments.append(f"[{data.generate_data()}]")


class DirectiveCommand(AbstractCommand):
    """
    Creates a SDL directive.
    """
    def __init__(self):
        super().__init__()
        self.__directive_string = ""

    def append_directive(self, string):
        self.__directive_string += string

    def generate(self):
        fragments = ["#", *self.__directive_string, ";\n"]
        return "".join(fragments)


class CreatorCommand(AbstractInputCommand):
    """
    Creates a SDL resource.
    """
    def __init__(self):
        super().__init__()
        self.__data_name = ""

    @abstractmethod
    def get_full_type(self):
        pass

    def generate(self):
        # TODO: some part can be pre-generated
        fragments = [f"{self.get_full_type()} @\"{self.__data_name}\" = "]
        self._generate_input_fragments(fragments)
        fragments.append(";\n")

        return "".join(fragments)

    def set_data_name(self, data_name):
        self.__data_name = data_name


class ExplicitExecutorCommand(AbstractInputCommand):
    """
    Creates a SDL executor caller with explicit type information.
    """
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
        fragments = [f"{self.get_full_type()}.{self.get_name()}(@\"{self.__target_name}\") = "]
        self._generate_input_fragments(fragments)
        fragments.append(";\n")

        return "".join(fragments)

    def set_target_name(self, data_name):
        self.__target_name = data_name


class ImplicitExecutorCommand(AbstractInputCommand):
    """
    Creates a SDL executor caller without explicit type information.
    """
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
        fragments = [f"{self.get_name()}(@\"{self.__target_name}\") = "]
        self._generate_input_fragments(fragments)
        fragments.append(";\n")

        return "".join(fragments)

    def set_target_name(self, data_name):
        self.__target_name = data_name


class RawCommand(AbstractCommand):
    """
    Take any string and use it as a command directly.
    """
    def __init__(self):
        super().__init__()
        self.__command_string = ""

    def append_string(self, string):
        self.__command_string += string

    def generate(self):
        return self.__command_string


class CachedPacketCommand(AbstractInputCommand):
    """
    Creates a cached SDL data packet.
    """
    def __init__(self):
        super().__init__()
        self.__data_name = ""

    def get_full_type(self):
        return ""

    def generate(self):
        # TODO: some part can be pre-generated
        fragments = [f"packet $\"{self.__data_name}\" = "]
        self._generate_input_fragments(fragments)
        fragments.append(";\n")

        return "".join(fragments)

    def set_data_name(self, data_name):
        self.__data_name = data_name
