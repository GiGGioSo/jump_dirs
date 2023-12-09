# jump_dirs

Jump around the most used directories, based on 'frecency'.

Inspired by [rupa/z](https://github.com/rupa/z).

## Installation / Basic Setup

Get a copy of `jump_dirs` by EITHER:
- downloading the zip from the releases and extract it wherever you want.
- cloning the whole repository and compiling the source file yourself.


If you want to start `jump_dirs` only for the current console, execute
```
.\start_jump_dirs.cmd
```

If you want to enable `jump_dirs` on every console you open, execute
```
.\enable_jump_dirs.cmd
```
__P.S.: Enabling `jump_dirs` does not start it in the current console. You can either open a new console or execute `.\start_jump_dirs.cmd`.__

You should now be able to use the command `z`.

__After having started/enabled it, you will need to `cd` around a bit to allow `jump_dirs` to build up the datafile and learn your favourite directories!__

_P.S.: If you move the `jump_dirs` directory you will need to re-start/enable it in order for it to work correctly_

## Usage / Examples

- `z foo` -> cd to the most frecent directory matching `foo`

- `z -e foo` -> print the most frecent directory matching `foo`

- `z -l` -> list all directories in the data file

- `z -a foo` -> add `foo` _(gets converted to a fully qualified path)_ to the datafile without cd-ing inside of it

- `z -x foo` -> remove `foo` _(gets converted to a fully qualified path)_ from the datafile

## Configuration

You can optionally set some environment variables yourself to change some default values.

- Set `JUMP_DIRS_CMD` to change the command name (default is `z`).

- Set `JUMP_DIRS_DATAFILE` to change the path of the datafile (default is `<jump_dirs dir>\_jump_dirs_data_file`).

## Uninstall / Disable / Stop

If you want to stop `jump_dirs` only for the current console, execute
```
.\stop_jump_dirs.cmd
```

If you want to disable `jump_dirs` on every console you open, execute
```
.\disable_jump_dirs.cmd
```

If you want to remove `jump_dirs` completly from your computer, disable it and then delete its directory.
