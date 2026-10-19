# Near misses

C drafts of functions that compile but do not match the ROM yet. Each file holds one function, written for `tools/check_c.py` (no includes; `common.h` is added when it is checked). The build does not use these files.

To work on one:

```sh
mkdir -p build/manual && cp nonmatching/func_XXXXXXXX.c build/manual/
python3 tools/check_c.py build/manual
python3 tools/diff_func.py build/manual/func_XXXXXXXX.c func_XXXXXXXX
```

When it matches, apply it with `tools/apply_auto.py build/manual` and delete it from here.
