"""Differential test against the locally supplied FlashGBX parser, without imports
of its UI/device stack. Only the named pure parser AST is executed."""
import ast
import ctypes
from pathlib import Path
import subprocess
root = Path(__file__).resolve().parents[1]
reference = root / 'references/upstream/lesserkuma-flash-database/FlashGBX/RomFileAGB.py'
tree = ast.parse(reference.read_text(encoding='utf-8-sig'))
cls = next(x for x in tree.body if isinstance(x, ast.ClassDef) and x.name == 'RomFileAGB')
fn = next(x for x in cls.body if isinstance(x, ast.FunctionDef) and x.name == 'GetSaveTypeFromHeaderMarker')
fn.decorator_list = []
scope = {}
exec(compile(ast.fix_missing_locations(ast.Module(body=[fn], type_ignores=[])), str(reference), 'exec'), scope)
out = root / 'build/sram138/marker139.so'
subprocess.run(['gcc', '-std=c11', '-Wall', '-Wextra', '-Werror', '-shared', '-fPIC',
               str(root/'core/save/detect139.c'), '-o', str(out)], check=True)
class Result(ctypes.Structure):
    _fields_ = [('type', ctypes.c_int), ('basis', ctypes.c_int), ('bytes', ctypes.c_uint32),
                ('flash_id', ctypes.c_uint16), ('rc', ctypes.c_int),
                ('backup', ctypes.c_uint), ('write', ctypes.c_uint)]
lib = ctypes.CDLL(str(out))
lib.cb_save139_header_marker.argtypes = [ctypes.POINTER(ctypes.c_uint8), ctypes.c_size_t]
lib.cb_save139_header_marker.restype = Result
for flags in range(256):
    h = bytearray(192)
    h[190:192] = bytes([0x4c, flags])
    expected = scope['GetSaveTypeFromHeaderMarker'](h)
    data = (ctypes.c_uint8 * len(h)).from_buffer_copy(h)
    got = lib.cb_save139_header_marker(data, len(h))
    actual = None if got.type == -1 else (got.type, got.bytes)
    assert actual == expected, (flags, actual, expected)
    assert not got.backup and not got.write
    for n in (0, 190, 191):
        assert lib.cb_save139_header_marker(data, n).type == -1
print('PASS: all 256 marker flag values match local FlashGBX; short inputs rejected; no operation authorized')
