import sys
import os
import random
import struct
import subprocess

from dataclasses import dataclass
from tempfile import NamedTemporaryFile


@dataclass
class Node:
    offset: int
    next_offset: int
    country: str
    part_number: int

    def dump(self) -> bytes:
        return (
            # "<i"
            #  |`- integer
            #  `-- little endian
            struct.pack("<i", self.next_offset)
            + self.country.encode("ascii")
            + struct.pack("<i", self.part_number)
        )


NODE_SIZE_BYTES = 10  # sizeof(int) + sizeof(char[2]) + sizeof(int)


def shuffled(x: list):
    random.shuffle(x)
    return x


TEST_CASES = [
    {
        "name": "one",
        "items": [
            {"offset": 0, "country": "ru", "part_number": 111},
        ],
    },
    {
        "name": "two sequential",
        "items": [
            {"offset": 0, "country": "ru", "part_number": 111},
            {"offset": NODE_SIZE_BYTES, "country": "ru", "part_number": 111},
        ],
    },
    {
        "name": "sequential",
        "items": [
            {"offset": NODE_SIZE_BYTES * i, "country": "ru", "part_number": i * 100 + i * 10 + i}
            for i in range(10)
        ],
    },
    {
        "name": "sequential LARGE",
        "items": [
            {"offset": NODE_SIZE_BYTES * i, "country": "ru", "part_number": i}
            for i in range(1000)
        ],
    },
    {
        "name": "reverced",
        "items": [
            {"offset": 0, "country": "am", "part_number": 1},
            {"offset": NODE_SIZE_BYTES * 3, "country": "sb", "part_number": 2},
            {"offset": NODE_SIZE_BYTES * 2, "country": "al", "part_number": 3},
            {"offset": NODE_SIZE_BYTES * 1, "country": "uk", "part_number": 4},
        ],
    },
    {
        "name": "non-aligned",
        "items": [
            {"offset": 0, "country": "us", "part_number": 1},
            {"offset": NODE_SIZE_BYTES * 5 + 50, "country": "ru", "part_number": 2},
            {"offset": NODE_SIZE_BYTES * 4 + 40, "country": "us", "part_number": 3},
            {"offset": NODE_SIZE_BYTES * 2 + 20, "country": "kz", "part_number": 4},
            {"offset": NODE_SIZE_BYTES * 3 + 30, "country": "ge", "part_number": 5},
            {"offset": NODE_SIZE_BYTES * 1 + 10, "country": "io", "part_number": 6},
        ],
    },
    {
        "name": "non-aligned random shuffled LARGE",
        "items": [
            {"offset": NODE_SIZE_BYTES * i, "country": "cy", "part_number": i * 10}
            for i in [0] + shuffled(list(range(1, 1000)))
        ]
    },
]


def run_test_case(cmd: str, test: dict):
    print(f"START TEST CASE '{test['name']}'")

    print("> creating file")
    all_nodes = [
        Node(
            offset=item["offset"],
            next_offset=(next_item and next_item["offset"]) or -1,
            country=item["country"],
            part_number=item["part_number"],
        )
        for item, next_item in zip(test["items"], test["items"][1::] + [None])
    ]

    file_size = max(n.next_offset for n in all_nodes) + 9
    file_size += 11  # просто чтоб самый "правый" элемент не примыкал вплотную к концу файла

    file_data = bytearray(b"\0" * file_size)

    for node in all_nodes:
        file_data[node.offset:node.offset + NODE_SIZE_BYTES] = node.dump()

    with NamedTemporaryFile(mode="w+b") as tempfile:
        tempfile.write(file_data)
        tempfile.flush()

        print("> starting program")

        result = subprocess.run([cmd, tempfile.name], stdout=subprocess.PIPE)
        assert result.returncode == 0

    print("> output comparation")
    stdout = result.stdout.decode("ascii")
    for node, line in zip(all_nodes, stdout.split("\n")):
        country, part_number = line.split(" ")
        part_number = int(part_number)

        assert node.country == country, f"expected: {node.country}, got: {country}"
        assert node.part_number == part_number, f"expected: {node.part_number}, got: {part_number}"
    
    print("> OK!")


def main():
    cmd = sys.argv[1]

    for test in TEST_CASES:
        run_test_case(cmd, test)


if __name__ == "__main__":
    main()
