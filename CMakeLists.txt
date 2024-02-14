set(CommonFiles
    "common/cli.c"
    "common/endian-util.c"
    "common/image.c"
    "common/io.c"
    "common/list.c"
    "common/mem.c"
    "common/pcx.c"
    "common/tga.c"
    "common/wal.c"
    )

add_executable("wal-convert"
    "src/wal-convert.c"
    ${CommonFiles})
add_executable("wal-edit"
    "src/wal-edit.c"
    ${CommonFiles})
add_executable("wal-export"
    "src/wal-export.c"
    ${CommonFiles})
add_executable("wal-info"
    "src/wal-info.c"
    ${CommonFiles})
add_executable("wal-term"
    "src/wal-term.c"
    ${CommonFiles})

