"""Integration checks in a disposable directory; only test fixtures are modified."""
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
BIN = ROOT / 'bin'

with tempfile.TemporaryDirectory() as folder:
    cwd = Path(folder)
    def run(name, *args, data=None):
        return subprocess.run([str(BIN / ('os-' + name)), *map(str, args)],
                              cwd=cwd, input=data, capture_output=True, timeout=5)
    content = b'first\nsecond\nlast' + bytes(range(256)) * 12
    source = cwd / 'source.txt'
    source.write_bytes(content)
    assert run('show', source).stdout == content
    assert run('copy', source).returncode == 0
    copied = cwd / 'source.txt.copy'
    assert copied.read_bytes() == content
    copied.write_bytes(content * 2)
    assert run('copy', source).returncode == 0 and copied.read_bytes() == content
    destination = cwd / 'destination.txt'
    destination.write_bytes(b'prefix\n')
    assert run('append', source, destination).returncode == 0
    assert destination.read_bytes() == b'prefix\n' + content
    assert run('append', source, source).returncode != 0
    assert source.read_bytes() == content
    assert run('append', source, cwd / 'missing').returncode != 0
    assert run('count', source).stdout == f'Newline count: {content.count(bytes([10]))}\n'.encode()
    empty = cwd / 'empty'; empty.touch()
    assert run('count', empty).stdout == b'Newline count: 0\n'
    assert run('show', empty).stdout == b''
    assert b'source.txt\t[file]' in run('list', cwd).stdout
    assert b'Type: Regular file' in run('info', source).stdout
    assert b'Status changed:' in run('info', source).stdout
    link = cwd / 'link'; link.symlink_to(source)
    assert b'Type: Symbolic link' in run('info', link).stdout
    assert b'Type: Directory' in run('info', cwd).stdout
    # Copy must reject an existing destination that aliases its own source.
    copied.unlink(); copied.symlink_to(source)
    assert run('copy', source).returncode != 0 and source.read_bytes() == content
    assert run('remove', empty).returncode == 0 and not empty.exists()
    for command in ['show', 'copy', 'append', 'remove', 'count', 'info']:
        result = run(command)
        assert result.returncode != 0 and b'Usage:' in result.stderr
        assert run(command, cwd / 'absent').returncode != 0
    assert run('list', cwd / 'absent').returncode != 0
    assert run('show', cwd).returncode != 0
    assert run('count', cwd).returncode != 0
    shell = run('shell', data=(str(BIN / 'os-count') + ' source.txt\n'
                                'no-such-toolkit-command\nexit\n').encode())
    assert shell.returncode == 0
    assert b'exited with status 0' in shell.stdout and b'exited with status 127' in shell.stdout
    assert run('shell', data=b' \t\nexit\n').returncode == 0
    assert run('shell', data=b'').returncode == 0
    result = run('shell', data=b'x' * 1050 + b'\nexit\n')
    assert b'Command line is too long.' in result.stderr
    result = run('shell', data=b'x ' * 65 + b'\nexit\n')
    assert b'Too many command arguments.' in result.stderr
print('PASS: all eight commands, binary copying, append guards, metadata, errors and shell process execution')
